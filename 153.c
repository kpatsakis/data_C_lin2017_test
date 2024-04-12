}
static void rpng2_x_finish_display(void)
{
    Trace((stderr, "beginning rpng2_x_finish_display()\n"))
    /* last row has already been displayed by rpng2_x_display_row(), so we
     * have nothing to do here except set a flag and let the user know that
     * the image is done */
    rpng2_info.state = kDone;
    printf(
      "Done.  Press Q, Esc or mouse button 1 (within image window) to quit.\n");
    fflush(stdout);
}
