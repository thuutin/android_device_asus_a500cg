/*
 * Copyright (C) 2014 Intel Corporation
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Implementation to add battery ocv property and disable periodic polling
 */

#define LOG_TAG "healthd-intel"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/String8.h>
#include <cutils/klog.h>
#include <cutils/properties.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <semaphore.h>

#include <healthd.h>

#define POWER_SUPPLY_SUBSYSTEM "power_supply"
#define POWER_SUPPLY_SYSFS_PATH "/sys/class/" POWER_SUPPLY_SUBSYSTEM
#define PERIODIC_CHORES_INTERVAL_FAST (60 * 1)

#define FG_HELPER           "fg_conf"
#define FG_HELPER_PATH      "/system/bin/" FG_HELPER
#define FG_HELPER_EXEC      FG_HELPER_PATH " -r"
#define INOTIFY_PATH        "/sys/devices/virtual/misc/watchdog"

static struct healthd_config *ghc;
static int savedStatus, savedLevel;
static bool isHelperPresent;
static int mos;
static sem_t fg_sem;
static bool sem_valid = true;
static bool kill_thread = false;

using namespace android;

static int readFromFile(const String8& path, char* buf, size_t size) {
    char *cp = NULL;

    if (path.isEmpty())
        return -1;
    int fd = open(path.string(), O_RDONLY, 0);
    if (fd == -1) {
        KLOG_ERROR(LOG_TAG, "Could not open '%s'\n", path.string());
        return -1;
    }

    ssize_t count = TEMP_FAILURE_RETRY(read(fd, buf, size));
    if (count > 0)
            cp = (char *)memrchr(buf, '\n', count);

    if (cp)
        *cp = '\0';
    else
        buf[0] = '\0';

    close(fd);
    return count;
}

static bool isBattery(const String8 &path)
{
    const int SIZE = 128;
    char buf[SIZE] = {0};

    int length = readFromFile(path, buf, SIZE);

    if (length <= 0)
        return false;

    if (strcmp(buf, "Battery") == 0)
        return true;

    return false;
}

static void init_battery_path(struct healthd_config *hc) {
    String8 path;

    DIR* dir = opendir(POWER_SUPPLY_SYSFS_PATH);
    if (dir == NULL) {
        KLOG_ERROR(LOG_TAG, "Could not open %s\n", POWER_SUPPLY_SYSFS_PATH);
    } else {
        struct dirent* entry;

        while ((entry = readdir(dir))) {
            const char* name = entry->d_name;

            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;

            char buf[20];
            // Look for "type" file in each subdirectory
            path.clear();
            path.appendFormat("%s/%s/type", POWER_SUPPLY_SYSFS_PATH, name);
            if (isBattery(path)) {
                if (hc->batteryVoltagePath.isEmpty()) {
                    path.clear();
                    path.appendFormat("%s/%s/voltage_ocv",
                                      POWER_SUPPLY_SYSFS_PATH, name);
                    if (access(path, R_OK) == 0) {
                        hc->batteryVoltagePath = path;
                    } else {
                        path.clear();
                        path.appendFormat("%s/%s/voltage_now",
                                          POWER_SUPPLY_SYSFS_PATH, name);
                        if (access(path, R_OK) == 0) {
                            hc->batteryVoltagePath = path;
                        } else {
                                path.clear();
                                path.appendFormat("%s/%s/batt_vol",
                                          POWER_SUPPLY_SYSFS_PATH, name);
                                if (access(path, R_OK) == 0)
                                    hc->batteryVoltagePath = path;
                        }
                    }
                    break;
                }

            }
        }
        closedir(dir);
    }
}

#define MAX_COMMAND_LINE_BUF	1024
static int is_mos()
{
    int mos = 0;
    char cmdline_buf[MAX_COMMAND_LINE_BUF];
    char *ptr;

    int fd, size;

    fd = open("/proc/cmdline", O_RDONLY);
    if (fd < 0) {
        ALOGI("%s:Unable to read commandline\n", __func__);
    } else {
        size = read(fd, cmdline_buf, MAX_COMMAND_LINE_BUF);
        cmdline_buf[MAX_COMMAND_LINE_BUF - 1] = '\0';
        if (size <= 0) {
            ALOGI("%s:error to read commandline\n", __func__);
        } else {
            ptr = strstr(cmdline_buf, "androidboot.mode=");
            if (ptr != NULL) {
                ptr += strlen("androidboot.mode=");
                if (strncmp(ptr, "main", 4) == 0)
                    mos = 1;
            }
        }
        close(fd);
    }
    return mos;
}

