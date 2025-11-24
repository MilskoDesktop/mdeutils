#ifndef __MAUPLAY_H__
#define __MAUPLAY_H__

#define VERSION "1.0"

#include "config.h"

/* Milsko */
#include <MDE/Foundation.h>
#include <Mw/Milsko.h>

/* External */

/* Standard */
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ALBUMWIDTH 128

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
	MDESound sound;
	int	 frames;
} queue_t;

/* main.c */
extern MwWidget	      window, menu;
extern MwWidget	      album, info, timelabel;
extern MwWidget	      bskipback, bplay, bpause, bstop, bskipfwd, brepeat, bshuffle;
extern MwWidget	      seekbar;
extern MwWidget	      tree, list;
extern unsigned char* pxalbumdata;
extern MwLLPixmap     pxalbum, pxrepeat, pxrepeatsong;
extern MwLLPixmap     pxoptical, pxfoldermusic, pxartist, pxgenre, pxradio;

/* db.c */
extern numkv_t* db_musics;
extern numkv_t* db_albums;
extern numkv_t* db_artists;
extern numkv_t* db_genres;

int    db_exec(const char* s);
int    db_init(void);
void   db_add(const char* path);
void   db_scan(void);
char** db_find(const char* query, const char* param, const char* additional);

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
extern int	       queue_seek;
extern int	       paused;
extern int	       repeated;

void audio_init(void);
void audio_queue(const char* path);

/* id3.c */
unsigned char* id3_findimage(const char* path, size_t* size);

/* playlist.c */
void playlist_add(const char* path);

#endif
