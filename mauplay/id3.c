#include "mauplay.h"

static unsigned int syncsafe(FILE* f) {
	unsigned char d[4];
	unsigned int  r = 0;
	int	      i;
	fread(d, 4, 1, f);

	for(i = 0; i < 4; i++) {
		r = r << 7;
		r = r | (d[i] & 127);
	}

	return r;
}

static unsigned int big(FILE* f) {
	unsigned char d[4];
	unsigned int  r = 0;
	int	      i;
	fread(d, 4, 1, f);

	for(i = 0; i < 4; i++) {
		r = r << 8;
		r = r | d[i];
	}

	return r;
}

unsigned char* id3_findimage(const char* path, size_t* size) {
	FILE*	       f = fopen(path, "rb");
	unsigned char* d = NULL;
	char	       sig[3];
	unsigned char  ver[2];
	unsigned char  flag;
	unsigned char  frame[4];
	unsigned int   dsz = 0;
	if(f == NULL) return NULL;

	fread(sig, 3, 1, f);
	if(memcmp(sig, "ID3", 3) != 0) return NULL;

	fread(ver, 2, 1, f);
	if(memcmp(ver, "\x04\x00", 2) != 0 && memcmp(ver, "\x03\x00", 2) != 0) return NULL;

	fread(&flag, 1, 1, f);
	if(flag & (1 << 6)) return NULL; /* FIXME */
	if(flag & (1 << 7)) return NULL; /* what even is this??? */

	dsz = syncsafe(f);

	while(dsz > 0) {
		unsigned int fsz;

		fread(frame, 4, 1, f);
		if(frame[0] == 0) break; /* XXX: Is this ok? */

		if(memcmp(ver, "\x04\x00", 2) == 0) {
			fsz = syncsafe(f);
		} else {
			fsz = big(f);
		}

		fseek(f, 2, SEEK_CUR);
		if(memcmp(frame, "APIC", 4) == 0) {
			unsigned char* buffer = malloc(fsz);
			int	       i;
			fread(buffer, fsz, 1, f);
			for(i = 1; i < fsz; i++) {
				if(buffer[i] == 0) break;
			}
			i++;
			/* XXX: 0 is technically Other - am not sure if this should be done... */
			if(buffer[i] == 0 || buffer[i] == 3) {
				int count = (buffer[0] == 1 || buffer[0] == 2) ? 2 : 1;

				i++;

				for(; i < fsz; i++) {
					if(buffer[i] == 0) count--;
					if(count == 0) break;
				}
				i++;
				d = malloc(fsz - i);
				memcpy(d, buffer + i, fsz - i);
				*size = fsz - i;
			}
			free(buffer);
			if(d != NULL) break;
		} else {
			fseek(f, fsz, SEEK_CUR);
		}

		dsz -= 4 + 4 + 2 + fsz;
	}

	fclose(f);

	return d;
}
