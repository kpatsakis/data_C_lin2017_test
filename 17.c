//CVE-2004-0599
#if defined(PNG_READ_sPLT_SUPPORTED)
void /* PRIVATE */
png_handle_sPLT(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
/* Note: this does not properly handle chunks that are > 64K under DOS */
{
   png_bytep chunkdata;
   png_bytep entry_start;
   png_sPLT_t new_palette;
#ifdef PNG_NO_POINTER_INDEXING
   png_sPLT_entryp pp;
#endif
   int data_length, entry_size, i;
   png_uint_32 skip = 0;
   png_size_t slength;

   png_debug(1, "in png_handle_sPLT\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sPLT");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sPLT after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "sPLT chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   chunkdata = (png_bytep)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)chunkdata, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, chunkdata);
      return;
   }

   chunkdata[slength] = 0x00;

   for (entry_start = chunkdata; *entry_start; entry_start++)
      /* empty loop to find end of name */ ;
   ++entry_start;

   /* a sample depth should follow the separator, and we should be on it  */
   if (entry_start > chunkdata + slength - 2)
   {
      png_free(png_ptr, chunkdata);
      png_warning(png_ptr, "malformed sPLT chunk");
      return;
   }

   new_palette.depth = *entry_start++;
   entry_size = (new_palette.depth == 8 ? 6 : 10);
   data_length = (slength - (entry_start - chunkdata));

   /* integrity-check the data length */
   if (data_length % entry_size)
   {
      png_free(png_ptr, chunkdata);
      png_warning(png_ptr, "sPLT chunk has bad length");
      return;
   }

   new_palette.nentries = (png_int_32) ( data_length / entry_size);
   if ((png_uint_32) new_palette.nentries > (png_uint_32) (PNG_SIZE_MAX /
       png_sizeof(png_sPLT_entry)))
   {
       png_warning(png_ptr, "sPLT chunk too long");
       return;
   }
   new_palette.entries = (png_sPLT_entryp)png_malloc_warn(
       png_ptr, new_palette.nentries * png_sizeof(png_sPLT_entry));
   if (new_palette.entries == NULL)
   {
       png_warning(png_ptr, "sPLT chunk requires too much memory");
       return;
   }

#ifndef PNG_NO_POINTER_INDEXING
   for (i = 0; i < new_palette.nentries; i++)
   {
      png_sPLT_entryp pp = new_palette.entries + i;

      if (new_palette.depth == 8)
      {
          pp->red = *entry_start++;
          pp->green = *entry_start++;
          pp->blue = *entry_start++;
          pp->alpha = *entry_start++;
      }
      else
      {
          pp->red   = png_get_uint_16(entry_start); entry_start += 2;
          pp->green = png_get_uint_16(entry_start); entry_start += 2;
          pp->blue  = png_get_uint_16(entry_start); entry_start += 2;
          pp->alpha = png_get_uint_16(entry_start); entry_start += 2;
      }
      pp->frequency = png_get_uint_16(entry_start); entry_start += 2;
   }
#else
   pp = new_palette.entries;
   for (i = 0; i < new_palette.nentries; i++)
   {

      if (new_palette.depth == 8)
      {
          pp[i].red   = *entry_start++;
          pp[i].green = *entry_start++;
          pp[i].blue  = *entry_start++;
          pp[i].alpha = *entry_start++;
      }
      else
      {
          pp[i].red   = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].green = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].blue  = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].alpha = png_get_uint_16(entry_start); entry_start += 2;
      }
      pp->frequency = png_get_uint_16(entry_start); entry_start += 2;
   }
#endif

   /* discard all chunk data except the name and stash that */
   new_palette.name = (png_charp)chunkdata;

   png_set_sPLT(png_ptr, info_ptr, &new_palette, 1);

   png_free(png_ptr, chunkdata);
   png_free(png_ptr, new_palette.entries);
}
#endif 