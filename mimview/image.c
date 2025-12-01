#include "mimview.h"

#include <stb_image.h>
#include <stb_ds.h>

image_t*	      images = NULL;
static int	      oldw = 0, oldh = 0;
static unsigned char* buffer = NULL;

void image_init(void) {
	unsigned char* d = ((MwLLCommonPixmap)pxdata)->raw;
	int	       w = ((MwLLCommonPixmap)pxdata)->width;
	int	       h = ((MwLLCommonPixmap)pxdata)->height;
	if(oldw != w || oldh != h) {
		int y, x;
		if(buffer != NULL) free(buffer);
		for(y = 0; y < h; y++) {
			for(x = 0; x < w; x++) {
				unsigned char* px = &d[(y * w + x) * 4];
				if(((y / 16) + (x / 16)) % 2) {
					px[0] = px[1] = px[2] = 128;
				} else {
					px[0] = px[1] = px[2] = 64;
				}
				px[3] = 255;
			}
		}
		buffer = malloc(w * h * 4);
		memcpy(buffer, d, w * h * 4);
	} else {
		memcpy(d, buffer, w * h * 4);
	}
	oldw = w;
	oldh = h;

	MwLLPixmapUpdate(pxdata);
	MwForceRender(image);
}

void image_add(const char* path) {
	image_t img;
	img.path   = MDEStringDuplicate(path);
	img.data   = NULL;
	img.width  = 0;
	img.height = 0;
	img.x	   = 0;
	img.y	   = 0;
	img.scale  = 0;
	arrput(images, img);
}

void image_render(void) {
	if(current < arrlen(images) && images[current].data == NULL) {
		int	       w, h, ch;
		unsigned char* d;
	again:;
		d = stbi_load(images[current].path, &w, &h, &ch, 4);
		if(d != NULL) {
			int    pw = ((MwLLCommonPixmap)pxdata)->width;
			int    ph = ((MwLLCommonPixmap)pxdata)->height;
			double sw = (double)w / pw;
			double sh = (double)h / ph;

			images[current].data   = d;
			images[current].width  = w;
			images[current].height = h;
			images[current].scale  = sw > sh ? sw : sh;
		} else {
			free(images[current].path);
			arrdel(images, current);
			if(arrlen(images) <= current) {
				current = 0;
			}
			goto again;
		}
	}

	if(current < arrlen(images)) {
		unsigned char* d = ((MwLLCommonPixmap)pxdata)->raw;
		int	       y;
		int	       x;
		int	       w  = ((MwLLCommonPixmap)pxdata)->width;
		int	       h  = ((MwLLCommonPixmap)pxdata)->height;
		int	       ox = (w - images[current].width / images[current].scale) / 2 - images[current].x / images[current].scale;
		int	       oy = (h - images[current].height / images[current].scale) / 2 - images[current].y / images[current].scale;
		char	       str[128];

		sprintf(str, "mimview [%d of %d] %s", current + 1, arrlen(images), images[current].path);

		MwVaApply(window,
			  MwNtitle, str,
			  NULL);

		for(y = (oy < 0 ? 0 : 0); y < h + (oy < 0 ? -oy : -oy * 2); y++) {
			for(x = (ox < 0 ? 0 : 0); x < w + (ox < 0 ? -ox : -ox * 2); x++) {
				if(0 <= (y + oy) && (y + oy) < h && 0 <= (x + ox) && (x + ox) < w) {
					int	       ix = x * images[current].scale;
					int	       iy = y * images[current].scale;
					unsigned char* opx;
					unsigned char* ipx;

					if(0 <= iy && iy < images[current].height && 0 <= ix && ix < images[current].width) {
						int i;
						opx = &d[((y + oy) * w + x + ox) * 4];
						ipx = &images[current].data[(iy * images[current].width + ix) * 4];

						for(i = 0; i < 3; i++) {
							opx[i] *= (255 - ipx[3]) / 255.0;
							opx[i] += ipx[i] * (ipx[3] / 255.0);
						}
					}
				}
			}
		}
		MwLLPixmapUpdate(pxdata);
		MwForceRender(image);
	}
}
