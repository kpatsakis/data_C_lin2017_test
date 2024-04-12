//CVE-2011-2692
#ifdef PNG_READ_sCAL_SUPPORTED
/* Read the sCAL chunk */
void /* PRIVATE */
png_handle_sCAL(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_size_t slength, index;
   int state;

   png_debug(1, "in png_handle_sCAL");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sCAL");

   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }

   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sCAL))
   {
      png_warning(png_ptr, "Duplicate sCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_debug1(2, "Allocating and reading sCAL chunk data (%u bytes)",
      length + 1);

   png_ptr->chunkdata = (png_charp)png_malloc_warn(png_ptr, length + 1);

   if (png_ptr->chunkdata == NULL)
   {
      png_warning(png_ptr, "Out of memory while processing sCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)png_ptr->chunkdata, slength);
   png_ptr->chunkdata[slength] = 0x00; /* Null terminate the last string */

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      return;
   }

   /* Validate the unit. */
   if (png_ptr->chunkdata[0] != 1 && png_ptr->chunkdata[0] != 2)
   {
      png_warning(png_ptr, "Invalid sCAL ignored: invalid unit");
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      return;
   }

   /* Validate the ASCII numbers, need two ASCII numbers separated by
    * a '\0' and they need to fit exactly in the chunk data.
    */
   index = 0;
   state = 0;

   if (png_ptr->chunkdata[1] == 45 /* negative width */ ||
       !png_check_fp_number(png_ptr->chunkdata, slength, &state, &index) ||
       index >= slength || png_ptr->chunkdata[index++] != 0)
      png_warning(png_ptr, "Invalid sCAL chunk ignored: bad width format");

   else
   {
      png_size_t heighti = index;

      if (png_ptr->chunkdata[index] == 45 /* negative height */ ||
          !png_check_fp_number(png_ptr->chunkdata, slength, &state, &index) ||
          index != slength)
         png_warning(png_ptr, "Invalid sCAL chunk ignored: bad height format");

      else
         /* This is the (only) success case. */
         png_set_sCAL_s(png_ptr, info_ptr, png_ptr->chunkdata[0],
            png_ptr->chunkdata+1, png_ptr->chunkdata+heighti);
   }

   /* Clean up - just free the temporarily allocated buffer. */
   png_free(png_ptr, png_ptr->chunkdata);
   png_ptr->chunkdata = NULL;
}
#endif