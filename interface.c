/***************************************************************************
                          interfaces.c  -  description
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

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "interface.h"
#include "stack.h"

unsigned window_debug = 0;
unsigned wid2pid_debug = 0;

void SetDebug(int debug)
{
	window_debug = debug;
	wid2pid_debug = debug;
}

Display *Open_Display(char *display_name)
{
	Display *dpy;

	dpy = XOpenDisplay(display_name);
	if (dpy == NULL)
	{
	    fprintf (stderr, "Unable to open display '%s'\n", XDisplayName (display_name));
	}
	return(dpy);
}

Window Get_Top_Window(Display *dpy, Window child)
{
	Window root, parent;
	Window *child_list;
	unsigned int num_children;

	if (!XQueryTree(dpy, child, &root, &parent, &child_list, &num_children))
  			fprintf(stderr, "Can't query window tree.\n");
  	if(parent == root) return child;
	while(parent != root)
	{
		child = parent;
		if (!XQueryTree(dpy, child, &root, &parent, &child_list, &num_children))
			fprintf(stderr, "Can't query window tree.\n");
	}
	return child;
}

Window Select_Window_by_Mouse(Display *dpy)
{
  int status;
  Cursor cursor;
  XEvent event;
  Window target_win = 0, root;
  int buttons = 0;
  unsigned screen = 0;

  root = RootWindow(dpy, screen);
  cursor = XCreateFontCursor(dpy, XC_crosshair);
  status = XGrabPointer(dpy, root, False, ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			GrabModeAsync, root, cursor, CurrentTime);
  if (status != GrabSuccess)
  {
  		fprintf(stderr, "Can't grab the mouse!\n");
  		exit(1);
  }
  while ((target_win == 0) || (buttons != 0))
  {
    XAllowEvents(dpy, SyncPointer, CurrentTime);
    XWindowEvent(dpy, root, ButtonPressMask|ButtonReleaseMask, &event);
    switch (event.type)
    {
    	case ButtonPress:
      	if (target_win == 0)
      	{
      		target_win = event.xbutton.subwindow;
      		if (target_win == 0) target_win = root;
      	}
      	buttons++;
      	break;
    	case ButtonRelease:
      	if (buttons > 0)buttons--;
      	break;
    }
  }
  XUngrabPointer(dpy, CurrentTime);
  return(target_win);
}

Window Get_Window_by_Name(Display *dpy, Window top, char *name)
{
	Window *children, dummy;
	unsigned int nchildren;
	int i;
	Window w = 0;
	char *window_name;

	if (XFetchName(dpy, top, &window_name) && !strcmp(window_name, name)) return(top);
	if (!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren)) return(0);
	for (i=0; i<nchildren; i++)
	{
		w = Get_Window_by_Name(dpy, children[i], name);
		if (w) break;
	}
	if (children) XFree ((char *)children);
	return(w);
}

Window Get_Window_by_PID(Display *dpy, pid_t pid)
{
        Window w = 0;
        Window root_return;
        Window parent_return;
        Window *children_return, *children_return1, *children_return2;
        unsigned int nchildren_return, nchildren_return1, nchildren_return2;
        unsigned i, j, k;

        if(window_debug) fprintf(stdout, "Check WINDOWID for pid=%d\n", pid);
        Status status = XQueryTree(dpy, DefaultRootWindow(dpy), &root_return,
        					&parent_return, &children_return, &nchildren_return);
        if(children_return)
        {
            if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in root window\n",
            									nchildren_return);
            for (i = 0; i < nchildren_return; i++)
            {
                if (pid == WID2PID(dpy, children_return[i]))
                {
                   	w = children_return[i];
                    	break;
                }
                Status status1 = XQueryTree(dpy, children_return[i], &root_return,
                					&parent_return, &children_return1, &nchildren_return1);
                if(children_return1)
                {
                    if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in window %lu(0x%lx)\n",
                                                            nchildren_return1, children_return[i], children_return[i]);
                    for (j = 0; j < nchildren_return1; j++)
                    {
                        if (pid == WID2PID(dpy, children_return1[j]))
                        {
                        	w = children_return1[j];
                        	break;
                        }
                        Status status2 = XQueryTree(dpy, children_return1[j], &root_return,
                        					&parent_return, &children_return2, &nchildren_return2);
                    		if(children_return2)
                    		{
                    				if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in window %lu(0x%lx)\n",
                                                            nchildren_return2, children_return1[j], children_return1[j]);
                    				 for (k = 0; k < nchildren_return2; k++)
                    				 {
                    				 		if (pid == WID2PID(dpy, children_return2[k]))
                    				 		{
                    				 			w = children_return2[k];
                    				 			break;
                    				 		}
                    				 }
                    		 }
                    		 XFree(children_return2);
                    }
                    XFree(children_return1);
                }
                if(w) break;
            }
        }
        else
        {
        			fprintf(stdout, "No children return!\n");
        }
        XFree(children_return);
        if(!w){
        		if(window_debug) fprintf(stdout,"for pid=%d not found any window\n",pid);
        		return 0;
        }
        else {
            if(window_debug) fprintf(stdout, "for pid=%d found some window %lu(0x%lx).\n"
                                                " Interpret it as main application window\n",pid,w,w);
            return w;
        }
}

void Get_All_Windows(Display *dpy)
{
	Window w = 0;
        Window root_return;
        Window parent_return;
        Window *children_return, *children_return1, *children_return2;
        unsigned int nchildren_return, nchildren_return1, nchildren_return2;
        unsigned i, j, k;

        Status status = XQueryTree(dpy, DefaultRootWindow(dpy), &root_return,
        					&parent_return, &children_return, &nchildren_return);
        if(children_return)
        {
            STACKinit(100);
		if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in root window\n",
            									nchildren_return);
            for (i = 0; i < nchildren_return; i++)
            {
                STACKpush(children_return[i]);
                Status status1 = XQueryTree(dpy, children_return[i], &root_return,
                					&parent_return, &children_return1, &nchildren_return1);
                if(children_return1)
                {
                    if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in window %lu(0x%lx)\n",
                                                            nchildren_return1, children_return[i], children_return[i]);
                    for (j = 0; j < nchildren_return1; j++)
                    {
                        STACKpush(children_return1[j]);
                        Status status2 = XQueryTree(dpy, children_return1[j], &root_return,
                        					&parent_return, &children_return2, &nchildren_return2);
                    		if(children_return2)
                    		{
                    				if(window_debug) fprintf(stdout,"XQueryTree found %d child windows in window %lu(0x%lx)\n",
                                                            nchildren_return2, children_return1[j], children_return1[j]);
                    				 for (k = 0; k < nchildren_return2; k++)
                    				 {
                    				 	STACKpush(children_return2[k]);
                    				 }
                    		 }
                    		 XFree(children_return2);
                    }
                    XFree(children_return1);
                }
                if(w) break;
            }
        }
        else
        {
        			fprintf(stdout, "No children return!\n");
        }
        XFree(children_return);
}

pid_t WID2PID (Display *dpy, Window w)
{
    Atom atom,actual_type;
    char *atom_name;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *prop;
    int status;
    pid_t pid;

    if(wid2pid_debug) fprintf(stdout, "Check PID for wid=%d\n", w);

    atom = XInternAtom(dpy, "_NET_WM_PID", True);
    atom_name = XGetAtomName (dpy,atom);

    status = XGetWindowProperty(
        dpy, w, atom, 0, 1024,
        False, AnyPropertyType,
        &actual_type,
        &actual_format, &nitems,
        &bytes_after,
        &prop
    );

    if (status!=0)
    {
    	if(wid2pid_debug)  fprintf(stderr,"Cannot get _NET_WM_PID for window %lu(0x%lx)", w, w);
    	return -2;
    }
    if (! prop)
    {
    	if(wid2pid_debug)  fprintf(stderr,"window %lu(0x%lx): No properties\n", w, w);
    	return -3;
    }

    pid = prop[1] * 256;
    pid += prop[0];
    if(wid2pid_debug) fprintf(stdout,"pid of window %lu(0x%lx) = %d\n", w, w, pid);

    return pid;
}

