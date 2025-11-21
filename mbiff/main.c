#include "mbiff.h"

#include "mailempty.xpm"
#include "mailfull.xpm"

MwWidget window, image;
MwLLPixmap empty, full;
static int waiting = 0;

static void resize(MwWidget handle, void* user, void* client){
	int w = MwGetInteger(handle, MwNwidth);
	int h = MwGetInteger(handle, MwNheight);
	int iw = w < h ? w : h;

	(void)user;
	(void)client;

	MwVaApply(image,
		MwNx, (w - iw) / 2,
		MwNy, (h - iw) / 2,
		MwNwidth, iw,
		MwNheight, iw,
	NULL);
}

static void tick(MwWidget handle, void* user, void* client){
	(void)handle;
	(void)user;
	(void)client;

	waiting += MwWaitMS;
	if(waiting >= 1000){
		check_mail();

		waiting = 0;
	}
}

static void mouseUp(MwWidget handle, void* user, void* client){
	(void)handle;
	(void)user;
	(void)client;

	MwVaApply(window,
			MwNiconPixmap, empty,
	NULL);
	MwVaApply(image,
		MwNpixmap, empty,
	NULL);
}

int main() {
	init_mail();

	MwLibraryInit();

	window = MwVaCreateWidget(MwWindowClass, "main", NULL, MwDEFAULT, MwDEFAULT, 128, 128,
		MwNtitle, "mbiff",
	NULL);

	empty = MwLoadImage(window, DATADIR "/mde/icons/128x128/mailempty.png");
	full = MwLoadImage(window, DATADIR "/mde/icons/128x128/mailfull.png");
	if(empty == NULL) empty = MwLoadXPM(window, mailempty);
	if(full == NULL) full = MwLoadXPM(window, mailfull);

	MwVaApply(window, MwNiconPixmap, empty, NULL);

	image = MwVaCreateWidget(MwImageClass, "image", window, 0, 0, 128, 128,
		MwNpixmap, empty,
	NULL);

	MwAddUserHandler(image, MwNmouseUpHandler, mouseUp, NULL);
	MwAddUserHandler(window, MwNtickHandler, tick, NULL);
	MwAddUserHandler(window, MwNresizeHandler, resize, NULL);

	MwLoop(window);
}
