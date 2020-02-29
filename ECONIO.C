// =======================================================================
// Enhanced CONIO ver. 1.06 (C, CPP)
// Troy computing, Inc.
// Copyright (c) 1997-'2000
// --------------------------------------------------------------------------
// Extension of CONIO library.
// Used library: memc.h
// -----------------------------------------------------------------------
// File: econio.h
// -----------------------------------------------------------------------
//     Compiler: Borland C++ 3.1, 5.01
//  Application: DOS, Win32 console
// -----------------------------------------------------------------------
// Made date: 18/IIX.1997
// Last edit: 03/ VI.2000
//     Build: 2
// =======================================================================
// Precompile definitions ... ECONIO
// =======================================================================
#define _ECONIOS
#define _ECDOS16                // Don't allocate more than 8 Kb
// =======================================================================
// Debug defines ... Please, remark in release
// =======================================================================
//#define _ECDEBUG              // Enable output in estatus(), ehelp()
//#define _ECDEBUG00            // Show current command name on screen
#define _ECDEBUG01              // Make pause after _ECDEBUG00
// =======================================================================
#include "memc.h"
#include "econio.h"
// --------------------------------------------------------------------------
#include <dos.h>                        // geninterrupt
#include <stdio.h>                      // vsprintf
#include <string.h>                     // strnicmp
#include <stdarg.h>                     // va_list
// ==========================================================================
#define ever            "1.06"          // Version
#define ECTRL           "#"             // Control character
#define EMAXC           50              // MAX quantity of commands
// ==========================================================================
#ifdef _ECDOS16
#define _ECMEMALLOC     8162            // uses in eprintf(...)
#define _ECMAXBLOCK     8162            // max block in eputs(...)
#else
#define _ECWIN32
#define _ECMEMALLOC     65535           // uses in eprintf(...)
#define _ECMAXBLOCK     16384           // max block in eputs(...)
#endif
// ==========================================================================
/*
        Syntax of eputs:

        ....#<...>....          - знак '#' является yпpавляющим
        ....##........          - вывод знака '#'

        Commands:

        ---------------------------------------------------------------
        --- New Commands (these commands don't exist in ver. 1.05) ---
        ---------------------------------------------------------------
        #batch<m>               - batch mode switch
                                  m=0 - batch mode off, otherwise on
                                  in the batch mode skips 'adr', 'gkh',
                                  'gk' and 'gke', 'gtxt'
        #gtxt<ml><vn>           - gets 'ml' or less symbols from 'stdin'
                                  (console) to variable with no. 'vn'
        #rem                    - remarks (comments) start/stop flag
        #gkh<vn>                - gets status of keyboard buffer (empty,
                                  or not) into variable with no. 'vn'
        #gk<vn>, #gke<vn>       - gets key code ('gke' with echo) into
                                  variable with no. 'vn'
        #var<vn><vl>            - creates variable with no. 'vn' with size
                                  'vl' bytes
                                  'vn' range is [0..99]
                                  'vl' range is [0..99]
        #win<xx><yy><wd><hg>    - creating of text window
                                  xx, yy   - position
                                  wd, hg   - width, height
        #wina<x1><y1><x2><y2>   - creating of text window
                                  x1, y1   - left corner
                                  x2, y2   - right corner
        #adr<xx>                - addressing mode
                                  00 - immediate addressing
                                  01 - direct addressing
                                  for ex.:
                                  #adr0#c0110  - makes color 01, 10
                                  #adr1#c0110  - makes color with
                                                 values of memory cells
                                                 01 and 10, even they
                                                 aren't defined
        #loc<xx><yy>            - locate cursor position at current window
        #sub<xx>                - macro-subroutine with number 'xx'
                                  always skips. All substs must be at the
                                  end of TXC file
        #if<vn><xx>             - check variable with no. 'vn' for non-zero
                                  value and tries to call sub with no. 'xx'
        ---------------------------------------------------------------
        --- Old Commands (this commands exists in ver. 1.05)        ---
        ---------------------------------------------------------------
        #cls                    - clear screen
        #cur<p>                 - тип кypсоpа
        #bl<p>                  - включение или выключение мигания
        #br<p>                  - включение или выключение яpкости фона
        #c<ffbb>                - yстановка цвета символов и фона
        #f<ff>                  - yстановка цвета символов
        #b<bb>                  - yстановка цвета фона
        #g                      - паyза (остановка до нажатия клавиши)
*/
// ==========================================================================
// Internal constants ... ECONIO
// ==========================================================================
#define mBATCH  1               // Batch mode
#define mINTER  0               // Interactive mode
// -----------------------------------------------------------------------
#define aIMM    0               // Immediate addresssing
#define aDIR    1               // Direct addressing
#define aPTR    2               // Pointer addressing (not realized)
// -----------------------------------------------------------------------
#define eMAXVAR 100             // Max. quantity of variables
// -----------------------------------------------------------------------
#define eRADIX  100             // Radix*10 of digits
// =======================================================================
void _econtrol(char*);  void _ebatch(char*);    void _egtxt(char* par);
void _ecls(char* par);  void _ecur(char* par);  void _erem(char* par);
void _egkh(char* par);  void _egke(char* par);  void _evar(char* par);
void _ewin(char* par);  void _eadr(char* par);  void _eloc(char* par);
void _esub(char* par);  void _ebl(char* par);   void _ebr(char* par);
void _egk(char* par);   void _eif(char* par);   void _ec(char* par);
void _ef(char* par);    void _eb(char* par);    void _eg(char* par);
void _ewina(char* par);
// =======================================================================
// Econio manager's struct
// =======================================================================
struct  {
        int init;               // "init" flag
        int bmode;              // mode of work (default is mINTER)
        int amode;              // addressing mode (default is aIMM)
        int rem;                // remark (comment) flag
        int sub;                // current sub number (main = 0)
        int commands;           // quantity of commands
        int blink;              // blinking mode
        int varl[255];          // length of variables
        void* var[255];         // variable's pointers
        } _emgr={0};
