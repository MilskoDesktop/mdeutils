#include "mbiff.h"

static time_t last = 0;
static char* mailpath;

void init_mail(void){
	struct stat s;
	struct passwd* pwd = getpwuid(getuid());

	if(getenv("MAIL") != NULL){
		mailpath = malloc(strlen(getenv("MAIL")) + strlen(pwd->pw_name) + 1);
		sprintf(mailpath, getenv("MAIL"), pwd->pw_name);
	}else{
		mailpath = malloc(strlen(_PATH_MAILDIR) + 1 + strlen(pwd->pw_name) + 1);
		sprintf(mailpath, _PATH_MAILDIR "/%s", pwd->pw_name);
	}

	if(stat(mailpath, &s) == 0){
		last = s.st_mtime;
	}
}

void check_mail(void){
	struct stat s;
	s.st_mtime = 0;
	if(stat(mailpath, &s) == 0 || last != 0){
		if((s.st_mtime == 0 && last != 0) || s.st_mtime > last){
			MwVaApply(image,
				MwNpixmap, full,
			NULL);
			MwVaApply(window,
				MwNiconPixmap, full,
			NULL);
		}

		last = s.st_mtime;
	}
}
