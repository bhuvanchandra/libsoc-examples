/* 
 * servo.c
 * Servo motor control helper functions using libsoc PWM API.
 * libsoc PWM API Documentation: https://jackmitch.github.io/libsoc/c/pwm/
 *
 * Copyright (C) 2017 - BhuvanChandra DV <bhuvanchandra.dv@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "servo.h"

void s_calib(servo *s_motor)
{
	/* Default to SG90 Servo s_motor */
	s_motor->calib_data.d_min = 470000;
	s_motor->calib_data.d_max = 2000000;
	s_motor->calib_data.calibrated = true;

	/* TODO: Get actual calibrated data from user at runtime.. */
}

uint8_t s_init(servo *s_motor)
{
	uint16_t ret = 0;

	s_motor->pwm_instance = libsoc_pwm_request(s_motor->chip, s_motor->channel, LS_GREEDY);
	if (s_motor->pwm_instance == NULL) {
		perror("PWM request failed");
		return EXIT_FAILURE;
	}

	ret = libsoc_pwm_set_period(s_motor->pwm_instance, s_motor->period);
	if (ret == EXIT_FAILURE) {
		perror("PWM set period failed");
		return EXIT_FAILURE;
	}

	libsoc_pwm_set_enabled(s_motor->pwm_instance, ENABLED);
	
	/* Servo s_motor position calibration */
	s_calib(s_motor);
	
	return 0;
}

void s_de_init(servo *s_motor)
{
	libsoc_pwm_set_enabled(s_motor->pwm_instance, DISABLED);
	libsoc_pwm_free(s_motor->pwm_instance);
}

void s_set_angle(servo *s_motor, uint8_t angle)
{
	uint32_t d_cycle;

	if (angle > 180)
		angle = 180;

	/* Dmin, Dmax are calibrated values of duty cycle for 0 and 180 degrees respectively */
	d_cycle = (((s_motor->calib_data.d_max - s_motor->calib_data.d_min) / 180) * angle) + s_motor->calib_data.d_min;
	libsoc_pwm_set_duty_cycle(s_motor->pwm_instance, d_cycle);
}
