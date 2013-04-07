#include <windows.h>
#include <fcntl.h> 
#include <io.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "SDL.h"
#include "gfx.h"
#include "moondust.h"
#include "motion_data.h"


SDL_Surface *g_screen=0;
int screen_colors[4];
int reset_flag=0;
int esc_flag=0;
int key_modify=0;
BYTE VIC_SPRITE_ENABLE;
int VIC_SPRITE_POSREG[8*2];// 0xd000 ;general label
BYTE VIC_SPRITE_COL[8];// 0xd027 ;general label
#define VIC_SPRITE0_COL  VIC_SPRITE_COL[0]
#define VIC_SPRITE1_COL  VIC_SPRITE_COL[1]
#define VIC_SPRITE2_COL  VIC_SPRITE_COL[2]
#define VIC_SPRITE3_COL  VIC_SPRITE_COL[3]
#define VIC_SPRITE4_COL  VIC_SPRITE_COL[4]
#define VIC_SPRITE5_COL  VIC_SPRITE_COL[5]
#define VIC_SPRITE6_COL  VIC_SPRITE_COL[6]
#define VIC_SPRITE7_COL  VIC_SPRITE_COL[7]
BYTE VIC_SPR2SPR_CDET=0;

BYTE SID_SOUND_REGS[255];
#define SID_V1_CONTROL_REG SID_SOUND_REGS[0x4]
#define SID_V2_CONTROL_REG SID_SOUND_REGS[0xB]
#define SID_FILTER_RES_VCTRL SID_SOUND_REGS[0x17]
//color constants

int RGB[]={
0x000000,
0xFFFFFF,
0x68372B,
0x70A4B2,
0x6F3D86,
0x588D43,
0x352879,
0xB8C76F,
0x6F4F25,
0x433900,
0x9A6759,
0x444444,
0x6C6C6C,
0x9AD284,
0x6C5EB5,
0x959595
};
int get_rgb(int c)
{
	if(c>0xF)
		return 0xFF0000;
	return RGB[c&0xF];
}
typedef struct{
	BYTE c;
	int x;
	int y;
	int scale;
	int color;
}OBJECTS;
OBJECTS objects[255];
int enable_walker=FALSE;

//----------------------------------------------
//SOFTWARE DEFINES

int CircleOffset        ;// 0x16
int CircleArcPosOffset  ;// 0x18
BYTE CircleOffsetAdder[3];// 0x1b
BYTE OscilTableOffset_1[3];// 0x1e
BYTE OscilTableOffset_2[3];// 0x21
int CircleRandomSeed    ;// 0x71
int Center_XPosition    ;// 0x78
int Center_YPosition    ;// 0x79
int ProximityLimit      ;// 0x94
int XDistance           ;// 0xc07d
int YDistance           ;// 0xc07e
int MaxPoints=0;// 0xc07f
int MaxPoints_High      ;// 0xc080
int SSmearDistance=0;// 0xc081
int Distance=0;// 0xc084
int Distance_High       ;// 0xc085
BYTE OscilTable[256];// 0x5d00
int CircleDivCounter    ;// 0xc030
int CArcPointsCounter   ;// 0x10
int CircleCntrPointX    ;// 0x5e
int CircleCntrPointY    ;// 0x5f

int BitmapXPos          ;// 0x11
int BitmapXPosMSB       ;// 0x12
int BitmapYPos          ;// 0x13
int BitmapBitpair       ;// 0x14

BYTE SpritePointers[8];// 0x07f8
int YPosition[10];// 0x24
int XPosition[10];// 0x2c
//BYTE XPositionMSB[8];// 0x39

int SpriteDataPointer   ;// 0xc01a
int SpriteDataNumber    ;// 0xc01d
BYTE SpriteDataCopy[63];// 0xc38a
BYTE SpriteDataBuffer[63];// 0xc326
BYTE sprite_mem[63*(2*16+1)];
int CurrentSpriteOffset ;// 0xc018
int OriginalRow         ;// 0x73
int OriginalColumn      ;// 0x72
int ModifiedRow         ;// 0x75
int ModifiedColumn      ;// 0x74
int CurrentRow          ;// 0x77
int CurrentColumn       ;// 0x76
BYTE SpriteLineBitpair[8];// 0x69

int TotalScore;
int TopScore;
int RoundScore=0;// 0xc08c
int RoundScoreHigh      ;// 0xc08d

int Pause_Flag          ;// 0x90

int TextData_Offset     ;// 0x63
int Text_XPosition      ;// 0x65
int Text_YPosition      ;// 0x67
int Text_YPosOverride   ;// 0x68
int NumMoveTextFlag     ;// 0x93
int DisplayNum=0;		// 0x05
int TextRenderEnable    ;// 0x8b
int AutoSwitchToMenu    ;// 0x5e
int AutoSwitchDelay     ;// 0xc07b
int Character_XPos[255];// 0xc0c8
int Character_XPosMSB   ;// 0xc12c
int Character_YPos[255];// 0xc190
BYTE *Character_DDAddr[255];// 0xc57e
int Character_DDOffset  ;// 0x15
int XPos_Subtractor     ;// 0x48
int YPos_Subtractor     ;// 0x49
int TextRefreshCounter  ;// 0x7b
int NumOfCharacters     ;// 0x7d
int InputCharOffset     ;// 0x7e
int OutputCharOffset    ;// 0x7c
int CurrentCharAddrOfs  ;// 0x64
int TitleScreenDelay    ;// 0x57
int BitmapChar_XPos     ;// 0x0c
int BitmapChar_XPosMSB  ;// 0x0d
int BitmapChar_YPos     ;// 0x0e
int GDBitmaskOffset     ;// 0x43
int BytePerRowCounter   ;// 0x42
int MaxBytesPerRow      ;// 0x47
int CharRowCounter      ;// 0x44
int MaxCharRows         ;// 0x46
int CharBitpairCtrl     ;// 0x37

int DecimalDigit        ;// 0xc093
int DecimalNumerals     ;// 0xc08e
int ZeroPrintFlag       ;// 0x55
int EndTermOffset       ;// 0x52

int MainEventDispCntr   ;// 0xc027
int ClearScreenDelay    ;// 0xc009
int LostSeedDispCntr    ;// 0xc010

int MenuSelectMoveDir   ;// 0xc012

BYTE ShiptrailDataOffset[8];// 0xc059
BYTE ShiptrailDataAddr[8];// 0xc1f4
BYTE ShipTurns[8];// 0xc061
BYTE CurrentCommandDelay[8];// 0xc051
BYTE CurrentTurnDelay[8]    ;// 0xc069
BYTE OriginalDelay[8]       ;// 0xc071
int NumberOfShips       ;// 0xc050

BYTE FacingDir[8];// 0x96
BYTE TurningDirFlag[8];// 0x9e
BYTE BorderCollisionFlag[8];// 0xa6
BYTE CommandDisableFlag[8];// 0xae

int ProgModeFlag        ;// 0xc07a
int ClearScreenFlag     ;// 0xc07c

int GameExecCtrlFlag    ;// 0x91
int GameOverFlag        ;// 0x92

int KnocksEnabled       ;// 0x86
int KnockLimit          ;// 0x8a
int NumberOfKnocks      ;// 0xc089
int NumberOfCollisions  ;// 0xc08b
int EvasiveFlag         ;// 0x8c
int EvasionActiveFlag   ;// 0xc02d
int SpinsanityFlag      ;// 0x88
int SeedsRemaining      ;// 0x89
int SWSmearExtent       ;// 0xc016
int MenuSelectorYPos    ;// 0xc013
int MenuSelectorXPos    ;// 0xc014
int MenuSelectEnable    ;// 0x56
BYTE SpinsanityCmdData[128];// 0x5e04
int PreciseFollowFlag   ;// 0x87
int SWalkerXPos         ;// 0xc028
int SWalkerYPos         ;// 0xc02a
int SWalkerXScrnSide    ;// 0xc02b
int SWalkerMSBXPos      ;// 0xc029
int SWalkerYPosAdder    ;// 0xc02c

int SoundChannel_Offset     ;// 0x5700
int SoundChannel_AddrOffset ;// 0x5701
int SoundRegs_BaseOffset    ;// 0x5702
BYTE SoundRegDump[255];     // 0x5705
int BaseEnvDataOffset       ;// 0x574c
int BaseFreqDataOffset      ;// 0x574d
int GenSoundFreqAdder       ;// 0x574e
WORD SoundFrequencyTable[12*8];// 0x59e1
BYTE PulseWidthSeed[3];// 0x7f
int SoundCtrlUpdateFlag     ;// 0x8e
BYTE *AttackDecayDataAddr[3];     ;// 0x5759
BYTE *EnvDelayDataAddr[3];        ;// 0x573d
BYTE CurrentSoundChannel;// 0x5749
BYTE ChordFreqAdders[7];// 0x59da
BYTE EnvDelayDataOffset[3];// 0x573a
BYTE EnvDelayCounter[3];// 0x5743
BYTE EnvCtrlFlag[3];             ;// 0x5762
int UpdateSoundMemoryFlag   ;// 0x574b
BYTE SoundMemory_Envelope[256];// 0x5774
BYTE SoundMemory_Frequency[256];// 0x5874
BYTE SoundMemoryOffset       ;// 0x5746
BYTE SoundMemoryOffsetAdder  ;// 0x5747
BYTE SoundOutDelayCounter    ;// 0x574f
BYTE SoundOutDelayLimit      ;// 0x5750
BYTE FilterRegOutFlag        ;// 0x5751
BYTE PreviousDirBits         ;// 0x5761
BYTE FilterCutoffFreqOffset  ;// 0x575f
BYTE CurrentResonance        ;// 0x5760

BYTE DivideLimit    ;// 0x4a
BYTE FreqDivCounter ;// 0x4b
BYTE TempBitmask    ;// 0x5b
BYTE TempXRegister  ;// 0x62

BYTE BitmapScreen[SCREEN_WIDTH*SCREEN_HEIGHT];	// 0x2000
BYTE ColorNybbles[40*25];		// 0xd800
BYTE CharData[1088];// 0x5000

int SeedSmearDispFlag       ;// 0xc00e
int SeedSmearEraseFlag      ;// 0xc023
WORD SeedSmearAddrTbl[40*25];// 0xc3ee
BYTE SeedSmearOfsEnd;// 0x38
BYTE SeedSmearColorData[256];// 0xc226
BYTE SeedSmearRemaining[8];// 0xc000
BYTE SS_CurrentOffset;// 0xc079
BYTE SSColorData_StartOfs;// 0xc040
int SeedSmearOfsMax=40*20; // 200
int SeedDropAllowFlag       ;// 0xc019
int SeedDropValidator       ;// 0xc088
int BullseyeDisplayFlag     ;// 0xc00a
int BullseyeFlag            ;// 0xc00b

int FrameCounter            ;// 0xc01b
int PreviousColorNybbles=0  ;// 0xc00c
int JoystickDelaySetting    ;// 0xc021
int JoystickDelayCounter    ;// 0xc022
BYTE JoystickDirBits=0;// 0xc00f
BYTE joyfire=0;

int TimeToWaste             ;// 0xc017
int FireButtonDebouncer     ;// 0xc00d
int SeedSmearDiff_Mul16     ;// 0x34
int XRegisterContents       ;// 0x030d
int KnockSpinAndSeedCntr    ;// 0xc08a



int main(){
	int i;
	srand(GetTickCount());
	init_audio();
    CopyCharacterData();
    LoadSprData();
    InitSoundAndIRQs();
    CreateOscilTable();  //do sub to create oscillating table
    Pause_Flag=0;
    TopScore=0;

WarmStartJump:
	TextRenderEnable=1;
	DisableSprites();
	ClearScreenStuff();

    TotalScore=0;         //clear total score
    KnocksEnabled=0;
    SpinsanityFlag=0;
    GameExecCtrlFlag=0;
    GameOverFlag=0;
    SoundCtrlUpdateFlag=0;   //clear sound control reg update flag and continue on here

    InitBitpairsAndCircles();   //do sub to setup bitpairs and some circle-related variables
    ClearScreenStuff();         //do sub to clear color and screen, memory, get seeds 

TitleScreenCore:
//	title_screen();
//	menu();
	game();
	goto TitleScreenCore;
}

int game(int game_mode){
	int i;
	int soundspeed=100;
	char str[255];
	POINT p;

restart:
	set_screen_buffer_size(120,50);
	set_screen_size(120,50);
	clrscr(TRUE);
    InitBitpairsAndCircles();   //do sub to setup bitpairs and some circle-related variables
    ClearScreenStuff();         //do sub to clear color and screen, memory, get seeds 

	game_mode=0;
	switch(game_mode){
	default:
	case 0:SetupBeginner();break;
	case 1:SetupEvasive();break;
	case 2:SetupFreestyle();break;
	case 3:SetupSpinsanity();break;
	}
	ResetSound();
	ProgModeFlag=TRUE;
	enable_walker=TRUE;
	DisableSprites();
	EnableSprites();
	InitSpacewalkerAndShips();
	reset_ship_smear();
	while(TRUE){
		circle_effect();
		FrameCounter++;
		PauseLoop();                //sit in an endless loop if game is paused
		if(SeedSmearDispFlag){
			SeedSmearColorCycle();
			CircleRenderCore();
			if(BullseyeFlag)
				CircleRenderCore();
			
		}
		if((FrameCounter&1)==0){
			CtrlSpacewalkerAndShips();
		}
		if((FrameCounter&0x3F)==0){
//			ManipulateFreqEnvOfs();
		}
		if((FrameCounter%soundspeed)==0)
			ManipulateFreqEnvOfs();
		if(joyfire)
			ManipulateFreqEnvOfs();
		soundspeed+=key_modify;
		if(soundspeed<=0)
			soundspeed=1;
		p.y=p.x=0;
		goto_xy(p);	
		printf("\nsoundspeed=%6i FrameCounter=%6i\n"
			"BaseFreqDataOffset=%3i\n"
			"BaseEnvDataOffset=%3i\n"
			"SoundOutDelayCounter=%3i\n"
			"CurrentSoundChannel=%3i\n"
			"ChordFreqAdders=%3i %3i %3i %3i\n"
			"SoundMemoryOffset=%3i\n"
			"SoundMemoryOffsetAdder=%3i\n"
			"GenSoundFreqAdder=%3i\n"
			"SoundOutDelayLimit=%3i\n"
			"SoundCtrlUpdateFlag=%3i\n"
			,soundspeed,FrameCounter,BaseFreqDataOffset,BaseEnvDataOffset,
			SoundOutDelayCounter,
			CurrentSoundChannel,
			ChordFreqAdders[0],ChordFreqAdders[1],ChordFreqAdders[2],ChordFreqAdders[3],// 4
			SoundMemoryOffset,
			SoundMemoryOffsetAdder,
			GenSoundFreqAdder,
			SoundOutDelayLimit,
			SoundCtrlUpdateFlag
			);
		hex_dump_buffer(SoundMemory_Envelope,100);
		hex_dump_buffer(SoundMemory_Frequency,100);
		
		for(i=0;i<8;i++){
			SpriteObjMovementCore(i);
			smear_dust(i,ColorNybbles,XPosition[i],YPosition[i]);
		}

		ScreenDissolve(120);
		video_test();
		do{
			keys();
		}while(Pause_Flag!=0);
		if(reset_flag){
			goto restart;
		}
		SoundEngineCore();
	}

}

