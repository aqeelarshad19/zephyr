/*
 *
 *
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr.h>
#include <drivers/sensor.h>
#include <stdio.h>

K_SEM_DEFINE(sem, 0, 1);	/* starts off "not available" */

static struct sensor_value accel[3];
static struct sensor_value magn[3];
static struct sensor_value temp;
static struct device *dev;

static void trigger_handler(struct device *dev, struct sensor_trigger *trigger)
{
	ARG_UNUSED(trigger);

	printf("trigger");
	/* Always fetch the sample to clear the data ready interrupt in the
	 * sensor.
	 */
	if (sensor_sample_fetch(dev)) {
		return;
	}

	k_sem_give(&sem);
}

int app_sensor_init(void)
{
	int ret;
	dev = device_get_binding(DT_INST_0_NXP_FXOS8700_LABEL);

	if (dev == NULL) {
		return -ENODEV;
	}

	struct sensor_value attr = {
		.val1 = 6,
		.val2 = 250000,
	};

	ret = sensor_attr_set(dev, SENSOR_CHAN_ALL,
			SENSOR_ATTR_SAMPLING_FREQUENCY, &attr);
	if (ret != 0 ) {
		return ret;
	}

#ifdef CONFIG_FXOS8700_MOTION
	attr.val1 = 10;
	attr.val2 = 600000;
	ret = sensor_attr_set(dev, SENSOR_CHAN_ALL,
			    SENSOR_ATTR_SLOPE_TH, &attr);

	if (ret != 0 ) {
		return ret;
	}
#endif

	struct sensor_trigger trig = {
#ifdef CONFIG_FXOS8700_MOTION
		.type = SENSOR_TRIG_DELTA,
#else
		.type = SENSOR_TRIG_DATA_READY,
#endif
		.chan = SENSOR_CHAN_ACCEL_XYZ,
	};

	ret = sensor_trigger_set(dev, &trig, trigger_handler);
	if (ret != 0 ) {
		return ret;
	}
	return 0;
}

void app_sensor_loop(void)
{
	while (1) {
		k_sem_take(&sem, K_FOREVER);
		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
#if defined(CONFIG_FXOS8700_MODE_MAGN) || defined(CONFIG_FXOS8700_MODE_HYBRID)
		sensor_channel_get(dev, SENSOR_CHAN_MAGN_XYZ, magn);
#endif
#ifdef CONFIG_FXOS8700_TEMP
		sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP, &temp);
#endif
	}
}
