#include "mauplay.h"

#include <samplerate.h>
#include <stb_ds.h>

static void*	audio;
pthread_mutex_t audio_mutex;
queue_t*	queue	   = NULL;
int		queue_seek = -1;
int		paused	   = 0;

static void handler(void* handle, void* user, void* data, int frames) {
	memset(data, 0, frames * 2 * 2);

	pthread_mutex_lock(&audio_mutex);
	if(queue_seek != -1 && !paused) {
		int	 from_frames = frames * queue[queue_seek].sfi.samplerate / MDEAudioRate;
		float*	 from	     = malloc(from_frames * sizeof(*from) * 2);
		float*	 to	     = malloc(frames * sizeof(*to) * 2);
		SRC_DATA d;
		int	 f;

		d.data_in	= from;
		d.data_out	= to;
		d.input_frames	= from_frames;
		d.output_frames = frames;
		d.src_ratio	= (double)MDEAudioRate / queue[queue_seek].sfi.samplerate;

		f = sf_readf_float(queue[queue_seek].sf, from, from_frames);
		src_simple(&d, SRC_SINC_BEST_QUALITY, 2);
		src_float_to_short_array(to, data, frames * 2);
		queue[queue_seek].frames += f;

		free(from);
		free(to);

		if(f < from_frames) {
			queue_seek++;
			if(queue_seek >= arrlen(queue)) {
				queue_seek = -1;
			}
		}
	}
	pthread_mutex_unlock(&audio_mutex);
}

void audio_init(void) {
	audio = MDEAudioOpen(handler, NULL);

	pthread_mutex_init(&audio_mutex, NULL);

	MDEAudioStart(audio);
}

void audio_queue(const char* path) {
	queue_t q;
	q.path	 = MDEFileAbsolutePath(path);
	q.sf	 = sf_open(q.path, SFM_READ, &q.sfi);
	q.frames = 0;

	pthread_mutex_lock(&audio_mutex);
	arrput(queue, q);
	if(queue_seek == -1) queue_seek = arrlen(queue) - 1;

	ui_set_play_queue(arrlen(queue));
	pthread_mutex_unlock(&audio_mutex);
}
