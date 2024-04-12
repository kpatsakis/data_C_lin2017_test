} /* end function rpng2_x_load_bg_image() */
static void rpng2_x_display_row(ulg row)
{
    uch bg_red   = rpng2_info.bg_red;
    uch bg_green = rpng2_info.bg_green;
    uch bg_blue  = rpng2_info.bg_blue;
    uch *src, *src2=NULL;
    char *dest;
    uch r, g, b, a;
    int ximage_rowbytes = ximage->bytes_per_line;
    ulg i, pixel;
    static int rows=0, prevpass=(-1);
    static ulg firstrow;
/*---------------------------------------------------------------------------
    rows and firstrow simply track how many rows (and which ones) have not
    yet been displayed; alternatively, we could call XPutImage() for every
    row and not bother with the records-keeping.
  ---------------------------------------------------------------------------*/
    Trace((stderr, "beginning rpng2_x_display_row()\n"))
    if (rpng2_info.pass != prevpass) {
        if (pause_after_pass && rpng2_info.pass > 0) {
            XEvent e;
            KeySym k;
            fprintf(stderr,
              "%s:  end of pass %d of 7; click in image window to continue\n",
              PROGNAME, prevpass + 1);
            do
                XNextEvent(display, &e);
            while (!QUIT(e,k));
        }
        fprintf(stderr, "%s:  pass %d of 7\r", PROGNAME, rpng2_info.pass + 1);
        fflush(stderr);
        prevpass = rpng2_info.pass;
    }
    if (rows == 0)
        firstrow = row;   /* first row that is not yet displayed */
    ++rows;   /* count of rows received but not yet displayed */
/*---------------------------------------------------------------------------
    Aside from the use of the rpng2_info struct, the lack of an outer loop
    (over rows) and moving the XPutImage() call outside the "if (depth)"
    tests, this routine is identical to rpng_x_display_image() in the non-
    progressive version of the program.
  ---------------------------------------------------------------------------*/
    if (depth == 24 || depth == 32) {
        ulg red, green, blue;
        int bpp = ximage->bits_per_pixel;
        src = rpng2_info.image_data + row*rpng2_info.rowbytes;
        if (bg_image)
            src2 = bg_data + row*bg_rowbytes;
        dest = ximage->data + row*ximage_rowbytes;
        if (rpng2_info.channels == 3) {
            for (i = rpng2_info.width;  i > 0;  --i) {
                red   = *src++;
                green = *src++;
                blue  = *src++;
                pixel = (red   << RShift) |
                        (green << GShift) |
                        (blue  << BShift);
                /* recall that we set ximage->byte_order = MSBFirst above */
                if (bpp == 32) {
                    *dest++ = (char)((pixel >> 24) & 0xff);
                    *dest++ = (char)((pixel >> 16) & 0xff);
                    *dest++ = (char)((pixel >>  8) & 0xff);
                    *dest++ = (char)( pixel        & 0xff);
                } else {
                    /* GRR BUG?  this assumes bpp == 24 & bits are packed low */
                    /*           (probably need to use RShift, RMask, etc.) */
                    *dest++ = (char)((pixel >> 16) & 0xff);
                    *dest++ = (char)((pixel >>  8) & 0xff);
                    *dest++ = (char)( pixel        & 0xff);
                }
            }
        } else /* if (rpng2_info.channels == 4) */ {
            for (i = rpng2_info.width;  i > 0;  --i) {
                r = *src++;
                g = *src++;
                b = *src++;
                a = *src++;
                if (bg_image) {
                    bg_red   = *src2++;
                    bg_green = *src2++;
                    bg_blue  = *src2++;
                }
                if (a == 255) {
                    red   = r;
                    green = g;
                    blue  = b;
                } else if (a == 0) {
                    red   = bg_red;
                    green = bg_green;
                    blue  = bg_blue;
                } else {
                    /* this macro (from png.h) composites the foreground
                     * and background values and puts the result into the
                     * first argument */
                    alpha_composite(red,   r, a, bg_red);
                    alpha_composite(green, g, a, bg_green);
                    alpha_composite(blue,  b, a, bg_blue);
                }
                pixel = (red   << RShift) |
                        (green << GShift) |
                        (blue  << BShift);
                /* recall that we set ximage->byte_order = MSBFirst above */
                if (bpp == 32) {
                    *dest++ = (char)((pixel >> 24) & 0xff);
                    *dest++ = (char)((pixel >> 16) & 0xff);
                    *dest++ = (char)((pixel >>  8) & 0xff);
                    *dest++ = (char)( pixel        & 0xff);
                } else {
                    /* GRR BUG?  this assumes bpp == 24 & bits are packed low */
                    /*           (probably need to use RShift, RMask, etc.) */
                    *dest++ = (char)((pixel >> 16) & 0xff);
                    *dest++ = (char)((pixel >>  8) & 0xff);
                    *dest++ = (char)( pixel        & 0xff);
                }
            }
        }
    } else if (depth == 16) {
        ush red, green, blue;
        src = rpng2_info.row_pointers[row];
        if (bg_image)
            src2 = bg_data + row*bg_rowbytes;
        dest = ximage->data + row*ximage_rowbytes;
        if (rpng2_info.channels == 3) {
            for (i = rpng2_info.width;  i > 0;  --i) {
                red   = ((ush)(*src) << 8);
                ++src;
                green = ((ush)(*src) << 8);
                ++src;
                blue  = ((ush)(*src) << 8);
                ++src;
                pixel = ((red   >> RShift) & RMask) |
                        ((green >> GShift) & GMask) |
                        ((blue  >> BShift) & BMask);
                /* recall that we set ximage->byte_order = MSBFirst above */
                *dest++ = (char)((pixel >>  8) & 0xff);
                *dest++ = (char)( pixel        & 0xff);
            }
        } else /* if (rpng2_info.channels == 4) */ {
            for (i = rpng2_info.width;  i > 0;  --i) {
                r = *src++;
                g = *src++;
                b = *src++;
                a = *src++;
                if (bg_image) {
                    bg_red   = *src2++;
                    bg_green = *src2++;
                    bg_blue  = *src2++;
                }
                if (a == 255) {
                    red   = ((ush)r << 8);
                    green = ((ush)g << 8);
                    blue  = ((ush)b << 8);
                } else if (a == 0) {
                    red   = ((ush)bg_red   << 8);
                    green = ((ush)bg_green << 8);
                    blue  = ((ush)bg_blue  << 8);
                } else {
                    /* this macro (from png.h) composites the foreground
                     * and background values and puts the result back into
                     * the first argument (== fg byte here:  safe) */
                    alpha_composite(r, r, a, bg_red);
                    alpha_composite(g, g, a, bg_green);
                    alpha_composite(b, b, a, bg_blue);
                    red   = ((ush)r << 8);
                    green = ((ush)g << 8);
                    blue  = ((ush)b << 8);
                }
                pixel = ((red   >> RShift) & RMask) |
                        ((green >> GShift) & GMask) |
                        ((blue  >> BShift) & BMask);
                /* recall that we set ximage->byte_order = MSBFirst above */
                *dest++ = (char)((pixel >>  8) & 0xff);
                *dest++ = (char)( pixel        & 0xff);
            }
        }
    } else /* depth == 8 */ {
        /* GRR:  add 8-bit support */
    }
/*---------------------------------------------------------------------------
    Display after every 16 rows or when on one of last two rows.  (Region
    may include previously displayed lines due to interlacing--i.e., not
    contiguous.  Also, second-to-last row is final one in interlaced images
    with odd number of rows.)  For demos, flush (and delay) after every 16th
    row so "sparse" passes don't go twice as fast.
  ---------------------------------------------------------------------------*/
    if (demo_timing && (row - firstrow >= 16 || row >= rpng2_info.height-2)) {
        XPutImage(display, window, gc, ximage, 0, (int)firstrow, 0,
          (int)firstrow, rpng2_info.width, row - firstrow + 1);
        XFlush(display);
        rows = 0;
        usleep(usleep_duration);
    } else
    if (!demo_timing && ((rows & 0xf) == 0 || row >= rpng2_info.height-2)) {
        XPutImage(display, window, gc, ximage, 0, (int)firstrow, 0,
          (int)firstrow, rpng2_info.width, row - firstrow + 1);
        XFlush(display);
        rows = 0;
    }
}
