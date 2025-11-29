#include "mimview.h"

static void* help_about;

static void menu_menu(MwWidget handle, void* user, void* client) {
	if(client == help_about) {
		MDEAboutDialog(window, "mimview", VERSION, pxprojector);
	}
}

void ui_init(void) {
	void* m;

	m	   = MwMenuAdd(menu, NULL, "?Help");
	help_about = MwMenuAdd(menu, m, "About");

	MwAddUserHandler(menu, MwNmenuHandler, menu_menu, NULL);
}
