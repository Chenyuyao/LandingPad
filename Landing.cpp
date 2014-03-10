

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <vector>
#include <cmath>
#include <string>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>


using namespace std;

const int BufferSize = 10;
const int FPS = 30;

//globel variable
int in_game;
int started;// = 0;
int pad_pos_x1;// = my_rand(0,18);
int pad_pos_x2;// = my_rand(21,39);
int pad_pos_y1;// = -1;
int pad_pos_y2;// = -1;
int done_init;// = 0;
int pause_switch;// = 1;
int last_pos;
int sx = 0;
int sy = 0;
vector<int> ypos(41);


// handy struct to save display, window, and screen
struct XInfo {
	Display	 *display;
	int	 screen;
	Window	 window;
	GC		 gc[5];
	int		width;		// size of window
	int		height;
};

// Function to put out a message on error and exits
void error( string str ) {
  	cerr << str << endl;
  	exit(0);
}



int my_rand(int a, int b)
{
 	 return a + (b - a) * (double)rand() / RAND_MAX;
}




class Ship {
	public:
		void paint(XInfo &xinfo) {
			Display  *display = xinfo.display;
			Window   win = xinfo.window;

			int s = 40;
			int x0 = x + sx; int y0 = y + sy;			
			
			XDrawRectangle(display, win, xinfo.gc[2], x0-s/2, y0-s/2, s, s); 
			XDrawArc(display, win, xinfo.gc[2], x0-(s/4), y0-(s/4), 20, 20, 0, 360*64);
			XDrawLine(display, win, xinfo.gc[2], x0-s/2,y0+s/2,x0-s/2-5,y0+s/2+5);
			XDrawLine(display, win, xinfo.gc[2], x0+s/2,y0+s/2,x0+s/2+5,y0+s/2+5);
			XFillRectangle(display, win, xinfo.gc[2], x0-s/2-9, y0+s/2+5, 6, 3);
			XFillRectangle(display, win, xinfo.gc[2], x0+s/2+3, y0+s/2+5, 6, 3);
		}
		
		void move(XInfo &xinfo) {
			y += vy;
			x += vx;
			if (vx != 0) {
				if (vx > 0) vx -= d;
				else vx +=d;
			}
			if (vy != g) {
				if (vy > g) vy -= d;
				else vy +=d;
			}
		}
		
		void engin( int dir ) {
			if (dir == 0 ) vx +=2; //right
			if (dir == 1 ) vx -=2; //left
			if (dir == 2 ) vy +=2; //down
			if (dir == 3 ) vy -=2; //up
		}
		
		void restart(int X, int Y) {
			x = X; y = Y;
			vx = 0; vy = g;
		}

		int too_fast() {
			return (vy > vmax)?1:0;
		}
		
		int getX() {
			return x;
		}
		
		void relocation() {
			x = x + sx;
			y = y + sy;
		}		

		int getY() {
			return y;
		}
		
	Ship(int X, int Y) {
		x = X;
		y = Y;
		g = 4;
		vx = 0;
		vy = g;
		d = 1;
		vmax = 4;
	}

	private:
		int x;
		int y;
		int vx;
		int vy;
		int g;
		int d;
		int vmax;
};




void drawMountain(XInfo &xinfo, GC gc) {
	Display  *display = xinfo.display;
	Window   win = xinfo.window;

	int top = 540;
	int bottom = 300;

	int current_pos_1 = my_rand(bottom, top);
	int current_pos_2 = 0;

	for (int i = 0; i < 40; i++) {
		if (done_init == 0 ) {
			ypos[i] = current_pos_1;
			if ( (i != pad_pos_x1) && (i != pad_pos_x2) && (i != pad_pos_x1+1 ) && (i != pad_pos_x2+1 ) && (i != pad_pos_x1+2 ) && (i != pad_pos_x2+2 ) ) {
				current_pos_2 = my_rand(bottom, top);
				XDrawLine(display, win, gc, i*20+sx, current_pos_1+sy, (i+1)*20+sx, current_pos_2+sy);
				if (i == 39 ) 	last_pos = current_pos_2;
				current_pos_1 = current_pos_2;
			}
			else {
				if (pad_pos_y1 == -1) pad_pos_y1 = current_pos_1;
				else pad_pos_y2 = current_pos_2;
			}
		}
		else {
			if ( (i != pad_pos_x1) && (i != pad_pos_x2) && (i != pad_pos_x1+1 ) && (i != pad_pos_x2+1 ) && (i != pad_pos_x1+2 ) && (i != pad_pos_x2+2 ) ) {
					XDrawLine(display, win, gc, i*20+sx, ypos[i]+sy, (i+1)*20+sx, ypos[i+1]+sy);
			}
		}
	}

}



