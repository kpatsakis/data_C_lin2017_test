//CVE-2008-6218
void /* PRIVATE */
png_handle_tEXt(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_textp text_ptr;
   png_charp key;
   png_charp text;
   png_uint_32 skip = 0;
   png_size_t slength;
   int ret;

   png_debug(1, "in png_handle_tEXt\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before tEXt");

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "tEXt chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   key = (png_charp)png_malloc_warn(png_ptr, length + 1);
   if (key == NULL)
   {
     png_warning(png_ptr, "No memory to process text chunk.");
     return;
   }
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)key, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, key);
      return;
   }

   key[slength] = 0x00;

   for (text = key; *text; text++)
      /* empty loop to find end of key */ ;

   if (text != key + slength)
      text++;

   text_ptr = (png_textp)png_malloc_warn(png_ptr,
      (png_uint_32)png_sizeof(png_text));
   if (text_ptr == NULL)
   {
     png_warning(png_ptr, "Not enough memory to process text chunk.");
     png_free(png_ptr, key);
     return;
   }
   text_ptr->compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr->key = key;
#ifdef PNG_iTXt_SUPPORTED
   text_ptr->lang = NULL;
   text_ptr->lang_key = NULL;
   text_ptr->itxt_length = 0;
#endif
   text_ptr->text = text;
   text_ptr->text_length = png_strlen(text);

   ret = png_set_text_2(png_ptr, info_ptr, text_ptr, 1);

   png_free(png_ptr, key);
   png_free(png_ptr, text_ptr);
   if (ret)
     png_warning(png_ptr, "Insufficient memory to process text chunk.");
}
#endif