BYTE SelectorChar[]={
    0x80, //X.......
    0xC0, //XX......
    0xE6, //XXX..XX.
    0xF9, //XXXXX..X
    0xF9, //XXXXX..X
    0xE6, //XXX..XX.
    0xC0, //XX......
    0x80  //X.......
};
int menu()
{
	char str[80];
	int position=0,selection=0,maxobject;
	DWORD obj;
	int frame_counter=0;

	ClearScreenStuff();
restart:
	maxobject=0;
	maxobject+=add_str_objects(objects+maxobject,"CHOOSE PLAY STYLE",8,20,0,2);
	maxobject+=add_str_objects(objects+maxobject,"BEGINNER",60,55,0,2);
	maxobject+=add_str_objects(objects+maxobject,"SPINSANITY",60,145,0,2);
	maxobject+=add_str_objects(objects+maxobject,"FREESTYLE",60,115,0,2);
	maxobject+=add_str_objects(objects+maxobject,"EVASIVE",60,85,0,2);
	maxobject+=add_str_objects(objects+maxobject,"TOP SCORE",3,180,0,2);

	DisableSprites();
	memset(BitmapScreen,0,sizeof(BitmapScreen));
	do{
		if(TRUE){
			obj=obj%maxobject;
			render_objects(BitmapScreen,objects+obj,1);
			obj++;

			/*
			render_str(BitmapScreen,"CHOOSE PLAY STYLE",8,20,0,2);
			render_str(BitmapScreen,"BEGINNER",60,55,0,2);
			render_str(BitmapScreen,"EVASIVE",60,85,0,2);
			render_str(BitmapScreen,"FREESTYLE",60,115,0,2);
			render_str(BitmapScreen,"SPINSANITY",60,145,0,2);
			render_str(BitmapScreen,"TOP SCORE",3,180,0,2);
			*/
		}
		render_bitmap(BitmapScreen,SelectorChar,40,position+55,0,2,1,8);
		frame_counter++;
		frame_counter=frame_counter%1;
		ScreenDissolve(40);

		if(position<selection*30)
			position++;
		else if(position>selection*30)
			position--;
		if(JoystickDirBits & JOY_DOWN){
			if(position==selection*30)
				selection++;
			if(selection>=4)
				selection=3;
		}
		if(JoystickDirBits & JOY_UP){
			if(position==selection*30)
				selection--;
			if(selection<0)
				selection=0;
		}


		video_test();
		do{
			keys();
		}while(Pause_Flag!=0);
		if(check_fire_trigger())
			break;
		if(reset_flag){
			goto restart;
		}
	}while(TRUE);

	return selection;
}


int UpdateVICSpriteRegs(BYTE num)
{
	VIC_SPRITE_POSREG[num*2]=XPosition[num];
	VIC_SPRITE_POSREG[num*2+1]=YPosition[num];
}
BYTE RegBitmask[8]={
	1,2,4,8,16,32,64,128
};

//facing direction table based on joystick direction
//note: not all values are valid
BYTE FacingDirTbl[]={
    0x01, 0x01, 0x01, 0x01, 0x01, 0x06, 0x02, 0x04,
    0x01, 0x0a, 0x0e, 0x0c, 0x01, 0x08, 0x00, 0x01
};


//X coordinate adders used for spacewalker head
short HeadXPosAdders[]={
    0x0000, 0x0008, 0x000c, 0x000f, 0x0011, 0x000f, 0x000c, 0x0008,
    0x0000, 0xfff8, 0xfff4, 0xfff1, 0xffef, 0xfff1, 0xfff4, 0xfff8
};
//Y coordinate adders used for spacewalker head
char HeadYPosAdders[]={
    0xf0, 0xf1, 0xf4, 0xf8, 0x00, 0x08, 0x0c, 0x0f,
    0x10, 0x0f, 0x0c, 0x08, 0x00, 0xf8, 0xf4, 0xf1
};
char YPosDirAdders[]={
    0xfe, 0xfe, 0xfe, 0xff, 0x00, 0x01, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x01, 0x00, 0xff, 0xfe, 0xfe
};
short XPosDirAdders[]={
    0x0000, 0x0001, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001,
    0x0000, 0xffff, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xfffe, 0xffff
};

int BitmapScreenOutput(BYTE pixel)
{
	int i;
	i=BitmapXPos+BitmapYPos*SCREEN_WIDTH;
	if((i-SCREEN_WIDTH)>=sizeof(BitmapScreen))
		return;
	BitmapScreen[i]=pixel;
	BitmapScreen[i+SCREEN_WIDTH]=pixel;
}

BYTE StripedBitmasks[]={
	0x00,0x55,0xAA,0xFF
};


int reverse_bits(BYTE v)
{
	BYTE r = v; // r will be reversed bits of v; first get LSB of v
	BYTE s = sizeof(v) * 8 - 1; // extra shift needed at end
	
	for (v >>= 1; v; v >>= 1)
	{   
		r <<= 1;
		r |= v & 1;
		s--;
	}
	r <<= s; // shift when v's highest bits are zero
	return r;
}
int HorizontalFlip(BYTE *data,int x,int y){
	int i,j;
	BYTE a,b;
	for(i=0;i<y;i++){
		for(j=0;j<x;j++)
			data[i*x+j]=reverse_bits(data[i*x+j]);
		for(j=0;j<(x>>1);j++){
			a=data[i*x+j];
			b=data[i*x+x-1-j];
			data[i*x+j]=b;
			data[i*x+x-1-j]=a;
		}
	}
}
int VerticalFlip(BYTE *data,int x,int y)
{
	int i,j;
	BYTE a,b;
	for(i=0;i<x;i++){
		for(j=0;j<(y>>1);j++){
			a=data[i+j*x];
			b=data[i+(y-1-j)*x];
			data[i+j*x]=b;
			data[i+(y-1-j)*x]=a;			
		}
	}
}
int DiagonalFlip(BYTE *data,int x,int y)
{
	int i,j,k;
	BYTE *tmp;
	BYTE a,b;
	tmp=malloc(x*y);
	if(tmp==0)
		return;
	memset(tmp,0,x*y);
	for(i=0;i<y;i++){
		for(j=0;j<x;j++){
			a=0;
			k=0;
			for(k=0;k<8;k++){
				b=data[x*(y-1-k)-j*x*8+i/8]>>(7-(i%8));
				b&=1;
				a<<=1;
				a|=b;
			}
			tmp[i*x+j]=a;
		}
	}
	memcpy(data,tmp,x*y);
	free(tmp);
	
}


//----------------------------------------------

int SetSpritePointer(BYTE x){
	if(!enable_walker){
		SpritePointers[x]=FacingDir[x]; //32
	}
	else{
		if(x==0)
			SpritePointers[0]=32; //15; //space walker head
		else if(x==1)
			SpritePointers[x]=FacingDir[x]+16; //+48;
		else 
			SpritePointers[x]=FacingDir[x]; //32
	}
}
 
/*
//----------------------------------------------

//----------------------------------------------
//0x05 - used to store contents of X from whatever we jumped here from
//0x07 - used as bitpair byte to fill in bitmap screen
//0x03-0x04 - used to hold pseudorandom bitmap screen address
*/
int ScreenDissolve(int rate)
{
	int i,j,offset;
	for(i=0;i<rate;i++){
		offset=_rand()%sizeof(BitmapScreen);
		for(j=0;j<4;j++)
			BitmapScreen[(offset+j)%sizeof(BitmapScreen)]=0;
	}
}

int DrawShiptrail(BYTE num){
	int color=(num%3)+1;
	render_pixel(BitmapScreen,color,XPosition[num]+10,YPosition[num]+10);
	render_pixel(BitmapScreen,color,XPosition[num]+10,YPosition[num]+10+1);
	render_pixel(BitmapScreen,color,XPosition[num]+10+1,YPosition[num]+10);
	render_pixel(BitmapScreen,color,XPosition[num]+10+1,YPosition[num]+10+1);
}
extern char line8x8[];
extern char line8x8end[];
int CopyCharacterData(void){
	memcpy(CharData,line8x8,line8x8end-line8x8);
//	memset(CharData,0,8);
}

int GetRandomNumber(){
	static BYTE LSFR_Variable=0;
	BYTE i;
    i=LSFR_Variable;    //load linear shift feedback register
    i>>1;//shift to the right once
    i^=LSFR_Variable;    //perform XOR on current vs unshifted
    i^=rand()&0xFF; //CIA1_TIMERA_LOW;  //perform another XOR on result vs CIA timer A low
    LSFR_Variable=i;    //store result here
    return i;
}

int SetupTitleSprColors(){
	int i;
	for(i=0;i<8;i++){
		VIC_SPRITE_COL[i]=i;
	}
	VIC_SPRITE0_COL=LightGreen;
}

int InitSpacewalkerAndShips()
{
	int i;
	BYTE a;
	Center_XPosition=163;      //set central coordinates in memory
	Center_YPosition=92;
	InitSeedSmearStuff();    //init seed smear offset and get central color nybble address
	
	VIC_SPRITE0_COL=White;       //to white (this part not necessary)
	VIC_SPRITE1_COL=White;
	
	a=GetRandomNumber()&0xF;
	
	for(i=0;i<8;i++){
		XPosition[i]=Center_XPosition;
		YPosition[i]=Center_YPosition;
		FacingDir[i]=a;
		SetSpritePointer(i,TRUE);
		a+=3;
		a=a&0xF;
		TurningDirFlag[i]=0;
	}
	
	CircleOffset=5;
	
	RandomizeSSColorData();  //get random color data to use for seed smear later
}
int ClearScreenStuff(){
	BYTE i;

	for(i=0;i<4;i++)
		screen_colors[i]=((rand()%16)+1);
	screen_colors[0]=0;

    ClearColorNybbles();
	memset(BitmapScreen,0,sizeof(BitmapScreen));
    i=GetVMColors();           //get two random colors in nybbles
    JoystickDelayCounter=JoystickDelaySetting; //move delay setting connected to game mode
    FrameCounter=0;
    TimeToWaste=0;
    SeedSmearDispFlag=0;
    SeedSmearOfsEnd=0;
    OutputCharOffset=0;
    MainEventDispCntr=0;
    BullseyeFlag=0;
    SeedSmearEraseFlag=0;
    NumMoveTextFlag=0;
    CharBitpairCtrl=0;
    ClearScreenDelay=0;
    SeedDropAllowFlag=0;
    NumOfCharacters=0;
    InputCharOffset=0;
    ClearScreenFlag=0;
    SoundCtrlUpdateFlag=0;
    i=GetRandomNumber();     //get a random number
    CircleRandomSeed=i;    //save it here to be used later
	return i;
}

//----------------------------------------------

int InitBitpairsAndCircles(){
	int i;
	BYTE a;
	a=RandomNumberZero2Two();
	for(i=0;i<8;i++){
		SpriteLineBitpair[i]=RandomNumberZero2Two()+1;//StripedBitmasks[a];
		OscilTableOffset_1[a]=0;
		OscilTableOffset_2[a]=0;
		CircleOffsetAdder[a]=GetRandomNumber();
		a++;
		a=a%3;
	}

}

//----------------------------------------------
int BorderCollisionDetect(BYTE num)
{
	BorderCollisionFlag[num]=0;
	if(YPosition[num]<21){
		BorderCollisionFlag[num]|=4;
	}
	else if(YPosition[num]>170){
		BorderCollisionFlag[num]|=8;
	}
	if(XPosition[num]<24){
		BorderCollisionFlag[num]|=1;
	}
	else if(XPosition[num]>=298){
		BorderCollisionFlag[num]|=2;
	}

}
int RotateSpriteObj(BYTE num)
{
	BYTE a;
	if(TurningDirFlag[num]){
		if(FacingDir[num]==0xF)
			FacingDir[num]=0;
		else
			FacingDir[num]++;
	}
	else{
		if(FacingDir[num]==0)
			FacingDir[num]=0xF;
		else
			FacingDir[num]--;
	}
	SetSpritePointer(num);
}
int MoveSpriteObjs(BYTE num)
{
	BYTE a;
	if(ProgModeFlag)
		if(num==0) //head
			return;
	XPosition[num]+=XPosDirAdders[FacingDir[num]];
	YPosition[num]+=YPosDirAdders[FacingDir[num]];
	if(ProgModeFlag)
		if(num==1){ //body position sets head position below
			if(GetJoyDirectionBits()){
				YPosition[0]=YPosition[1]+HeadYPosAdders[FacingDirTbl[JoystickDirBits^0xF]];
				XPosition[0]=XPosition[1]+HeadXPosAdders[FacingDirTbl[JoystickDirBits^0xF]];
			}
			else{
				YPosition[0]=YPosition[1]+HeadYPosAdders[FacingDir[1]];
				XPosition[0]=XPosition[1]+HeadXPosAdders[FacingDir[1]];
			}
		}
}


int SpriteObjMovementCore(BYTE num)
{
    BorderCollisionDetect(num);  //do sub to handle sprite position range-checking
	if(BorderCollisionFlag[num]!=0){
		RotateSpriteObj(num);
		CommandDisableFlag[num]=1;
	}
	else
		CommandDisableFlag[num]=0;

	MoveSpriteObjs(num);
	UpdateVICSpriteRegs(num);
	DrawShiptrail(num);
}
int DisableSprites(){
	int i;
	for(i=0;i<8;i++){
		VIC_SPRITE_COL[i]=Black;
	}
	VIC_SPRITE_ENABLE=0;	//sprites off
}
int EnableSprites(){
	VIC_SPRITE_ENABLE=0xFF;
	SetupTitleSprColors();
}

//----------------------------------------------

int PauseLoop(){
	if(Pause_Flag)
		return;
}

//----------------------------------------------

int GetRandomColor(){
	BYTE i;
	do{
		i=GetRandomNumber();     //get random number
		i&=0xF;			         //mask out all but lower nybble
		//if 0-1 (black or white), loop back
	}while(i==DarkGrey || i==MidGrey || i==0 || i==1 || i==PreviousColorNybbles);
	PreviousColorNybbles=i;
	return i;
}

