/* 
 * servo.h
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <libsoc_pwm.h>

struct s_calib {
	bool calibrated;			/* Calibration status */
	uint32_t d_min;				/* Value in nano sec */
	uint32_t d_max;				/* Value in nano sec */
};

typedef struct {
	pwm *pwm_instance;			/* libsoc pwm instance */
	uint32_t period;			/* Servo motor operating freq */
	uint16_t chip;				/* PWM Chip instance */	
	uint16_t channel;			/* PWM Channel number */
	struct s_calib calib_data;	/* Position calibrated values */
} servo;

uint8_t s_init(servo *s_motor);
void s_de_init(servo *s_motor);
void s_set_angle(servo *s_motor, uint8_t angle);
void s_calib(servo *s_motor);
