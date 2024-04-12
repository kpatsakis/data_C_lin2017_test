} /* end function rpng_win_create_window() */
static int rpng_win_display_image()
{
    uch *src, *dest;
    uch r, g, b, a;
    ulg i, row, lastrow;
    RECT rect;
    Trace((stderr, "beginning display loop (image_channels == %d)\n",
      image_channels))
    Trace((stderr, "(width = %ld, rowbytes = %ld, wimage_rowbytes = %d)\n",
      image_width, image_rowbytes, wimage_rowbytes))
/*---------------------------------------------------------------------------
    Blast image data to buffer.  This whole routine takes place before the
    message loop begins, so there's no real point in any pseudo-progressive
    display...
  ---------------------------------------------------------------------------*/
    for (lastrow = row = 0;  row < image_height;  ++row) {
        src = image_data + row*image_rowbytes;
        dest = wimage_data + row*wimage_rowbytes;
        if (image_channels == 3) {
            for (i = image_width;  i > 0;  --i) {
                r = *src++;
                g = *src++;
                b = *src++;
                *dest++ = b;
                *dest++ = g;   /* note reverse order */
                *dest++ = r;
            }
        } else /* if (image_channels == 4) */ {
            for (i = image_width;  i > 0;  --i) {
                r = *src++;
                g = *src++;
                b = *src++;
                a = *src++;
                if (a == 255) {
                    *dest++ = b;
                    *dest++ = g;
                    *dest++ = r;
                } else if (a == 0) {
                    *dest++ = bg_blue;
                    *dest++ = bg_green;
                    *dest++ = bg_red;
                } else {
                    /* this macro (copied from png.h) composites the
                     * foreground and background values and puts the
                     * result into the first argument; there are no
                     * side effects with the first argument */
                    alpha_composite(*dest++, b, a, bg_blue);
                    alpha_composite(*dest++, g, a, bg_green);
                    alpha_composite(*dest++, r, a, bg_red);
                }
            }
        }
        /* display after every 16 lines */
        if (((row+1) & 0xf) == 0) {
            rect.left = 0L;
            rect.top = (LONG)lastrow;
            rect.right = (LONG)image_width;      /* possibly off by one? */
            rect.bottom = (LONG)lastrow + 16L;   /* possibly off by one? */
            InvalidateRect(global_hwnd, &rect, FALSE);
            UpdateWindow(global_hwnd);     /* similar to XFlush() */
            lastrow = row + 1;
        }
    }
    Trace((stderr, "calling final image-flush routine\n"))
    if (lastrow < image_height) {
        rect.left = 0L;
        rect.top = (LONG)lastrow;
        rect.right = (LONG)image_width;      /* possibly off by one? */
        rect.bottom = (LONG)image_height;    /* possibly off by one? */
        InvalidateRect(global_hwnd, &rect, FALSE);
        UpdateWindow(global_hwnd);     /* similar to XFlush() */
    }
/*
    last param determines whether or not background is wiped before paint
    InvalidateRect(global_hwnd, NULL, TRUE);
    UpdateWindow(global_hwnd);
 */
    return 0;
}
