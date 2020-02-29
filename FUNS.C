// ==========================================================================
// ==========================================================================
#include <string.h>
#include <stdio.h>
#define __uses_bgi_
#define __uses_pcpal_
#define __uses_pcvga_
#define __uses_pcspr_
#define __uses_pcfnt_
#define __uses_pcgif_
#include "_graphs.h"
#include "econio.h"
#include "funs.h"
// ==========================================================================
char* defaultExtension[]={"", "pal", "vga", "lmo", "spr",	// 0-4
"f16", "f14", "f8", "txc", "cga", "gif"};			// 5-10
// --------------------------------------------------------------------------
#define MAXRESOL	5
// --------------------------------------------------------------------------
short stdresX[]={320, 640, 640, 800, 1024};
short stdresY[]={200, 400, 480, 600, 768};
short stdMode[]={0x13, 0x100, 0x101, 0x103, 0x105};
// ==========================================================================
rgbpal_struct white;
// ==========================================================================
short getBestRes(short wd, short hg)
	{
	short i;
	for (i=0; i<MAXRESOL; i++)
		if (wd>stdresX[i] || hg>stdresY[i])
			i++;
		else
			break;
	return (i);
	}
// ==========================================================================
int getFileType(char* filename)
	{
	int type, i;
	char* dotPointer;		// Указатель на точки в имени файла
	dotPointer=(char*)strstr(filename, dotString)+1;
	if (dotPointer)
		{
		for (i=0; i<=MAXTYPES; i++)
			if (!stricmp(dotPointer, defaultExtension[i]))
				type=i;
		}
	else
		return(typeUNKNOW);
	return (type);
	}
// ==========================================================================
short setXMode	(short mode)
	{
	//short ret;
	asm	{
		mov ax, 0x4f02
		mov bx, mode
		int 0x10;
		//mov word ptr ret, ax
		}
	return (0);
	}
// ==========================================================================
int viewPal (char* filename)
	{
	// ------------------------------------------------------------------
	int errorcode;
	char pal[memh_PAL()+(MAXCOLORS+1)*3];
	// ------------------------------------------------------------------
	errorcode=loadHeader_PAL(filename, pal);
	if (!errorcode)
		{
		// ----------------------------------------------------------
		infoPal (pal, filename);
		// ----------------------------------------------------------
		set_PAL((rgbpal_struct*)(pal+memh_PAL()));
		scr_SWITCH (OFF);
		drw_PAL (0, 50, 316, 150);
		scr_SWITCH (ON);
		getch();
		// ----------------------------------------------------------
		}
	return(errorcode);
	}
// ==========================================================================
int viewDump (char* filename, void* ptr)
	{
	int errorcode;
	char name[64];
	rgbpal_struct pal[MAXCOLORS+1];
	// ------------------------------------------------------------------
	strcpy (name, filename);
	if (strstr(name, defaultExtension[2]))
		strcpy (strstr(name, defaultExtension[2]),
						defaultExtension[1]);
	if (load_PAL(name, pal)==load_OK) set_PAL(pal);
	// ------------------------------------------------------------------
	errorcode=active_load_VGA(filename, ptr);
	if (errorcode==load_OK) getch();
	// ------------------------------------------------------------------
	return(errorcode);
	}
// ==========================================================================
int viewLmo (char* filename)
	{
	int errorcode=load_NOMEM;
	void* lmo=newm(char, meml_LMO(filename));
	if (lmo)
		{
		errorcode=load_LMO(filename, lmo);
		if (errorcode==load_OK)
			infoLmo(lmo, filename);
		deletem(lmo);
		}
	return (errorcode);
	}
// ==========================================================================
int viewSpr (char* filename)
	{
	int errorcode=load_NOMEM;
	void* spr=newm(char, meml_SPR(filename));
	if (spr)
		{
		errorcode=load_SPR(filename, spr);
		if (errorcode==load_OK)
			{
			if (pflag_SPR(spr))
				set_PAL((void*)pflag_SPR(spr));
			infoSpr(spr, filename);
			}
		deletem(spr);
		}
	return (errorcode);
	}
// ==========================================================================
int viewFnt (char* filename, int size)
	{
	int errorcode=load_NOMEM;
	int k, l;
	int wd, hg;
	void* fnt=load_data_FNT(filename, 8, size);
	if (fnt)
		{
		// ----------------------------------------------------------
		errorcode=load_OK;
		infoFnt(fnt, filename);
		// ----------------------------------------------------------
		scr_SWITCH (OFF);
		// ----------------------------------------------------------
		for (k=0; k<8; k++)
			for (l=0; l<16; l++)
				{
				print_char (k*16+l, fnt, l*wd_FNT(fnt)+dx,
							k*hg_FNT(fnt)+dy, 7);
				print_char (k*16+l+128, fnt, scrWD/2+dx+
					 l*wd_FNT(fnt), k*hg_FNT(fnt)+dy, 7);
				}
		// ----------------------------------------------------------
		scr_SWITCH (ON);
		// ----------------------------------------------------------
		getch();
		deletem(fnt);
		// ----------------------------------------------------------
		}
	return (errorcode);
	}
