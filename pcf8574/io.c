/*
 * io.c
 * A simple test application for toggling PCF8574 IOs using libsoc API.
 *
 * Copyright (C) 2017 - BhuvanChandra DV <bhuvanchandradv@gmail.com>
 *
 * io is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * io is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with io. If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include "pcf8574.h"

int main(int argc, char **argv)
{
	i2c *i2c_pcf8574;
	uint8_t ret = 0, count = 11;
	static bool on = false;
	
	if(argc < 2){
			printf("Usage: %s <io number>\n", argv[0]);
			return -1;
	}

	i2c_pcf8574 = pcf8574_init(0, PCF8574_ADDRESS);
	if (i2c_pcf8574 == NULL)
		return EXIT_FAILURE;

	while ( count > 0 ) {
		pcf8574_gpio_write(i2c_pcf8574, atoi(argv[1]), on);
		on = !on;
		count--;
		sleep(1);
	}
	goto exit;

exit:
	ret = pcf8574_free(i2c_pcf8574);
	if (ret == EXIT_FAILURE)
		perror("Failed to close I2C port");

	return 0;
}
