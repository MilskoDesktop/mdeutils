#include "mauplay.h"

#include "skipback.xpm"
#include "play.xpm"
#include "pause.xpm"
#include "stop.xpm"
#include "skipfwd.xpm"

#include "repeat.xpm"
#include "shuffle.xpm"

MwWidget       window;
MwWidget       menu, album, infoframe, info, infosep, bskipback, bplay, bpause, bstop, bskipfwd, modesep, brepeat, bshuffle;
MwWidget       mainsep;
MwWidget       eltime, seekbar, rmtime;
MwWidget       tree, list;
MwLLPixmap     pxalbum, pxskipback, pxplay, pxpause, pxstop, pxskipfwd, pxrepeat, pxshuffle;
unsigned char* pxalbumdata;

static void resize(MwWidget handle, void* user, void* client) {
	int ww = MwGetInteger(window, MwNwidth);
	int wh = MwGetInteger(window, MwNheight) - MwGetInteger(menu, MwNheight);
	int y  = MwGetInteger(menu, MwNheight);

	MwVaApply(album,
		  MwNx, 5,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(infoframe,
		  MwNx, 5 + 32 + 5,
		  MwNy, y + 5,
		  MwNwidth, ww - 5 - 32 - 5 - 5 - 10 - 10 - 32 * 7,
		  MwNheight, 32,
		  NULL);

	MwVaApply(info,
		  MwNx, MwDefaultBorderWidth(infoframe),
		  MwNy, MwDefaultBorderWidth(infoframe),
		  MwNwidth, MwGetInteger(infoframe, MwNwidth) - MwDefaultBorderWidth(infoframe) * 2,
		  MwNheight, MwGetInteger(infoframe, MwNheight) - MwDefaultBorderWidth(infoframe) * 2,
		  NULL);

	MwVaApply(infosep,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth),
		  MwNy, y + 5,
		  MwNwidth, 10,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bskipback,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bplay,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bpause,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 + 32,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bstop,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 + 32 + 32,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bskipfwd,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 + 32 + 32 + 32,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(modesep,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 * 5,
		  MwNy, y + 5,
		  MwNwidth, 10,
		  MwNheight, 32,
		  NULL);

	MwVaApply(brepeat,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 * 5 + 10,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(bshuffle,
		  MwNx, 5 + 32 + 5 + MwGetInteger(infoframe, MwNwidth) + 10 + 32 * 5 + 10 + 32,
		  MwNy, y + 5,
		  MwNwidth, 32,
		  MwNheight, 32,
		  NULL);

	MwVaApply(mainsep,
		  MwNx, 5,
		  MwNy, y + 5 + 32,
		  MwNwidth, ww - 5 - 5,
		  MwNheight, 10,
		  NULL);

	MwVaApply(eltime,
		  MwNx, 5,
		  MwNy, y + 5 + 32 + 10,
		  MwNwidth, MwTextWidth(window, "00:00"), /* please, never put 1 hour music */
		  MwNheight, 16,
		  NULL);

	MwVaApply(seekbar,
		  MwNx, 5 + MwTextWidth(window, "00:00") + 5,
		  MwNy, y + 5 + 32 + 10,
		  MwNwidth, ww - (5 + MwTextWidth(window, "00:00") + 5) * 2,
		  MwNheight, 16,
		  NULL);

	MwVaApply(rmtime,
		  MwNx, ww - 5 - MwTextWidth(window, "00:00"),
		  MwNy, y + 5 + 32 + 10,
		  MwNwidth, MwTextWidth(window, "00:00"),
		  MwNheight, 16,
		  NULL);

	MwVaApply(tree,
		  MwNx, 5,
		  MwNy, y + 5 + 32 + 10 + 16 + 5,
		  MwNwidth, 128 + 16,
		  MwNheight, wh - 5 - 32 - 10 - 16 - 5 - 5,
		  NULL);

	MwVaApply(list,
		  MwNx, 5 + 128 + 16 + 5,
		  MwNy, y + 5 + 32 + 10 + 16 + 5,
		  MwNwidth, ww - 5 - 128 - 16 - 5 - 5,
		  MwNheight, wh - 5 - 32 - 10 - 16 - 5 - 5,
		  NULL);
}

int main() {
	int st;

	if((st = db_init()) != 0) return st;

	MwLibraryInit();

	window	  = MwVaCreateWidget(MwWindowClass, "main", NULL, MwDEFAULT, MwDEFAULT, 640, 400,
				     MwNtitle, "mauplay",
				     NULL);
	menu	  = MwCreateWidget(MwMenuClass, "menu", window, 0, 0, 0, 0);
	album	  = MwCreateWidget(MwImageClass, "album", window, 0, 0, 0, 0);
	infoframe = MwVaCreateWidget(MwFrameClass, "infoframe", window, 0, 0, 0, 0,
				     MwNbackground, "#000",
				     MwNforeground, "#0f0",
				     MwNhasBorder, 1,
				     MwNinverted, 1,
				     NULL);
	info	  = MwVaCreateWidget(MwLabelClass, "info", infoframe, 0, 0, 0, 0,
				     MwNalignment, MwALIGNMENT_BEGINNING,
				     NULL);
	infosep	  = MwVaCreateWidget(MwSeparatorClass, "infosep", window, 0, 0, 0, 0,
				     MwNorientation, MwVERTICAL,
				     NULL);
	bskipback = MwVaCreateWidget(MwButtonClass, "skipback", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	bplay	  = MwVaCreateWidget(MwButtonClass, "play", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	bpause	  = MwVaCreateWidget(MwButtonClass, "pause", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	bstop	  = MwVaCreateWidget(MwButtonClass, "stop", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	bskipfwd  = MwVaCreateWidget(MwButtonClass, "skipfwd", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	modesep	  = MwVaCreateWidget(MwSeparatorClass, "modesep", window, 0, 0, 0, 0,
				     MwNorientation, MwVERTICAL,
				     NULL);
	brepeat	  = MwVaCreateWidget(MwButtonClass, "repeat", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	bshuffle  = MwVaCreateWidget(MwButtonClass, "shuffle", window, 0, 0, 0, 0,
				     MwNflat, 1,
				     NULL);
	mainsep	  = MwVaCreateWidget(MwSeparatorClass, "mainsep", window, 0, 0, 0, 0,
				     MwNorientation, MwHORIZONTAL,
				     NULL);
	eltime	  = MwVaCreateWidget(MwLabelClass, "eltime", window, 0, 0, 0, 0,
				     MwNalignment, MwALIGNMENT_END,
				     MwNtext, "0:00",
				     NULL);
	seekbar	  = MwVaCreateWidget(MwScrollBarClass, "seekbar", window, 0, 0, 0, 0,
				     MwNorientation, MwHORIZONTAL,
				     MwNareaShown, 15,
				     MwNshowArrows, 0,
				     NULL);
	rmtime	  = MwVaCreateWidget(MwLabelClass, "rmtime", window, 0, 0, 0, 0,
				     MwNalignment, MwALIGNMENT_END,
				     MwNtext, "0:00",
				     NULL);
	tree	  = MwCreateWidget(MwTreeViewClass, "tree", window, 0, 0, 0, 0);
	list	  = MwVaCreateWidget(MwListBoxClass, "list", window, 0, 0, 0, 0,
				     MwNhasHeading, 1,
				     MwNleftPadding, 16,
				     NULL);

	pxalbumdata = malloc(128 * 128 * 4);
	memset(pxalbumdata, 0, 128 * 128 * 4);

	pxalbum	   = MwLoadRaw(window, pxalbumdata, 128, 128);
	pxskipback = MwLoadXPM(window, skipback_xpm);
	pxplay	   = MwLoadXPM(window, play_xpm);
	pxpause	   = MwLoadXPM(window, pause_xpm);
	pxstop	   = MwLoadXPM(window, stop_xpm);
	pxskipfwd  = MwLoadXPM(window, skipfwd_xpm);

	free(pxalbumdata);

	pxrepeat  = MwLoadXPM(window, repeat_xpm);
	pxshuffle = MwLoadXPM(window, shuffle_xpm);

	MwSetVoid(album, MwNpixmap, pxalbum);

	MwSetVoid(bskipback, MwNpixmap, pxskipback);
	MwSetVoid(bplay, MwNpixmap, pxplay);
	MwSetVoid(bpause, MwNpixmap, pxpause);
	MwSetVoid(bstop, MwNpixmap, pxstop);
	MwSetVoid(bskipfwd, MwNpixmap, pxskipfwd);

	MwSetVoid(brepeat, MwNpixmap, pxrepeat);
	MwSetVoid(bshuffle, MwNpixmap, pxshuffle);

	MwAddUserHandler(window, MwNresizeHandler, resize, NULL);

	resize(window, NULL, NULL);

	ui_init();

	db_add("test.mp3");
	db_add("test2.mp3");
	db_scan();

	audio_init();

	MwLoop(window);
}
