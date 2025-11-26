#include "mauplay.h"

#include <stb_ds.h>
#include <stb_image.h>

static void* play_queue;
static void* all_music;
static void* albums;
static void* artists;
static void* genres;
void*	     ui_last;
static int   changed = 0;

static void* file_add_file;
static void* file_add_directory;
static void* file_add_playlist;

static void* help_about;

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
		MwListBoxSetWidth(list, 1, -48 - 96 - 64);
		MwListBoxSetWidth(list, 2, 96);
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

static char*  current	   = NULL;
static char** current_list = NULL;
static void   tree_activate(MwWidget handle, void* user, void* client) {
	  if(client == play_queue || client == all_music || client == albums || client == artists || client == genres) {
		  void* p = MwListBoxCreatePacket();
		  int	i;
		  if(current != NULL) free(current);
		  current = NULL;
		  if(client == play_queue) {
			  MDEMutexLock(audio_mutex);
			  for(i = 0; i < arrlen(queue); i++) {
				  char buf[64];
				  int  ind   = shgeti(db_musics, queue[i].path);
				  int  index = MwListBoxPacketInsert(p, -1);
				  sprintf(buf, "%d", i + 1);

				  MwListBoxPacketSetIcon(p, index, pxoptical);
				  MwListBoxPacketSet(p, index, 0, buf);
				  MwListBoxPacketSet(p, index, 1, db_musics[ind].title);
				  MwListBoxPacketSet(p, index, 2, db_musics[ind].artist);

				  sprintf(buf, "%d:%02d", db_musics[ind].length / 60, db_musics[ind].length % 60);

				  MwListBoxPacketSet(p, index, 3, buf);
			  }
			  MDEMutexUnlock(audio_mutex);
		  }
		  if(client == all_music) {
			  for(i = 0; i < shlen(db_musics); i++) {
				  char buf[64];
				  int  index = MwListBoxPacketInsert(p, -1);
				  sprintf(buf, "%d", i + 1);

				  MwListBoxPacketSetIcon(p, index, pxoptical);
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

				  MwListBoxPacketSetIcon(p, index, pxfoldermusic);
				  MwListBoxPacketSet(p, index, 0, buf);
				  MwListBoxPacketSet(p, index, 1, db_albums[i].key);
				  sprintf(buf, "%d:%02d", db_albums[i].length / 60, db_albums[i].length % 60);

				  MwListBoxPacketSet(p, index, 2, buf);
			  }
		  }
		  if(client == artists) {
			  for(i = 0; i < shlen(db_artists); i++) {
				  char buf[64];
				  int  index = MwListBoxPacketInsert(p, -1);
				  sprintf(buf, "%d", i + 1);

				  MwListBoxPacketSetIcon(p, index, pxartist);
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

				  MwListBoxPacketSetIcon(p, index, pxgenre);
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
		MDEMutexLock(audio_mutex);
		if(queue_seek != -1) {
			queue[queue_seek].frames = 0;
			MDESoundSeek(queue[queue_seek].sound, 0);
		}
		queue_seek		 = (*(int*)client) - 1;
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
		MDEMutexUnlock(audio_mutex);
	} else if(ui_last == all_music) {
		audio_queue(db_musics[(*(int*)client) - 1].key);
	} else if(ui_last == albums || ui_last == artists || ui_last == genres) {
		numkv_t*    kv = NULL;
		const char* n;
		if(ui_last == albums) {
			kv = db_albums;
			n  = "album";
		}
		if(ui_last == artists) {
			kv = db_artists;
			n  = "artist";
		}
		if(ui_last == genres) {
			kv = db_genres;
			n  = "genre";
		}

		if(current == NULL) {
			void* p = MwListBoxCreatePacket();
			int   i;
			int   index;

			current = MDEStringDuplicate(kv[(*(int*)client) - 1].key);

			for(i = 0; i < arrlen(current_list); i++) {
				free(current_list[i]);
			}
			arrfree(current_list);

			index = MwListBoxPacketInsert(p, -1);

			MwListBoxPacketSetIcon(p, index, pxfoldermusic);
			MwListBoxPacketSet(p, index, 0, "");
			MwListBoxPacketSet(p, index, 1, "..");

			current_list = db_find(n, current, ui_last == albums ? "track" : NULL);
			for(i = 0; i < arrlen(current_list); i++) {
				char	buf[128];
				numkv_t kv = shgets(db_musics, current_list[i]);

				index = MwListBoxPacketInsert(p, -1);

				sprintf(buf, "%d", i + 1);

				MwListBoxPacketSetIcon(p, index, pxoptical);
				MwListBoxPacketSet(p, index, 0, buf);
				MwListBoxPacketSet(p, index, 1, kv.title);
				MwListBoxPacketSet(p, index, 2, kv.artist);

				sprintf(buf, "%d:%02d", kv.length / 60, kv.length % 60);

				MwListBoxPacketSet(p, index, 3, buf);
			}

			ui_list_reset(NULL);
			MwListBoxInsert(handle, -1, p);
			MwListBoxDestroyPacket(p);
		} else {
			int index = *(int*)client;
			if(index == 1) {
				MwDispatchUserHandler(tree, MwNactivateHandler, ui_last);
			} else {
				audio_queue(current_list[index - 2]);
			}
		}
	}
}

int queue_last = -1;

static void window_tick(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
	if(queue_last != queue_seek && queue_seek != -1) {
		int	       ind;
		char*	       buf;
		size_t	       sz;
		unsigned char* img;

		img = id3_findimage(queue[queue_seek].path, &sz);
		if(img != NULL) {
			int	       iw, ih, ic;
			unsigned char* data = stbi_load_from_memory(img, sz, &iw, &ih, &ic, 4);
			if(data != NULL) {
				int y, x;
				for(y = 0; y < ALBUMWIDTH; y++) {
					for(x = 0; x < ALBUMWIDTH; x++) {
						int	       ix  = x * iw / ALBUMWIDTH;
						int	       iy  = y * ih / ALBUMWIDTH;
						unsigned char* ipx = &data[(iy * iw + ix) * 4];
						unsigned char* opx = &((MwLLCommonPixmap)pxalbum)->raw[(y * ALBUMWIDTH + x) * 4];

						memcpy(opx, ipx, 4);
					}
				}
				free(data);
			} else {
				memset(((MwLLCommonPixmap)pxalbum)->raw, 0, ALBUMWIDTH * ALBUMWIDTH * 4);
			}

			free(img);
		} else {
			memset(((MwLLCommonPixmap)pxalbum)->raw, 0, ALBUMWIDTH * ALBUMWIDTH * 4);
		}
		MwLLPixmapUpdate(pxalbum);
		MwForceRender(album);

		if(ui_last == play_queue) {
			MDEMutexUnlock(audio_mutex);
			MwDispatchUserHandler(tree, MwNactivateHandler, ui_last);
			MDEMutexLock(audio_mutex);
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
		int  ttsec = queue[queue_seek].sound->context->frames / queue[queue_seek].sound->context->sample_rate;
		char buf[64];

		sprintf(buf, "%d:%02d | %d:%02d", elsec / 60, elsec % 60, ttsec / 60, ttsec % 60);
		MwVaApply(timelabel,
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
		MwVaApply(timelabel,
			  MwNtext, "",
			  NULL);
		memset(((MwLLCommonPixmap)pxalbum)->raw, 0, ALBUMWIDTH * ALBUMWIDTH * 4);
		MwLLPixmapUpdate(pxalbum);
		MwForceRender(album);
	}
	queue_last = queue_seek;
	MDEMutexUnlock(audio_mutex);
}

static void seekbar_changed(MwWidget handle, void* user, void* client) {
	changed = 1;
}

static void bskipback_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
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
	MDEMutexUnlock(audio_mutex);
}

static void bplay_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
	paused = 0;
	MDEMutexUnlock(audio_mutex);
}

static void bpause_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
	paused = 1;
	MDEMutexUnlock(audio_mutex);
}

static void bstop_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
	if(queue_seek != -1) {
		queue[queue_seek].frames = 0;
		MDESoundSeek(queue[queue_seek].sound, 0);
	}
	queue_seek = -1;
	MDEMutexUnlock(audio_mutex);
}

