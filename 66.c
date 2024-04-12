}
static int rpng2_x_create_window(void)
{
    ulg bg_red   = rpng2_info.bg_red;
    ulg bg_green = rpng2_info.bg_green;
    ulg bg_blue  = rpng2_info.bg_blue;
    ulg bg_pixel = 0L;
    ulg attrmask;
    int need_colormap = FALSE;
    int screen, pad;
    uch *xdata;
    Window root;
    XEvent e;
    XGCValues gcvalues;
    XSetWindowAttributes attr;
    XTextProperty windowName, *pWindowName = &windowName;
    XTextProperty iconName, *pIconName = &iconName;
    XVisualInfo visual_info;
    XSizeHints *size_hints;
    XWMHints *wm_hints;
    XClassHint *class_hints;
    Trace((stderr, "beginning rpng2_x_create_window()\n"))
    screen = DefaultScreen(display);
    depth = DisplayPlanes(display, screen);
    root = RootWindow(display, screen);
#ifdef DEBUG
    XSynchronize(display, True);
#endif
    if (depth != 16 && depth != 24 && depth != 32) {
        int visuals_matched = 0;
        Trace((stderr, "default depth is %d:  checking other visuals\n",
          depth))
        /* 24-bit first */
        visual_info.screen = screen;
        visual_info.depth = 24;
        visual_list = XGetVisualInfo(display,
          VisualScreenMask | VisualDepthMask, &visual_info, &visuals_matched);
        if (visuals_matched == 0) {
/* GRR:  add 15-, 16- and 32-bit TrueColor visuals (also DirectColor?) */
            fprintf(stderr, "default screen depth %d not supported, and no"
              " 24-bit visuals found\n", depth);
            return 2;
        }
        Trace((stderr, "XGetVisualInfo() returned %d 24-bit visuals\n",
          visuals_matched))
        visual = visual_list[0].visual;
        depth = visual_list[0].depth;
/*
        colormap_size = visual_list[0].colormap_size;
        visual_class = visual->class;
        visualID = XVisualIDFromVisual(visual);
 */
        have_nondefault_visual = TRUE;
        need_colormap = TRUE;
    } else {
        XMatchVisualInfo(display, screen, depth, TrueColor, &visual_info);
        visual = visual_info.visual;
    }
    RMask = visual->red_mask;
    GMask = visual->green_mask;
    BMask = visual->blue_mask;
/* GRR:  add/check 8-bit support */
    if (depth == 8 || need_colormap) {
        colormap = XCreateColormap(display, root, visual, AllocNone);
        if (!colormap) {
            fprintf(stderr, "XCreateColormap() failed\n");
            return 2;
        }
        have_colormap = TRUE;
        if (depth == 8)
            bg_image = FALSE;   /* gradient just wastes palette entries */
    }
    if (depth == 15 || depth == 16) {
        RShift = 15 - rpng2_x_msb(RMask);    /* these are right-shifts */
        GShift = 15 - rpng2_x_msb(GMask);
        BShift = 15 - rpng2_x_msb(BMask);
    } else if (depth > 16) {
        RShift = rpng2_x_msb(RMask) - 7;     /* these are left-shifts */
        GShift = rpng2_x_msb(GMask) - 7;
        BShift = rpng2_x_msb(BMask) - 7;
    }
    if (depth >= 15 && (RShift < 0 || GShift < 0 || BShift < 0)) {
        fprintf(stderr, "rpng2 internal logic error:  negative X shift(s)!\n");
        return 2;
    }
/*---------------------------------------------------------------------------
    Finally, create the window.
  ---------------------------------------------------------------------------*/
    attr.backing_store = Always;
    attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
    attrmask = CWBackingStore | CWEventMask;
    if (have_nondefault_visual) {
        attr.colormap = colormap;
        attr.background_pixel = 0;
        attr.border_pixel = 1;
        attrmask |= CWColormap | CWBackPixel | CWBorderPixel;
    }
    window = XCreateWindow(display, root, 0, 0, rpng2_info.width,
      rpng2_info.height, 0, depth, InputOutput, visual, attrmask, &attr);
    if (window == None) {
        fprintf(stderr, "XCreateWindow() failed\n");
        return 2;
    } else
        have_window = TRUE;
    if (depth == 8)
        XSetWindowColormap(display, window, colormap);
    if (!XStringListToTextProperty(&window_name, 1, pWindowName))
        pWindowName = NULL;
    if (!XStringListToTextProperty(&icon_name, 1, pIconName))
        pIconName = NULL;
    /* OK if either hints allocation fails; XSetWMProperties() allows NULLs */
    if ((size_hints = XAllocSizeHints()) != NULL) {
        /* window will not be resizable */
        size_hints->flags = PMinSize | PMaxSize;
        size_hints->min_width = size_hints->max_width = (int)rpng2_info.width;
        size_hints->min_height = size_hints->max_height =
          (int)rpng2_info.height;
    }
    if ((wm_hints = XAllocWMHints()) != NULL) {
        wm_hints->initial_state = NormalState;
        wm_hints->input = True;
     /* wm_hints->icon_pixmap = icon_pixmap; */
        wm_hints->flags = StateHint | InputHint  /* | IconPixmapHint */ ;
    }
    if ((class_hints = XAllocClassHint()) != NULL) {
        class_hints->res_name = res_name;
        class_hints->res_class = res_class;
    }
    XSetWMProperties(display, window, pWindowName, pIconName, NULL, 0,
      size_hints, wm_hints, class_hints);
    /* various properties and hints no longer needed; free memory */
    if (pWindowName)
       XFree(pWindowName->value);
    if (pIconName)
       XFree(pIconName->value);
    if (size_hints)
        XFree(size_hints);
    if (wm_hints)
       XFree(wm_hints);
    if (class_hints)
       XFree(class_hints);
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, &gcvalues);
    have_gc = TRUE;
