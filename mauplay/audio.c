#include "mauplay.h"

#include <samplerate.h>
#include <stb_ds.h>

static void*	audio;
pthread_mutex_t audio_mutex;
queue_t*	queue = NULL;

static void handler(void* handle, void* user, void* data, int frames) {
	memset(data, 0, frames * 2 * 2);

	pthread_mutex_lock(&audio_mutex);
	if(arrlen(queue) > 0) {
		int	 from_frames = frames * queue[0].sfi.samplerate / MDEAudioRate;
		float*	 from	     = malloc(from_frames * sizeof(*from) * 2);
		float*	 to	     = malloc(frames * sizeof(*to) * 2);
		SRC_DATA d;
		int	 f;

		d.data_in	= from;
		d.data_out	= to;
		d.input_frames	= from_frames;
		d.output_frames = frames;
		d.src_ratio	= (double)MDEAudioRate / queue[0].sfi.samplerate;

		f = sf_readf_float(queue[0].sf, from, from_frames);
		src_simple(&d, SRC_SINC_BEST_QUALITY, 2);
		src_float_to_short_array(to, data, frames * 2);
		queue[0].frames += f;

		free(from);
		free(to);

		if(f < from_frames) {
			arrdel(queue, 0);
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
	pthread_mutex_unlock(&audio_mutex);
}
