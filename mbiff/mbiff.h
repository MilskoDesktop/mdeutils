#ifndef __MBIFF_H__
#define __MBIFF_H__

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

#ifdef HAS_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_MAILDIR
#define _PATH_MAILDIR "/var/spool/mail"
#endif

/* main.c */
extern MwWidget	  window, image;
extern MwLLPixmap empty, full;

/* mail.c */
void init_mail(void);
void check_mail(void);

#endif
