} /* end function rpng2_win_display_row() */
static void rpng2_win_finish_display()
{
    Trace((stderr, "beginning rpng2_win_finish_display()\n"))
    /* last row has already been displayed by rpng2_win_display_row(), so
     * we have nothing to do here except set a flag and let the user know
     * that the image is done */
    rpng2_info.state = kDone;
    printf(
#ifndef __CYGWIN__
      "Done.  Press Q, Esc or mouse button 1 (within image window) to quit.\n"
#else
      "Done.  Press mouse button 1 (within image window) to quit.\n"
#endif
    );
    fflush(stdout);
}
