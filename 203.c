#endif /* FEATURE_LOOP */
static void rpng2_x_cleanup(void)
{
    if (bg_image && bg_data) {
        free(bg_data);
        bg_data = NULL;
    }
    if (rpng2_info.image_data) {
        free(rpng2_info.image_data);
        rpng2_info.image_data = NULL;
    }
    if (rpng2_info.row_pointers) {
        free(rpng2_info.row_pointers);
        rpng2_info.row_pointers = NULL;
    }
    if (ximage) {
        if (ximage->data) {
            free(ximage->data);           /* we allocated it, so we free it */
            ximage->data = (char *)NULL;  /*  instead of XDestroyImage() */
        }
        XDestroyImage(ximage);
        ximage = NULL;
    }
    if (have_gc)
        XFreeGC(display, gc);
    if (have_window)
        XDestroyWindow(display, window);
    if (have_colormap)
        XFreeColormap(display, colormap);
    if (have_nondefault_visual)
        XFree(visual_list);
}
