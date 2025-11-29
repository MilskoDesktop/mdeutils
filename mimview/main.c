#include "mimview.h"

#define ButtonWidth 32
#define ButtonHeight 24

MwWidget	  window, menu, image;
MwLLPixmap	  pxprojector, pxdata = NULL;
static MwLLPixmap pxprev, pxnext, pxzoomin, pxzoomout;
static MwWidget	  bprev, bnext, sep, bzoomin, bzoomout;

static void resize(MwWidget handle, void* user, void* client) {
	int	       y  = MwGetInteger(menu, MwNheight);
	int	       ww = MwGetInteger(window, MwNwidth);
	int	       wh = MwGetInteger(window, MwNheight) - y;
	int	       bx = 0;
	unsigned char* data;
	int	       iw, ih;

	if(pxdata != NULL) MwLLDestroyPixmap(pxdata);
	iw   = ww - 10;
	ih   = wh - 5 - ButtonHeight - 5;
	data = malloc(iw * ih * 4);
	memset(data, 0, iw * ih * 4);
	pxdata = MwLoadRaw(window, data, iw, ih);
	free(data);
	MwVaApply(image,
		  MwNpixmap, pxdata,
		  MwNx, 5,
		  MwNy, y + 5,
		  MwNwidth, ww - 10,
		  MwNheight, wh - 5 - ButtonHeight - 5,
		  NULL);

	bx = (ww - 10 - (ButtonWidth * 2 + 5) * 2) / 2;
	MwVaApply(bprev,
		  MwNx, bx,
		  MwNy, y + wh - 5 - ButtonHeight,
		  MwNwidth, ButtonWidth,
		  MwNheight, ButtonHeight,
		  NULL);
	MwVaApply(bnext,
		  MwNx, bx + ButtonWidth + 5,
		  MwNy, y + wh - 5 - ButtonHeight,
		  MwNwidth, ButtonWidth,
		  MwNheight, ButtonHeight,
		  NULL);
	MwVaApply(sep,
		  MwNx, bx + ButtonWidth + 5 + ButtonWidth,
		  MwNy, y + wh - 5 - ButtonHeight,
		  MwNwidth, 10,
		  MwNheight, ButtonHeight,
		  NULL);
	MwVaApply(bzoomin,
		  MwNx, bx + ButtonWidth + 5 + ButtonWidth + 10,
		  MwNy, y + wh - 5 - ButtonHeight,
		  MwNwidth, ButtonWidth,
		  MwNheight, ButtonHeight,
		  NULL);
	MwVaApply(bzoomout,
		  MwNx, bx + ButtonWidth + 5 + ButtonWidth + 10 + ButtonWidth + 5,
		  MwNy, y + wh - 5 - ButtonHeight,
		  MwNwidth, ButtonWidth,
		  MwNheight, ButtonHeight,
		  NULL);
}

int main(int argc, char** argv) {
	int pad;
	MwLibraryInit();

	window	 = MwVaCreateWidget(MwWindowClass, "main", NULL, MwDEFAULT, MwDEFAULT, 320, 240,
				    MwNtitle, "mimview",
				    NULL);
	menu	 = MwCreateWidget(MwMenuClass, "menu", window, 0, 0, 0, 0);
	image	 = MwCreateWidget(MwImageClass, "image", window, 0, 0, 0, 0);
	bprev	 = MwCreateWidget(MwButtonClass, "prev", window, 0, 0, 0, 0);
	bnext	 = MwCreateWidget(MwButtonClass, "next", window, 0, 0, 0, 0);
	sep	 = MwVaCreateWidget(MwSeparatorClass, "sep", window, 0, 0, 0, 0,
				    MwNorientation, MwVERTICAL,
				    NULL);
	bzoomin	 = MwCreateWidget(MwButtonClass, "zoomin", window, 0, 0, 0, 0);
	bzoomout = MwCreateWidget(MwButtonClass, "zoomout", window, 0, 0, 0, 0);

	pxprojector = MwLoadImage(window, ICON128DIR "/projector.png");
	pxprev	    = MwLoadImage(window, ICON16DIR "/previous.png");
	pxnext	    = MwLoadImage(window, ICON16DIR "/next.png");
	pxzoomin    = MwLoadImage(window, ICON16DIR "/zoomin.png");
	pxzoomout   = MwLoadImage(window, ICON16DIR "/zoomout.png");

	pad = ButtonHeight / 2 - 16 / 2 - MwDefaultBorderWidth(window);

	MwVaApply(bprev,
		  MwNpixmap, pxprev,
		  MwNpadding, pad,
		  NULL);

	MwVaApply(bnext,
		  MwNpixmap, pxnext,
		  MwNpadding, pad,
		  NULL);

	MwVaApply(bzoomin,
		  MwNpixmap, pxzoomout,
		  MwNpadding, pad,
		  NULL);

	MwVaApply(bzoomout,
		  MwNpixmap, pxzoomin,
		  MwNpadding, pad,
		  NULL);

	MwAddUserHandler(window, MwNresizeHandler, resize, NULL);

	ui_init();

	resize(window, NULL, NULL);

	MwLoop(window);
}
