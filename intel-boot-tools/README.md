ZenFone boot tools
==================

Fork from https://github.com/sndnvaps/intel-boot-tools

Tools to pack/unpack ZenFone 5/6 boot images


### Build

- run `make` to build tools: `pack_intel` & `unpack_intel`


### Usage

To build boot.img (or recovery.img) from custom kernel (bzImage) and initrd (ramdisk.cpio.gz), use original boot.img (or recovery.img) as input:

	$ pack_intel boot.img bzImage ramdisk.cpio.gz new_boot.img

To extract kernel (bzImage) and initrd (ramdisk.cpio.gz) from boot.img (or recovery.img):

	$ unpack_intel boot.img bzImage ramdisk.cpio.gz


### How to get original boot.img

For ZenFone 5
- Go http://www.asus.com/Phones/ZenFone_5_A500CG/HelpDesk_Download/ and choose "Android" in OS field (or http://www.asus.com/support/Download/39/1/0/2/96nqlxHp1VKV4Rdz/32/)

For ZenFone 6
- Go http://www.asus.com/Phones/ZenFone_6_A600CG/HelpDesk_Download/ and choose "Android" in OS field (or http://www.asus.com/support/Download/39/1/0/3/paJ6GdiFh3rgCrfL/32/)

- Choose ROM file from `firmware` section and download
- Unzip ROM file and you'll see `boot.img`


### How to get original recovery.img

Please refer to: http://23pin.logdown.com/posts/222680-build-zenfone-recovery-image-from-bootimg
