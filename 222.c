 * read and processed--i.e., we now have enough info to finish initializing */
static void rpng2_x_init(void)
{
    ulg i;
    ulg rowbytes = rpng2_info.rowbytes;
    Trace((stderr, "beginning rpng2_x_init()\n"))
    Trace((stderr, "  rowbytes = %d\n", rpng2_info.rowbytes))
    Trace((stderr, "  width  = %ld\n", rpng2_info.width))
    Trace((stderr, "  height = %ld\n", rpng2_info.height))
    rpng2_info.image_data = (uch *)malloc(rowbytes * rpng2_info.height);
    if (!rpng2_info.image_data) {
        readpng2_cleanup(&rpng2_info);
        return;
    }
    rpng2_info.row_pointers = (uch **)malloc(rpng2_info.height * sizeof(uch *));
    if (!rpng2_info.row_pointers) {
        free(rpng2_info.image_data);
        rpng2_info.image_data = NULL;
        readpng2_cleanup(&rpng2_info);
        return;
    }
    for (i = 0;  i < rpng2_info.height;  ++i)
        rpng2_info.row_pointers[i] = rpng2_info.image_data + i*rowbytes;
    /* do the basic X initialization stuff, make the window, and fill it with
     * the user-specified, file-specified or default background color or
     * pattern */
    if (rpng2_x_create_window()) {
        /* GRR TEMPORARY HACK:  this is fundamentally no different from cases
         * above; libpng should call our error handler to longjmp() back to us
         * when png_ptr goes away.  If we/it segfault instead, seems like a
         * libpng bug... */
        /* we're here via libpng callback, so if window fails, clean and bail */
        readpng2_cleanup(&rpng2_info);
        rpng2_x_cleanup();
        exit(2);
    }
    rpng2_info.state = kWindowInit;
}
