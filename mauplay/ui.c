#include "mauplay.h"

#include <stb_ds.h>

static void* play_queue;
static void* all_music;
static void* albums;
static void* artists;
static void* genres;
void*	     ui_last;
static int   changed = 0;

void ui_list_reset(const char* merge) {
	void* p;

	MwListBoxReset(list);

	p = MwListBoxCreatePacket();
	MwListBoxPacketInsert(p, -1);
	MwListBoxPacketSet(p, 0, 0, "#");
	if(merge != NULL) {
		MwListBoxPacketSet(p, 0, 1, merge);
		MwListBoxPacketSet(p, 0, 2, "Length");
	} else {
		MwListBoxPacketSet(p, 0, 1, "Title");
		MwListBoxPacketSet(p, 0, 2, "Artist");
		MwListBoxPacketSet(p, 0, 3, "Length");
	}

	MwListBoxInsert(list, -1, p);
	MwListBoxDestroyPacket(p);

	MwListBoxSetWidth(list, 0, 48);
	if(merge != NULL) {
		MwListBoxSetWidth(list, 1, -48 - 64);
		MwListBoxSetWidth(list, 2, 64);
	} else {
		MwListBoxSetWidth(list, 1, -48 - 128);
		MwListBoxSetWidth(list, 2, 64);
		MwListBoxSetWidth(list, 3, 64);
	}

	MwListBoxSetAlignment(list, 0, MwALIGNMENT_CENTER);
	MwListBoxSetAlignment(list, 2, MwALIGNMENT_END);
	MwListBoxSetAlignment(list, 3, MwALIGNMENT_END);
}

void ui_tree_reset(void) {
	void* t;

	MwTreeViewReset(tree);

	play_queue = MwTreeViewAdd(tree, NULL, NULL, "Play Queue (?)");

	t	  = MwTreeViewAdd(tree, NULL, NULL, "Library");
	all_music = MwTreeViewAdd(tree, t, NULL, "All Music (?)");
	albums	  = MwTreeViewAdd(tree, t, NULL, "Albums (?)");
	artists	  = MwTreeViewAdd(tree, t, NULL, "Artists (?)");
	genres	  = MwTreeViewAdd(tree, t, NULL, "Genres (?)");
}

void ui_set_play_queue(int v) {
	char buf[64];
	sprintf(buf, "Play Queue (%d)", v);
	MwTreeViewSetLabel(tree, play_queue, buf);
}

void ui_set_all_music(int v) {
	char buf[64];
	sprintf(buf, "All Music (%d)", v);
	MwTreeViewSetLabel(tree, all_music, buf);
}

void ui_set_albums(int v) {
	char buf[64];
	sprintf(buf, "Albums (%d)", v);
	MwTreeViewSetLabel(tree, albums, buf);
}

void ui_set_artists(int v) {
	char buf[64];
	sprintf(buf, "Artists (%d)", v);
	MwTreeViewSetLabel(tree, artists, buf);
}

void ui_set_genres(int v) {
	char buf[64];
	sprintf(buf, "Genres (%d)", v);
	MwTreeViewSetLabel(tree, genres, buf);
}

static void tree_activate(MwWidget handle, void* user, void* client) {
	if(client == play_queue || client == all_music || client == albums || client == artists || client == genres) {
		void* p = MwListBoxCreatePacket();
		int   i;
		if(client == play_queue) {
			pthread_mutex_lock(&audio_mutex);
			for(i = 0; i < arrlen(queue); i++) {
				char buf[64];
				int  ind   = shgeti(db_musics, queue[i].path);
				int  index = MwListBoxPacketInsert(p, -1);
				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, db_musics[ind].title);
				MwListBoxPacketSet(p, index, 2, db_musics[ind].artist);

				sprintf(buf, "%d:%02d", db_musics[ind].length / 60, db_musics[ind].length % 60);

				MwListBoxPacketSet(p, index, 3, buf);
			}
			pthread_mutex_unlock(&audio_mutex);
		}
		if(client == all_music) {
			for(i = 0; i < shlen(db_musics); i++) {
				char buf[64];
				int  index = MwListBoxPacketInsert(p, -1);
				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, db_musics[i].title);
				MwListBoxPacketSet(p, index, 2, db_musics[i].artist);

				sprintf(buf, "%d:%02d", db_musics[i].length / 60, db_musics[i].length % 60);

				MwListBoxPacketSet(p, index, 3, buf);
			}
		}
		if(client == albums) {
			for(i = 0; i < shlen(db_albums); i++) {
				char buf[64];
				int  index = MwListBoxPacketInsert(p, -1);
				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, db_albums[i].key);

				sprintf(buf, "%d:%02d", db_musics[i].length / 60, db_albums[i].length % 60);

				MwListBoxPacketSet(p, index, 2, buf);
			}
		}
		if(client == artists) {
			for(i = 0; i < shlen(db_artists); i++) {
				char buf[64];
				int  index = MwListBoxPacketInsert(p, -1);
				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, db_artists[i].key);

				sprintf(buf, "%d:%02d", db_artists[i].length / 60, db_artists[i].length % 60);

				MwListBoxPacketSet(p, index, 2, buf);
			}
		}
		if(client == genres) {
			for(i = 0; i < shlen(db_genres); i++) {
				char buf[64];
				int  index = MwListBoxPacketInsert(p, -1);
				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, db_genres[i].key);

				sprintf(buf, "%d:%02d", db_genres[i].length / 60, db_genres[i].length % 60);

				MwListBoxPacketSet(p, index, 2, buf);
			}
		}

		if(client == albums) {
			ui_list_reset("Album");
		} else if(client == artists) {
			ui_list_reset("Artist");
		} else if(client == genres) {
			ui_list_reset("Genre");
		} else {
			ui_list_reset(NULL);
		}
		MwListBoxInsert(list, -1, p);
		MwListBoxDestroyPacket(p);

		ui_last = client;
	}
}