static void bskipfwd_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
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
	MDEMutexUnlock(audio_mutex);
}

static void brepeat_activate(MwWidget handle, void* user, void* client) {
	MDEMutexLock(audio_mutex);
	repeated = repeated ? 0 : 1;
	MwVaApply(brepeat,
		  MwNpixmap, repeated ? pxrepeatsong : pxrepeat,
		  NULL);
	MDEMutexUnlock(audio_mutex);
}

static void file_fc_chosen(MwWidget handle, void* user, void* client) {
	db_add(client);
	db_scan();
}

static void file_dc_scan(const char* path, int dir, int symlink, void* uesr) {
	if(dir) {
		MDEDirectoryScan(path, file_dc_scan, NULL);
	} else {
		db_add(path);
	}
}

static void file_dc_chosen(MwWidget handle, void* user, void* client) {
	MDEDirectoryScan(client, file_dc_scan, NULL);
	db_scan();
}

static void playlist_fc_chosen(MwWidget handle, void* user, void* client) {
	playlist_add(client);
}

static void menu_menu(MwWidget handle, void* user, void* client) {
	if(client == file_add_file) {
		MwWidget fc = MwFileChooser(window, "Choose a file to add");
		MwAddUserHandler(fc, MwNfileChosenHandler, file_fc_chosen, NULL);
	} else if(client == file_add_directory) {
		MwWidget dc = MwDirectoryChooser(window, "Choose a directory to add");
		MwAddUserHandler(dc, MwNdirectoryChosenHandler, file_dc_chosen, NULL);
	} else if(client == file_add_playlist) {
		MwWidget fc = MwFileChooser(window, "Choose a playlist to add");
		MwAddUserHandler(fc, MwNfileChosenHandler, playlist_fc_chosen, NULL);
	} else if(client == help_about) {
		MDEAboutDialog(window, "mauplay", VERSION, pxradio);
	}
}

