/*
 * pack.c
 *
 * Copyright 2012 Emilio López <turl@tuxfamily.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#ifdef __APPLE__
  #include <libkern/OSByteOrder.h>

  #define htobe16(x) OSSwapHostToBigInt16(x)
  #define htole16(x) OSSwapHostToLittleInt16(x)
  #define be16toh(x) OSSwapBigToHostInt16(x)
  #define le16toh(x) OSSwapLittleToHostInt16(x)

  #define htobe32(x) OSSwapHostToBigInt32(x)
  #define htole32(x) OSSwapHostToLittleInt32(x)
  #define be32toh(x) OSSwapBigToHostInt32(x)
  #define le32toh(x) OSSwapLittleToHostInt32(x)

  #define htobe64(x) OSSwapHostToBigInt64(x)
  #define htole64(x) OSSwapHostToLittleInt64(x)
  #define be64toh(x) OSSwapBigToHostInt64(x)
  #define le64toh(x) OSSwapLittleToHostInt64(x)
#include <sys/types.h>
#else
#include <endian.h>
#endif  /* __APPLE__ */

#include "bootheader.h"

#define ERROR(...) do { fprintf(stderr, __VA_ARGS__); return 1; } while(0)

uint8_t calculate_checksum(struct bootheader * hdr) {
        uint8_t sum = hdr->xor;
        uint8_t *data = (uint8_t *) hdr;
        int i;
        
        for (i = 0; i < HEADER_SIZE; i++) { 
                sum ^= data[i]; 
        } 
        
        return sum;	
}	
               
int main(int argc, char *argv[])
{
	char *origin;
	char *bzImage;
	char *ramdisk;
	char *output;
	FILE *forigin;
	FILE *foutput;
	FILE *fbzImage;
	FILE *framdisk;
	struct stat st;
	uint32_t tmp, paddings, totalImageSize;
	char buf[BUFSIZ];
	size_t size;
	struct bootheader *file;

	if (argc != 5)
		ERROR("Usage: %s <valid image> <bzImage> <ramdisk> <output>\n", argv[0]);

	origin = argv[1];
	bzImage = argv[2];
	ramdisk = argv[3];
	output = argv[4];

	forigin = fopen(origin, "r");
	fbzImage = fopen(bzImage, "r");
	framdisk = fopen(ramdisk, "r");
	foutput = fopen(output, "w");
	if (!forigin || !foutput)
		ERROR("ERROR: failed to open origin or output image\n");

	/* Allocate memory and copy bootstub to it */
	file = calloc(sizeof(struct bootheader), sizeof(char));
	if (file == NULL)
		ERROR("ERROR allocating memory\n");

	if (fread(file, sizeof(struct bootheader), 1, forigin) != 1)
		ERROR("ERROR reading bootstub\n");

	/* Figure out the bzImage size and set it */
	if (stat(bzImage, &st) == 0) {
		tmp = st.st_size;
		totalImageSize = tmp;
		file->bzImageSize = htole32(tmp);
	} else
		ERROR("ERROR reading bzImage size\n");

	totalImageSize += sizeof(struct bootheader);

	/* Figure out the ramdisk size and set it */
	if (stat(ramdisk, &st) == 0) {
		tmp = st.st_size;
		totalImageSize += tmp;
		if ((totalImageSize % SECTOR_SIZE) > 0) {
			paddings = SECTOR_SIZE - (totalImageSize % SECTOR_SIZE);
			totalImageSize += paddings;
		}
		file->initrdSize = htole32(tmp);
	} else
		ERROR("ERROR reading ramdisk\n");

	file->sectors = htole32(totalImageSize / SECTOR_SIZE - 1);
	file->xor = calculate_checksum(file);

	/* Write the patched bootstub to the new image */
	if (fwrite(file, sizeof(struct bootheader), 1, foutput) != 1)
		ERROR("ERROR writing image\n");

	/* Then copy the new bzImage */
	while ((size = fread(buf, 1, BUFSIZ, fbzImage))) {
		fwrite(buf, 1, size, foutput);
	}

	/* And finally copy the ramdisk */
	while ((size = fread(buf, 1, BUFSIZ, framdisk))) {
		fwrite(buf, 1, size, foutput);
	}

	if (paddings != 0) {
		memset(buf, 0xff, paddings);
		if (fwrite(buf, paddings, 1, foutput) != 1) 
			ERROR("ERROR writing output image\n");			
	}

	return 0;
}
