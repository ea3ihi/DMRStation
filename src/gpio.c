


#include "main.h"
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <stdio.h>

struct gpiohandle_request gpioreq;
struct gpiohandle_data gpiodata;
char chrdev_name[20];

extern AppSettingsStruct_t settings;

int fd, ret;


void gpio_init(void)
{
	g_snprintf(chrdev_name, 20, "%s%s", "/dev/", settings.gpioBank);

	fd = open(chrdev_name, 0);
	if (fd == -1)
	{
		g_printf("Failed to open %s\n", chrdev_name);
	}

	gpioreq.lineoffsets[0] = settings.gpioConnectedPort;
	gpioreq.flags = GPIOHANDLE_REQUEST_OUTPUT;

	memcpy(gpioreq.default_values, &gpiodata, sizeof(gpioreq.default_values));
	strcpy(gpioreq.consumer_label, "connected_gpio_out");
	gpioreq.lines  = 1;

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &gpioreq);
	if (ret == -1)
	{
		g_printf("Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
	}

	if (close(fd) == -1)
	{
		g_printf("Failed to close GPIO character device file\n");
	}

}

void gpio_deinit(void)
{
	ret = close(gpioreq.fd);
	if (ret == -1)
	{
		g_printf("Failed to close GPIO LINEHANDLE device file\n");

	}
}


void gpio_connected(int v)
{
	if (settings.gpioEnabled == 1)
	{
		gpiodata.values[0] = v;
		ret = ioctl(gpioreq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &gpiodata);
		if (ret == -1) {
			g_printf("Error setting connected gpio");
		}
	}
}


