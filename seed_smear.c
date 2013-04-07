#include <windows.h>
#include <math.h>
#include "SDL.h"


typedef struct{
	int on;
	BYTE len;
	BYTE start;
	int delay;
	int erase;
	int offset[256];
	int seed_x,seed_y;
	int cycle;
}SMEAR;
SMEAR ship_smear[10];
int SeedSmearColorCycle()
{
	int i;
	for(i=0;i<8;i++){
		if(ship_smear[i].len>0){

		}
	}
/*
	for(i=SS_CurrentOffset;i<SeedSmearOfsEnd;i++){
		ColorNybbles[SeedSmearAddrTbl[i]]=SeedSmearColorData[(SSColorData_StartOfs+i)&0xFF];
	}

	SSColorData_StartOfs++;
*/
	return TRUE;
}

extern BYTE BitmapScreen[];
extern BYTE BullseyeChar[];
extern BYTE VortexChar[];
int seed_dropped=FALSE;
int bulls_eye=FALSE;
BYTE color_strip[512];
int reset_ship_smear()
{
	int i;
	memset(ship_smear,0,sizeof(ship_smear));
	seed_dropped=FALSE;
	bulls_eye=FALSE;
	for(i=0;i<sizeof(color_strip)/sizeof(BYTE);i++){
		color_strip[i]=rand();
	}
	return TRUE;
}
int get_center_distance(int x, int y)
{
	double x1,y1;
	int d;
	x1=(320/2)-x;
	y1=(200/2)-y;
	d=(int)sqrt(x1*x1+y1*y1);
	return d;
}
int get_cn_pos(int x,int y)
{
	return (x/8+(y/8*40));
}

int seed_drop(BYTE *ColorNybbles,int x,int y)
{
	if(get_center_distance(x,y)<100){
		render_bitmap(BitmapScreen,BullseyeChar,x,y,0,2,1,8);
		return FALSE;
	}
	else{
		render_bitmap(BitmapScreen,VortexChar,x-8,y-8,0,2,2,16);
		if(!seed_dropped){
			seed_dropped=TRUE;
			ship_smear[0].seed_x=x+12;
			ship_smear[0].seed_y=y+11;
		}
		return TRUE;
	}
}
int color_cycle_smear(BYTE num,BYTE *ColorNybbles)
{
	int i;
	if(num==1)
		ColorNybbles[get_cn_pos(ship_smear[0].seed_x,ship_smear[0].seed_y)]=rand();

	for(i=ship_smear[num].start;i<ship_smear[num].len;i++){
		BYTE *c=&ColorNybbles[ship_smear[num].offset[i]];
		if(*c==0)
			*c=rand();
		else{
			int j=ship_smear[num].cycle+i+num*10;
			j%=sizeof(color_strip);
			*c=color_strip[j];
		}
	}
	ship_smear[num].cycle++;
	ship_smear[num].cycle%=sizeof(color_strip);

}
int check_bulls_eye(int x,int y)
{
	int position;
	if(get_cn_pos(x,y)==get_cn_pos(320/2,200/2))
		return TRUE;
	else
		return FALSE;
}
int smear_dust(BYTE num,BYTE *ColorNybbles,int x,int y,int *bulls_eye_flag)
{
	static int speed=0;
	int position;
	if(num==0)
		return FALSE;
	if(seed_dropped==FALSE)
		return FALSE;
	
	
	if(ship_smear[num].erase){
		if(ship_smear[num].delay<100){
			ship_smear[num].delay++;
		}
		else{
			if(ship_smear[num].start==ship_smear[num].len){
				ship_smear[num].erase=FALSE;
				ship_smear[num].on=FALSE;
				ship_smear[num].len=0;
				ship_smear[num].start=0;
			}
			else{
				speed++;
				if(speed>0){
					ColorNybbles[ship_smear[num].offset[ship_smear[num].start]]=0;
					ship_smear[num].start++;
					speed=0;
				}
			}
		}
	}
	else{
		position=get_cn_pos(x+12,y+10);
		if(!ship_smear[num].on){
			if(ColorNybbles[position]!=0){
				ship_smear[num].on=TRUE;
				ship_smear[num].len=0;
				ship_smear[num].start=0;
			}
		}
		else{
			if(ship_smear[num].len>0){
				if(ship_smear[num].offset[ship_smear[num].len-1]!=position){
					ship_smear[num].offset[ship_smear[num].len]=position;
					ship_smear[num].len++;
					if(ship_smear[num].len>30){
						ship_smear[num].on=FALSE;
						ship_smear[num].erase=TRUE;
						ship_smear[num].start=0;
						ship_smear[num].delay=0;
					}
				}
			}
			else{
				ship_smear[num].offset[ship_smear[num].len]=position;
				ship_smear[num].len++;
			}
		}
		if(check_bulls_eye(x+12,y+10)){

		}
	}
	color_cycle_smear(num,ColorNybbles);
	return TRUE;
	
}