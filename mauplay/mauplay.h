#ifndef __MAUPLAY_H__
#define __MAUPLAY_H__

#include "config.h"

/* Milsko */
#include <MDE/Foundation.h>
#include <Mw/Milsko.h>

/* External */
#include <sndfile.h>

/* Standard */
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct numkv {
	char* key;
	int   value;

	char* title;
	char* album;
	char* artist;
	char* genre;
	int   length;
} numkv_t;

typedef struct queue {
	char*	 path;
	SNDFILE* sf;
	SF_INFO	 sfi;
	int	 frames;
} queue_t;

/* main.c */
extern MwWidget	  window;
extern MwWidget	  album, info;
extern MwWidget	  bskipback, bplay, bpause, bstop, bskipfwd, brepeat, bshuffle;
extern MwWidget	  eltime, seekbar, rmtime;
extern MwWidget	  tree, list;
extern MwLLPixmap pxalbum;

/* db.c */
extern numkv_t* db_musics;
extern numkv_t* db_albums;
extern numkv_t* db_artists;
extern numkv_t* db_genres;

int  db_exec(const char* s);
int  db_init(void);
void db_add(const char* path);
void db_scan(void);

/* ui.c */
extern void* ui_last;

void ui_list_reset(const char* merge);
void ui_tree_reset(void);
void ui_init(void);
void ui_set_play_queue(int v);
void ui_set_all_music(int v);
void ui_set_albums(int v);
void ui_set_artists(int v);
void ui_set_genres(int v);

/* audio.c */
extern pthread_mutex_t audio_mutex;
extern queue_t*	       queue;

void audio_init(void);
void audio_queue(const char* path);

#endif
