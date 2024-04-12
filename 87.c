}
static void readpng2_row_callback(png_structp png_ptr, png_bytep new_row,
                                  png_uint_32 row_num, int pass)
{
    mainprog_info  *mainprog_ptr;
    /* first check whether the row differs from the previous pass; if not,
     * nothing to combine or display */
    if (!new_row)
        return;
    /* retrieve the pointer to our special-purpose struct so we can access
     * the old rows and image-display callback function */
    mainprog_ptr = png_get_progressive_ptr(png_ptr);
    /* save the pass number for optional use by the front end */
    mainprog_ptr->pass = pass;
    /* have libpng either combine the new row data with the existing row data
     * from previous passes (if interlaced) or else just copy the new row
     * into the main program's image buffer */
    png_progressive_combine_row(png_ptr, mainprog_ptr->row_pointers[row_num],
      new_row);
    /* finally, call the display routine in the main program with the number
     * of the row we just updated */
    (*mainprog_ptr->mainprog_display_row)(row_num);
    /* and we're ready for more */
    return;
}
