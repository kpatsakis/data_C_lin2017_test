//CVE-2004-0597
#if defined(PNG_READ_hIST_SUPPORTED)
void
png_handle_hIST(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   int num, i;

   png_debug(1, "in png_handle_hIST\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before hIST");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid hIST after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (!(png_ptr->mode & PNG_HAVE_PLTE))
   {
      png_warning(png_ptr, "Missing PLTE before hIST");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && info_ptr->valid & PNG_INFO_hIST)
   {
      png_warning(png_ptr, "Duplicate hIST chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (length != (png_uint_32)(2 * png_ptr->num_palette))
   {
      png_warning(png_ptr, "Incorrect hIST chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   num = (int)length / 2;
   png_ptr->hist = (png_uint_16p)png_malloc(png_ptr,
      (png_uint_32)(num * sizeof (png_uint_16)));
   png_ptr->flags |= PNG_FLAG_FREE_HIST;
   for (i = 0; i < num; i++)
   {
      png_byte buf[2];

      png_crc_read(png_ptr, buf, 2);
      png_ptr->hist[i] = png_get_uint_16(buf);
   }

   if (png_crc_finish(png_ptr, 0))
      return;

   png_set_hIST(png_ptr, info_ptr, png_ptr->hist);
}
#endif