static int healthd_process_inotify_evt(int fd)
{
#define BUF_LEN        1024
#define INOTIFY_SIZE   (sizeof(struct inotify_event))
#define PROP_SHUTDOWN  "sys.shutdown.requested"

    char buffer[BUF_LEN];
    int length, i = 0;
    int prop_len;
    char prop_shutdown[128];

    length = read(fd, buffer, BUF_LEN);

    if (length < 0) {
        KLOG_ERROR(LOG_TAG, "error reading inotify %d", length);
        return -1;
    }

    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        if (event->len) {
            if (event->mask & IN_MODIFY) {
                prop_len = property_get(PROP_SHUTDOWN, prop_shutdown, "NONE");
                if (strcmp(prop_shutdown, "NONE")) {
                    /* shutdown is triggered */
                    if (sem_valid)
                        sem_wait(&fg_sem);
                    if (system(FG_HELPER_EXEC) < 0) {
                        KLOG_ERROR(LOG_TAG, "Cannot execute %s\n",
                                                     FG_HELPER_EXEC);
                    }
                    if (sem_valid)
                        sem_post(&fg_sem);
                    kill_thread = true;
                }
            }
        }
        i += INOTIFY_SIZE + event->len;
    }
    return 0;
}

static void *healthd_inotify_thread(void * /* p */)
{
    struct epoll_event evt, nevts[1];
    int i, inotify_fd, inotify_wd, epoll_fd;

    epoll_fd = epoll_create(1);
    if (epoll_fd < 0) {
        KLOG_ERROR(LOG_TAG, "error epoll %d", epoll_fd);
        return NULL;
    }

    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        KLOG_ERROR(LOG_TAG, "error inotify %d", inotify_fd);
        close(epoll_fd);
        return NULL;
    }

    inotify_wd = inotify_add_watch(inotify_fd, INOTIFY_PATH, IN_MODIFY);

    evt.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inotify_fd, &evt);

    do {
       epoll_wait(epoll_fd, nevts, 1, -1);
       for (i = 0; i < 1; i++) {
           healthd_process_inotify_evt(inotify_fd);
           if (kill_thread)
               break;
       }
    }while(1);

    inotify_rm_watch(inotify_fd, inotify_wd);
    close(inotify_fd);
    close(epoll_fd);
    if (sem_valid)
        sem_destroy(&fg_sem);
    return NULL;
}

void healthd_board_init(struct healthd_config *config)
{
     int fd;
     pthread_t inotify_thread;

     mos = is_mos();
     init_battery_path(config);
     config->periodic_chores_interval_fast = -1;
     config->periodic_chores_interval_slow = -1;
     ghc = config;

     fd = open(FG_HELPER_PATH, O_RDONLY);
     isHelperPresent = (fd < 0) ? false: true;
     if (isHelperPresent) {
         close(fd);
     }
     if (sem_init(&fg_sem, 0, 1) < 0) {
         KLOG_ERROR(LOG_TAG, "unable to initialize sem");
         sem_valid = false;
     }
     if (pthread_create(&inotify_thread, NULL,
                        healthd_inotify_thread, NULL) < 0) {
         KLOG_ERROR(LOG_TAG, "cannot create thread.");
     }
}

static void *execfn(void * /* p */)
{
     if (sem_valid)
         sem_wait(&fg_sem);

     if (system(FG_HELPER_EXEC) < 0)
         KLOG_ERROR(LOG_TAG, "Cannot execute %s\n",FG_HELPER_EXEC);

     if (sem_valid)
         sem_post(&fg_sem);
     return NULL;
}


int healthd_board_battery_update(struct android::BatteryProperties *props)
{
    pthread_t t;

    if ((props->chargerAcOnline | props->chargerUsbOnline |
           props->chargerWirelessOnline) && (props->batteryLevel == 0))
        ghc->periodic_chores_interval_fast =
                                     PERIODIC_CHORES_INTERVAL_FAST;

    if (isHelperPresent) {
	if (((props->batteryLevel != savedLevel) &&
            (props->batteryLevel == 70 || props->batteryLevel == 5 ||
             props->batteryLevel == 0)) ||
            ((props->batteryStatus != savedStatus) &&
            (props->batteryStatus == BATTERY_STATUS_FULL ||
             props->batteryStatus == BATTERY_STATUS_DISCHARGING))) {
             if (pthread_create(&t, NULL, execfn, NULL) < 0) {
                    KLOG_ERROR(LOG_TAG, "cannot create thread\n");
             }
             else {
                pthread_join(t, NULL);
             }
             savedStatus = props->batteryStatus;
             savedLevel = props->batteryLevel;
        }
    }

    if (mos && BATTERY_STATUS_NOT_CHARGING == props->batteryStatus) {
           props->chargerWirelessOnline = false;
           props->chargerUsbOnline = false;
           props->chargerAcOnline = false;
    }

    return 0;
}
