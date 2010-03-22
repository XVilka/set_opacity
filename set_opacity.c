/***************************************************************************
                          set_opacity.c  -  description
                             -------------------
    begin                : Wed Aug  17 14:03:54 EST 2005
    copyright            : (C) 2005-2010 by XVilka
    email                : xvilka@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "interface.h"
#include "stack.h"

const char* program_name;
Window target_win = 0;

void print_usage(FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: %s options \n", program_name);
	fprintf(stream,
			" -h --help                           Display this usage information\n"
			" -v --verbose                        Print verbose messages\n"
			" -d --display [host]:display.screen  Select display and screen for usage\n"
			" -i --id PID                         Use pid for select window\n"
			" -m --mouse                          Use mouse for select window\n"
			" -a --all                            Select all windows\n"
			" -n --name NAME                      Use window name for selection\n"
			" -o --opacity 0 .. 1                 Window opacity\n"
			" -w --wid WID                        Use window id for selection\n");
	exit(exit_code);
}

int main(int argc, char *argv[])
{
  int next_option;
  const char *const short_options = "hvad:i:mn:w:o:";
  const struct option long_options[] = {
  		{ "help", 		0, NULL, 'h' },
  		{ "verbose",	0, NULL, 'v' },
		{ "all",		0, NULL, 'a' },
  		{ "display",	1, NULL, 'd' },
  		{ "id",			1, NULL, 'i' },
  		{ "mouse",		0, NULL, 'm' },
  		{ "name",		1, NULL, 'n' },
  		{ "wid",			1, NULL, 'w' },
  		{ "opacity",	1, NULL, 'o' },
  		{ NULL,			0, NULL, 0	 }
  };
  	int verbose = 0;
  	program_name = argv[0];

  	double vopacity;
  	unsigned int opacity;
  	char *display_name = NULL;
  	char *window_name = NULL;
  	pid_t pid = 0;
  	unsigned use_mouse = 1;
  	unsigned all = 0;
  	Window wid = 0;
  	unsigned char *data = NULL;
  	Atom actual;
  	int format;
  	unsigned long n, left;

  	do
	{
  		next_option = getopt_long(argc, argv, short_options, long_options, NULL);
  		switch (next_option)
  		{
  			case 'h':
  				print_usage(stdout, 0);
  			case 'v':
  				verbose = 1;
  				break;
  			case 'd':
  				display_name = optarg;
  				break;
  			case 'i':
  				pid = atoi(optarg);
  				use_mouse = 0;
  				break;
  			case 'm':
  				use_mouse = 1;
  				break;
  			case 'n':
  				window_name = optarg;
  				use_mouse = 0;
  				break;
  			case 'w':
  				wid = atoi(optarg);
  				use_mouse = 0;
  				break;
  			case 'o':
  				vopacity = atof(optarg);
  				opacity = 1;
  				break;
			case 'a':
				all = 1;
				pid = 0;
				wid = 0;
				window_name = NULL;
				break;
  			case '?':
  				print_usage(stderr, 1);
  			case -1:
  				break;
  			default:
  				abort();
  		}
  	}
  	while (next_option != -1);
  	if (verbose) SetDebug(1);

  	Display *dpy = Open_Display (display_name);
  	unsigned screen = DefaultScreen(dpy);

  	if (opacity)
  	{
  			opacity = (unsigned int) (vopacity * OPAQUE);
  	}
  	else
  	{
  			XGetWindowProperty(dpy, target_win, XInternAtom(dpy, OPACITY, False),
  						0L, 1L, False, XA_CARDINAL, &actual, &format, &n, &left, (unsigned char **) &data);
  			if (data != NULL)
  			{
  				memcpy (&opacity, data, sizeof (unsigned int));
  				XFree(( void *) data );
  				if (verbose) fprintf(stdout, "Found property: %g\n", (double) opacity / OPAQUE);
  			}
  			else opacity = OPAQUE;
  			if (opacity != OPAQUE) opacity = OPAQUE;
  			else opacity = 0xc0000000;
  	}
  	if (verbose) fprintf(stdout, "opacity 0x%x\n", opacity);
	if (!all)
	{
		if (use_mouse)
		{
			if(verbose) fprintf(stdout, "Using mouse for selection.\n");
			target_win = Select_Window_by_Mouse(dpy);
			if(verbose) fprintf(stdout, "Window id : %lu(0x%lx)\n", target_win, target_win);
		}
		else if (wid)
		{
			if(verbose) fprintf(stdout, "Using window id for selection.\n");
			target_win = wid;
		}
		else if (pid)
		{
			if(verbose) fprintf(stdout, "Using process id for selection.\n");
			target_win = Get_Window_by_PID(dpy, pid);
			if(target_win) target_win = Get_Top_Window(dpy, target_win);
			if(verbose) fprintf(stdout, "Window id : %lu(0x%lx)\n", target_win, target_win);
		}
		else if (window_name != NULL)
		{
			if(verbose) fprintf(stdout, "Using window name for selection.\n");
			target_win = Get_Window_by_Name(dpy, DefaultRootWindow(dpy), window_name);
			if(target_win) target_win = Get_Top_Window(dpy, target_win);
			if(verbose) fprintf(stdout, "Window id : %lu(0x%lx)\n", target_win, target_win);
		}
		if ((opacity == OPAQUE)&&(target_win != 0))
			XDeleteProperty (dpy, target_win, XInternAtom(dpy, OPACITY, False));
		else
			if (target_win != 0) XChangeProperty(dpy, target_win, XInternAtom(dpy, OPACITY, False),
				XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &opacity, 1L);
    	}
	if (all)
	{
	  Get_All_Windows(dpy);
	  while(!STACKempty())
	  {
		 if (opacity == OPAQUE)
			 XDeleteProperty (dpy, STACKpop(), XInternAtom(dpy, OPACITY, False));
		 else
			 XChangeProperty(dpy, STACKpop(), XInternAtom(dpy, OPACITY, False),
				 XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &opacity, 1L);
	  }
	}
	XSync(dpy, False);
	if(verbose) fprintf(stdout, "Set Property to %g\n", (double) opacity / OPAQUE);
	return EXIT_SUCCESS;
}
