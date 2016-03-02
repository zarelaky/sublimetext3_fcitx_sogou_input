/*
sublime-imfix.c
Use LD_PRELOAD to interpose some function to fix sublime input method support for linux.
By Cjacker Huang <jianzhong.huang at i-soft.com.cn>

gcc -shared -o libsublime-imfix.so sublime_imfix.c  `pkg-config --libs --cflags gtk+-2.0` -fPIC
LD_PRELOAD=./libsublime-imfix.so sublime_text 

2016-03-02 By zarelaky <zarelaky@hotmail.com>
fixed can not Browse packages
fixed open terminal failed 
*/

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

typedef GdkSegment GdkRegionBox;

struct _GdkRegion
{
    long size;
    long numRects;
    GdkRegionBox *rects;
    GdkRegionBox extents;
};

#ifdef VERBOSE
#define DEBUG(fmt, ...) do { \
    FILE* err = fopen("/tmp/sublime-imfix.log", "a"); \
    if (err) { \
      fprintf(err, fmt, __VA_ARGS__); \
      fclose(err); \
    } \
  } while(0)
#else
#define DEBUG(fmt, ...)
#endif


GtkIMContext *local_context;

static int pre_load_cleaned = 0;

static void clean_env() {
    if (!pre_load_cleaned) {
        putenv("LD_PRELOAD=");
        pre_load_cleaned = 1;
        DEBUG("clean env = %d\n",getpid());
    }
}


void
gdk_region_get_clipbox (const GdkRegion *region,
                        GdkRectangle    *rectangle)
{
    g_return_if_fail (region != NULL);
    g_return_if_fail (rectangle != NULL);

    clean_env();

    rectangle->x = region->extents.x1;
    rectangle->y = region->extents.y1;
    rectangle->width = region->extents.x2 - region->extents.x1;
    rectangle->height = region->extents.y2 - region->extents.y1;
    GdkRectangle rect;
    rect.x = rectangle->x;
    rect.y = rectangle->y;
    rect.width = 0;
    rect.height = rectangle->height;
    //The caret width is 2;
    //Maybe sometimes we will make a mistake, but for most of the time, it should be the caret.
    if(rectangle->width == 2 && GTK_IS_IM_CONTEXT(local_context)) {
        gtk_im_context_set_cursor_location(local_context, rectangle);
    }
}

//this is needed, for example, if you input something in file dialog and return back the edit area
//context will lost, so here we set it again.

static GdkFilterReturn event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer im_context)
{
    XEvent *xev = (XEvent *)xevent;
    if(xev->type == KeyRelease && GTK_IS_IM_CONTEXT(im_context)) {
        GdkWindow * win = g_object_get_data(G_OBJECT(im_context),"window");
        if(GDK_IS_WINDOW(win))
            gtk_im_context_set_client_window(im_context, win);
    }
    return GDK_FILTER_CONTINUE;
}

void gtk_im_context_set_client_window (GtkIMContext *context,
                                       GdkWindow    *window)
{
    GtkIMContextClass *klass;
    g_return_if_fail (GTK_IS_IM_CONTEXT (context));

    clean_env();

    klass = GTK_IM_CONTEXT_GET_CLASS (context);
    if (klass->set_client_window)
        klass->set_client_window (context, window);

    if(!GDK_IS_WINDOW (window))
        return;

    g_object_set_data(G_OBJECT(context),"window",window);
    int width = gdk_window_get_width(window);
    int height = gdk_window_get_height(window);
    if(width != 0 && height !=0) {
        gtk_im_context_focus_in(context);
        local_context = context;
    }
    gdk_window_add_filter (window, event_filter, context);
}