void drawLandingPad(XInfo &xinfo, GC gc) {
	Display *display = xinfo.display;
        Window   win = xinfo.window;
	
	if ( (pad_pos_y1 <0) || (pad_pos_y2 < 0)) error("Error when rendering Landing Pad!!");
	
	XFillRectangle(display, win, gc, pad_pos_x1*20+sx, pad_pos_y1+sy, 60, 6); 
	XFillRectangle(display, win, gc, pad_pos_x2*20+sx, pad_pos_y2+sy, 60, 6); 

}

void drawText(XInfo &xinfo, GC gc, int x, int y, string text) {
	Display  *display = xinfo.display;
	Window   win = xinfo.window;
	XDrawString(display, win, gc, x+sx, y+sy, text.c_str(), text.length());
}

void drawThruster(XInfo &xinfo, GC gc, int x, int y, int i) {
	Display  *display = xinfo.display;
	Window   win = xinfo.window;

	int dx,dy;

	switch (i) {
		case 0:		
			XFillRectangle(display, win, gc, x+23, y-6, 5, 13);
			XDrawLine(display, win, gc, x+28, y-6, x+38, y-6);
			XDrawLine(display, win, gc, x+28, y-3, x+38, y-3);
			XDrawLine(display, win, gc, x+28, y  , x+38, y  );
			XDrawLine(display, win, gc, x+28, y+3, x+38, y+3);
			XDrawLine(display, win, gc, x+28, y+6, x+38, y+6);
			break;
		case 1:		
			XFillRectangle(display, win, gc, x-6, y-28, 13, 5);
			XDrawLine(display, win, gc, x-6, y-28, x-6, y-38);
			XDrawLine(display, win, gc, x-3, y-28, x-3, y-38);
			XDrawLine(display, win, gc, x  , y-28, x  , y-38);
			XDrawLine(display, win, gc, x+3, y-28, x+3, y-38);
			XDrawLine(display, win, gc, x+6, y-28, x+6, y-38);
			break;
		case 2:		
			XFillRectangle(display, win, gc, x-28, y-6, 5, 13);
			XDrawLine(display, win, gc, x-28, y-6, x-38, y-6);
			XDrawLine(display, win, gc, x-28, y-3, x-38, y-3);
			XDrawLine(display, win, gc, x-28, y  , x-38, y  );
			XDrawLine(display, win, gc, x-28, y+3, x-38, y+3);
			XDrawLine(display, win, gc, x-28, y+6, x-38, y+6);
			break;
		case 3:		
			XFillRectangle(display, win, gc, x-6, y+23, 13, 5);
			XDrawLine(display, win, gc, x-6, y+28, x-6, y+38);
			XDrawLine(display, win, gc, x-3, y+28, x-3, y+38);
			XDrawLine(display, win, gc, x  , y+28, x  , y+38);
			XDrawLine(display, win, gc, x+3, y+28, x+3, y+38);
			XDrawLine(display, win, gc, x+6, y+28, x+6, y+38);
			break;
	}
}


Ship ship(400, 50);

void handleAnimation(XInfo &xinfo) {
	ship.move(xinfo);
}

void repaint( XInfo &xinfo) {
	
	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[3], 0, 0, xinfo.width, xinfo.height );
//	cout << xinfo.width << xinfo.height << endl;
	ship.paint(xinfo);
	drawMountain(xinfo, xinfo.gc[0]);
	drawLandingPad(xinfo, xinfo.gc[1]);
	
	XFlush( xinfo.display );
}

void initGame(XInfo &xInfo) {
	//Initialize globle variable
	in_game = 0;
	pad_pos_x1 = my_rand(0,18);
	pad_pos_x2 = my_rand(21,39);
	pad_pos_y1 = -1;
	pad_pos_y2 = -1;
	done_init = 0;
	pause_switch = 1;
	last_pos = 0;

	XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[3], 0, 0, xInfo.width, xInfo.height );
}
/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {

	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   	/*
	* Find out some things about the display you're using.
	*/

	
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;
	
	xInfo.width = 800;
	xInfo.height = 600;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		100, 100,			// upper left corner location
		xInfo.width, xInfo.height,	// size of the window
		5,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"LandingPad",			// window's title
		"SD",				// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints);			// size hints for the window

	//cout << xInfo.height << " " << xInfo.width << endl;

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	
	XFlush(xInfo.display);
	sleep(1);	// let server get set up before sending drawing commands

	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     2, LineSolid, CapButt, JoinRound);

	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapRound, JoinBevel);

	i = 2;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     2, LineSolid, CapButt, JoinBevel);

	i = 3;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	i = 4;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);

	XFontStruct * font;
	const char * fontname = "10x20";
    	font = XLoadQueryFont (xInfo.display, fontname);
	if (!font) {
    		error("XLoadQueryFont: failed loading font");
	}
	XSetFont(xInfo.display, xInfo.gc[i], font->fid);


	XSelectInput(xInfo.display, xInfo.window, 
		KeyPressMask | KeyReleaseMask | StructureNotifyMask); 

}

