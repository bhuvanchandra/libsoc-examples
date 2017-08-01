/*
 * oled.c
 * A simple test application for SSD1306 OLED disply using libsoc API.
 *
 * Copyright (C) 2017 - BhuvanChandra DV <bhuvanchandradv@gmail.com>
 *
 * oled is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * oled is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oled. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "ssd1306.h"

#define GPIO_DC			82 /* SODIMM 32 */
#define GPIO_RES		31 /* SODIMM 67 */

#define SPI_DEVICE		1
#define CHIP_SELECT		0

char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int main(int argc,char **argv)
{
	time_t now;
	struct tm *timenow;
	ssd1306 *oled;
	board_config *config;

	config = libsoc_board_init();

	printf("OLED Test Program !!!\n");

	oled = malloc(sizeof(ssd1306));
	if (!oled)
		exit(ENOMEM);

	oled->dc = libsoc_gpio_request(libsoc_board_gpio_id(config, "SODIMM_32"), LS_GPIO_SHARED);
	oled->res = libsoc_gpio_request(libsoc_board_gpio_id(config, "SODIMM_67"), LS_GPIO_SHARED);

	if (oled->dc == NULL || oled->res == NULL)
		goto fail;

	libsoc_gpio_set_direction(oled->dc, OUTPUT);
	libsoc_gpio_set_direction(oled->res, OUTPUT);
	if (libsoc_gpio_get_direction(oled->dc) != OUTPUT ||
		libsoc_gpio_get_direction(oled->res) != OUTPUT) {

		printf("Failed to set direction to OUTPUT\n");
		goto fail;
	}

	oled->switch_capvcc = 0;

	oled->spi_dev = libsoc_spi_init(SPI_DEVICE, CHIP_SELECT);

	if (!oled->spi_dev) {
		printf("Failed to get spidev device!\n");
		return EXIT_FAILURE;
	}

	libsoc_spi_set_mode(oled->spi_dev, MODE_0);
	libsoc_spi_get_mode(oled->spi_dev);

	libsoc_spi_set_speed(oled->spi_dev, 2000000);
	libsoc_spi_get_speed(oled->spi_dev);

	libsoc_spi_set_bits_per_word(oled->spi_dev, BITS_8);
	libsoc_spi_get_bits_per_word(oled->spi_dev);

	SSD1306_begin(oled);
	SSD1306_clear(oled);

	while(1) {
		time(&now);
		timenow = localtime(&now);

		SSD1306_bitmap(oled, 0, 2, Singal816, 16, 8);
		SSD1306_bitmap(oled, 24, 2, Bluetooth88, 8, 8);
		SSD1306_bitmap(oled, 40, 2, Msg816, 16, 8);
		SSD1306_bitmap(oled, 64, 2, GPRS88, 8, 8);
		SSD1306_bitmap(oled, 90, 2, Alarm88, 8, 8);
		SSD1306_bitmap(oled, 112, 2, Bat816, 16, 8);

		SSD1306_string(oled, 0, 52, "MUSIC", 12, 0);
		SSD1306_string(oled, 52, 52, "MENU", 12, 0);
		SSD1306_string(oled, 98, 52, "PHONE", 12, 0);

		SSD1306_char3216(oled, 0,16, value[timenow->tm_hour/10]);
		SSD1306_char3216(oled, 16,16, value[timenow->tm_hour%10]);
		SSD1306_char3216(oled, 32,16, ':');
		SSD1306_char3216(oled, 48,16, value[timenow->tm_min/10]);
		SSD1306_char3216(oled, 64,16, value[timenow->tm_min%10]);
		SSD1306_char3216(oled, 80,16, ':');
		SSD1306_char3216(oled, 96,16, value[timenow->tm_sec/10]);
		SSD1306_char3216(oled, 112,16, value[timenow->tm_sec%10]);

		SSD1306_display(oled);
    }

	libsoc_spi_free(oled->spi_dev);
	libsoc_gpio_free(oled->res);
	libsoc_gpio_free(oled->dc);

	fail:
		if (oled->dc)
			libsoc_gpio_free(oled->dc);

		if (oled->res)
			libsoc_gpio_free(oled->res);

	return 0;
}
