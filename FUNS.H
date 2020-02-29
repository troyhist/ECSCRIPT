// ==========================================================================
// ==========================================================================
#define scrWD	320
#define scrHG	200
#define scrnWD	stdresX[res]
#define scrnHG	stdresY[res]
#define dx	0			// Смещения по x, y для вывода шpифта
#define dy	50			// на экpан
#define outX	scrWD/2
#define outY	scrHG/2
#define ESC	0x1b
#define GREY	32+8
// ==========================================================================
#define typeERR	-1
#define typeUNKNOW	0
#define typePAL	1
#define typeVGA	2
#define typeLMO	3
#define typeSPR	4
#define typeF16	5
#define typeF14	6
#define typeF8	7
#define typeTXC 8
#define typeCGA 9
#define typeGIF 10
// ==========================================================================
#define MAXTYPES	10		// Кол-во обpабатываемых pасшиpений
// ==========================================================================
#define dotString	"."
// ==========================================================================
// Functions ... FUNS
// ==========================================================================
short getBestRes(short wd, short hg);
int getFileType	(char* filename);
short setXMode	(short mode);
int viewPal	(char* filename);
int viewDump	(char* filename, void* ptr);
int viewLmo	(char* filename);
int viewSpr	(char* filename);
int viewFnt	(char* filename, int size);
int viewTxc	(char* filename);
int viewGif	(char* filename);
// --------------------------------------------------------------------------
#define viewVga(f)	viewDump(f, videoVGA)
#define viewCga(f)	viewDump(f, videoCGA)
#define viewF16(f)	viewFnt(f, 16)
#define viewF14(f)	viewFnt(f, 14)
#define viewF8(f)	viewFnt(f, 8)
// ==========================================================================
// Procedures ... FUNS
// ==========================================================================
void infoPal 	(void* ptr, char* filename);
void infoLmo 	(void* ptr, char* filename);
void infoSpr 	(void* ptr, char* filename);
void infoFnt	(void* ptr, char* filename);
void infoGif	(void* ptr, char* filename);
void currPal 	(char* filename, int flag, int colors, int bpc);
// --------------------------------------------------------------------------
void saveWhite	(void);
#define showWhite()	set_RGBCOL(7,GREY,GREY,GREY)
#define loadWhite()	set_RGBCOL(7,white.r,white.g,white.b)
// ==========================================================================