static void list_activate(MwWidget handle, void* user, void* client) {
	if(ui_last == play_queue) {
		pthread_mutex_lock(&audio_mutex);
		queue_seek		 = (*(int*)client) - 1;
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
		pthread_mutex_unlock(&audio_mutex);
	} else if(ui_last == all_music) {
		audio_queue(db_musics[(*(int*)client) - 1].key);
	}
}

int queue_last = -1;

static void window_tick(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	if(queue_last != queue_seek && queue_seek != -1) {
		int   ind;
		char* buf;

		if(ui_last == play_queue) {
			pthread_mutex_unlock(&audio_mutex);
			MwDispatchUserHandler(tree, MwNactivateHandler, ui_last);
			pthread_mutex_lock(&audio_mutex);
		}

		ind = shgeti(db_musics, queue[queue_seek].path);
		buf = malloc(strlen(db_musics[ind].title) + 1 + strlen(db_musics[ind].artist) + 1);

		buf[0] = 0;
		strcpy(buf, db_musics[ind].title);
		strcat(buf, "\n");
		strcat(buf, db_musics[ind].artist);

		MwVaApply(info,
			  MwNtext, buf,
			  NULL);
		MwVaApply(seekbar,
			  MwNmaxValue, queue[queue_seek].sound->context->frames / queue[queue_seek].sound->context->sample_rate,
			  NULL);

		free(buf);
	} else if(queue_seek == -1) {
		MwVaApply(info,
			  MwNtext, "",
			  NULL);
	}
	if(arrlen(queue) > 0 && queue_seek != -1) {
		int  elsec = queue[queue_seek].frames / queue[queue_seek].sound->context->sample_rate;
		int  rmsec = (queue[queue_seek].sound->context->frames / queue[queue_seek].sound->context->sample_rate) - elsec;
		char buf[64];

		sprintf(buf, "%d:%02d", elsec / 60, elsec % 60);
		MwVaApply(eltime,
			  MwNtext, buf,
			  NULL);

		sprintf(buf, "%d:%02d", rmsec / 60, rmsec % 60);
		MwVaApply(rmtime,
			  MwNtext, buf,
			  NULL);

		if(changed) {
			MDESoundSeek(queue[queue_seek].sound, (queue[queue_seek].frames = MwGetInteger(seekbar, MwNvalue) * queue[queue_seek].sound->context->sample_rate));
			changed = 0;
		} else {
			MwVaApply(seekbar,
				  MwNvalue, elsec,
				  NULL);
		}
	} else {
		MwVaApply(eltime,
			  MwNtext, "0:00",
			  NULL);
		MwVaApply(rmtime,
			  MwNtext, "0:00",
			  NULL);
	}
	queue_last = queue_seek;
	pthread_mutex_unlock(&audio_mutex);
}

static void seekbar_changed(MwWidget handle, void* user, void* client) {
	changed = 1;
}

static void bskipback_activate(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	if(queue_seek == -1) {
		queue_seek = 0;
	} else {
		if(queue_seek > 0) queue_seek--;
	}
	if(arrlen(queue) > 0) {
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
	} else {
		queue_seek = -1;
	}
	pthread_mutex_unlock(&audio_mutex);
}

static void bplay_activate(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	paused = 0;
	pthread_mutex_unlock(&audio_mutex);
}

static void bpause_activate(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	paused = 1;
	pthread_mutex_unlock(&audio_mutex);
}

static void bstop_activate(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	if(queue_seek != -1) {
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
	}
	queue_seek = -1;
	pthread_mutex_unlock(&audio_mutex);
}

static void bskipfwd_activate(MwWidget handle, void* user, void* client) {
	pthread_mutex_lock(&audio_mutex);
	if(queue_seek == -1) {
		queue_seek = 0;
	} else {
		if(queue_seek < (arrlen(queue) - 1)) queue_seek++;
	}
	if(arrlen(queue) > 0) {
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
	} else {
		queue_seek = -1;
	}
	pthread_mutex_unlock(&audio_mutex);
}

void ui_init(void) {
	ui_list_reset(NULL);
	ui_tree_reset();

	ui_set_play_queue(0);
	ui_set_all_music(0);
	ui_set_albums(0);
	ui_set_artists(0);
	ui_set_genres(0);

	ui_last = all_music;
	MwAddUserHandler(list, MwNactivateHandler, list_activate, NULL);
	MwAddUserHandler(tree, MwNactivateHandler, tree_activate, NULL);
	MwAddUserHandler(window, MwNtickHandler, window_tick, NULL);
	MwAddUserHandler(seekbar, MwNchangedHandler, seekbar_changed, NULL);

	MwAddUserHandler(bskipback, MwNactivateHandler, bskipback_activate, NULL);
	MwAddUserHandler(bplay, MwNactivateHandler, bplay_activate, NULL);
	MwAddUserHandler(bpause, MwNactivateHandler, bpause_activate, NULL);
	MwAddUserHandler(bstop, MwNactivateHandler, bstop_activate, NULL);
	MwAddUserHandler(bskipfwd, MwNactivateHandler, bskipfwd_activate, NULL);
}