// ==========================================================================
int viewTxc (char* filename)
	{
	// ------------------------------------------------------------------
	long l=mem_load(filename);
	char* buffer;
	FILE* fp;
	// ------------------------------------------------------------------
	if (l!=-1 && l)
		{
		buffer=newm(char, l+1);
		if (buffer)
			{
			// --------------------------------------------------
			fp=fopen(filename, "rb");
			fread (buffer, l, 1, fp);
			fclose(fp);
			buffer[l]=NULL;
			// --------------------------------------------------
			closegraph();
			eprintf(buffer);
			getch();
			deletem(buffer);
			// --------------------------------------------------
			}
		else
			return (load_NOMEM);
		}
	else
		return (load_FILENFND);
	return (load_OK);
	// ------------------------------------------------------------------
	}
// ==========================================================================
int viewGif (char* filename)
	{
	// ------------------------------------------------------------------
	int i, errorcode=load_NOMEM;
	void* gif=newm(char, meml_GIF(filename));
	rgbpal_struct* pal;
	// ------------------------------------------------------------------
	if (gif)
		{
		errorcode=load_GIF(filename, gif);
		if (errorcode==load_OK)
			{
			if (pflag_GIF(gif))
				{
				pal=ptrpal_GIF(gif);
				for (i=0; i<colors_GIF(gif); i++)
					{
				pal[i].r=cvt_component_in(pal[i].r, VGA_BPC);
				pal[i].g=cvt_component_in(pal[i].g, VGA_BPC);
				pal[i].b=cvt_component_in(pal[i].b, VGA_BPC);
					}
				set_PAL(pal);
				}
			infoGif(gif, filename);
			}
		deletem(gif);
		}
	return (errorcode);
	}
// ==========================================================================
// Procedures ... FUNS
// ==========================================================================
void infoPal (void* ptr, char* filename)
	{
	set_CURSOR_POS (0, 0);
	printf ("Palette information (%s):\n\n"
		"exCOL  - %5d - exist colors\n"
		"stCOL  - %5d - start color\n"
		"ptype  - %5d - palette type\n",
		filename, exCOL_PAL(ptr), stCOL_PAL(ptr), ptype_PAL(ptr));
	}
// ==========================================================================
void infoLmo (void* ptr, char* filename)
	{
	int ch;
	do
	{
	scr_SWITCH (OFF);
	put_LMO((scrWD-wd_LMO(ptr))/2, (scrHG-hg_LMO(ptr))/2, ptr);
	scr_SWITCH (ON);
	ch=getch();
	// ------------------------------------------------------------------
	if (ch=='I' || ch=='i')
		{
		efore(0); clrscr(); set_CURSOR_POS(0, 4);
		printf ("Local Motion Object information:\n\n");
		infn_LMO(ptr, filename);
		getch();efore(0); clrscr();
		}
	// ------------------------------------------------------------------
	} while (ch=='I' || ch=='i');
	}
// ==========================================================================
void infoSpr (void* ptr, char* filename)
	{
	int ch=0, i=0;
	void *back, *lmo;
	void far* buffer;
	// ------------------------------------------------------------------
	buffer=crt_SPAGE (scrWD, scrHG, 8); if (!buffer) return;
	clr_SPAGE(buffer);
	// ------------------------------------------------------------------
	do
	{
	// ------------------------------------------------------------------
	// Пpовеpка наличия спpайтов в цепочке
	// ------------------------------------------------------------------
	if (sic_SCD(ptrchain_SPR(ptr, i)))
	{
	// ------------------------------------------------------------------
	putchain_sSPR(outX, outY, i, ptr, buffer);
	lmo=ptrsprite_SPR(ptr, i, sic_SCD(ptrchain_SPR(ptr, i))-1);
	back=newm(char, mem_LMO(lmo));
	if (back)
		{
		get_sBACK (outX-hsX_LMO(lmo), outY-hsY_LMO(lmo),
			outX+hsX_LMO(lmo), outY+hsY_LMO(lmo), back, buffer);
		put_sLMO (outX-hsX_LMO(lmo), outY-hsY_LMO(lmo), lmo, buffer);
		}
	// ------------------------------------------------------------------
	}
	// ------------------------------------------------------------------
	ch=getch();
	// ------------------------------------------------------------------
	if (ch=='I' || ch=='i')
		{
		// ----------------------------------------------------------
		saveWhite(); showWhite(); efore(0); clrscr();
		set_CURSOR_POS(0, 0);
		printf ("Sprite & Chain information:\n\n");
		infn_SPR(ptr, filename);
		info_SCD(ptrchain_SPR(ptr, i));
		ch=0;getch();efore(0); clrscr(); loadWhite();
		// ----------------------------------------------------------
		}
	else if (ch=='P' || ch=='p')
		{
		// ----------------------------------------------------------
		saveWhite(); showWhite(); efore(0); clrscr();
		set_CURSOR_POS(0, 0); scr_SWITCH (OFF);
		currPal (filename,pflag_SPR(ptr)&&1,colors_SPR(ptr),VGA_BPC);
		scr_SWITCH (ON);
		ch=0;getch();efore(0); clrscr(); loadWhite();
		// ----------------------------------------------------------
		}
	// ------------------------------------------------------------------
	if (sic_SCD(ptrchain_SPR(ptr, i)))
	{
	// ------------------------------------------------------------------
	if (back)
		{
		// Возвpащаем фон на стаpое место
		put_sBACK(outX-hsX_LMO(lmo),outY-hsY_LMO(lmo),back,buffer);
		deletem(back);
		}
	// ------------------------------------------------------------------
	}
	// ------------------------------------------------------------------
	if (++i>=chains_SPR(ptr))
		i=0;
	} while (ch!=ESC);
	deletem (buffer);
	}
