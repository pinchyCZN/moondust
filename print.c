#include <windows.h>
#include <fcntl.h> 
#include <io.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include "SDL.h"

extern char line8x8[];

int clrscr(int clear_all)
{
	int i,size;
	HANDLE hcon = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD home = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO con_info;

	con_info.srWindow.Top=0;
	con_info.srWindow.Bottom=30; //set some sane defaults just in case
	GetConsoleScreenBufferInfo(hcon,&con_info);
	home.X=0;
	if(clear_all==0)
		home.Y=con_info.srWindow.Top;
	else
		home.Y=0;
	SetConsoleCursorPosition(hcon,home);
	if(clear_all==0)
		size=con_info.dwSize.X*(con_info.srWindow.Bottom-con_info.srWindow.Top+1);
	else
		size=con_info.dwSize.X*con_info.dwSize.Y;
	FillConsoleOutputAttribute(hcon,FOREGROUND_INTENSITY,size,home,&i);
	FillConsoleOutputCharacter (hcon, ' ',  size, home, &i);
	SetConsoleCursorPosition(hcon,home);
	return con_info.srWindow.Bottom-con_info.srWindow.Top;
}
int get_xy(POINT *p)
{
	HANDLE hcon;
	CONSOLE_SCREEN_BUFFER_INFO conbuf;
	hcon=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hcon,&conbuf);
	if(p!=0)
	{
		//curs pos is zero based
		p->x=conbuf.dwCursorPosition.X;
		p->y=conbuf.dwCursorPosition.Y;
	}
	return TRUE;
}

int goto_xy(POINT p)
{
	HANDLE hcon;
	COORD c;
	hcon=GetStdHandle(STD_OUTPUT_HANDLE);
	c.X=(short)p.x;
	c.Y=(short)p.y;
	SetConsoleCursorPosition(hcon,c);
	return TRUE;
}
int set_screen_buffer_size(int x,int y)
{
	HANDLE hcon;
	COORD size;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	size.X=x;
	size.Y=y;
	SetConsoleScreenBufferSize(hcon,size);
	return TRUE;
}
int set_screen_size(int x, int y)
{
	HANDLE hcon;
	SMALL_RECT size;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	size.Bottom=y-1;
	size.Top=0;
	size.Left=0;
	size.Right=x-1;
	SetConsoleWindowInfo(hcon,TRUE,&size);
	return TRUE;
}

int hex_dump_buffer(const unsigned char *buf,int len)
{
	int i,row,end;
	int width=32;
	for(i=0;i<len;i+=width)
	{
		end=i+width;
		if(end>len)
			end=len;
		for(row=i;row<end;row++)
		{
			printf("%02X",buf[row]);
			if(row==(end-1))
				printf("  ");
			else if((row%4)==3)
				printf("-");
			else
				printf(" ");
		}
		if(end==len)
		{
//			for(row=0;row<(i+width-len);row++)
//				printf("   ");
		}
		for(row=i;row<end;row++)
		{
			unsigned char c;
			c=buf[row];
			if((c<0x20) || (c>0x7E))
				c='.';
//			printf("%c",c);
		}
		printf("\n");
	}
	return 0;
}


static void set_pixel(SDL_Surface *screen,int color,int x,int y)
{
	int offset;

	if(screen==0)
		return;
	if(x<0 || x >= screen->w)
		return;
	if(y<0 || y >= screen->h)
		return;

	offset=y*(screen->pitch / 4);
	offset+=x;
	((unsigned int*)screen->pixels)[offset]=color;
}

void print_str(SDL_Surface *screen,BYTE *str,int x,int y)
{
	int index;
	int i,j;
	int xpos,ypos;
	int offset;
	float scale=2.0f;

	xpos=x;
	ypos=y;
	for(index=0;index<255;index++)
	{
		if(str[index]=='\n')
		{
			xpos=x;
			ypos+=9;
			continue;
		}
		if(str[index]==0)
			break;
		offset=str[index];
		if(offset>='a') //captilize
			offset-=0x20;
		offset=(offset-' ')*8;
		for(i=0;i<8;i++)
		{
			BYTE a;
			a=line8x8[offset+i];
			for(j=0;j<8;j++)
			{
				int xscale,yscale;
				int color;
				if(a&(1<<(8-j)))
					color=0xFFFF;
				else
					color=0;
				yscale=scale;//+0.9f;
				for( ;yscale>0;yscale--)
				{
					xscale=scale;//+0.9f;
					for( ;xscale>0;xscale--)
						set_pixel(screen,color,((j+xpos)*scale)+xscale,((i+ypos)*scale)+yscale);
				}
			}
		}
		xpos+=8;

	}

}

int extended_key=FALSE;
int getkey()
{
	int i=0;
	i=getch();
	extended_key=FALSE;
	if(i==0 || i==0xE0)
	{
		extended_key=TRUE;
		i=getch();
	}
	return i&0xFF;

}
int getkey2()
{
	int i=0;
	if(kbhit())
	{
		i=getch();
		extended_key=FALSE;
		if(i==0 || i==0xE0)
		{
			extended_key=TRUE;
			i=getch();
		}
	}
	return i&0xFF;

}
int check_keyexit()
{
	static int keycheck=0;
	if(((keycheck%10)==0) && getkey2()==0x1b)
		exit(0);
	keycheck++;
}