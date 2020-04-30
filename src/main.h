

#ifndef _MAIN_H_
#define _MAIN_H_

#include <gtk/gtk.h>


#include <glib.h>
#include <glib/gutils.h>
#include <glib/gprintf.h>
#include <gio/gio.h>
#include <gio/gunixsocketaddress.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <openssl/sha.h>

#include "dmr.h"
#include "net.h"
#include "config.h"
#include "fw_mbelib.h"
#include "audio.h"
#include "ambe_client.h"
#include "lastheard.h"
#include "talkgroup.h"
#include "dmrids.h"
#include "ptt.h"

void ui_dmr_start(uint32_t src, uint32_t dst, uint8_t type);
void ui_dmr_stop(uint32_t src, uint32_t dst, uint8_t type);
void ui_set_tg(uint32_t tg);
void ui_net_connection(uint8_t status);
void init_CSS(void);
void tickTOT(void);

#endif
