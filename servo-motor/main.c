#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>

#include "servo.h"

#define ADC0_SYSFS		"/sys/bus/iio/devices/iio:device0/in_voltage8_raw"

int adc_init()
{
	int fd = 0;

	fd = open(ADC0_SYSFS, O_RDONLY);
	if(fd < 0){
		perror("ADC0: Open failed!");
		close(fd);
		return fd;
	}

	return fd;
}

void adc_de_init(int fd)
{
	close(fd);
}

int read_adc(int fd)
{
	char val[6] = {0};

	read(fd, &val, 5);
	lseek(fd, 0, SEEK_SET);		/* seek to start of file */

	return atoi(val);
}

int main(int argc, char **argv)
{
	servo s_motor;
	int f = 0;

	if(argc < 3){
			printf("Usage: %s <PWM-chip-instance> <PWM-channel>\ne.g. For PWM_B, run: %s 8 0\n", argv[0], argv[0]);
			return -1;
	}

	/* Select Colibri standard PWM_B  */
	s_motor.chip = atoi(argv[1]);	
	s_motor.channel = atoi(argv[2]);
	s_motor.period = 20000000;

	s_init(&s_motor);
	f = adc_init();

	while (1) {
		s_set_angle(&s_motor, ((180 * read_adc(f)) / 4095));
		usleep(50000);
	}

	s_de_init(&s_motor);
	adc_de_init(f);

	return 0;
}
