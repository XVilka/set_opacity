/***************************************************************************
                          interface.h  -  description
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

#define ENV_SIZE 8192
#define OPAQUE	0xffffffff
#define OPACITY "_NET_WM_WINDOW_OPACITY"

void SetDebug(int debug);
Display *Open_Display(char *display_name);
Window Get_Top_Window(Display *dpy, Window child);
Window Select_Window_by_Mouse(Display *dpy);
Window Get_Window_by_Name(Display *dpy, Window top, char *name);
Window Get_Window_by_PID(Display *dpy, pid_t pid);
void Get_All_Windows(Display *dpy);
pid_t WID2PID (Display *dpy, Window w);
