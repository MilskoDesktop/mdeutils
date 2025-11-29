#include "mclock.h"

#include <Mw/Widget/OpenGL.h>

MwWidget      window, opengl;
int	      ww, wh;
unsigned long last_tick;
MwLLPixmap    pxclock;

static void resize(MwWidget handle, void* user, void* client) {
	ww = MwGetInteger(window, MwNwidth);
	wh = MwGetInteger(window, MwNheight);

	MwVaApply(opengl,
		  MwNwidth, ww,
		  MwNheight, wh,
		  NULL);

	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void drawHand(float rad, float wid, float leng, int shadow) {
	int i;

	double n = M_PI / 2;
	if(shadow) {
		glPushMatrix();
		glTranslatef(wid / 1, -(wid / 1), 0);
		glColor3f(0.45, 0.45, 0.45);

		glBegin(GL_POLYGON);
		glVertex2f(cos(rad) * leng, sin(rad) * leng);
		glVertex2f(cos(rad - n) * wid, sin(rad - n) * wid);
		glVertex2f(-cos(rad) * leng * .1, -sin(rad) * leng * .1);
		glVertex2f(-cos(rad - n) * wid, -sin(rad - n) * wid);
		glEnd();

		glPopMatrix();

		return;
	}

	for(i = 0; i < 2; i++) {
		if(i == 0) {
			glColor3f(0.65, 0.65, 0.65);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else {
			glColor3f(0.2, 0.2, 0.2);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glBegin(GL_POLYGON);
		glVertex2f(cos(rad) * leng, sin(rad) * leng);
		glVertex2f(cos(rad - n) * wid, sin(rad - n) * wid);
		glVertex2f(-cos(rad) * leng * .1, -sin(rad) * leng * .1);
		glVertex2f(-cos(rad - n) * wid, -sin(rad - n) * wid);
		glEnd();
	};
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.65, 0.65, 0.65);
}

static void draw(MwWidget handle, void* user, void* client) {
	int	   i;
	time_t	   t;
	struct tm* tm_info;

	double srad;
	double mrad;
	double hrad;
	double rad;

	double len;

	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(1);

	t	= time(NULL);
	tm_info = localtime(&t);

	srad = M_PI / 2 - (tm_info->tm_sec / 60.0) * 2 * M_PI;
	mrad = M_PI / 2 - ((tm_info->tm_min + tm_info->tm_sec / 60.0) / 60.0) * 2 * M_PI;
	hrad = M_PI / 2 - (((tm_info->tm_hour % 12) + tm_info->tm_min / 60.0 + tm_info->tm_sec / 3600.0) / 12.0) * 2 * M_PI;

	drawHand(srad, 0.01, 0.98, 1);
	drawHand(mrad, 0.05, 0.98, 1);
	drawHand(hrad, 0.05, 0.7, 1);

	glColor3f(0, 0, 0);
	for(i = 0; i < 360; i += 6) {
		rad = i / 180.0 * M_PI;
		if(!(i % 30)) {
			glLineWidth(3);
			len = 0.8;
		} else {
			glLineWidth(1);
			len = 0.85;
		}

		glBegin(GL_LINES);
		glVertex2f(cos(rad) * len, sin(rad) * len);
		glVertex2f(cos(rad) * 0.9, sin(rad) * 0.9);
		glEnd();
	}

	glLineWidth(1);

	drawHand(hrad, 0.05, 0.7, 0);
	drawHand(mrad, 0.05, 0.98, 0);
	drawHand(srad, 0.02, 0.98, 0);

	MwOpenGLSwapBuffer(opengl);
}

static void tick(MwWidget handle, void* user, void* client) {
	unsigned long tick = MwTimeGetTick();
	if((tick - last_tick) >= 1000) {
		draw(opengl, NULL, NULL);
		last_tick = tick;
	}
}

static void mouseup(MwWidget handle, void* user, void* client) {
	MDEAboutDialog(window, "mclock", VERSION, pxclock);
}

int main() {
	MwLLColor col;
	int	  r, g, b;

	MwLibraryInit();

	last_tick = MwTimeGetTick();

	window = MwVaCreateWidget(MwWindowClass, "main", NULL, MwDEFAULT, MwDEFAULT, 150, 150,
				  MwNtitle, "mclock",
				  NULL);

	opengl	= MwCreateWidget(MwOpenGLClass, "opengl", window, 0, 0, 150, 150);
	pxclock = MwLoadImage(window, ICON128DIR "/clock.png");

	MwOpenGLMakeCurrent(opengl);

	col = MwParseColor(window, MwGetText(window, MwNbackground));
	MwGetColor(col, &r, &g, &b);
	MwLLFreeColor(col);

	glClearColor(r / 255.0, g / 255.0, b / 255.0, 1);

	resize(window, NULL, NULL);

	MwAddUserHandler(window, MwNresizeHandler, resize, NULL);
	MwAddUserHandler(opengl, MwNdrawHandler, draw, NULL);
	MwAddUserHandler(opengl, MwNmouseUpHandler, mouseup, NULL);
	MwAddUserHandler(window, MwNtickHandler, tick, NULL);

	draw(opengl, NULL, NULL);

	MwLoop(window);
}