int GetVMColors(){
	BYTE i,j;
    i=GetRandomColor();      //set first seed at PreviousColorNybbles
    j=GetRandomColor();      //set second seed again
	i=i<<4;
    i|=PreviousColorNybbles; //save second seed to lower nybble
	return i;
}

BYTE OriginalFacingDir[]={
    0x00, 0x07, 0x09, 0x00, 0x00, 0x00, 0x04, 0x00
};
BYTE SprOriginalX[]={
    0x1a, 0x9e, 0xac, 0xd8, 0x2e, 0x76, 0xb6, 0xf8
};
BYTE SprOriginalY[]={
    0x4e, 0x38, 0x30, 0x4e, 0x90, 0x84, 0x8c, 0x92
};
int title_screen()
{
	int i;
	int soundspeed=100;
	DWORD frame_counter,delay;
	int maxobject,obj;
	int last_fire=0;
restart:
	ResetSound();

	ClearScreenStuff();
	DisableSprites();
	frame_counter=0;
	delay=0;
	obj=maxobject=0;
	maxobject+=add_str_objects(objects+maxobject,"CREATIVE",80,80,0,2);
	maxobject+=add_str_objects(objects+maxobject,"SOFTWARE",80,110,0,2);
	maxobject+=add_str_objects(objects+maxobject,"PRESENTS",80,140,0,2);
	do{
		SoundEngineCore();
		if((frame_counter%soundspeed)==0)
			ManipulateFreqEnvOfs();
		if((frame_counter%2)==0){
			obj=obj%maxobject;
			render_objects(BitmapScreen,objects+obj,1);
			obj++;
		}
		frame_counter++;
		delay++;
		ScreenDissolve(40);
		video_test();
		do{
			keys();
		}while(Pause_Flag!=0);
		if(check_fire_trigger())
			break;
		if(reset_flag || esc_flag){
			goto restart;
		}
		if(delay>60*3)
			break;
		soundspeed+=key_modify;
	}while(TRUE);
restart2:
	for(i=0;i<8;i++){
		XPosition[i]=SprOriginalX[i];
		YPosition[i]=SprOriginalY[i];
		FacingDir[i]=OriginalFacingDir[i];
		SetSpritePointer(i,FALSE);
	}
	frame_counter=0;
	delay=0;
	enable_walker=FALSE;
	init_moondusttrail();
	EnableSprites();
	obj=maxobject=0;
	maxobject+=add_str_objects(objects+maxobject,"BY JARON LANIER",30,180,0,2);
	do{
		SoundEngineCore();
		if((frame_counter%soundspeed)==0)
			ManipulateFreqEnvOfs();

		if((frame_counter%1)==0){
			moondust_trail(LetterDataAddrTbl);
			for(i=0;i<8;i++){
				SpriteObjMovementCore(i);
				UpdateVICSpriteRegs(i);
			}
		}
		if(delay<60*4 && (frame_counter%2)==0){
			obj=obj%maxobject;
			render_objects(BitmapScreen,objects+obj,1);
			obj++;
		}
		frame_counter++;
		delay++;
		ScreenDissolve(80);
		video_test();
		do{
			keys();
		}while(Pause_Flag!=0);
		if(check_fire_trigger())
			break;
		if(reset_flag)
			goto restart;
		if(esc_flag)
			goto restart2;
		soundspeed+=key_modify;
		write_watch(1,soundspeed);

	}while(TRUE);
}

int moondust_trail(BYTE *motion_data[]){
	int i;
	for(i=0;i<8;i++){
		if(CurrentCommandDelay[i]!=0){
			CurrentCommandDelay[i]--;
			continue;
		}
		if(ShipTurns[i]!=0){
			if(CurrentTurnDelay[i]!=0){
				CurrentTurnDelay[i]--;
				continue;
			}
			CurrentTurnDelay[i]=OriginalDelay[i];
			ShipTurns[i]--;
			RotateSpriteObj(i);
			if(ShipTurns[i]==0)
				CurrentCommandDelay[i]=OriginalDelay[i];

		}
		else{
			if(CommandDisableFlag[i]==0){
				int quit=FALSE;
				do{
					switch(motion_data[i][ShiptrailDataOffset[i]++]){
					case 1:
						RotateSpriteObj(i);
						break;
					case 2:
						TurningDirFlag[i]^=0xFF;
						break;
					case 3:
						ShiptrailDataOffset[i]=0;
						break;
					case 4:
						CurrentCommandDelay[i]=motion_data[i][ShiptrailDataOffset[i]++];
						quit=TRUE;
						break;
					case 5:
						ShipTurns[i]=motion_data[i][ShiptrailDataOffset[i]++];
						OriginalDelay[i]=CurrentTurnDelay[i]=motion_data[i][ShiptrailDataOffset[i]++];
						quit=TRUE;
						break;

					}
				}while(!quit);
			}
		}

	}
}
int	init_moondusttrail(){
	int i;
	for(i=0;i<8;i++){
		ShiptrailDataOffset[i]=0;
		CurrentCommandDelay[i]=0;
		ShipTurns[i]=0;
		TurningDirFlag[i]=0xFF;
	}
	TurningDirFlag[2]=0;
}
int check_fire()
{
	if(joyfire)
		return TRUE;
	else 
		return FALSE;
}
int check_fire_trigger()
{
	static int fire=0;
	if(fire==0 && joyfire){
		fire=1;
		return TRUE;
	}
	else if(!joyfire){
		fire=0;
		return FALSE;
	}
	return FALSE;
}
 


int SetupSpinsanityCmdData()
{
	int i,offset;
	offset=0;
	for(i=0;i<7;i++){
		SpinsanityCmdData[offset++]=5;
		SpinsanityCmdData[offset++]=GetRandomNumber()&0xF;
		SpinsanityCmdData[offset++]=RandomNumberZero2Two();
		SpinsanityCmdData[offset++]=4;
		SpinsanityCmdData[offset++]=GetRandomNumber()&3;
	}
	SpinsanityCmdData[offset++]=3; //end command
}


int GetColorNybbleofPos(BYTE x){
	int a,b;
	a=YPosition[x];
	a=a/8*40;
	b=XPosition[x];
	b=b/8;
	SeedSmearAddrTbl[SeedSmearOfsEnd]=a+b;
	return ColorNybbles[a+b]&0xF;
}



int render_pixel(BYTE *screen,BYTE color,int x,int y)
{
	if((y*SCREEN_WIDTH+x)<0)
		return;
	if((y*SCREEN_WIDTH+x)<SCREEN_WIDTH*SCREEN_HEIGHT)
		screen[y*SCREEN_WIDTH+x]=color;
	//if(y<SCREEN_HEIGHT-1)
	//	screen[(y+1)*2*SCREEN_WIDTH+x]=color;
}
int render_bitmap(BYTE *screen,BYTE *bits,int x, int y,int color,int scale,int width,int height){
	int i,j,m,n,rc;
	for(i=0;i<height;i++){
		for(j=0;j<width*8;j++){
			if(color==0)
				rc=(GetRandomNumber()%3)+1;
			else
				rc=color;
			for(m=0;m<scale;m++){
				for(n=0;n<scale;n++){
					if(bits[i*width+(j/8)]&(1<<(7-(j%8)))){
						render_pixel(screen,rc,x+scale*j+m,y+scale*i+n);
					}
				}
			}
		}
	}
}
int render_char(BYTE *screen,BYTE c,int x, int y,int color,int scale)
{
	int i,j,m,n,rc;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			if(color==0)
				rc=(GetRandomNumber()%3)+1;
			else
				rc=color;
			for(m=0;m<scale;m++){
				for(n=0;n<scale;n++){
					if(CharData[c*8+i]&(1<<(7-j))){
							render_pixel(screen,rc,x+scale*j+m,y+scale*i+n);
					}
				}
			}
		}
	}
}
int render_str(BYTE *screen,BYTE *str,int x,int y,int color,int scale)
{
	int i;
	int xpos=0;
	strupr(str);
	for(i=0;i<255;i++){
		if(str[i]==0xFF || str[i]==0)
			break;
		if(str[i]=='\n'){
			xpos=0;
			y+=8;
			continue;
		}
		if(str[i]<' ')
			continue;
		if(str[i]>=0x60)
			continue;
		render_char(screen,str[i]-0x20,x+xpos,y,color,scale);
		xpos+=8*scale;
	}
}
int render_objects(BYTE *screen,OBJECTS *objs,int count)
{
	int i;
	for(i=0;i<count;i++){
		render_char(screen,objs[i].c&0x1F,objs[i].x,objs[i].y,objs[i].color,objs[i].scale);
	}
}
int add_str_objects(OBJECTS *objects,BYTE *str,int x,int y,int color,int scale)
{
	int i;
	for(i=0;i<80;i++){
		if(str[i]==0xFF || str[i]==0)
			break;
		objects[i].c=str[i];
		objects[i].x=x+i*8*scale;
		objects[i].y=y;
		objects[i].color=color;
		objects[i].scale=scale;
	}
	return i;
}





BYTE VortexChar[]={
    0x00, 0x70, //.........XXX....
    0x01, 0xd8, //.......XXX.XX...
    0x07, 0x04, //.....XXX.....X..
    0x06, 0x00, //.....XX.........
    0x0c, 0x00, //....XX..........
    0x0c, 0x38, //....XX....XXX...
    0x06, 0x7c, //.....XX..XXXXX..
    0x03, 0xe6, //......XXXXX..XX.
    0x81, 0x83, //X......XX.....XX
    0x80, 0x83, //X.......X.....XX
    0xc0, 0xc3, //XX......XX....XX
    0x41, 0xc2, //.X.....XXX....X.
    0x63, 0x86, //.XX...XXX....XX.
    0x3e, 0x04, //..XXXX.......X..
    0x0c, 0x08, //....XX......X...
    0x00, 0x10 //...........X....
};




int AddNewSeedSmear(BYTE x)
{
	if(SeedSmearOfsEnd>=SeedSmearOfsMax){
		SeedSmearEraseFlag=1;
		ProcDistancePoints();
	}
	GetColorNybbleofPos(x);
	CheckForBullseye();
	SeedSmearOfsEnd++;
	ManipulateFreqEnvOfs();
}
int ProcKnocks_SmearSpread(BYTE x)
{
	if(SeedSmearEraseFlag)
		return;
	smear_dust(x,ColorNybbles,XPosition,YPosition);
	return;
	if(MainEventDispCntr)
		return;
	if(x==0) //space walker head
		return;
	if(x==1){ //space walker body
		if(KnockSpinAndSeedCntr!=0){
			KnockSpinAndSeedCntr--;
			RotateSpriteObj(x);
			RotateSpriteObj(x);
			PulseWidthSeed[0]=GetRandomNumber();
			PulseWidthSeed[1]=GetRandomNumber();
			return;
		}
	}
	if(ClearScreenDelay)
		return;
	VIC_SPRITE0_COL=White;
	if(VIC_SPR2SPR_CDET&1){ //head collision
		NumberOfCollisions++;
		if(NumberOfCollisions==2){
			if((VIC_SPR2SPR_CDET&0x2)==0){ //no body collision
				if(BorderCollisionFlag[0]==0 && BorderCollisionFlag[1]==0){
					KnockHandler();
				}
			}
			NumberOfCollisions=0;
		}
	}
	else
		NumberOfCollisions=0;

	if(GetColorNybbleofPos(x)!=Black){
		if(SeedSmearRemaining[x]!=0)
			return;
		if(x==1)//space walker body
			SeedSmearRemaining[x]=SWSmearExtent;
		else
			SeedSmearRemaining[x]=0x18;
		return;
	}
	if(SeedSmearRemaining[x]!=0){
		SeedSmearRemaining[x]--;
		if(SeedSmearRemaining[x]>=8){
			if(SeedSmearEraseFlag==0){
				CheckSeedSmearVSPos(x);
			}
			EvasionActiveFlag=0;
			AddNewSeedSmear(x);
		}
	}
}
int PrintTotal_TopScores()
{
}
/*
        jsr GetRandomBitpair     //do sub to get a bitpair
        lda TotalScoreLow
        sta DisplayNumLow
        lda TotalScoreHigh       //put total score into display number variables
        sta DisplayNumHigh       //used to print the number
        lda #0x0e
        sta TextRefreshCounter
        lda #0x68
        sta Text_XPosition
        jsr PrintNumerals_YOvrd  //do sub to print the number
        lda #0x04                 //get text number to print " TOTAL"
        jsr ProcessCharInfo      //do sub to process game text character data information
        lda GameExecCtrlFlag
        beq ExPTTS               //if game execution control flag not set, branch to leave
        lda TopScoreLow
        sta DisplayNumLow        //otherwise put top score into display number variables
        lda TopScoreHigh
        sta DisplayNumHigh
        lda #0x46                 //setup display coordinates
        sta Text_XPosition
        lda #0x0a
        sta Text_YPosition
        lda #0x00
        sta Text_XPositionMSB
        jsr PrintNumerals        //do sub to print the number
        lda #0x13                 //get text number to print "TOP SCORE "
        jmp ProcessCharInfo      //do sub to process game text character data information
ExPTTS: rts

//----------

PrintBullseyeMsg:
    jsr GetRandomBitpair      //do a sub to obtain a bitpair
    lda MaxPoints_Low         //copy max points for this round
    sta RoundScoreLow         //to score for this round
    lda MaxPoints_High
    sta RoundScoreHigh
    jsr AddRoundPtsToTotal    //do sub to add score for this round to total score
    lda #0x0e
    sta TextRefreshCounter    //set text frame_counter
    lda #0x53
    sta Text_XPosition        //set X position
    sta BullseyeDisplayFlag   //set bullseye-related flag here
    jsr SetYOverrideAndInit   //do sub to set Y position appropriately
    lda #0x06                  //load text number here to print "BULLSEYE!"
    jmp ProcessCharInfo       //do sub to process game text character data information, do not return

//-----------
*/
int PrintRoundScore()
{
	DisplayNum=RoundScore;
	TextRefreshCounter=0xE;
	Text_YPosOverride=0xA;
	Text_XPosition=0x5F;
	printf("POINTS  PrintRoundScore\n",RoundScore);
}

int EventDisplayHandler()
{
	EraseCurrentSSmear();
	CircleCntrPointY++;
	if(CircleCntrPointY&1){
		MainEventDispCntr--;
		if(MainEventDispCntr==0){
			printf("EventDisplayHandler bullseye message\n");
		}
		else if(MainEventDispCntr==0xAA){
			printf("EventDisplayHandler PrintRoundScore\n");
		}
		else if(MainEventDispCntr==0x50){
			printf("EventDisplayHandler PrintRoundScore\n");
		}
		else if(MainEventDispCntr==0x1){
			ClearScreenFlag=1;
		}
		else{
			FireButtonCtrlDelay();
			MainEventDispCntr=MainEventDispCntr;
		}

	}

}