// =======================================================================
struct  {
        int parl;               // parameter's length
        void (*f)(char*);       // function address
        char* cname;            // command name
        } _ecmds[]=
        {{0, &_econtrol, "#"},  {0, &_ecls,"cls"},
        {1, &_ebl, "bl"},       {1, &_ebr, "br"},
        {4, &_ec, "c"},         {2, &_ef, "f"},
        {2, &_eb, "b"},         {1, &_ecur, "cur"},
        {0, &_eg, "g"},         {1, &_ebatch, "batch"},
        {4, &_egtxt, "gtxt"},   {0, &_erem, "rem"},
        {2, &_egkh, "gkh"},     {2, &_egk, "gk"},
        {2, &_egke, "gke"},     {4, &_evar, "var"},
        {8, &_ewin, "win"},     {1, &_eadr, "adr"},
        {4, &_eloc, "loc"},     {2, &_esub, "sub"},
        {4, &_eif, "if"},       {8, &_ewina, "wina"},
        {0, NULL, NULL}};
// =======================================================================
struct  {
        int enumb;              // Number of the command in sorted list
        int ecoml;              // Length of mnemonic name
        int eparl;              // Length of parameter list
        void (*f)(char*);       // Function address
        char* ecmds;            // Command
        } edata[EMAXC];