// ==========================================================================
void infoFnt (void* ptr, char* filename)
	{
	set_CURSOR_POS (0, 0);
	printf ("Bit Font information (%s):\n\n"
		"ftype  - %2d - bit font type\n"
		"cWD    - %2d - character width\n"
		"cHG    - %2d - character height\n",
		filename, ftype_FNT(ptr), wd_FNT(ptr), hg_FNT(ptr));
	}
// ==========================================================================
void infoGif (void* ptr, char* filename)
	{
	short res, clsflag=1;
	int ch;
	void far* buffer=NULL;
	// ------------------------------------------------------------------
	res=getBestRes(sWD_GIF(ptr), sHG_GIF(ptr));
	// ------------------------------------------------------------------
	// Устанавливаем SVGA видеоpежим
	// ------------------------------------------------------------------
	if (res)
		{
		setXMode(stdMode[res]);
		if (pflag_GIF(ptr))
			set_PAL(ptrpal_GIF(ptr));
		}
	// ------------------------------------------------------------------
	if (!res)
		{
		buffer=crt_SPAGE (scrWD, scrHG, 8); if (!buffer) return;
		}
	// ------------------------------------------------------------------
	if (!res) clr_SPAGE(buffer);
	scr_SWITCH (OFF);
	uput_GIF((scrnWD-sWD_GIF(ptr))/2,(scrnHG-sHG_GIF(ptr))/2, ptr, buffer);
	scr_SWITCH (ON);
	// ------------------------------------------------------------------
	do
	{
	if (clsflag)
		{
		if (!res)
			shw_SPAGE(buffer);
		else
			{
			scr_SWITCH (OFF);
			uput_GIF((scrnWD-sWD_GIF(ptr))/2,
			(scrnHG-sHG_GIF(ptr))/2, ptr, buffer);
			scr_SWITCH (ON);
			}
		clsflag=0;
		}
	ch=getch();
	// ------------------------------------------------------------------
	if (ch=='I' || ch=='i')
		{
		// ----------------------------------------------------------
		saveWhite(); showWhite(); efore(0); clrscr();
		set_CURSOR_POS(0, 4);
		printf ("Graphics Interchange Format information:\n");
		infn_GIF(ptr, filename);
		ch=0;getch();efore(0); clrscr(); loadWhite(); clsflag=1;
		// ----------------------------------------------------------
		}
	else if (ch=='P' || ch=='p')
		{
		// ----------------------------------------------------------
		saveWhite(); showWhite(); efore(0); clrscr();
		set_CURSOR_POS(0, 0); scr_SWITCH (OFF);
		currPal (filename, pflag_GIF(ptr), colors_GIF(ptr),
							       bpc_GIF(ptr));
		scr_SWITCH (ON);
		ch=0;getch();efore(0); clrscr(); loadWhite(); clsflag=1;
		// ----------------------------------------------------------
		}
	// ------------------------------------------------------------------
	} while (ch!=ESC);
	if (!res) deletem (buffer);
	}
// ==========================================================================
void currPal (char* filename, int flag, int colors, int bpc)
	{
	printf ("Palette information (%s):\n\n"
		"pflag  - %3d - флаг палитpы\n"
		"colors - %3d - цветов в палитpе\n"
		"bpc    - %3d - бит на компонентy\n",
		filename, flag, colors, bpc);
	// ------------------------------------------------------------------
	if (flag && colors<(MAXCOLORS+1))
		{
		int i;
		rgbpal_struct pal[MAXCOLORS+1];
		get_PAL(pal);
		for (i=colors; i<MAXCOLORS+1; i++)
			pal[i].r=pal[i].g=pal[i].b=0;
		set_PAL(pal);
		}
	// ------------------------------------------------------------------
	drw_PAL (0, 96, 316, 196);
	}
// ==========================================================================
void saveWhite (void)
	{
	white.r=get_REDATTR(7);
	white.g=get_GRNATTR(7);
	white.b=get_BLUATTR(7);
	}
// ==========================================================================
