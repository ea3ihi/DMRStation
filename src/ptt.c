


#include "main.h"
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <stdio.h>

struct gpiohandle_request pttreq;
struct gpiohandle_data pttdata;
char chrdev_name[20];

extern AppSettingsStruct_t settings;
extern dmr_control_struct_t dmr_control;
extern GtkWidget *buttonPTT;

int fd, ret;

bool pttUsed = false;

void ptt_init(void)
{
	g_snprintf(chrdev_name, 20, "%s%s", "/dev/", settings.pttBank);

	fd = open(chrdev_name, 0);
	if (fd == -1)
	{
		g_printf("Failed to open %s\n", chrdev_name);
	}

	pttreq.lineoffsets[0] = settings.pttPort;
	pttreq.flags = GPIOHANDLE_REQUEST_INPUT;

	memcpy(pttreq.default_values, &pttdata, sizeof(pttreq.default_values));
	strcpy(pttreq.consumer_label, "ptt_gpio");
	pttreq.lines  = 1;

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &pttreq);
	if (ret == -1)
	{
		g_printf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
	}

	if (close(fd) == -1)
	{
		g_printf("Failed to close GPIO character device file\n");
	}

}

void ptt_deinit(void)
{
	ret = close(pttreq.fd);
	if (ret == -1)
	{
		g_printf("Failed to close GPIO LINEHANDLE device file\n");

	}
}


void ptt_tick(void)
{

	ret = ioctl(pttreq.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &pttdata);

	uint8_t pttStatus = pttdata.values[0];

	pttStatus ^= settings.pttInvert;

	if (pttStatus == 1)
	{
		if (dmr_control.dmr_status != DMR_STATUS_TX)
		{
			gtk_toggle_button_set_active ( (GtkToggleButton *) buttonPTT,
												  TRUE);
			pttUsed = true;
		}
	}
	else
	{
		if (dmr_control.dmr_status == DMR_STATUS_TX && pttUsed == true)
		{
			gtk_toggle_button_set_active ( (GtkToggleButton *) buttonPTT,
												  FALSE);
			pttUsed = false;
		}
	}
}