int LostSeedDispHandler()
{
	EraseSSHandler();
	LostSeedDispCntr--;
	if(LostSeedDispCntr==0xAA){
		PrintRoundScore();
	}
	else if(LostSeedDispCntr==0x50){
		PrintTotal_TopScores();
	}
	FireButtonCtrlDelay(LostSeedDispCntr);
	if(LostSeedDispCntr<=8)
		TextRefreshCounter=LostSeedDispCntr;
}
int ClrScnDelayHandler()
{
	ClearScreenDelay--;
	if(ClearScreenDelay==0){
		ClearScreenFlag=1;
	}
}

BYTE BullseyeChar[]={
    0x3C, //..XXXX..
    0x7E, //.XXXXXX.
    0xE7, //XXX..XXX
    0xDB, //XX.XX.XX
    0xDB, //XX.XX.XX
	0xE7, //XXX..XXX
    0x7E, //.XXXXXX.
    0x3C  //..XXXX..
};

BYTE *spinsanity_motion[8]={
	SpinsanityCmdData,SpinsanityCmdData,SpinsanityCmdData,SpinsanityCmdData,
	SpinsanityCmdData,SpinsanityCmdData,SpinsanityCmdData,SpinsanityCmdData
};
int ProcessSprObjTurns(){
	int i;
	for(i=0;i<8;i++){
		if(CommandDisableFlag[i]==0){
			if(BorderCollisionFlag[i]==0){
				if(PreciseFollowFlag){
					if(FacingDir[i]!=FacingDirTbl[(JoystickDirBits&0xF)^0xF]){
						RotateSpriteObj(i);
					}
				}
				else{
					if(FacingDir[1]!=FacingDirTbl[(JoystickDirBits&0xF)^0xF]){
						RotateSpriteObj(i);
					}
				}
			}
		}
	}

}
int CtrlSpacewalkerAndShips()
{	
	int i;
	FireButtonGameMechs();
	if(GetJoyDirectionBits()){
		ProcessSprObjTurns();
	}
	else{
		if(SpinsanityFlag)
			moondust_trail(spinsanity_motion);
	}
	if(MainEventDispCntr!=0){
		EventDisplayHandler();
		return;
	}
	if(ClearScreenDelay!=0){
		ClrScnDelayHandler();
		return;
	}
	if(LostSeedDispCntr!=0){
		LostSeedDispHandler();
		return;
	}
	if(SeedSmearDispFlag==0){

		CheckForGameOver();
		return;
	}
	SeedDropAllowFlag=0;
	if((FrameCounter&16)!=0){
		PrintBullseyeAtCenter();
	}
	if((FrameCounter&7)!=0){
		EvasiveGameMechs();
	}
	if(SeedSmearEraseFlag!=0){
		LostSeedDispCntr=0xAB;
		return;
	}
	if(SeedSmearEraseFlag!=0){
		SeedSmearDispFlag=0;
		SeedSmearEraseFlag=0;
		SeedSmearOfsEnd=0;
		VIC_SPRITE0_COL=White;
		VIC_SPRITE1_COL=White;
		SoundCtrlUpdateFlag=0;
		ClearColorNybbles();
		return;
	}

}

int EraseSSHandler()
{
	EraseSeedSmear();
	if(SeedSmearEraseFlag!=3){
		EraseSeedSmear();
	}
}

int GameOverDisplay()
{
	TextRenderEnable=1;
	if(TopScore<TotalScore)
		TopScore=TotalScore;
	Text_XPosition=0x14;
	Text_YPosOverride=0x54;
	SetYOverrideAndInit();
	//render_string("game over");
	MainEventDispCntr=0xA9;
	GameExecCtrlFlag++;
}

int CheckForGameOver(){
	if(BullseyeDisplayFlag!=0 || SeedsRemaining!=0){
		SeedDropHandler();
		return;
	}
	if(GameOverFlag!=0){
		GameOverDisplay();
		return;
	}
	GameOverFlag++;
	ClearScreenFlag=1;
}

int SeedDropHandler()
{
	if(joyfire){
 		if(SeedDropAllowFlag!=0){
			SeedSmearOfsEnd=0;
			NumberOfKnocks=0;
			NumberOfCollisions=0;
			SS_CurrentOffset=0;
			ClearScreenDelay=0;
			KnockSpinAndSeedCntr=0;
			//ProcMaxScoreAndSeeds(1);
			if(seed_drop(ColorNybbles,XPosition[1],YPosition[1]))
				SeedDropAllowFlag=0;
			SeedSmearDiff_Mul16=7;
			if(SeedDropValidator!=0){
				SeedSmearDispFlag=0xFF;
				InitSeedSmearRemain();
			}
		}
	}
	else{
		SeedDropAllowFlag=1;
	}
}

int GetJoyDirectionBits(){
	JoystickDirBits&=0xF;
	if(JoystickDirBits!=0)
		return TRUE;
	else
		return FALSE;
}


int ClearColorNybbles()
{
	memset(ColorNybbles,0,sizeof(ColorNybbles));
	return 0;
}


int InitSeedSmearRemain()
{
	int i;
	for(i=0;i<8;i++){
		SeedSmearRemaining[i]=0;
	}
}

//----------------------------------------------

int RandomizeSSColorData(){
	int i;
	for(i=0;i<sizeof(SeedSmearColorData);i++)
		SeedSmearColorData[i]=GetRandomNumber();
}

//----------------------------------------------
//0x03-0x04 used to hold color nybble address at the center of screen

int InitSeedSmearStuff(){
	BYTE a;
	XPosition[0]=Center_XPosition;
	YPosition[0]=Center_YPosition;
	SeedSmearOfsEnd=0;
	GetColorNybbleofPos(0);
}

int CheckForBullseye(){
	if((40*12+20)==SeedSmearAddrTbl[SeedSmearOfsEnd]){
		MainEventDispCntr=0xFF;
		BullseyeFlag=0xFF;
	}
}
int EraseSeedSmear()
{
	if(SeedSmearOfsEnd!=0){
		SeedSmearOfsEnd--;
		if(SeedSmearOfsEnd==0)
			SeedSmearEraseFlag=3;
		ColorNybbles[SeedSmearAddrTbl[SeedSmearOfsEnd]]=Black;
	}
}
int EraseOneSeedSmear(BYTE num)
{
	ColorNybbles[SeedSmearAddrTbl[num]]=Black;
}

int PrintBullseyeAtCenter(){
	render_bitmap(BitmapScreen,BullseyeChar,Center_XPosition,Center_YPosition,0,2,1,8);
}
/*
    lda Center_XPosition    //set character coordinates for the center of screen
    sta BitmapChar_XPos
    lda #0x00
    sta BitmapChar_XPosMSB
    lda Center_YPosition
    sta BitmapChar_YPos
    lda CharBitpairCtrl     //get old bitpair and save elsewhere for now
    sta 0x56
    lda #0x00
    sta CharBitpairCtrl     //set bitpair to draw in random color
    ldy #0x02                //set Y to print bullseye character
    jsr DrawSpecialChar     //do sub to print character
    lda 0x56                 //return old bitpair and leave
    sta CharBitpairCtrl
    rts

//----------------------------------------------
//0x0a-0x0b used as address to dot data for character defined in game rom

DrawSpecialChar:
    lda #0x01                 //set for one byte per row
    sta MaxBytesPerRow
    lda #0x08                 //set for eight byte rows
    sta MaxCharRows
    lda #0x06                 //set amounts to subtract to position it properly
    sta XPos_Subtractor
    sta YPos_Subtractor
    lda BitmapCharAddrTbl,y  //load character dot data address with Y as offset
    sta 0x0a
    iny
    lda BitmapCharAddrTbl,y
    sta 0x0b
    jmp BitmapCharRenderer   //jump to get bitmap coordinates and draw on screen

//----------------------------------------------
    %10001001 //unused
//----------------------------------------------

1- joystick up/forward
2- down/backward
3- left
4- right
5- fire
*/
int ReadJoystickPorts(){
	return JoystickDirBits;
}

int ProcMaxScoreAndSeeds(BYTE num)
{
	GetDistanceFromCenter(num);
	if(SeedDropValidator==0){
		render_bitmap(BitmapScreen,BullseyeChar,XPosition[num],YPosition[num],0,2,1,8);
		return;
	}
	ResetSound();
	MaxPoints=Distance;
	SSmearDistance=Distance;
	render_bitmap(BitmapScreen,VortexChar,XPosition[num],YPosition[num],0,2,2,16);
	AddNewSeedSmear(num);
	EvasionActiveFlag=EvasiveFlag;
	SetupSeedToEvade(num);
	DisplayNum=MaxPoints;
	//print max DisplayNum
	if(BullseyeDisplayFlag!=0){
		//print bonus seed
		return;
	}
	SeedsRemaining--;
	if(SeedsRemaining==0){
		//print last seed
	}

}

int GetDistanceFromCenter(BYTE num)
{
	int x,y;
	x=abs(XPosition[num]-Center_XPosition);
	y=abs(YPosition[num]-Center_YPosition);
	XDistance=x;
	YDistance=y;
	Distance=x+y;
	if(Distance<ProximityLimit)
		SeedDropValidator=0;
	else
		SeedDropValidator=1;
	return SeedDropValidator;
}

int CheckSeedSmearVSPos(BYTE x)
{
	GetDistanceFromCenter(x);
	if(SSmearDistance<Distance)
		SSmearDistance=Distance;
	
}
/*
//----------------------------------------------

PrintBullseyeChar:
    lda XPosition,x          //get X and Y coordinates of spacewalker
    sta BitmapChar_XPos      //and set them as coordinates for the bullseye character
    lda XPositionMSB,x
    sta BitmapChar_XPosMSB
    lda YPosition,x
    sta BitmapChar_YPos      //do sub to print bullseye character at coordinates
    jmp DrawSpecialChar      //of the spacewalker, do not return

//----------------------------------------------
*/
BYTE SWalkerBodyColors[]={
    White, Red, Yellow, Green, LightBlue, Purple
};

int KnockHandler()
{
	if(KnocksEnabled!=0 && KnockSpinAndSeedCntr==0){
		KnockSpinAndSeedCntr=0x20;
		NumberOfKnocks++;
		VIC_SPRITE1_COL=SWalkerBodyColors[KnockLimit-NumberOfKnocks];
		if(NumberOfKnocks==KnockLimit){
			Text_XPosition=0x46;
			TextRefreshCounter=0x19;
			SetYOverrideAndInit();
			//print KNOCKED OUT
			SetSoundCtrlUpdFlag();
			RoundScore=0;
		}
	}
}
/*

//----------------------------------------------

AddRoundPtsToTotal:
    clc
    lda TotalScoreLow    //add this round's score low to
    adc RoundScoreLow    //the total score low
    sta TotalScoreLow
    lda TotalScoreHigh   //add this round's score high to
    adc RoundScoreHigh   //the total score high
    sta TotalScoreHigh
    rts

//----------------------------------------------

DecPowerof10:
    .dw 10000, 1000, 100, 10, 1

BinToDecConvert:
           lda #0x00            //init temp decimal digit
           sta DecimalDigit
ConvLoop:  sec                 //set carry for first subtraction
           lda DisplayNumLow   //subtract from binary number low
           sbc DecPowerof10,y  //the first byte in the decimal power of ten we fetched from here
           sta DisplayNumLow   //and store the result here
           iny                 //increment Y to do the next one
           lda DisplayNumHigh  //subtract from binary number high   
           sbc DecPowerof10,y  //the second byte in the decimal power of ten we fetched from here
           sta DisplayNumHigh
           dey
           bcs NextDigit       //if borrow not set, branch to increment decimal digit
           clc
           lda DisplayNumLow   //otherwise add back to binary number low
           adc DecPowerof10,y  //what we subtracted from it
           sta DisplayNumLow
           iny
           lda DisplayNumHigh  //and add back to binary number high
           adc DecPowerof10,y  //what we subtracted from that
           sta DisplayNumHigh
           dey
           rts                 //leave
NextDigit: inc DecimalDigit    //increment decimal digit
           jmp ConvLoop        //do another subtraction iteration

ConvertScoreToDecimal:
         ldx #0x04               //set up offsets
         ldy #0x00
B2DLoop: jsr BinToDecConvert    //do a sub to get one of the decimal digits
         lda DecimalDigit       //get decimal digit
         sta DecimalNumerals,x  //store it here
         iny                    //increment offset of Y to do next power of 10
         iny
         dex                    //decrement to move onto the next digit
         bpl B2DLoop            //branch until all digits are done
         rts

//----------------------------------------------

ConvertDecimalToASCII:
         ldy #0x04               //setup offset for decimal digits
         lda #0x00               //init flag to print zeroes
         sta ZeroPrintFlag
         tax                    //init character offset
D2ALoop: lda DecimalNumerals,y  //get current decimal digit
         beq ZeroDigit          //if zero, branch elsewhere before conversion
         jmp NonZeroDigit       //otherwise jump to setup for future zero digits

ConvertDigit:
         lda DecimalNumerals,y  //load current decimal digit
         clc
         adc #0x30               //add 0x30 to make into PETASCII numeral character
         sta NumeralText,x      //store result here as a character
         inx                    //increment character offset
ZeroSup: dey                    //move onto next digit
         bpl D2ALoop            //if not done with all digits, loop until all done
         rts                    //otherwise leave

ZeroDigit:
    lda ZeroPrintFlag      //load zero-print flag
    beq ZeroSup            //if flag is clear, branch to skip, do not print zero digit
    jmp ConvertDigit       //otherwise jump to setup zero digit for printing

NonZeroDigit:
    lda ZeroPrintFlag      //load zero-print flag
    bne ConvertDigit       //if already set, branch to go ahead and setup digit for printing
    iny                    //otherwise go forward one digit
    lda #0xff               //put end terminator in this byte
    sta NumeralText,y      //so we print only the digits before this
    sty EndTermOffset      //store offset of end terminator here
    dey                    //put Y offset back the way it was
    sta ZeroPrintFlag      //set zero-print flag to print zeroes from now on
    jmp ConvertDigit       //and with that done, jump to setup digit for printing

//----------------------------------------------
*/
int ProcDistancePoints()
{
	RoundScore=MaxPoints-SSmearDistance;
	if(RoundScore<0)
		RoundScore=0;
}
/*
//----------------------------------------------

PrintNumerals_YOvrd:
    jsr SetYOverrideAndInit  //do sub to check Y position and set one, then init variables

PrintNumerals:
    lda DisplayNumLow      //check first part of score
    bne PrintNumberChars   //if set, branch to print number
    lda DisplayNumHigh     //check second part of score
    bne PrintNumberChars   //if set, branch to print number
    lda NumMoveTextFlag    //otherwise score is zero, so check flag
    bne PrintZeroText      //if flag set, go ahead and print "ZERO" in preset X position
    sec
    lda Text_XPosition     //otherwise subtract 25 pixels
    sbc #0x19               //from X position first and THEN print "ZERO"
    sta Text_XPosition

PrintZeroText:
    lda #0x05               //set text number in A to print "ZERO"
    jmp ProcessCharInfo    //do sub to process game text character data information

PrintNumberChars:
         jsr ConvertScoreToDecimal  //do sub to get decimal digits from stored number
         jsr ConvertDecimalToASCII  //do sub to turn decimal digits into printable characters
         lda NumMoveTextFlag        //check flag
         bne NowNum                 //if set, branch elsewhere to go ahead and print numbers
NumbPos: lda EndTermOffset          //otherwise get offset of end terminator
         cmp #0x01                   //if on next char after decrementing or on second one
         beq NowNum                 //then branch to print the number
         sec
         lda Text_XPosition
         sbc #0x08                   //otherwise subtract 8 pixels
         sta Text_XPosition         //from X position for all digits except the first one
         dec EndTermOffset          //decrement offset
         jmp NumbPos                //loop back to check it again
NowNum:  lda #0x00                   //set text number in A to print numeral characters
         jmp ProcessCharInfo        //do sub to process game text character data information

//----------------------------------------------
*/
int SetYOverrideAndInit()
{
	if(Text_YPosOverride==0)
		Text_YPosition=0xAE;
	else
		Text_YPosition=Text_YPosOverride;
	Text_XPosition=0;
	Text_YPosOverride=0;
	NumOfCharacters=0;
	OutputCharOffset=0;
	InputCharOffset=0;
}


