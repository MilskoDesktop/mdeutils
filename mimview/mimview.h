#ifndef __MIMVIEW_H__
#define __MIMVIEW_H__

#define VERSION "0.0.0"

#include "config.h"

/* Milsko */
#include <MDE/Foundation.h>
#include <Mw/Milsko.h>

/* External */

/* Standard */
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

/* main.c */
extern MwWidget	  window, menu;
extern MwLLPixmap pxprojector;

/* ui.c */
void ui_init(void);

#endif
