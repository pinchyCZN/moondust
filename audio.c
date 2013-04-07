#include <windows.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "SDL.h"
#include "SDL_audio.h"
#include "fastsid.h"

extern BYTE SID_SOUND_REGS[];
sound_t *psid;

int watchmem[255];

void mixaudio(void *unused, Uint8 *stream, int len)
{
	int i;
	short *data;
	data=(short*)stream;

	for(i=0;i<0x1D;i++)
		fastsid_hooks.store(psid,i,SID_SOUND_REGS[i]);

	fastsid_hooks.calculate_samples(psid,data,len/2,1,0);
}

int init_audio()
{
    SDL_AudioSpec fmt;

//	return;

	psid=fastsid_hooks.open(SID_SOUND_REGS);
	fastsid_hooks.init(psid,44100,1022730);

    fmt.freq = 44100;
    fmt.format = AUDIO_S16;
    fmt.channels = 1;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = mixaudio;
    fmt.userdata = NULL;

    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        printf("Unable to open audio: %s\n", SDL_GetError());
		return FALSE;
    }
    SDL_PauseAudio(0);
	return TRUE;
}

int close_audio()
{
	fastsid_hooks.close(psid);
    SDL_CloseAudio();
	return TRUE;
}

int write_watch(int index,int data){
	watchmem[index]=data;
}


char *sid_names1[]={
	"noise","pulse","saw","triangle","test","ring","sync","gate"
};


int sid_attack_rates[]={
	2,
	8,
	16,
	24,
	38,
	56,
	68,
	80,
	100,
	250,
	500,
	800,
	1000,
	3000,
	5000,
	8000
};
int sid_release_rates[]={ //also decay
	6,
	24,
	48,
	72,
	114,
	168,
	204,
	240,
	300,
	750,
	1500,
	2400,
	3000,
	9000,
	15000,
	24000
};

int dump_sid()
{
	int i,j,k;
	WORD a;
	float f;
	POINT p;
	static BYTE regs[0x14];

	check_keyexit();


	if(memcmp(regs,SID_SOUND_REGS,sizeof(regs))!=0){
//		p.y=p.x=0;
//		goto_xy(p);
		memcpy(regs,SID_SOUND_REGS,sizeof(regs));
		for(i=0;i<3;i++){
			printf("voice %i:\n",i+1);
			a=SID_SOUND_REGS[i*7]|(SID_SOUND_REGS[i*7+1]<<8);
			f=a*0.0596;
			printf("\tfout=%f\n",f);
			a=SID_SOUND_REGS[i*7+2]|(SID_SOUND_REGS[i*7+3]<<8);
			a&=0xFFF;
			f=a/40.95;
			printf("\tpwidth=%%%f\n\t",f);
			for(j=0;j<8;j++){
				if(SID_SOUND_REGS[i*7+4]&(1<<(7-j)))
					printf("%s,",sid_names1[j]);
			}
			printf("\b                                  \n");
			printf("\tattack =%i   \n",sid_attack_rates[SID_SOUND_REGS[i*7+5]>>4]);
			printf("\tdecay  =%i   \n",sid_release_rates[SID_SOUND_REGS[i*7+5]&0xF]);
			printf("\tsustain=%i   \n",SID_SOUND_REGS[i*7+6]>>4);
			printf("\trelease=%i   \n",sid_release_rates[SID_SOUND_REGS[i*7+6]&0xF]);

		}
		a=(SID_SOUND_REGS[0x15]&7)|(SID_SOUND_REGS[0x16]<<3);
		printf("filter=%i\n",a);
		printf("res/filt=%02X\n",SID_SOUND_REGS[0x17]);
		printf("pass/vol=%02X\n",SID_SOUND_REGS[0x18]);
		for(i=0x15;i<0x1D;i++){
			printf("%02X ",SID_SOUND_REGS[i]);
		}
		printf("\n");
	}
	p.x=0;
	p.y=30;
//	goto_xy(p);
	for(i=0;i<10;i++){
		printf("%i ",watchmem[i]);
	}



}