int EraseCurrentSSmear()
{
	if(SS_CurrentOffset!=SeedSmearOfsEnd){
		EraseOneSeedSmear(SS_CurrentOffset);
		SS_CurrentOffset++;
	}
}

/*
ProcessSpinsanity:
          lda SpinsanityFlag        //check for value set according to game mode in menu
          beq ExSpin                //if not set, branch to leave
          ldx #0x00                  //otherwise start with first object
SpinLoop: lda BorderCollisionFlag,x //check border collision flag
          bne NextSpin              //if set, move onto next object
          jsr ProcessShiptrailCmds  //otherwise do sub to process shiptrail commands, if any
NextSpin: inx                       //move onto next object
          cpx #0x08
          bne SpinLoop              //do this until all objects are checked
ExSpin:   rts
*/
int FireButtonGameMechs()
{
	int i;
	if(check_fire_trigger()){
		for(i=1;i<8;i++){ //start with spacewalker body
			if(BorderCollisionFlag[i]==0){
				TurningDirFlag[i]^=0xFF;
			}
		}
	}
}
/*
           jsr ReadJoystickPorts      //do sub to read joystick ports
           and #JOY_FIRE            //check firebutton bit 
           bne Debnced                //branch elsewhere if button is not being pressed
           lda FireButtonDebouncer    //check for firebutton debouncer flag
           beq ExFBGM                 //if flag clear, branch to leave (not debounced)
           ldy #0x01                   //set Y to start with spacewalker body
TurnLoop:  lda BorderCollisionFlag,y  //check to see if object collided with border
           bne NoTurnObj              //if so, branch to override, do not alter turning dir flag
           lda TurningDirFlag,y
           eor #0xff                   //otherwise invert turning dir flag to change
           sta TurningDirFlag,y       //from clockwise to counterclockwise, or vice versa
NoTurnObj: iny                        //increment offset
           cpy #0x08                   //check to see if we did the spacewalker and all ships
           bne TurnLoop               //if not, loop until they're all done
           lda #0x00                   //clear flag to implement firebutton debouncing
           sta FireButtonDebouncer    //to avoid messing up the game mechanics here
ExFBGM:    rts                        //leave!
Debnced:   lda #0x01                   //set flag here to indicate that the
           sta FireButtonDebouncer    //firebutton input is being accepted
           rts
*/
//----------------------------------------------
    

//----------------------------------------------

int FireButtonCtrlDelay(int delay)
{
	int i;
	if(ReadJoystickPorts()&JOY_FIRE){
		
	}
}

int SetupSeedToEvade(BYTE num)
{
	XPosition[0]=XPosition[num]; //should be 1
	if(XPosition[0]<Center_XPosition)
		SWalkerXScrnSide=0;
	else
		SWalkerXScrnSide=1;
	SWalkerYPos=YPosition[num];
	if(SWalkerYPos<Center_YPosition)
		SWalkerYPosAdder=-8;
	else
		SWalkerYPosAdder=8;
}


int EvasiveGameMechs()
{
	if(EvasionActiveFlag==0)
		return;
	if(SWalkerXScrnSide!=0){
		SWalkerXPos+=8;
		XPosition[8]=SWalkerXPos;
	}
	else{
		SWalkerXPos-=8;
		XPosition[8]=SWalkerXPos;
	}
	SWalkerYPos+=SWalkerYPosAdder;
	YPosition[8]=SWalkerYPos;
	BorderCollisionDetect(8);
	if(BorderCollisionFlag[8]==0){
		EraseOneSeedSmear(0);
		SeedSmearOfsEnd=0;
		GetColorNybbleofPos(8);
		SeedSmearOfsEnd=1;
	}
	else{
		SeedSmearEraseFlag=1;
		EvasionActiveFlag=0;
		RoundScore=0;
	}
}
 
//----------------------------------------------

int CircleRenderCore()
{
}
/*
    lda Center_XPosition       //initialize central coordinates in variables
    sta CircleCntrPointX       //used in this routine to move the circles around
    lda Center_YPosition
    sta CircleCntrPointY
    dec CircleOffset           //decrement circle offset (move onto another circle)
    bpl ReadyCircleForProc     //if not done with all circles, skip past reinit part
    lda #0x02
    sta CircleOffset           //otherwise reset the circle offset
    ldy #0x00                   //and init offset in Y to start with first set of values

CircleInitLoop:
          lda OscilTableOffset_1,y   //load first offset, whatever it is
          tax
          sec
          sbc OscilTableOffset_2,y   //subtract second offset from it
          cmp #0x07                   //if difference between the two is 7 or greater,
          bcs SetOTOfs               //branch to next part
          jsr GetRandomNumber        //otherwise get a random number
          sta CircleOffsetAdder,y    //store it here for this circle to use
          jsr GetRandomNumber        //then get another random number
          sta CircleRandomSeed       //and store that one here for all circles to use
SetOTOfs: txa                        //put X back into A
          clc
          adc #0x03                   //add 3
          sta OscilTableOffset_1,y   //save as first offset to oscillating table
          lda OscilTableOffset_2,y   //get value here
          clc
          adc #0x02                   //add 2
          sta OscilTableOffset_2,y   //save as second offset to oscillating table
          iny                        //increment offset
          cpy #0x03                   //check to see if we've done three sets of values
          bne CircleInitLoop         //if not, loop again until we have done for all three circles

ReadyCircleForProc:
    ldx CircleOffset           //get offset for the circle we're doing now
    lda OscilTableDivider,x    //get divider used for this circle
    sta CircleDivCounter       //store it here
    lda #0x00                   //init value here (not used except in drawing routine
    sta BitmapXPosMSB          //as the MSB of X position)
    lda NumOfCircleArcPts,x    //get a different constant used by this circle
    sta CArcPointsCounter      //store it here
    jsr RandomNumberZero2Two   //get a number between 0 and 2
    tay                        //use it as offset
    lda StripedBitmasks,y      //get striped bitmask using new offset
    sta BitmapBitpair          //save it here to get the circle's color
    jsr CircleMovementProc     //do sub to move circle around by its center if necessary

DrawCircleLoop:
            lda CircleArcPosOffset,x   //load circle arc position offset
            jsr GetCircleArcPosition   //do sub to fetch appropriate value from table template
            ldx CircleDivCounter       //get stored value here from earlier, use as counter
CDivLoop1:  lsr                        //divide by power of 2
            dex                        //decrement counter
            bne CDivLoop1              //branch until all dividing is done
            cpy #0xff                   //check Y for being set earlier
            bne SetArcXPos             //if not set, branch to use value of A as-is
            eor #0xff
            clc                        //otherwise get two's compliment
            adc #0x01
SetArcXPos: clc                        //add value to circle center's current X position
            adc CircleCntrPointX
            sta BitmapXPos             //store as X position for drawing
            ldx CircleOffset           //get offset saved from earlier
            lda CircleArcPosOffset,x   //load circle arc position offset
            clc
            adc #0x40                   //add 64 bytes to make Y go in opposite direction from X in table
            jsr GetCircleArcPosition   //do sub to fetch appropriate value from table and set Y if necessary
            ldx CircleDivCounter       //get stored value here from earlier, use as counter
CDivLoop2:  lsr                        //divide by power of 2
            dex                        //decrement counter
            bne CDivLoop2              //branch until all dividing is done
            cpy #0xff                   //check Y for being set earlier
            bne SetArcYPos             //if not set, branch to use value of A as-is
            eor #0xff
            clc                        //otherwise get two's compliment
            adc #0x01
SetArcYPos: clc                        //add value to circle center's current Y position
            adc CircleCntrPointY
            sta BitmapYPos             //store as Y position for drawing
            jsr BitmapScreenOutput     //do sub to draw onto bitmap screen at current position
            ldx CircleOffset
            clc
            lda CircleArcPosOffset,x   //add random number set earlier for specific circle
            adc CircleOffsetAdder,x    //to the offset we used earlier, store as new offset
            sta CircleArcPosOffset,x
            dec CArcPointsCounter      //decrement number of points to draw in circle
            bpl DrawCircleLoop         //branch until we draw as much of circle as necessary
            rts

//-----------
//0x07 used as subtractor if necessary

GetCircleArcPosition:
        ldy #0x00                  //init Y to use value here as-is by default
        cmp #0x80                  //check circle arc offset for value => 0x80 (other side of circle)
        bcc ChkOsc                //if less, branch ahead to use values as-is to draw
        sbc #0x80                  //otherwise subtract 0x80 to use a value relative to point of oscillation
        ldy #0xff                  //and set Y as flag to use two's compliment to draw other side of circle
ChkOsc: cmp #0x40                  //check for value => 0x40
        bcc GetOTT                //if less, branch to use value in X
        sta 0x07                   //otherwise use offset, modified or not, as subtractor
        lda #0x80                  //load value 0x80 and subtract from it the amount
        sbc 0x07                   //we saved, creating difference, thus making oscillation possible
GetOTT: tax                       //put value into X
        lda OscilTableTemplate,x  //load from oscillating table template
        rts                       //then leave

//-----------
*/
//used to draw circles
BYTE OscilTableTemplate[64]={
    0x00, 0x06, 0x0d, 0x13, 0x19, 0x1f, 0x25, 0x2c,
    0x32, 0x38, 0x3e, 0x44, 0x4a, 0x50, 0x56, 0x5c,
    0x62, 0x67, 0x6d, 0x73, 0x78, 0x7e, 0x83, 0x88,
    0x8e, 0x93, 0x98, 0x9d, 0xa2, 0xa7, 0xab, 0xb0,
    0xb4, 0xb9, 0xbd, 0xc1, 0xc5, 0xc9, 0xcd, 0xd0,
    0xd4, 0xd7, 0xdb, 0xde, 0xe1, 0xe4, 0xe7, 0xe9,
    0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf7, 0xf9,
    0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xfe, 0xff, 0xff
};

BYTE OscilTableDivider[3]={
    0x02, 0x03, 0x04
};
BYTE NumOfCircleArcPts[3]={
    0x11, 0x0a, 0x09
};
/*
//-----------
//0x07 used as loop counter
*/
int CreateOscilTable(){
	int i,j;
	for(j=0;j<2;j++){
		for(i=0;i<64;i++){
			OscilTable[i+j*64]=OscilTableTemplate[64-1-i]^0xFF;
		}
		for(i=0;i<64;i++){
			OscilTable[i+64+j*64]=OscilTableTemplate[i]^0xFF;
		}
	}

}
/*
//-----------
//0x5c use as counter for number of divides to add

CircleMovementProc:
            lda #SeedSmearOfsMax+2   //load constant
            sec
            sbc SeedSmearOfsEnd      //subtract value here to get difference
            lsr
            lsr                      //then divide by 16 roughly
            lsr
            lsr
            cmp SeedSmearDiff_Mul16  //compare against value set here
            bcs ChkRDM               //if divided value is equal or greater, branch to skip
            sta SeedSmearDiff_Mul16  //otherwise store as the new value
ChkRDM:     lda SeedSmearDiff_Mul16  //check current value
            cmp #0x08                 //for specific range of values
            bcs ExMovC               //branch to leave if equal to or greater than 8
            cmp #0x03
            bcs ChkBuF               //branch to use value if between 3 and 7
            lda #0x03                 //otherwise set 3 as the minimum value
ChkBuF:     tax                      //put value into X
            lda BullseyeFlag         //check for a value set here
            beq GetXOscil            //if not set, branch to use current value of X
            ldx #0x02                 //otherwise load 2 as X
GetXOscil:  stx 0x5c                  //store value here into temp location as divide counter
            ldy CircleOffset         //get circle offset (which circle we're on)
            lda OscilTableOffset_1,y //load first oscillating table offset for this circle
            tay
            lda OscilTable,y         //load value from oscillating table using newly loaded offset
XODivLoop:  lsr                      //divide number by power of 2
            dex                      //decrement current value of X
            bpl XODivLoop            //loop to divide until we're done dividing
            tax                      //save copy of result into X for now
            lda CircleRandomSeed     //load the random number we got earlier
            and #%00001000           //mask out all but d3
            beq UseXAdderCurrent     //if clear, branch to use value as-is
            jsr UseTwosComplement    //otherwise do sub to fetch two's compliment
AddXToC:    clc
            adc CircleCntrPointX     //add value in A or two's compliment of it to whatever
            sta CircleCntrPointX     //the current X coordinate of the circle's center is
            lda CircleRandomSeed     //load the same random number we got earlier
            and #%00000011           //but this time mask out all but d1-d0
            tay                      //use as Y to add divides to divide counter
            ldx 0x5c                  //get the former value of X we saved near the beginning
AddDivides: dey                      //decrement Y as counter
            bmi GetYOscil            //if expired, branch to break out of loop
            inx                      //otherwise add 1 to X
            bne AddDivides           //and loop until broken out of (note branch here will always be taken)
GetYOscil:  ldy CircleOffset         //load circle offset
            lda OscilTableOffset_2,y //load other oscillating table offset
            tay
            lda OscilTable,y         //load value from oscillating table using presumably different offset
YODivLoop:  lsr                      //divide number by power of 2
            dex                      //decrement current value of X
            bpl YODivLoop            //loop to divide until we're done dividing
            tax                      //use result as X
            lda CircleRandomSeed     //load the random number one more time
            and #%00010000           //mask out all but d4
            beq UseYAdderCurrent     //if clear, branch to use value as-is
            jsr UseTwosComplement    //otherwise do sub to fetch two's compliment
AddYToC:    clc                      //(note X here is in A now)
            adc CircleCntrPointY     //add value in A or two's compliment of it to whatever
            sta CircleCntrPointY     //the current Y coordinate of the circle's center is
ExMovC:     rts                      //leave

UseXAdderCurrent:
    txa             //put X contents (X adder) into A
    jmp AddXToC     //and jump back to the part where X of circle is handled

UseYAdderCurrent:
    txa             //put X contents (Y adder) into A
    jmp AddYToC     //and jump back to the part where Y of circle is handled

UseTwosComplement:
    txa             //put X into A
    eor #0xff        //invert and add 1 to
    clc             //create two's compliment
    adc #0x01        //then go back
    rts

//----------------------------------------------
*/
int InitSoundAndIRQs(){
    SoundChannel_Offset=0;     //init sound-related offsets
    SoundChannel_AddrOffset=0;
    SoundRegs_BaseOffset=0;
									//set to alter filter for band-pass, full volume
    OutputToSoundReg(0x18,0x2F);    //do sub to set SID register
    InitSoundMemory();         //do sub to get some seeds, setup SID frequency table
    SoundOutDelayLimit=0x10;      //set limit to delay sounds
}

