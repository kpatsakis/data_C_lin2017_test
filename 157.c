#endif
static int read_png(FILE *fp)
{
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
   png_infop info_ptr = NULL;
   png_bytep row = NULL, display = NULL;
   if (png_ptr == NULL)
      return 0;
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      if (row != NULL) free(row);
      if (display != NULL) free(display);
      return 0;
   }
   png_init_io(png_ptr, fp);
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
      png_error(png_ptr, "OOM allocating info structure");
   png_read_info(png_ptr, info_ptr);
   {
      png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
      row = malloc(rowbytes);
      display = malloc(rowbytes);
      if (row == NULL || display == NULL)
         png_error(png_ptr, "OOM allocating row buffers");
      {
         png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
         int passes = png_set_interlace_handling(png_ptr);
         int pass;
         png_start_read_image(png_ptr);
         for (pass = 0; pass < passes; ++pass)
         {
            png_uint_32 y = height;
            /* NOTE: this trashes the row each time; interlace handling won't
             * work, but this avoids memory thrashing for speed testing.
             */
            while (y-- > 0)
               png_read_row(png_ptr, row, display);
         }
      }
   }
   /* Make sure to read to the end of the file: */
   png_read_end(png_ptr, info_ptr);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   free(row);
   free(display);
   return 1;
}