void handleResize(XInfo &xinfo, XEvent &event) {
	XConfigureEvent xce = event.xconfigure;
//	fprintf(stderr, "Handling resize  w=%d  h=%d\n", xce.width, xce.height);
	if (xce.width >= 800 && xce.height >= 600) {
		xinfo.width = xce.width; 
		xinfo.height = xce.height;
		sx = ( xce.width - 800 ) / 2;
		sy = ( xce.height - 600 ) / 2;
//		cout << xinfo.height << " " << xinfo.width << endl;
		repaint(xinfo);
	}
	else {
		error("TOO SMALL");	
	}
}


void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];

	int i = XLookupString( (XKeyEvent*)&event, text, BufferSize, &key, 0 );
	if (in_game) {
		if (pause_switch < 0 ) {
			int px = ship.getX()+sx;
			int py = ship.getY()+sy;
			if (key == XK_Up) {
				ship.engin(3);
				drawThruster(xinfo, xinfo.gc[1],px,py,3);
			}
			if (key == XK_Left) {
				ship.engin(1);
				drawThruster(xinfo, xinfo.gc[1],px,py,0);
			}
			if (key == XK_Right) {
				ship.engin(0);
				drawThruster(xinfo, xinfo.gc[1],px,py,2);
			}
			if (key == XK_Down) {
				ship.engin(2);
				drawThruster(xinfo, xinfo.gc[1],px,py,1);
			}
		}
		if (key == XK_space) {
			pause_switch = pause_switch * -1;
		}
	}
	else {
		if (key == XK_space) {
			in_game=1;
			pause_switch = pause_switch * -1;
		}
	}
	if (i==1 && (text[0] == 'q' || text[0] == 'Q') ) error("terminate normally");
}

int intercept() {
	int x0 = ship.getX();
	int y0 = ship.getY();

	int x = x0-20;
	int y = y0+23;

	if (y0  > 500 || x0 < 26 || x0 > 774) return -2;

	for (int i =0;  i<41; i++) {
		x+=1;
		int yd = (x-x/20*20)/20*(ypos[x/20+1]-ypos[x/20])+ypos[x/20];
		if (yd <= y) {
			if(abs(x0 - pad_pos_x1*20-30) <8 || abs(x0 - pad_pos_x2*20-30) <8 ) return 1;
			else return -1;
		}
	}
	return 0;
}


unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	XEvent event;
	unsigned long lastRepaint = 0;
	int this_result;

	while ( 1 ) {
		//if (event.type == KeyPress) 
		initGame(xinfo);
		XFlush(xinfo.display);

		string message= (started)?"PRESS SPACE TO CONTINUE":"PRESS SPACE TO START";
		drawText(xinfo, xinfo.gc[4], 320, 280, message);
		if (!started) {
			drawText(xinfo, xinfo.gc[4], 500, 450, "ARROW KEY to control the ship");
			drawText(xinfo, xinfo.gc[4], 500, 475, "Q to quit");
			drawText(xinfo, xinfo.gc[4], 500, 500, "SPACE to pause/continue");
			started =1;
		}
		else if (this_result == -1) {						
			drawText(xinfo, xinfo.gc[4], 380, 230, "YOU LOSS");
		}
		else if (this_result == 1) {
			drawText(xinfo, xinfo.gc[4], 380, 230, "YOU WON");
		}
		else if (this_result == -2) {
			drawText(xinfo, xinfo.gc[4], 350, 230, "OUT OF CONTROL");
		}
		
		else if (this_result == -3) {
			drawText(xinfo, xinfo.gc[4], 380, 230, "TOO FAST");
		}
	
		XNextEvent( xinfo.display, &event );
		switch( event.type ) {
			case KeyPress:
				handleKeyPress(xinfo, event);
				break;
			case ConfigureNotify:
				handleResize(xinfo, event);
				break;	
		}
		
		if (in_game) {
			XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[3], 0, 0, xinfo.width, xinfo.height );
			drawMountain(xinfo, xinfo.gc[0]);
			ypos[40] = last_pos;
			drawLandingPad(xinfo, xinfo.gc[1]);
			done_init = 1;
	
			ship.restart(400,50);
			ship.paint(xinfo);
			XFlush(xinfo.display);
		}
		while ( in_game) {
			if (XPending(xinfo.display) > 0) {
				XNextEvent( xinfo.display, &event );
				switch( event.type ) {
					case KeyPress:
						handleKeyPress(xinfo, event);
						break;

					case ConfigureNotify:
						handleResize(xinfo, event);
						break;	
				}
			}
			
	
			unsigned long end = now();
			if (end - lastRepaint > 1000000/FPS &&  pause_switch < 0) {
				handleAnimation(xinfo);
				repaint(xinfo);
				lastRepaint = now();
				int result = intercept();
				if (result!=0) { 
					pause_switch=1;
					this_result = result;
					if (ship.too_fast() && result == 1) this_result = -3;
					break;
				}
			}
		}
	}		

}
/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;
	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