/*---------------------------------------------------------------------------
    Allocate memory for the X- and display-specific version of the image.
  ---------------------------------------------------------------------------*/
    if (depth == 24 || depth == 32) {
        xdata = (uch *)malloc(4*rpng2_info.width*rpng2_info.height);
        pad = 32;
    } else if (depth == 16) {
        xdata = (uch *)malloc(2*rpng2_info.width*rpng2_info.height);
        pad = 16;
    } else /* depth == 8 */ {
        xdata = (uch *)malloc(rpng2_info.width*rpng2_info.height);
        pad = 8;
    }
    if (!xdata) {
        fprintf(stderr, PROGNAME ":  unable to allocate image memory\n");
        return 4;
    }
    ximage = XCreateImage(display, visual, depth, ZPixmap, 0,
      (char *)xdata, rpng2_info.width, rpng2_info.height, pad, 0);
    if (!ximage) {
        fprintf(stderr, PROGNAME ":  XCreateImage() failed\n");
        free(xdata);
        return 3;
    }
    /* to avoid testing the byte order every pixel (or doubling the size of
     * the drawing routine with a giant if-test), we arbitrarily set the byte
     * order to MSBFirst and let Xlib worry about inverting things on little-
     * endian machines (e.g., Linux/x86, old VAXen, etc.)--this is not the
     * most efficient approach (the giant if-test would be better), but in
     * the interest of clarity, we'll take the easy way out... */
    ximage->byte_order = MSBFirst;
/*---------------------------------------------------------------------------
    Fill window with the specified background color (default is black) or
    faked "background image" (but latter is disabled if 8-bit; gradients
    just waste palette entries).
  ---------------------------------------------------------------------------*/
    if (bg_image)
        rpng2_x_load_bg_image();    /* resets bg_image if fails */
    if (!bg_image) {
        if (depth == 24 || depth == 32) {
            bg_pixel = (bg_red   << RShift) |
                       (bg_green << GShift) |
                       (bg_blue  << BShift);
        } else if (depth == 16) {
            bg_pixel = (((bg_red   << 8) >> RShift) & RMask) |
                       (((bg_green << 8) >> GShift) & GMask) |
                       (((bg_blue  << 8) >> BShift) & BMask);
        } else /* depth == 8 */ {
            /* GRR:  add 8-bit support */
        }
        XSetForeground(display, gc, bg_pixel);
        XFillRectangle(display, window, gc, 0, 0, rpng2_info.width,
          rpng2_info.height);
    }
/*---------------------------------------------------------------------------
    Wait for first Expose event to do any drawing, then flush and return.
  ---------------------------------------------------------------------------*/
    do
        XNextEvent(display, &e);
    while (e.type != Expose || e.xexpose.count);
    XFlush(display);
    return 0;
} /* end function rpng2_x_create_window() */