static void list_mouseup(MwWidget handle, void* user, void* client) {
	MwLLMouse* m = client;
	int	   s;
	if(ui_last != play_queue) return;
	if(m->button != MwLLMouseRight) return;
	if((s = MwGetInteger(list, MwNvalue)) == -1) return;

	MDEMutexLock(audio_mutex);
	MwListBoxDelete(list, s);
	arrdel(queue, s - 1);
	ui_set_play_queue(arrlen(queue));

	if(queue_last == (s - 1)) {
		queue_last = -1;
	} else if(queue_last > (s - 1)) {
		queue_last--;
		queue_seek--;
	}
	if(arrlen(queue) <= queue_seek) {
		queue_seek = arrlen(queue) - 1;
	}
	MDEMutexUnlock(audio_mutex);
}

void ui_init(void) {
	MwMenu	  m;
	int	  i;
	MwWidget* l;

	m		   = MwMenuAdd(menu, NULL, "File");
	file_add_file	   = MwMenuAdd(menu, m, "Add File");
	file_add_directory = MwMenuAdd(menu, m, "Add Directory");
	file_add_playlist  = MwMenuAdd(menu, m, "Add Playlist");

	m	   = MwMenuAdd(menu, NULL, "?Help");
	help_about = MwMenuAdd(menu, m, "About");

	ui_list_reset(NULL);
	ui_tree_reset();

	ui_set_play_queue(0);
	ui_set_all_music(0);
	ui_set_albums(0);
	ui_set_artists(0);
	ui_set_genres(0);

	MwAddUserHandler(menu, MwNmenuHandler, menu_menu, NULL);

	ui_last = all_music;
	l	= MwGetChildren(list);
	for(i = 0; l[i] != NULL; i++) {
		if(MwGetClass(l[i]) == MwFrameClass) MwAddUserHandler(l[i], MwNmouseUpHandler, list_mouseup, NULL);
	}
	free(l);
	MwAddUserHandler(list, MwNactivateHandler, list_activate, NULL);
	MwAddUserHandler(tree, MwNactivateHandler, tree_activate, NULL);
	MwAddUserHandler(window, MwNtickHandler, window_tick, NULL);
	MwAddUserHandler(seekbar, MwNchangedHandler, seekbar_changed, NULL);

	MwAddUserHandler(bskipback, MwNactivateHandler, bskipback_activate, NULL);
	MwAddUserHandler(bplay, MwNactivateHandler, bplay_activate, NULL);
	MwAddUserHandler(bpause, MwNactivateHandler, bpause_activate, NULL);
	MwAddUserHandler(bstop, MwNactivateHandler, bstop_activate, NULL);
	MwAddUserHandler(bskipfwd, MwNactivateHandler, bskipfwd_activate, NULL);

	MwAddUserHandler(brepeat, MwNactivateHandler, brepeat_activate, NULL);
}