// =======================================================================
// Manager functions
// =======================================================================
int efindnumb (int numb)
{
// -----------------------------------------------------------------------
// Description: Searchs command with given number
//   Parametrs: numb    - number of command
//      Return: number of command
// -----------------------------------------------------------------------
unsigned i;
for (i=0; i<_emgr.commands; i++)
        if (edata[i].enumb==numb)
                return(i);
return(0);
}
// =======================================================================
int einit(void)
{
// -----------------------------------------------------------------------
// Description: Init's all econio system. Must be called FIRST
//   Parametrs: <none>
//      Return: NULL - SUCCESS ...
// -----------------------------------------------------------------------
int i, j, done, tempnums[EMAXC];
_emgr.init=1;           // inited
_emgr.bmode=mINTER;     // interactive mode
_emgr.amode=aIMM;       // immediate addressing
_emgr.rem=0;            // remark = FALSE
_emgr.sub=0;            // sub = main
_emgr.commands=0;       // no commands yet
_emgr.blink=0;          // no blinking
for (i=0; i<eMAXVAR; i++) { _emgr.varl[i]=0; _emgr.var[i]=NULL; }
// -----------------------------------------------------------------------
// Calculates commands quantity
// -----------------------------------------------------------------------
for (i=0; i<EMAXC; i++)
        if (_ecmds[i].cname)
                { _emgr.commands++; edata[i].enumb=i; tempnums[i]=i;
                edata[i].ecoml=strlen(_ecmds[i].cname);
                edata[i].eparl=_ecmds[i].parl; edata[i].f=_ecmds[i].f;
                edata[i].ecmds=_ecmds[i].cname;}
        else    break;
// -----------------------------------------------------------------------
// Sorts commands by mnemonic length (max->min)
// -----------------------------------------------------------------------
do      {
        done=1;
        for (i=1; i<_emgr.commands-1; i++) // i=1, for skipping the '#'
                if(edata[tempnums[i]].ecoml<edata[tempnums[i+1]].ecoml)
                        { int temp=tempnums[i];
                        tempnums[i]=tempnums[i+1];
                        tempnums[i+1]=temp; done=0;
                        }
        } while (!done);
// -----------------------------------------------------------------------
for (i=0; i<_emgr.commands; i++)
        for (j=0; j<_emgr.commands; j++)
                if (tempnums[j]==i) edata[i].enumb=j;
// -----------------------------------------------------------------------
return (NULL);
}
// =======================================================================
// Econio Commands
// =======================================================================
void _econtrol(char* par)
{
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: econtrol (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; cputs(ECTRL); if (par); }
// -----------------------------------------------------------------------
void _ebatch(char* par)
{
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ebatch (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; sscanf(par, "%d", &_emgr.bmode);
}
// -----------------------------------------------------------------------
void _egtxt(char* par)
{
int len, varn;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _egtxt (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; if (_emgr.bmode) return;
sscanf(par, "%d", &len); varn=len%eRADIX; len/=eRADIX;
if (!_emgr.varl[varn]) return;
if (len>_emgr.varl[varn]) len=_emgr.varl[varn]-1;
*(int*)_emgr.var=len; cgets((char*)_emgr.var);
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _egtxt (varn=%d, len=%d)", varn, len);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _ecls(char* par)
{
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ecls (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; clrscr(); if (par); }
// -----------------------------------------------------------------------
void _ecur(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ecur (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p);
if (p==0) _setcursortype(_NOCURSOR);
if (p==1) _setcursortype(_NORMALCURSOR);
if (p==2) _setcursortype(_SOLIDCURSOR);
}
// -----------------------------------------------------------------------
void _erem(char* par)
{
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _erem (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
_emgr.rem=!_emgr.rem; if (par); }
// -----------------------------------------------------------------------
void _egkh(char* par)
{
int p0;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _egkh (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; if (_emgr.bmode) return;
sscanf(par, "%d", &p0); if (!_emgr.varl[p0]) return;
((int*)_emgr.var)[p0]=kbhit();
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _egkh (varn=%d, varl=%d)",p0,_emgr.varl[p0]);
#ifdef _ECDEBUG01
getch();
#endif
#endif

}
// -----------------------------------------------------------------------
void _egke(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _egke (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; if (_emgr.bmode) return;
sscanf(par, "%d", &p); if (!_emgr.varl[p]) return;
((int*)_emgr.var)[p]=getche();
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _egke (varn=%d, varl=%d)", p, _emgr.var[p]);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _evar(char* par)
{
int p0, p1;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _evar (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p0); p1=p0%eRADIX; p0/=eRADIX;
if (_emgr.varl[p0]) deletem(_emgr.var);
if (!p1) return;                // Deletes of variable
(int*)_emgr.var[p0]=newm(int, p1); _emgr.varl[p0]=p1;
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _evar (varn=%d, varl=%d)", p0, p1);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _ewin(char* par)
{
int p0, p1, p2, p3;
long p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ewin (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%ld", &p); p3=p%eRADIX; p/=eRADIX;
p2=p%eRADIX; p/=eRADIX; p1=p%eRADIX; p0=p/eRADIX;
if (_emgr.amode==aDIR)
        {
        if (!(_emgr.varl[p0] && _emgr.varl[p1] &&
                _emgr.varl[p2] && _emgr.varl[p3])) return;
        p0=*(int*)_emgr.var[p0]; p1=*(int*)_emgr.var[p1];
        p2=*(int*)_emgr.var[p2]; p3=*(int*)_emgr.var[p3];
        }
window(p0, p1, p0+p2, p1+p3);
#ifdef _ECDEBUG00
gotoxy(1,2);
printf ("_ECDEBUG: _ewin (l,r,t,b=%d, %d, %d, %d)",p0,p1,p0+p2,p1+p3);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _ewina(char* par)
{
int p0, p1, p2, p3;
long p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ewina (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%ld", &p); p3=p%eRADIX; p/=eRADIX;
p2=p%eRADIX; p/=eRADIX; p1=p%eRADIX; p0=p/eRADIX;
if (_emgr.amode==aDIR)
        {
        if (!(_emgr.varl[p0] && _emgr.varl[p1] &&
                _emgr.varl[p2] && _emgr.varl[p3])) return;
        p0=*(int*)_emgr.var[p0]; p1=*(int*)_emgr.var[p1];
        p2=*(int*)_emgr.var[p2]; p3=*(int*)_emgr.var[p3];
        }
window(p0, p1, p2, p3);
#ifdef _ECDEBUG00
gotoxy(1,2);
printf ("_ECDEBUG: _ewina (l,r,t,b=%d, %d, %d, %d)",p0,p1,p2,p3);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _eadr(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _eadr (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p); _emgr.amode=p;
}
// -----------------------------------------------------------------------
void _eloc(char* par)
{
int p0, p1;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _eloc (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p0); p1=p0%eRADIX; p0/=eRADIX;
if (_emgr.amode==aDIR)
        {
        if (!(_emgr.varl[p0] && _emgr.varl[p1])) return;
        p0=*(int*)_emgr.var[p0]; p1=*(int*)_emgr.var[p1];
        }
gotoxy(p0, p1);
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _eloc (x=%d, y=%d)", p0, p1);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _esub(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _esub (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p); printf ("\a");
}
// -----------------------------------------------------------------------
void _ebl(char* par)
{
int p;
struct text_info ti;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ebl (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p); _emgr.blink=p?BLINK:0;
gettextinfo(&ti); efore(ti.attribute&0x0f);
//textattr(ti.attribute);
}
// -----------------------------------------------------------------------
void _ebr(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ebr (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p); ebright(p);
}
// -----------------------------------------------------------------------
void _egk(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _egk (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return; if (_emgr.bmode) return;
sscanf(par, "%d", &p); if (!_emgr.varl[p]) return;
((int*)_emgr.var)[p]=getch();
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _egk (varn=%d, varl=%d)",p,_emgr.varl[p]);
#ifdef _ECDEBUG01
getch();
#endif
#endif

}
// -----------------------------------------------------------------------
void _eif(char* par)
{
int p0, p1;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _eif (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p0); p1=p0%eRADIX; p0/=eRADIX;
if (!_emgr.varl[p0]) return;
if (p1);
printf ("\a");
}
// -----------------------------------------------------------------------
void _ec(char* par)
{
int p0, p1;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ec (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p0); p1=p0%eRADIX; p0/=eRADIX;
if (_emgr.amode==aDIR)
        {
        if (!(_emgr.varl[p0] && _emgr.varl[p1])) return;
        p0=*(int*)_emgr.var[p0]; p1=*(int*)_emgr.var[p1];
        }
ecolor(p0, p1);
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _ec (f=%d, b=%d)", p0, p1);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _ef(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _ef (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p);
if (_emgr.amode==aDIR)
        { if (!_emgr.varl[p]) return; p=*(int*)_emgr.var[p]; }
efore(p);
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _ef (f=%d)", p);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _eb(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _eb (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if (_emgr.rem) return;
sscanf(par, "%d", &p);
if (_emgr.amode==aDIR)
        { if (!_emgr.varl[p]) return; p=*(int*)_emgr.var[p]; }
eback(p);
#ifdef _ECDEBUG00
gotoxy(1,2); printf ("_ECDEBUG: _eb (b=%d)", p);
#ifdef _ECDEBUG01
getch();
#endif
#endif
}
// -----------------------------------------------------------------------
void _eg(char* par)
{
int p;
#ifdef _ECDEBUG00
gotoxy(1,1); printf ("_ECDEBUG: _eg (par=%s)", par);
#ifdef _ECDEBUG01
getch();
#endif
#endif
if(_emgr.rem)return; if(_emgr.bmode)return; getch(); if(par);}
// =======================================================================
// Interface functions (Standart functions)
// =======================================================================
void ebright (int par)
{
par=!par;
_AX=0x1003;
_BL=(char)par;
#ifndef _ECWIN32
geninterrupt(0x10);
#endif
}
// =======================================================================
void ecolor (int fg, int bg)
{
efore (fg); eback (bg);
}
// =======================================================================
int eprintf (const char* format, ...)
{
char *buffer, *tempbuf;
int i, blks, ablks, len;
va_list arglist;
va_start(arglist, format);
if ((buffer=newm(char, _ECMEMALLOC))==NULL) return(EOF);
if ((tempbuf=newm(char, _ECMAXBLOCK))==NULL)
        { deletem(buffer); return(EOF); }
vsprintf (buffer, format, arglist);
// -----------------------------------------------------------------------
// Calculates quantity of blocks
// -----------------------------------------------------------------------
len=strlen(buffer)+1; blks=len/_ECMAXBLOCK; ablks=len%_ECMAXBLOCK;
// -----------------------------------------------------------------------
// Output by blocks
// -----------------------------------------------------------------------
for (i=0; i<blks; i++)
        {
        strncpy(tempbuf, buffer+i*_ECMAXBLOCK, _ECMAXBLOCK);
        eputs (tempbuf);
        }
if (ablks)
        {
        strncpy(tempbuf, buffer+blks*_ECMAXBLOCK, len-blks*_ECMAXBLOCK);
        eputs (tempbuf);
        }
// -----------------------------------------------------------------------
deletem(buffer); deletem(tempbuf);
return (EPOSITIVE);
}
// =======================================================================
int eputs (char *s)
{
// -----------------------------------------------------------------------
char temp[256], buffer[_ECMEMALLOC];
char *cptr;
int spos, cpos;         // Start & control position of text block
int i, k;
int len=strlen(s);
// -----------------------------------------------------------------------
if (!_emgr.init) return(printf("Error: Init Econio first !\n"));
// -----------------------------------------------------------------------
spos=0;
// -----------------------------------------------------------------------
do      {
        // ---------------------------------------------------------------
        // Searchs the control code '#'
        // ---------------------------------------------------------------
        cptr=strstr(s+spos, ECTRL);
        if (cptr)
                {
                // -------------------------------------------------------
                // Outputs a text block w/o control code
                // -------------------------------------------------------
                cptr=cptr+1;
                cpos=cptr-s;
                strncpy(buffer, s+spos, cpos-spos-1);
                buffer[cpos-spos-1]=0;
                if (!_emgr.rem) cputs(buffer);
                // -------------------------------------------------------
                // Compares all commands
                // -------------------------------------------------------
                for (i=0; i<_emgr.commands; i++)
                        {
                        k=efindnumb(i);
                        if (!strnicmp (cptr,edata[k].ecmds, edata[k].ecoml))
                                {
                                strncpy(temp,cptr+edata[k].ecoml,
                                        edata[k].eparl);
                                temp[edata[k].eparl]=NULL;
                                edata[k].f(temp);
                                spos=cpos+edata[k].ecoml+edata[k].eparl;
                                break;
                                }
                        }
                // -------------------------------------------------------
                }
        // ---------------------------------------------------------------
        else    {
                if (!_emgr.rem) cputs (s+spos);
                spos=len;
                }
        // ---------------------------------------------------------------
        } while (spos<len);
return (len);
}
// =======================================================================
int estatus(void)
{
// -----------------------------------------------------------------------
// Description: Prints Econio status
//   Parametrs: <none>
//      Return: NULL - SUCCESS ...
// -----------------------------------------------------------------------
#ifdef _ECDEBUG
int i;
printf ("\n"
        "Econio status information:\n"
        "version        = %s\n"
        "_emgr.init     = %d\n"
        "_emgr.bmode    = %d\tmode of work (default is mINTER)\n"
        "_emgr.amode    = %d\taddressing mode (default is aIMM)\n"
        "_emgr.commands = %d\tquantity of commands\n"
        "_emgr.blink    = %d\tblinking mode\n\n",
        ever,_emgr.init,_emgr.bmode,_emgr.amode,_emgr.commands,_emgr.blink
        );
printf ("Variables information\n");
for (i=0; i<eMAXVAR; i++)
        if (_emgr.varl[i])
                printf ("var [%d] = %d\n", i, ((int*)_emgr.varl)[i]);
#endif
return(!_emgr.init);
}
// ==========================================================================
int ehelp(void)
{
// -----------------------------------------------------------------------
// Description: Prints Econio's help page
//   Parametrs: <none>
//      Return: NULL - SUCCESS ...
// -----------------------------------------------------------------------
#ifdef _ECDEBUG
int i;
printf ("\n"
        "Econio's help page (Available commands):\n");
for (i=0; i<_emgr.commands; i++)
        printf ("%2d. %-8s - %2d. #%-8s coml=%2d parl=%2d\n",
                edata[efindnumb(i)].enumb,      edata[efindnumb(i)].ecmds,
                efindnumb(i),                   edata[efindnumb(i)].ecmds,
                edata[efindnumb(i)].ecoml,      edata[efindnumb(i)].eparl);
#endif
return(NULL);
}
// ==========================================================================