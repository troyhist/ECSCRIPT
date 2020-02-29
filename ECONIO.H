// =======================================================================
// Enhanced CONIO ver. 1.06 (C, CPP)
// Troy computing, Inc.
// Copyright (c) 1997-2000
// --------------------------------------------------------------------------
// Extension of CONIO library.
// Used library: <none>
// -----------------------------------------------------------------------
// File: econio.h
// -----------------------------------------------------------------------
//     Compiler: Borland C++ 5.02
//  Application: DOS, Win32 console
// -----------------------------------------------------------------------
// Made date: 18/IIX.1997
// Last edit: 28/  V.2000
//     Build: 2
// =======================================================================
#include <conio.h>                      // clrscr, textcolor, textbackground
// =======================================================================
#define EPOSITIVE       1               // Положительное число
// =======================================================================
// =======================================================================
// Procedures ... ECONIO
// =======================================================================
#define ecls()          clrscr()        // Очистка экpана
#define efore(fg)       textcolor(fg+_emgr.blink)
#define eback(bg)       textbackground(bg+_emgr.blink)
#define escanf          cscanf
// =======================================================================
#ifdef EConvert                         // Конвеpтиpование CR->CR/LF
#include <stdio.h>
#undef escanf
#define escanf          scanf
#endif
// =======================================================================
#ifdef __cplusplus
extern "C" {
#endif
// =======================================================================
int einit(void);		// inits econio
void ebright(int);
// -----------------------------------------------------------------------
// Description: Brightness / Intense of background
//   Parametrs: 1 - br
//		0 - in
//      Return: <none>
// -----------------------------------------------------------------------
void ecolor (int, int);
// -----------------------------------------------------------------------
// Description: Color setup
//   Parametrs:
//      Return: NULL - SUCCESS ...
// -----------------------------------------------------------------------
int eprintf (const char* format, ...);  // cprintf analog
int eputs (char*);                      // Output to stdout
// -----------------------------------------------------------------------
int estatus(void);		// prints status of Econio
int ehelp(void);		// prints help screen of Econio
// =======================================================================
#ifdef __cplusplus
}
#endif
// =======================================================================
#ifndef _ECONIOS
//#define _ECONIOS
extern struct 	{
	int init;		// "init" flag
	int bmode;		// mode of work (default is mINTER)
	int amode;		// addressing mode (default is aIMM)
	int rem;		// remark (comment) flag
	int sub;		// current sub number (main = 0)
	int commands;		// quantity of commands
	int blink;		// blinking mode
	int varl[255];		// length of variables
	void* var[255];		// variable's pointers
	} _emgr;
#endif
// =======================================================================