int GetSoundRegDump(BYTE a){
    a+=SoundRegs_BaseOffset;        //add base offset to contents of A
    return SoundRegDump[a];      //load dump of sound reg
}
//----------------------------------------------

int OutputToSoundReg(BYTE a,BYTE x){
    a+=SoundRegs_BaseOffset;
    SID_SOUND_REGS[a]=x;      //set SID reg, and save copy elsewhere
    SoundRegDump[a]=x;
}

//----------------------------------------------

WORD FrequencyBaseTable[12]={
    0x867a, 0x8e79, 0x96f2, 0x9fec, 0xa96e, 0xb382, 0xbe2e, 0xc97d,
	0xd578, 0xe22a, 0xef9d, 0xfddc
};

//offsets to voice-specific sound registers
BYTE SCBaseOffsets[]={
   0x00, 0x07, 0x0e
};

//----------------------------------------------
//0x4c-0x4d used as address for envelope delay data

int ProcEnvelopeAttDec()
{
	DumpPulseWidthToRegs(SoundChannel_Offset);
	if(EnvDelayCounter[SoundChannel_Offset]==0){
		if(EnvDelayDataAddr[SoundChannel_AddrOffset][EnvDelayDataOffset[SoundChannel_Offset]]!=0xFF){
			EnvDelayCounter[SoundChannel_Offset]=EnvDelayDataAddr[SoundChannel_AddrOffset][EnvDelayDataOffset[SoundChannel_Offset]];
			SetEnv_AttackDecay();
			EnvDelayDataOffset[SoundChannel_Offset]++;
			OutputToSoundReg(4,GetSoundRegDump(4)^1);
		}
	}
	else
		EnvDelayCounter[SoundChannel_Offset]--;
}
/*

LoadAD_SetEnvDelay:
         ldy SoundChannel_AddrOffset  //get offset used to get addresses for this channel
         lda EnvDelayDataAddr,y       //get address low and store it in zero page indirect
         sta 0x4c
         iny                          //increment Y
         lda EnvDelayDataAddr,y       //get address high and store it in zero page indirect
         sta 0x4d
         ldy EnvDelayDataOffset,x     //get offset saved here to use with indirect
         lda (0x4c),y                  //load envelope delay data
         cmp #0xff                     //check for terminator value
         beq ExEnvAD                  //if found, branch to leave
         sta EnvDelayCounter,x        //otherwise save current byte in delay counter
         jsr SetEnv_AttackDecay       //do sub to load attack/delay registers
         ldx SoundChannel_Offset      //get current sound channel offset
         inc EnvDelayDataOffset,x     //increment the offset used by indirects
         lda #0x04                     //set offset in A for sound channel control reg
         jsr GetSoundRegDump          //do sub to read dump for control reg
         eor #%00000001               //invert gate bit that controls envelope of channel
         tax                          //save in X as value to be written
         lda #0x04                     //set offset in A for sound channel control reg again
         jmp OutputToSoundReg         //do sub to write to dump and sound reg
ExEnvAD: rts                          //and then leave!

//----------------------------------------------
    jsr GetSoundRegDump   //dead code
    and #%11111110        //were this code accessible, it would essentially
    tax                   //clear the gate bit in the control reg
    lda #0x04
    jmp OutputToSoundReg
//----------------------------------------------
*/

BYTE EnvDelayData_1[]={
    0x03, 0x02, 0xff
};

BYTE AttackDecayData_1[]={
    0x13, 0x10, 0x33, 0x37, 0x33
};

BYTE EnvDelayData_2[]={
    0x03, 0x01, 0xff
};

BYTE AttackDecayData_2[]={
    0x82, 0x11, 0x74
};

BYTE EnvDelayData_3[]={
    0x03, 0x02, 0x03, 0x02, 0xff
};

BYTE AttackDecayData_3[]={
    0x22, 0x21, 0x31, 0x21, 0x21, 0x46
};
BYTE *EnvDelayDataAddrTbl[]={
    EnvDelayData_1, EnvDelayData_2, EnvDelayData_3
};

//----------------------------------------------

int UpdateAddr_RegBaseOfs(){
	SoundChannel_Offset++;
	SoundRegs_BaseOffset=SCBaseOffsets[SoundChannel_Offset];
	SoundChannel_AddrOffset=SoundChannel_Offset;
}
/*
    inc SoundChannel_Offset      //increment offset for sound channel
    ldx SoundChannel_Offset      //load offset
    lda SCBaseOffsets,x          //get base offset for sound channel
    sta SoundRegs_BaseOffset     //store it here, then multiply for addresses
    txa
    asl 
    sta SoundChannel_AddrOffset  //store here for addresses and leave
    rts

//----------------------------------------------
*/
int InitSoundMemory(){
    UpdateSoundMemoryFlag=0;
    CreateFreqTable();       //do sub to create lookup table of frequency values
    InitSoundSeeds();        //do sub to init sound memory and random sound seeds
}
/*
//----------------------------------------------
//0x4c-0x4d used as address for attack/decay envelope data
*/
int SetEnv_AttackDecay()
{
	OutputToSoundReg(5,AttackDecayDataAddr[SoundChannel_AddrOffset][EnvDelayDataOffset[SoundChannel_Offset]]);
}
/*
    ldy SoundChannel_AddrOffset  //get address offset for current sound channel
    lda AttackDecayDataAddr,y    //get address low and store it in zero page indirect
    sta 0x4c
    iny                          //increment Y
    lda AttackDecayDataAddr,y    //get address high and store it in zero page indirect
    sta 0x4d
    ldx SoundChannel_Offset      //get offset for sound channel
    ldy EnvDelayDataOffset,x     //get offset used by indirect
    lda (0x4c),y                  //load data now
    tax                          //save in X as value to be written
    lda #0x05                     //set offset in A for attack/delay register
    jmp OutputToSoundReg         //do sub to write to sound reg and dump, do not return

//----------------------------------------------
//0x4b used as temp variable to hold pulse width random seed
*/
int DumpPulseWidthToRegs(BYTE x)
{
	BYTE i;
	OutputToSoundReg(3,PulseWidthSeed[x]>>4);
	OutputToSoundReg(2,PulseWidthSeed[x]<<4);
}


//----------------------------------------------

int SoundEngineCore()
{
	int i;
	SoundOutDelayCounter++;
	ProcSoundCtrlUpdate();
	if(SoundOutDelayCounter==SoundOutDelayLimit){
		SoundOutDelayCounter=0;
		_PlaySound();
		SoundMemoryOffset+=SoundMemoryOffsetAdder;
		ProcJoyForSound();
	}
	SoundChannel_Offset=0;
	SoundChannel_AddrOffset=0;
	SoundRegs_BaseOffset=0;
	FilterRegOutFlag++;
	if(FilterRegOutFlag&1)
		UpdateResAndCutoff();

	while(TRUE){
		switch(EnvCtrlFlag[SoundChannel_Offset]){
		case 0:
			break;
		case 1:
			EnvCtrlFlag[SoundChannel_Offset]++;
			OutputToSoundReg(0x6,0);
			break;
		case 2:
			EnvCtrlFlag[SoundChannel_Offset]++;
			OutputToSoundReg(0x6,0x1C);
			ProcEnvelopeAttDec();
			break;
		default:
			ProcEnvelopeAttDec();
			break;
		}
		UpdateAddr_RegBaseOfs();
		if(SoundChannel_Offset==3)
			break;
	}
	dump_sid();
}


BYTE *AttackDecayAddrTbl[]={
    AttackDecayData_1, AttackDecayData_2, AttackDecayData_3
};

/*
 

EnvelopeCore:
    ldx SoundChannel_Offset      //load current sound channel offset
    lda EnvCtrlFlag,x            //get envelope control flag
    beq EnvCoreLoopChk           //if flag not set, branch to skip this entirely
    cmp #0x01
    beq InitSusRel               //if set to 1, branch to init sustain/release
    cmp #0x02
    beq SetSusRel                //if set to 2, branch to set sustain/release
    jsr ProcEnvelopeAttDec       //otherwise set higher, thus do sub
    jmp EnvCoreLoopChk           //and then jump ahead to the last part

InitSusRel:
    inc EnvCtrlFlag,x            //increment flag to do next bit the next time around
    ldx #0x00                     //init set sustain volume and release rate
    lda #0x06                     //set offset in A to sustain/release reg
    jsr OutputToSoundReg         //do sub to write to sound reg and dump
    jmp EnvCoreLoopChk           //skip ahead to the last part

SetSusRel:
    inc EnvCtrlFlag,x            //increment flag to do next bit the next time around
    ldx #0x1c                     //set sustain volume and release rate in X (small amp, slow release)
    lda #0x06                     //set offset in A to sustain/release reg
    jsr OutputToSoundReg         //do sub to write to sound reg and dump
    jmp EnvelopeCore             //loop back to do the next bit (process attack/decay)

EnvCoreLoopChk:
    jsr UpdateAddr_RegBaseOfs    //do sub to update sound offsets appropriately
    lda SoundChannel_Offset      //check current sound channel offset
    cmp #0x03                     //for being out of range
    bne EnvelopeCore             //loop back and do another channel if not
    jmp ChkPause                 //otherwise jump back to the IRQ handler

//----------------------------------------------
//0x4a used as temp offset to envelope address tables for envelope delay data and attack/decay data
*/
int NextSoundChannel()
{
	CurrentSoundChannel++;
	CurrentSoundChannel=CurrentSoundChannel%3;
}

int _PlaySound(){
	if(UpdateSoundMemoryFlag!=0){
		UpdateSoundMemoryFlag=0;
		SoundMemory_Envelope[SoundMemoryOffset]=BaseEnvDataOffset;
		SoundMemory_Frequency[SoundMemoryOffset]=BaseFreqDataOffset;
	}
	if(SoundMemory_Frequency[SoundMemoryOffset]!=0){
		NextSoundChannel();
		SoundRegs_BaseOffset=SCBaseOffsets[CurrentSoundChannel];
		SoundChannel_Offset=CurrentSoundChannel;
		SoundChannel_AddrOffset=SoundChannel_Offset;
		EnvCtrlFlag[CurrentSoundChannel]=1;
		ProcessSoundFrequency(SoundMemory_Frequency[SoundMemoryOffset]);
		OutputToSoundReg(4,RegBitmask[RandomNumberZero2Two()+4]); //this sets random wave type :pulse,saw,triangle
		EnvDelayDataAddr[SoundChannel_AddrOffset]=EnvDelayDataAddrTbl[SoundMemory_Envelope[SoundMemoryOffset]];
		AttackDecayDataAddr[SoundChannel_AddrOffset]=AttackDecayAddrTbl[SoundMemory_Envelope[SoundMemoryOffset]];
		OutputToSoundReg(4,GetSoundRegDump(4)&0xFE);
		EnvDelayDataOffset[SoundChannel_Offset]=0;
		EnvDelayCounter[SoundChannel_Offset]=0;
	}
}
/*
 
        lda #0x04
        jsr GetSoundRegDump           //do sub to get control reg contents of current sound channel
        and #%11111110                //mask out d0 (is this really necessary?)
        tax                           //use as value 
        lda #0x04                      //load A with base offset to be used in sub
        jsr OutputToSoundReg          //then do sub to load data into control reg of sound regs
        ldx SoundChannel_Offset       //get offset based on sound channel
        lda #0x01
        sta 0x5765,x                   //residual
        lda #0x00
        sta EnvDelayDataOffset,x      //clear envelope delay counter and offset to data
        sta EnvDelayCounter,x
        rts

//----------------------------------------------
*/
BYTE FreqDumpLowAdder[]={
    0x00, 0x03, 0x06
};

BYTE ChordAdderOfsTbl[]={
    0x00, 0x00, 0x01,
    0x02, 0x00, 0x01,
    0x02, 0x00, 0x00,
    0x01, 0x02, 0x03,
    0x00, 0x00, 0x01,
    0x01, 0x02, 0x02,
    0x03, 0x00, 0x00
};

BYTE BaseFreqTblOffsets[]={
    0x00, 0x00, 0x03,
    0x07, 0x0c, 0x0f,
    0x13, 0x18, 0x18,
    0x1b, 0x1f, 0x22,
    0x24, 0x26, 0x27,
    0x27, 0x2b, 0x2b,
    0x2e, 0x30, 0x00
};

int ProcessSoundFrequency(BYTE a)
{
	WORD i;
	write_watch(0,a);
	i=SoundFrequencyTable[GenSoundFreqAdder+ChordFreqAdders[ChordAdderOfsTbl[a]]+BaseFreqTblOffsets[a]];
	i+=FreqDumpLowAdder[SoundChannel_Offset];
	SoundRegDump[SoundRegs_BaseOffset]=i;
	SoundRegDump[SoundRegs_BaseOffset+1]=i>>8;
	SID_SOUND_REGS[SoundRegs_BaseOffset]=i;
	SID_SOUND_REGS[SoundRegs_BaseOffset+1]=i>>8;
}

