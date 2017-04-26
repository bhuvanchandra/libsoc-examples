/*
 * ssd1306.c
 * SSD1306 128x64 OLED display helper functions using libsoc API.
 *
 * Copyright (C) 2017 - BhuvanChandra DV <bhuvanchandradv@gmail.com>
 *
 * ssd1306 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ssd1306 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ssd1306. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "libsoc_spi.h"
#include "libsoc_gpio.h"
#include "ssd1306.h"

#define width		128
#define height		64
#define pages		8

void command(ssd1306 *oled, char cmd)
{
	uint8_t command[1] = {0};

	command[0] = cmd;
	libsoc_gpio_set_level(oled->dc, LOW);
	libsoc_spi_write(oled->spi_dev, command, 1);
}

void SSD1306_begin(ssd1306 *oled)
{	
	libsoc_gpio_set_level(oled->res, HIGH);
	usleep(1000);
	libsoc_gpio_set_level(oled->res, LOW);
	usleep(1000);
	libsoc_gpio_set_level(oled->res, HIGH);

	command(oled, SSD1306_DISPLAYOFF);
	command(oled, SSD1306_SETDISPLAYCLOCKDIV);
	command(oled, 0x80);		/* the suggested ratio 0x80 */

	command(oled, SSD1306_SETMULTIPLEX);
	command(oled, 0x3F);
	command(oled, SSD1306_SETDISPLAYOFFSET);
	command(oled, 0x0);			/* no offset */
	command(oled, SSD1306_SETSTARTLINE | 0x0);	/* line #0 */
	command(oled, SSD1306_CHARGEPUMP);
	command(oled, (oled->switch_capvcc == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	command(oled, SSD1306_MEMORYMODE);
	command(oled, 0x00);		/* 0x0 act like ks0108 */

	command(oled, SSD1306_SEGREMAP | 0x1);
	command(oled, SSD1306_COMSCANDEC);
	command(oled, SSD1306_SETCOMPINS);
	command(oled, 0x12);		/* TODO - calculate based on _rawHieght ? */
	command(oled, SSD1306_SETCONTRAST);
	command(oled, (oled->switch_capvcc == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF);
	command(oled, SSD1306_SETPRECHARGE);
	command(oled, (oled->switch_capvcc == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
	command(oled, SSD1306_SETVCOMDETECT);
	command(oled, 0x40);
	command(oled, SSD1306_DISPLAYALLON_RESUME);
	command(oled, SSD1306_NORMALDISPLAY);
	command(oled, SSD1306_DISPLAYON);
}

void SSD1306_clear(ssd1306 *oled)
{
	int i;

	for(i = 0; i < sizeof(oled->buffer); i++) {
		oled->buffer[i] = 0;
	}
}

void SSD1306_pixel(ssd1306 *oled,
					int x,
					int y,
					char color)
{
    if (x > width || y > height)
		return ;

    if (color)
        oled->buffer[x+(y/8)*width] |= 1<<(y%8);
    else
        oled->buffer[x+(y/8)*width] &= ~(1<<(y%8));
}

void SSD1306_char1616(ssd1306 *oled,
						uint8_t x,
						uint8_t y,
						uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, y0 = y, chMode = 0;

	for (i = 0; i < 32; i ++) {
		chTemp = Font1612[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80? 1 : 0; 
			SSD1306_pixel(oled, x, y, chMode);
			chTemp <<= 1;
			y ++;
			if ((y - y0) == 16) {
				y = y0;
				x ++;
				break;
			}
		}
	}
}

void SSD1306_char3216(ssd1306 *oled,
						uint8_t x,
						uint8_t y,
						uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, y0 = y, chMode = 0;

	for (i = 0; i < 64; i ++) {
		chTemp = Font3216[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80 ? 1 : 0; 
			SSD1306_pixel(oled, x, y, chMode);
			chTemp <<= 1;
			y ++;
			if ((y - y0) == 32) {
				y = y0;
				x ++;
				break;
			}
		}
	}
}

void SSD1306_char(ssd1306 *oled,
					unsigned char x,
					unsigned char y,
					char acsii,
					char size,
					char mode)
{
	char temp;
	unsigned char i, j, y0 = y;
	unsigned char ch = acsii - ' ';

	for(i = 0; i < size; i++) {
		if (size == 12) {
			if (mode)temp = Font1206[ch][i];
			else temp = ~Font1206[ch][i];
		} else {			
			if (mode)
				temp = Font1608[ch][i];
			else
				temp = ~Font1608[ch][i];
		}
		for(j = 0; j < 8; j++) {
			if (temp & 0x80)
				SSD1306_pixel(oled, x, y, 1);
			else
				SSD1306_pixel(oled, x, y, 0);

			temp <<= 1;
			y++;

			if ((y - y0) == size) {
				y = y0;
				x++;
				break;
			}
		}
	}
}

void SSD1306_string(ssd1306 *oled,
					uint8_t x,
					uint8_t y,
					const char *pString,
					uint8_t Size,
					uint8_t Mode)
{
    while (*pString != '\0') {       
        if (x > (width - Size / 2)) {
			x = 0;
			y += Size;
			if (y > (height - Size)) {
				y = x = 0;
			}
		}
		
        SSD1306_char(oled, x, y, *pString, Size, Mode);
        x += Size / 2;
        pString ++;
    }
}

void SSD1306_bitmap(ssd1306 *oled,
					unsigned char x,
					unsigned char y,
					const unsigned char *pBmp,
					char chWidth,
					char chHeight)
{
	unsigned char i, j, byteWidth = (chWidth + 7) / 8;

	for (j = 0; j < chHeight; j++) {
		for (i = 0; i < chWidth; i ++) {
			if (*(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7)))
				SSD1306_pixel(oled, x + i, y + j, 1);
		}
	}		
}

void SSD1306_display(ssd1306 *oled)
{
	command(oled, SSD1306_COLUMNADDR);
	command(oled, 0);			/* cloumn start address */
	command(oled, width - 1);	/*cloumn end address */
	command(oled, SSD1306_PAGEADDR);
	command(oled, 0);			/* page atart address */
	command(oled, pages - 1);	/* page end address */

	libsoc_gpio_set_level(oled->dc, HIGH);
	/* TODO: get the correct size of buf */
	libsoc_spi_write(oled->spi_dev, oled->buffer, sizeof(oled->buffer));
}
