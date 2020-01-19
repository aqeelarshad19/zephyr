/*
 *
 *
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>

/* http_server sample based headers */
#include <app_sensor.h>

void main(void)
{
	int ret;
	ret = app_sensor_init();
	if ( ret != 0 ){
		printf("error");
	}

	printf("init done");

	while (1) {
		app_sensor_loop();
	}
}