int CreateFreqTable(){
	BYTE i,j;
	for(i=0;i<8;i++){
		for(j=0;j<12;j++){
			SoundFrequencyTable[i*12+j]=FrequencyBaseTable[j];
			SoundFrequencyTable[i*12+j]>>=(7-i);
		}
	}
}
/*
//----------------------------------------------


//----------------------------------------------
*/
int InitSoundSeeds(){
	BYTE i,j,a,x,y;
	memset(SoundMemory_Envelope,0,sizeof(SoundMemory_Envelope));
	memset(SoundMemory_Frequency,0,sizeof(SoundMemory_Frequency));
	memset(EnvCtrlFlag,0,sizeof(EnvCtrlFlag));
	SoundOutDelayCounter=0;     //clear sound delay counter
	CurrentSoundChannel=0;
	ChordFreqAdders[0]=0;          //clear first chord freq adder
	for(i=1;i<4;i++)
		ChordFreqAdders[i]=RandomNumberZero2Two();
	
	SoundMemoryOffsetAdder=0x10;   //set adder to be used later
	SoundMemoryOffsetAdder=1;
	i=GetRandomNumber()&0xF;
	i+=13;
	GenSoundFreqAdder=i;
	i=RandomNumberZero2Two();
	TempBitmask=RegBitmask[i];
	i=SoundRegDump[0x17];
	i&=0xF0;
	i|=TempBitmask;
	
	SoundRegDump[0x17]=i;
	SID_FILTER_RES_VCTRL=i;
	
	j=RandomNumberZero2Two();
	for(i=0;i<3;i++){
		a=GetRandomNumber();
		if(i<=j){
			a&=0x7;
			a+=128;
		}
		else if(i==2){
			a&=1;
			if(a)
				a=0x80;
		}
		PulseWidthSeed[i]=a;
	}
	SoundOutDelayLimit=8;
	SoundCtrlUpdateFlag=0;
}

int SetSoundCtrlUpdFlag()
{
    SoundCtrlUpdateFlag=1;
}


int ResetSound(){
	BaseFreqDataOffset=7+(GetRandomNumber()&0x7);
	BaseEnvDataOffset=RandomNumberZero2Two();
	InitSoundSeeds();
	UpdateSoundMemoryFlag=1;
}

int ManipulateFreqEnvOfs(){
	if(BaseFreqDataOffset==18){
		BaseFreqDataOffset=2;
	}
	else
		BaseFreqDataOffset++;
	BaseEnvDataOffset=RandomNumberZero2Two();
	UpdateSoundMemoryFlag=1;
}

int ProcJoyForSound()
{
	BYTE dir;
	dir=ReadJoystickPorts()&0xF;
	if(PreviousDirBits!=dir){
		PreviousDirBits=dir;
		if(dir&JOY_UP){

		}

	}
}
/*
    jsr ReadJoystickPorts    //do sub to read joystick ports
    and #%00001111           //mask out all but directional bits
    cmp PreviousDirBits      //compare to previously saved directional bits
    bne JoySoundSubs         //if directional bits differ, branch to process
    rts                      //otherwise leave to take debounce into account

JoySoundSubs:
        sta PreviousDirBits       //store as new directional bits
        tay                       //save temporarily into Y
        and #JOY_UP
        beq SndInfluenceSub_Up    //branch if pressing up to raise pitch
        tya                       //otherwise copy bits
        and #JOY_DOWN
        beq SndInfluenceSub_Down  //branch if pressing down to lower pitch
LRBits: tya                       //otherwise copy bits
        and #JOY_LEFT
        beq 0x9ae9                 //branch if pressing left
        tya                       //otherwise copy bits
        and #JOY_RIGHT
        beq 0x9aee                 //branch if pressing right
        rts                       //otherwise leave

//-----------

SndInfluenceSub_Up:
        jsr GetRandomNumber    //do sub to get random number
        and #%00000011         //mask out all but d1-d0
        tax                    //save as X and increment, use as counter
        inx
AddGen: inc GenSoundFreqAdder  //increment random number used as general use frequency adder
        lda GenSoundFreqAdder
        cmp #0x2e               //check to see if number reached certain range
        bcc UpLoop             //if less than amount, branch to skip this part
        sbc #0x0c               //otherwise subtract 12 from it
        sta GenSoundFreqAdder  //and store result
UpLoop: dex                    //decrement counter
        bpl AddGen             //loop back if counter not yet expired
        jmp LRBits             //otherwise jump to process left/right bits

//-----------

SndInfluenceSub_Down:
          jsr GetRandomNumber    //do sub to get random number
          and #%00000011         //mask out all but d1-d0
          tax                    //save as X and increment, use as counter
          inx
SubGen:   dec GenSoundFreqAdder  //decrement random number set a while back
          lda GenSoundFreqAdder
          cmp #0x0c               //check to see if number reached certain range
          bcs DownLoop           //if equal to or greater than amount, branch to skip this part
          clc                    //otherwise add 12 to it
          adc #0x0c
          sta GenSoundFreqAdder  //and store result
DownLoop: dex                    //decrement counter
          bpl SubGen             //loop back if counter not yet expired
          jmp LRBits             //otherwise jump to process left/right bits

//-----------

SndInfluenceSub_Left:
    lda #0x0f                      //set counter to 15 (note chord more likely to change here)
    jmp ModifyChordAdders         //jump to change chord frequency adders

SndInfluenceSub_Right:
    lda #0x09                      //set counter to 9, then continue

ModifyChordAdders:
         tay                      //save counter into Y
MChLoop: jsr GetRandomNumber      //do sub to get random number
         and #%00000011           //mask out all but d1-d0
         tax                      //save as X and increment, use as offset
         inx
         jsr RandomNumberZero2Two //get random number between 0 and 2
         sta ChordFreqAdders,x    //save as random number seed here
         dey                      //decrement counter
         bpl MChLoop              //loop back if counter not yet expired
         rts                      //otherwise leave

//----------------------------------------------
//0x4a used as temp variable for resonance bits
*/
int UpdateResAndCutoff()
{
	while(TRUE){
		FilterCutoffFreqOffset++;
		if(OscilTable[FilterCutoffFreqOffset]!=0){
			OutputToSoundReg(0x16,OscilTable[FilterCutoffFreqOffset]);
			break;
		}
		CurrentResonance++;
		OutputToSoundReg(0x17,(GetSoundRegDump(0x17)&0xF)|(CurrentResonance<<4));
		FilterCutoffFreqOffset++;
	};
}

int RandomNumberZero2Two(){
	BYTE i;
	i=GetRandomNumber();
	i=i%3;
	return i;
}

//----------------------------------------------

int ProcSoundCtrlUpdate()
{
	if(SoundOutDelayCounter==4 || SoundOutDelayCounter==SoundOutDelayLimit){
		if(SoundCtrlUpdateFlag!=0){
			SID_V1_CONTROL_REG=RandomCtrlBits()^SoundRegDump[4];
			SoundRegDump[4]=SID_V1_CONTROL_REG;
			SID_V2_CONTROL_REG=RandomCtrlBits()^SoundRegDump[0xB];
			SoundRegDump[0xB]=SID_V2_CONTROL_REG;
		}
	}
}

int RandomCtrlBits()
{
    return GetRandomNumber()&0x30;
}

int SetupBeginner()
{
    GeneralGameInit();
	SWSmearExtent=8;
    KnocksEnabled=1;
    KnockLimit=4;
    ProximityLimit=0x5A;
    PreciseFollowFlag=0x5A;
	JoystickDelaySetting=0x1E;
}

int SetupFreestyle()
{
    GeneralGameInit();
    SWSmearExtent=8;
    ProximityLimit=0x3C;
}

int SetupSpinsanity()
{
    GeneralGameInit();
    SpinsanityFlag=1;
    KnocksEnabled=1;
    init_moondusttrail();
    SetupSpinsanityCmdData();
    KnockLimit=2;
    SWSmearExtent=0xA;
    JoystickDelaySetting=0x64;
    ProximityLimit=0x50;
}
int SetupEvasive()
{
    GeneralGameInit();
    KnocksEnabled=1;
    PreciseFollowFlag=1;
    EvasiveFlag=1;
    SWSmearExtent=0;
    KnockLimit=4;
    JoystickDelaySetting=0x1E;
    SpinsanityFlag=0;
    ProximityLimit=0x1E;
}
int GeneralGameInit()
{
    SpinsanityFlag=0;
    KnocksEnabled=0;
    PreciseFollowFlag=0;
    EvasiveFlag=0;
    JoystickDelaySetting=0;
    SeedsRemaining=3;
}
/*
ResetPModeAndSprs:
    lda #0x00                 //reset mode to title screen
    sta ProgModeFlag
    jmp SwitchOffSpriteDisp  //jump to reset sprite colors, priorities

//------------------------------------------------------------------------------------------
//SPRITE DATA TEMPLATES
*/
BYTE SpacewalkerHeadData[]={
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x3c, 0x00, //..........XXXX..........
    0x00, 0x6e, 0x00, //.........XX.XXX.........
    0x00, 0xdf, 0x00, //........XX.XXXXX........
    0x01, 0xbf, 0x80, //.......XX.XXXXXXX.......
    0x01, 0xbf, 0x80, //.......XX.XXXXXXX.......
    0x01, 0xbf, 0x80, //.......XX.XXXXXXX.......
    0x01, 0xff, 0x80, //.......XXXXXXXXXX.......
    0x00, 0xff, 0x00, //........XXXXXXXX........
    0x00, 0x7e, 0x00, //.........XXXXXX.........
    0x00, 0x3c, 0x00, //..........XXXX..........
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00, //........................
    0x00, 0x00, 0x00  //........................
};
BYTE SpacewalkerBody1Data[]={
    0x00, 0x06, 0x00, //.............XX.........
    0x01, 0xff, 0x00, //.......XXXXXXXXX........
    0x1f, 0xff, 0xd0, //...XXXXXXXXXXXXXXX.X....
    0x3f, 0x9f, 0x98, //..XXXXXXX..XXXXXX..XX...
    0x0c, 0x3f, 0xb8, //....XX....XXXXXXX.XXX...
    0x08, 0xff, 0xdc, //....X...XXXXXXXXXX.XXX..
    0x03, 0xff, 0xfc, //......XXXXXXXXXXXXXXXX..
    0x07, 0xff, 0x7c, //.....XXXXXXXXXXX.XXXXX..
    0x07, 0xff, 0x3c, //.....XXXXXXXXXXX..XXXX..
    0x07, 0xfb, 0x98, //.....XXXXXXXX.XXX..XX...
    0x03, 0xfb, 0x90, //......XXXXXXX.XXX..X....
    0x03, 0xf7, 0x80, //......XXXXXX.XXXX.......
    0x0f, 0xcf, 0x00, //....XXXXXX..XXXX........
    0x0f, 0xfe, 0x00, //....XXXXXXXXXXX.........
    0x1f, 0x78, 0x00, //...XXXXX.XXXX...........
    0x1e, 0xe0, 0x00, //...XXXX.XXX.............
    0x1e, 0xe0, 0x00, //...XXXX.XXX.............
    0x0d, 0xe0, 0x00, //....X.XXXXX.............
    0x01, 0xe0, 0x00, //.......XXXX.............
    0x00, 0xc0, 0x00, //........XX..............
    0x00, 0x00, 0x00  //........................
};
BYTE SpacewalkerBody2Data[]={
    0x00, 0x00, 0x00, //........................
    0x00, 0x3f, 0x00, //..........XXXXXX........
    0x07, 0xff, 0xc0, //.....XXXXXXXXXXXXX......
    0x03, 0xff, 0xc0, //......XXXXXXXXXXXX......
    0x02, 0x0f, 0xe0, //......X.....XXXXXXX.....
    0x00, 0x7f, 0xe0, //.........XXXXXXXXXX.....
    0x01, 0xff, 0xc4, //.......XXXXXXXXXXX...X..
    0x03, 0xff, 0xec, //......XXXXXXXXXXXXX.XX..
    0x03, 0xff, 0xec, //......XXXXXXXXXXXXX.XX..
    0x03, 0xff, 0x7c, //......XXXXXXXXXX.XXXXX..
    0x07, 0xff, 0x7c, //.....XXXXXXXXXXX.XXXXX..
    0x1f, 0xfb, 0x3c, //...XXXXXXXXXX.XX..XXXX..
    0x1f, 0xb7, 0x38, //...XXXXXX.XX.XXX..XXX...
    0x3e, 0xcf, 0x10, //..XXXXX.XX..XXXX...X....
    0x3d, 0xfe, 0x00, //..XXXX.XXXXXXXX.........
    0x1b, 0xdc, 0x00, //...XX.XXXX.XXX..........
    0x17, 0x80, 0x00, //...X.XXXX...............
    0x0f, 0x80, 0x00, //....XXXXX...............
    0x07, 0x80, 0x00, //.....XXXX...............
    0x02, 0x00, 0x00, //.....X..................
    0x00, 0x00, 0x00  //........................
};

