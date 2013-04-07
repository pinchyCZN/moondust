#include "SDL.h"
#include "gfx.h"

typedef struct{
	int Xpos;
	int Ypos;
	float dir;
	int speed;
}PLAYER;

extern char Ship0Data[];
extern char Ship1Data[];
extern char Ship2Data[];
extern char Ship1Data[];

extern char SpacewalkerBody0Data[];
extern char SpacewalkerBody1Data[];
extern char SpacewalkerBody2Data[];
extern char SpacewalkerBody0Data[];
extern char SpacewalkerHeadData[];

static int rotate(ROT *v,Point *in,Point *out)
{
	float cx,cy,cz;
	float sx,sy,sz;

	cx=cos(v->x);
	cy=cos(v->y);
	cz=cos(v->z);
	sx=sin(v->x);
	sy=sin(v->y);
	sz=sin(v->z);

	//_control87(_RC_DOWN,_MCW_RC);
	out->x = in->x*cy*cz + in->y*cy*sz - in->z*sy;
	out->y = in->x*(sx*sy*cz-cx*sz) + in->y*(sx*sy*sz+cx*cz) + in->z*sx*cy;
	out->z = in->x*(cx*sy*cz+sx*sz) + in->y*(cx*sy*sz-sx*cz) + in->z*cx*cy;
	//zx=s3*s1-c3*s2*c1;s2*c1+c3*s1
	//zy=-s3*c1-c3*s2*s1;c3*c1-s2*s1
    //zz=c3*c2
//	out->x = in->x*cx*cy + in->y*cy*sx - in->z*sy;
//	out->y = in->x*(sy*sz*cx-cz*sx) + in->y*(sx*sy*sz-cx*cz) - in->z*sx*cy;
//	out->z = in->x*(sy*cx+sx*cz) + in->y*(cz*cx-sy*sx) + in->z*cy*cz;
}
static void set_pixel(SDL_Surface *screen,int color,int x,int y)
{
	int offset;

	if(x<0 || x >= screen->w)
		return;
	if(y<0 || y >= screen->h)
		return;


	offset=y*(screen->pitch / 4);
	offset+=x;
	((unsigned int*)screen->pixels)[offset]=color;
}


int draw_sprite(SDL_Surface *screen,char *data,int x,int y,ROT *v)
{
	int i,j,k;
	Point in,out;
	for(i=0;i<20;i++){
		for(j=0;j<3;j++){
			for(k=0;k<8;k++){
				if(data[(i*3)+j]&(1<<(7-k))){
					in.x=-10+k+(j*8);
					in.y=-10+i;
					in.z=0;
					rotate(v,&in,&out);
					out.x+=x;
					out.y+=y;
					//set_pixel(screen,0xFFFF,x+k+(j*8),y+i);
					set_pixel(screen,0xFFFF,out.x,out.y);
				}
			}
		}
	}
	return 0;
}

int sprite_test(SDL_Surface *screen)
{
	static int ship=0;
	ROT v;
	static float count=0;
	char *buf;

	if (SDL_MUSTLOCK(screen)) 
		if (SDL_LockSurface(screen) < 0) 
			return;
	SDL_FillRect(screen,NULL,0);
	
	switch(ship){
		case 0:	buf=Ship0Data;break;
		case 1:	buf=Ship1Data;break;
		case 2:	buf=Ship2Data;break;
		default:buf=Ship0Data;break;
	}
	v.x=0;
	v.y=0;
	v.z=count;

	draw_sprite(screen,buf,100,100,&v);
	count+=.01;
	if(count>PI*2){
//		ship++;
		count=0;
	}
	if(ship>2){
		ship=0;
	}
	// Unlock if needed
	if (SDL_MUSTLOCK(screen)) 
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);    

	return 0;
}