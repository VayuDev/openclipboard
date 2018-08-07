#include <stdio.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <X11/Xlib.h>
#include <string.h>
#include <stdlib.h>

char *getUtf8Property(Display *dpy, Window w, Atom p)
{
    Atom da, incr, type;
    int di;
    unsigned long size, dul;
    unsigned char *prop_ret = NULL;

    XGetWindowProperty(dpy, w, p, 0, 0, False, AnyPropertyType,
                       &type, &di, &dul, &size, &prop_ret);
    XFree(prop_ret);

    incr = XInternAtom(dpy, "INCR", False);
    if (type == incr)
    {
        printf("Data too large and INCR mechanism not implemented\n");
        return NULL;
    }

    XGetWindowProperty(dpy, w, p, 0, size, False, AnyPropertyType,
                       &da, &di, &dul, &dul, &prop_ret);
    
    

    XDeleteProperty(dpy, w, p);

    return prop_ret;
}

Display *dpy;
Window target_window, root;
int screen;
Atom sel, target_property, utf8;
XEvent ev;
XSelectionEvent *sev;

char* getClipboardContent()
{
    Window owner = XGetSelectionOwner(dpy, sel);
    if (owner == None)
    {
        printf("'CLIPBOARD' has no owner\n");
        return NULL;
    }

    /* The selection owner will store the data in a property on this
     * window: */
    
    XSelectInput(dpy, target_window, SelectionNotify);
    

    /* Request conversion to UTF-8. Not all owners will be able to
     * fulfill that request. */
    XConvertSelection(dpy, sel, utf8, target_property, target_window,
                      CurrentTime);

    while(1)
    {
        XNextEvent(dpy, &ev);
        switch (ev.type)
        {
            case SelectionNotify:
                sev = (XSelectionEvent*)&ev.xselection;
                if (sev->property == None)
                {
                    
                }
                else
                {
                    return getUtf8Property(dpy, target_window, target_property);
                    
                }
                break;
        }
    }
}

int main()
{
    dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        fprintf(stderr, "Could not open X display\n");
        return 1;
    }

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    sel = XInternAtom(dpy, "CLIPBOARD", False);
    utf8 = XInternAtom(dpy, "UTF8_STRING", False);

    target_window = XCreateSimpleWindow(dpy, root, -10, -10, 1, 1, 0, 0, 0);

    target_property = XInternAtom(dpy, "MY_PROPERTY", False);

    char *last = NULL;
    while(1)
    {
        char *data = getClipboardContent();
        if(data == NULL)
        {
            usleep(500000);
            continue;
        }
        else if(last == NULL)
        {
            last = data;
            printf("%s", data);
            fflush(stdout);
            continue;
        }

        if(strcmp(last, data) != 0)
        {
            printf("%s", data);
            fflush(stdout);
        }
        XFree((void*) last);
        last = data;
        usleep(250000);
    }
}