BYTE SpacewalkerBody0Data[]={
    0x00, 0x36, 0x40, //..........XX.XX..X......
    0x00, 0x7c, 0xe0, //.........XXXXX..XXX.....
    0x01, 0xfe, 0x70, //.......XXXXXXXX..XXX....
    0x03, 0xff, 0xf8, //......XXXXXXXXXXXXXXX...
    0x07, 0x7f, 0xf8, //.....XXX.XXXXXXXXXXXX...
    0x0e, 0x7f, 0x38, //....XXX..XXXXXXX..XXX...
    0x1c, 0xff, 0x10, //...XXX..XXXXXXXX...X....
    0x39, 0xff, 0x80, //..XXX..XXXXXXXXXX.......
    0x31, 0xfb, 0x80, //..XX...XXXXXX.XXX.......
    0x13, 0xfd, 0xc0, //...X..XXXXXXXX.XXX......
    0x03, 0xfd, 0xc0, //......XXXXXXXX.XXX......
    0x03, 0xfd, 0xc0, //......XXXXXXXX.XXX......
    0x01, 0xfb, 0x80, //.......XXXXXX.XXX.......
    0x00, 0xf7, 0x00, //........XXXX.XXX........
    0x00, 0xfe, 0x00, //........XXXXXXX.........
    0x01, 0xee, 0x00, //.......XXXX.XXX.........
    0x01, 0xef, 0x00, //.......XXXX.XXXX........
    0x01, 0xef, 0x00, //.......XXXX.XXXX........
    0x00, 0xef, 0x00, //........XXX.XXXX........
    0x00, 0xee, 0x00, //........XXX.XXX.........
    0x00, 0x00, 0x00  //........................
};
BYTE Ship0Data[]={
    0x00, 0xf0, 0x00, //........XXXX............
    0x03, 0xbc, 0x00, //......XXX.XXXX..........
    0x06, 0x76, 0x00, //.....XX..XXX.XX.........
    0x04, 0xda, 0x00, //.....X..XX.XX.X.........
    0x0c, 0xcd, 0x00, //....XX..XX..XX.X........
    0x19, 0xc5, 0x80, //...XX..XXX...X.XX.......
    0x1d, 0x87, 0x80, //...XXX.XX....XXXX.......
    0x17, 0x8e, 0x80, //...X.XXXX...XXX.X.......
    0x1d, 0xfb, 0x80, //...XXX.XXXXXX.XXX.......
    0x15, 0x6a, 0x80, //...X.X.X.XX.X.X.X.......
    0x1b, 0x9d, 0x80, //...XX.XXX..XXX.XX.......
    0x34, 0x02, 0xc0, //..XX.X........X.XX......
    0x39, 0x99, 0xc0, //..XXX..XX..XX..XXX......
    0x38, 0x01, 0xc0, //..XXX..........XXX......
    0x39, 0x09, 0xc0, //..XXX..X....X..XXX......
    0x39, 0x99, 0xc0, //..XXX..XX..XX..XXX......
    0x39, 0xf9, 0xc0, //..XXX..XXXXXX..XXX......
    0x19, 0xf9, 0x80, //...XX..XXXXXX..XX.......
    0x07, 0xfe, 0x00, //.....XXXXXXXXXX.........
    0x00, 0xf0, 0x00, //........XXXX............
    0x00, 0x00, 0x00  //........................
};
BYTE Ship1Data[]={
    0x00, 0x07, 0x80, //.............XXXX.......
    0x00, 0x3f, 0xc0, //..........XXXXXXXX......
    0x00, 0x78, 0xe0, //.........XXXX...XXX.....
    0x00, 0xe3, 0xe0, //........XXX...XXXXX.....
    0x01, 0x6f, 0xa0, //.......X.XX.XXXXX.X.....
    0x03, 0x3f, 0xb0, //......XX..XXXXXXX.XX....
    0x07, 0x9c, 0x30, //.....XXXX..XXX....XX....
    0x0f, 0xce, 0x70, //....XXXXXX..XXX..XXX....
    0x0d, 0xc7, 0xf0, //....XX.XXX...XXXXXXX....
    0x1e, 0xf0, 0x10, //...XXXX.XXXX.......X....
    0x1d, 0x3c, 0xe0, //...XXX.X..XXXX..XXX.....
    0x3d, 0xcf, 0xe0, //..XXXX.XXX..XXXXXXX.....
    0x3b, 0xf1, 0xc0, //..XXX.XXXXXX...XXX......
    0x3b, 0xfe, 0x40, //..XXX.XXXXXXXXX..X......
    0x1f, 0xfb, 0xc0, //...XXXXXXXXXX.XXXX......
    0x1f, 0xfb, 0x80, //...XXXXXXXXXX.XXX.......
    0x07, 0xf7, 0x80, //.....XXXXXXX.XXXX.......
    0x03, 0xf7, 0x00, //......XXXXXX.XXX........
    0x01, 0xff, 0x00, //.......XXXXXXXXX........
    0x00, 0x1e, 0x00, //...........XXXX.........
    0x00, 0x00, 0x00  //........................
};
BYTE Ship2Data[]={
    0x00, 0x03, 0x80, //..............XXX.......
    0x00, 0x1f, 0xe0, //...........XXXXXXXX.....
    0x00, 0x34, 0x50, //..........XX.X...X.X....
    0x00, 0x7c, 0xd8, //.........XXXXX..XX.XX...
    0x00, 0xf7, 0x58, //........XXXX.XXX.X.XX...
    0x07, 0xaf, 0x3c, //.....XXXX.X.XXXX..XXXX..
    0x0f, 0x9c, 0x34, //....XXXXX..XXX....XX.X..
    0x1e, 0xbc, 0x2c, //...XXXX.X.XXXX....X.XX..
    0x3e, 0x9e, 0x28, //..XXXXX.X..XXXX...X.X...
    0x1c, 0x5f, 0xa8, //...XXX...X.XXXXXX.X.X...
    0x19, 0x4f, 0x78, //...XX..X.X..XXXX.XXXX...
    0x13, 0x22, 0xf0, //...X..XX..X...X.XXXX....
    0x1f, 0x18, 0x60, //...XXXXX...XX....XX.....
    0x0f, 0x87, 0xc0, //....XXXXX....XXXXX......
    0x0f, 0xf1, 0x80, //....XXXXXXXX...XX.......
    0x07, 0xe7, 0x80, //.....XXXXXX..XXXX.......
    0x03, 0xcf, 0x80, //......XXXX..XXXXX.......
    0x01, 0xdf, 0x00, //.......XXX.XXXXX........
    0x00, 0x7e, 0x00, //.........XXXXXX.........
    0x00, 0x04, 0x00, //.............X..........
    0x00, 0x00, 0x00  //........................
};
BYTE *SprTemplateAddr[]={
    Ship0Data,
    Ship1Data,
    Ship2Data,
    SpacewalkerBody0Data,
    SpacewalkerBody1Data,
    SpacewalkerBody2Data,
    SpacewalkerHeadData
};
BYTE SpriteFlipBits[33]={
 0x00, 0x01, 0x02, 0x21, 0x20, 0xa1, 0x42, 0x41, //lower nybble is sprite template number in
 0x40, 0xc1, 0xc2, 0xe1, 0x60, 0x61, 0x82, 0x81, //order listed in template address data,
 0x03, 0x04, 0x05, 0x24, 0x23, 0xa4, 0x45, 0x44, //higher nybble is flip bits
 0x43, 0xc4, 0xc5, 0xe4, 0x63, 0x64, 0x85, 0x84,
 0x06
};
int ProcessSprTemplate(BYTE y){
	BYTE i,j;
	j=i=SpriteFlipBits[y];   //get data from look-up table
	i&=0x1F;
	memcpy(SpriteDataBuffer,SprTemplateAddr[i],63);

	
	if(j&0x80)
		HorizontalFlip(SpriteDataBuffer,3,21);
	if(j&0x40)
		VerticalFlip(SpriteDataBuffer,3,21);
	if(j&0x20){
		DiagonalFlip(SpriteDataBuffer,3,21);
		VerticalFlip(SpriteDataBuffer,3,21);
	}

}
int LoadSprData()
{
	int i;
	memset(sprite_mem,0,sizeof(sprite_mem));
	for(i=0;i<sizeof(SpriteFlipBits);i++){
		ProcessSprTemplate(i);
		memcpy(sprite_mem+i*63,SpriteDataBuffer,63);
	}
}
static void set_block(SDL_Surface *screen,int color,int x,int y,int width,int height)
{
	int i,j,offset;

	for(i=0;i<width;i++){
		for(j=0;j<height;j++){
			offset=(y+j)*(screen->pitch/4);
			offset+=x+i;
			if((x+i)<0 || (x+i) >= screen->w)
				continue;
			if((y+j)<0 || (y+j) >= screen->h)
				continue;
			((unsigned int*)screen->pixels)[offset]=color;
		}
	}
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
int resize_console()
{
	SMALL_RECT rect;
	rect.Bottom=50-1;
	rect.Right=80-1;
	rect.Top=0;
	rect.Left=0;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),TRUE,&rect);
}
int scale_factor(DWORD scale,DWORD x)
{
	return (x*scale)>>16;
}
int frame_rate(int rate)
{
	static DWORD time=0;
	double t;
	t=GetTickCount()-time;
	t=((double)1.0/(double)rate)-t;
	if(t>0)
		Sleep((DWORD)(t*1000));
	else
		Sleep(0);
	time=GetTickCount();
}
int video_test()
{
	static int init=TRUE;
	int i,j,k,x,y;
	static SDL_Surface *screen=0;
	SDL_Rect rect;
	int scale=2<<16;
	int offsetx=5;
	int offsety=5;


	if(init){
		SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO);
		SDL_WM_SetCaption("Moondust",NULL);
		atexit(SDL_Quit);
		screen=SDL_SetVideoMode(scale_factor(scale,SCREEN_WIDTH)+offsetx*2, scale_factor(scale,SCREEN_HEIGHT)+offsety*2, 32, SDL_SWSURFACE|SDL_DOUBLEBUF);
		if ( screen == NULL ) 
		{
			fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
			exit(1);
		}
		init=FALSE;
		resize_console();
		g_screen=screen;
		//SDL_WM_IconifyWindow();
	}
	
	do{
		if (SDL_MUSTLOCK(screen)) 
			if (SDL_LockSurface(screen) < 0) 
				return;
			SDL_FillRect(screen,NULL,0);
			
			/*
			for(i=0;i<sizeof(sprite_mem)/3;i++){
				for(j=0;j<3;j++){
					for(k=0;k<8;k++){
						if(sprite_mem[i*3+j]&(1<<(7-k)))
							set_pixel(screen,0xFFFF,j*8+k,i);
					}
				}
			}
			*/
			//horozontal bars
			rect.x=offsetx;
			rect.y=offsety;
			rect.w=(320*scale)>>16;
			rect.h=1;
			SDL_FillRect(screen,&rect,0xFF00);
			rect.y=offsety+((200*scale)>>16);
			SDL_FillRect(screen,&rect,0xFF00);
			//vertical bars
			rect.x=offsetx;
			rect.y=offsety;
			rect.w=1;
			rect.h=(200*scale)>>16;
			SDL_FillRect(screen,&rect,0xFF00);
			rect.x=offsetx+((320*scale)>>16);
			SDL_FillRect(screen,&rect,0xFF00);
			
			draw_videomem(screen,offsetx,offsety,scale);
			for(i=0;i<SCREEN_WIDTH;i++){

				for(j=0;j<SCREEN_HEIGHT;j++){
					int c,x,y;
					c=get_rgb(screen_colors[BitmapScreen[i+j*SCREEN_WIDTH]&3]);
					if(c==0)
						continue;
					x=offsetx+((i*scale)>>16);
					y=offsety+((j*scale)>>16);
					rect.x=x;
					rect.y=y;
					rect.w=scale>>16;
					rect.h=scale>>16;
					SDL_FillRect(screen,&rect,c);
				}
			}
			draw_sprites(screen,offsetx,offsety,scale);
			
			// Unlock if needed
			if (SDL_MUSTLOCK(screen)) 
				SDL_UnlockSurface(screen);
			
			SDL_Flip(screen);
			//SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);

	}while(FALSE);
	frame_rate(60);
}

int draw_sprites(SDL_Surface *screen,int offx,int offy,int scale)
{
	int i,j,k,s;
	SDL_Rect rect;
	for(s=0;s<8;s++){
		if((VIC_SPRITE_ENABLE&(1<<s))==0)
			continue;
		for(i=0;i<21;i++){
			for(j=0;j<3;j++){
				for(k=0;k<8;k++){
					int frame;
					frame=SpritePointers[s]*63;
					if(sprite_mem[i*3+j+frame]&(1<<(7-k))){
						int c,x,y;
						c=get_rgb(VIC_SPRITE_COL[s]);
						if(c==0)
							continue;
						x=(j*8+k+VIC_SPRITE_POSREG[s*2])*scale;
						x=(x>>16)+offx;
						y=(i+VIC_SPRITE_POSREG[s*2+1])*scale;
						y=(y>>16)+offy;
						rect.x=x;
						rect.y=y;
						rect.w=scale>>16;
						rect.h=scale>>16;
						SDL_FillRect(screen,&rect,c);
					}
				}
			}
		}
	}
}
int draw_videomem(SDL_Surface *screen,int offx,int offy,int scale)
{
	int i,j,x,y,c;
	SDL_Rect rect;
	scale=(8*scale)>>16;
	for(y=0;y<SCREEN_HEIGHT/8;y++){
		for(x=0;x<SCREEN_WIDTH/8;x++){
			c=get_rgb(ColorNybbles[x+y*(SCREEN_WIDTH/8)]&0xF);
			if(c==0)
				continue;
			rect.x=offx+x*scale;
			rect.y=offy+y*scale;
			rect.w=scale;
			rect.h=scale;
			SDL_FillRect(screen,&rect,c);
			//set_block(screen,c,offx+x*16,offy+y*16,16,16);
		}
	}

}
int keys()
{
	int i;
	SDL_Event event;

	if(Pause_Flag)
		Sleep(100);
	esc_flag=reset_flag=0;
	if(SDL_PollEvent(&event)) 
	{
		switch (event.type) 
		{
		case SDL_MOUSEMOTION:
			i=SDL_GetMouseState(NULL,NULL);
			if(i&SDL_BUTTON(SDL_BUTTON_LEFT))
			{
			}
			break;
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				esc_flag=1;
				break;
			case SDLK_p:
			case SDLK_BACKQUOTE:
				Pause_Flag^=1;
				if(Pause_Flag)
					SDL_PauseAudio(TRUE);
				else
					SDL_PauseAudio(FALSE);
				break;
			case SDLK_KP_MINUS:
				key_modify=-1;
				break;
			case SDLK_KP_PLUS:
				key_modify=1;
				break;
			case SDLK_1:
				reset_flag=1;
				GameExecCtrlFlag=1;
				ClearScreenFlag=1;
				break;
			case SDLK_LCTRL:
			case SDLK_RETURN:
			case SDLK_SPACE:
				joyfire=1;
				break;
			case SDLK_LEFT:
				JoystickDirBits|=1<<2;
				break;
			case SDLK_RIGHT:
				JoystickDirBits|=1<<3;
				break;
			case SDLK_UP:
				JoystickDirBits|=1<<0;
				break;
			case SDLK_DOWN:
				JoystickDirBits|=1<<1;
				break;
			}
			break;
		case SDL_KEYUP:
			switch(event.key.keysym.sym)
			{
			case SDLK_KP_PLUS:
			case SDLK_KP_MINUS:
				key_modify=0;
				break;
			case SDLK_LCTRL:
			case SDLK_RETURN:
			case SDLK_SPACE:
				joyfire=0;
				break;
			case SDLK_LEFT:
				JoystickDirBits&=~(1<<2);
				break;
			case SDLK_RIGHT:
				JoystickDirBits&=~(1<<3);
				break;
			case SDLK_UP:
				JoystickDirBits&=~(1<<0);
				break;
			case SDLK_DOWN:
				JoystickDirBits&=~(1<<1);
				break;
			case SDLK_ESCAPE:
				exit(0);
				break;
			}
			break;
		case SDL_QUIT:
			exit(0);
			break;
		}
	}
	
}

