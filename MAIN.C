// ==========================================================================
#include <string.h>
#include <stdio.h>
// --------------------------------------------------------------------------
#define __uses_bgi_
#include "_graphs.h"
#include "econio.h"
#include "pcerr.h"
#include "funs.h"
#include "help.h"
// ==========================================================================
int main(int argc, char* argv[])
	{
	// ------------------------------------------------------------------
	int t, errorcode=0;
	// ------------------------------------------------------------------
	einit();
	eprintf("#cls"
		"#c1500PcvsView 2.32       #f11Universal Graphics Viewer!"
		"\r\n#f14Copyright (c) 1995-'98 by Troy computing, Inc.\r\n"
		"#f07Compiled on "__DATE__", "__TIME__" All rights reserved"
		"\r\n\r\n");
	// ------------------------------------------------------------------
	if (argc==1)
		eprintf ("#f07Type PCVSVIEW #f15/?#f07 - to help\r\n");
	else if (argc>=2)
		if (!strcmp(argv[1], "/?"))
			if (!strcmp(argv[2], "/r")) eprintf (help_rus);
			else eprintf (help_eng);
		else
			{
			t=getFileType(argv[1]);
			if (t)
			{
			if (kbhit()) getch();
			if (t!=typeTXC) setgraphmode(VGALO);
			switch(t)
				{
				case 1: errorcode=viewPal(argv[1]);
				break;
				case 2: errorcode=viewVga(argv[1]);
				break;
				case 3: errorcode=viewLmo(argv[1]);
				break;
				case 4: errorcode=viewSpr(argv[1]);
				break;
				case 5: errorcode=viewF16(argv[1]);
				break;
				case 6: errorcode=viewF14(argv[1]);
				break;
				case 7: errorcode=viewF8(argv[1]);
				break;
				case 8: errorcode=viewTxc(argv[1]);
				break;
				case 10: errorcode=viewGif(argv[1]);
				break;
				case 9:
				setgraphmode(CGALO);
				errorcode=viewCga(argv[1]);
				break;
				default:
				errorcode=load_WRNGFMT;
				break;
				}
			if (t!=typeTXC) closegraph();
			if (errorcode)
				eprintf ("#f15Error: #f07%d.%s\r\n",
				errorcode, _ferror[errorcode]);
			}
			else
			   eprintf ("#f15Error: #f07Need an extension.\r\n");
			};
	// ------------------------------------------------------------------
	return (0);
	}
// ==========================================================================
// ==========================================================================