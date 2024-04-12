} /* end function rpng2_win_load_bg_image() */
static void rpng2_win_display_row(ulg row)
{
    uch bg_red   = rpng2_info.bg_red;
    uch bg_green = rpng2_info.bg_green;
    uch bg_blue  = rpng2_info.bg_blue;
    uch *src, *src2=NULL, *dest;
    uch r, g, b, a;
    ulg i;
    static int rows=0;
    static ulg firstrow;
/*---------------------------------------------------------------------------
    rows and firstrow simply track how many rows (and which ones) have not
    yet been displayed; alternatively, we could call InvalidateRect() for
    every row and not bother with the records-keeping.
  ---------------------------------------------------------------------------*/
    Trace((stderr, "beginning rpng2_win_display_row()\n"))
    if (rows == 0)
        firstrow = row;   /* first row not yet displayed */
    ++rows;   /* count of rows received but not yet displayed */
/*---------------------------------------------------------------------------
    Aside from the use of the rpng2_info struct and the lack of an outer
    loop (over rows), this routine is identical to rpng_win_display_image()
    in the non-progressive version of the program.
  ---------------------------------------------------------------------------*/
    src = rpng2_info.image_data + row*rpng2_info.rowbytes;
    if (bg_image)
        src2 = bg_data + row*bg_rowbytes;
    dest = wimage_data + row*wimage_rowbytes;
    if (rpng2_info.channels == 3) {
        for (i = rpng2_info.width;  i > 0;  --i) {
            r = *src++;
            g = *src++;
            b = *src++;
            *dest++ = b;
            *dest++ = g;   /* note reverse order */
            *dest++ = r;
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
/*---------------------------------------------------------------------------
    Display after every 16 rows or when on last row.  (Region may include
    previously displayed lines due to interlacing--i.e., not contiguous.)
  ---------------------------------------------------------------------------*/
    if ((rows & 0xf) == 0 || row == rpng2_info.height-1) {
        RECT rect;
        rect.left = 0L;
        rect.top = (LONG)firstrow;
        rect.right = (LONG)rpng2_info.width;       /* possibly off by one? */
        rect.bottom = (LONG)row + 1L;              /* possibly off by one? */
        InvalidateRect(global_hwnd, &rect, FALSE);
        UpdateWindow(global_hwnd);                 /* similar to XFlush() */
        rows = 0;
    }
} /* end function rpng2_win_display_row() */
