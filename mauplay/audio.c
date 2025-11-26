#include "mauplay.h"

#include <samplerate.h>
#include <stb_ds.h>

static MDEAudio audio;
MDEMutex	audio_mutex;
queue_t*	queue	   = NULL;
int		queue_seek = -1;
int		paused	   = 0;
int		repeated   = 0;

static void handler(MDEAudio handle, void* user, void* data, int frames) {
	memset(data, 0, frames * 2 * 2);

	MDEMutexLock(audio_mutex);
	if(queue_seek != -1 && !paused) {
		int	 from_frames = frames * queue[queue_seek].sound->context->sample_rate / MDEAudioRate;
		float*	 from	     = malloc(from_frames * sizeof(*from) * 2);
		float*	 to	     = malloc(frames * sizeof(*to) * 2);
		SRC_DATA d;
		int	 f;

		d.data_in	= from;
		d.data_out	= to;
		d.input_frames	= from_frames;
		d.output_frames = frames;
		d.src_ratio	= (double)MDEAudioRate / queue[queue_seek].sound->context->sample_rate;

		f = MDESoundReadFloat(queue[queue_seek].sound, from, from_frames);
		src_simple(&d, SRC_SINC_BEST_QUALITY, 2);
		src_float_to_short_array(to, data, frames * 2);
		queue[queue_seek].frames += f;

		free(from);
		free(to);

		if(f < from_frames) {
			if(repeated) {
				queue[queue_seek].frames = 0;
				MDESoundSeek(queue[queue_seek].sound, 0);
			} else {
				queue_seek++;
				if(queue_seek >= arrlen(queue)) {
					queue_seek = -1;
				}
			}
		}
	}
	MDEMutexUnlock(audio_mutex);
}

void audio_init(void) {
	audio = MDEAudioOpen(handler, NULL);

	audio_mutex = MDEMutexCreate();

	MDEAudioStart(audio);
}

void audio_queue(const char* path) {
	queue_t q;
	q.path	 = MDEFileAbsolutePath(path);
	q.sound	 = MDESoundOpen(q.path);
	q.frames = 0;

	if(q.sound == NULL) {
		free(q.path);
		return;
	}

	MDEMutexLock(audio_mutex);
	arrput(queue, q);
	if(queue_seek == -1) queue_seek = arrlen(queue) - 1;

	ui_set_play_queue(arrlen(queue));
	MDEMutexUnlock(audio_mutex);
}
