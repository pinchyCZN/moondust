#include <windows.h>
#include <math.h>
#include "SDL.h"

#define PI 3.14159265

extern BYTE BitmapScreen[];

typedef struct {
	double x;
	double y;
	double z;
}ROT;

typedef struct {
	double x;
	double y;
	double z;
}Point;

void rotate(Point *v,Point *in,Point *out)
{
	double cx,cy,cz;
	double sx,sy,sz;
	double x,y,z;
	double xy,xz,yz,yx,zx,zy;
	double scaleFactor;
	double focalLength=10000;

	cx=cos(v->x);
	cy=cos(v->y);
	cz=cos(v->z);
	sx=sin(v->x);
	sy=sin(v->y);
	sz=sin(v->z);
	x=in->x;
	y=in->y;
	z=in->z;

	// rotation around x
	xy = cx*y - sx*z;
	xz = sx*y + cx*z;
	// rotation around y
	yz = cy*xz - sy*x;
	yx = sy*xz + cy*x;
	// rotation around z
	zx = cz*yx - sz*xy;
	zy = sz*yx + cz*xy;
	
	scaleFactor = focalLength/(focalLength + yz);
	//scaleFactor = 20.0;

	x = zx*scaleFactor;
	y = zy*scaleFactor;
	z = yz;
	out->x=x;
	out->y=y;
	out->z=z;
}

void translate(Point *p,Point *trans)
{
	p->x+=trans->x;
	p->y+=trans->y;
	p->z+=trans->z;
}
void plot_pixel(BYTE *screen,int color,double x,double y,double z,double scale)
{
	int x_screen;
	int y_screen;
	int offset;

	if(z==0)
		return;
	x_screen=((x*scale)/z);
	y_screen=((y*scale)/z);
	x_screen+=160;
	y_screen+=100;
//	x_screen=(((float)x*scale)/(z+200));
//	y_screen=(((float)y*scale)/(z+200));
//	x_screen=(x*scale)/20;
//	y_screen=(y*scale)/20;
	render_pixel(screen,color,x_screen,y_screen);
	return;

}

int circle_effect()
{
	int i,j,k;
	static int height=35;
	static double freq=1;
	double x,y,z,r;
	Point in,out;
	static Point rot={20,3,66};
	static Point trans={0,0,-1600};
	BYTE *keys;
	BYTE str[255];
//	memset(BitmapScreen,0,320*200);
	keys=SDL_GetKeyState(NULL);
	if(keys[SDLK_UP])
		rot.x+=.01;
	if(keys[SDLK_DOWN])
		rot.x-=.01;
	if(keys[SDLK_LEFT])
		trans.z-=1;
	if(keys[SDLK_RIGHT])
		trans.z+=1;
	if(keys[SDLK_a])
		height+=1;
	if(keys[SDLK_s])
		height-=1;
	if(keys[SDLK_z])
		freq+=.1;
	if(keys[SDLK_x])
		freq-=.1;



	sprintf(str,"z=%f\nheight=%i,freq=%f",trans.z,height,freq);
	render_str(BitmapScreen,str,0,0,0,1);
	for(i=-30;i<30;i+=1){
		for(j=-30;j<30;j+=1){
			x=i;
			y=j;
			r=sqrt(x*x+y*y);
			if(r==0)
				continue;
			z=sin(r+freq)*height/r;
			in.x=x;
			in.y=y;
			in.z=z;
			rotate(&rot,&in,&out);
			translate(&out,&trans);

//			plot_pixel(BitmapScreen,rand(),out.x,out.y,out.z,4000.0);
		}
	}
}