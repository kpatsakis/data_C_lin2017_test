void PNGCBAPI
png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check;
   if (png_ptr == NULL)
      return;
   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = fread(data, 1, length, png_voidcast(png_FILE_p, png_ptr->io_ptr));
   if (check != length)
      png_error(png_ptr, "Read Error");
}
