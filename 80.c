/* returns 0 for success, 2 for libpng (longjmp) problem */
int writepng_encode_image(mainprog_info *mainprog_ptr)
{
    png_structp png_ptr = (png_structp)mainprog_ptr->png_ptr;
    png_infop info_ptr = (png_infop)mainprog_ptr->info_ptr;
    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */
    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        mainprog_ptr->png_ptr = NULL;
        mainprog_ptr->info_ptr = NULL;
        return 2;
    }
    /* and now we just write the whole image; libpng takes care of interlacing
     * for us */
    png_write_image(png_ptr, mainprog_ptr->row_pointers);
    /* since that's it, we also close out the end of the PNG file now--if we
     * had any text or time info to write after the IDATs, second argument
     * would be info_ptr, but we optimize slightly by sending NULL pointer: */
    png_write_end(png_ptr, NULL);
    return 0;
}
