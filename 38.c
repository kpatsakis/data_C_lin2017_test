//CVE-2007-5269
void /* PRIVATE */
png_handle_iTXt(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_textp text_ptr;
   png_charp chunkdata;
   png_charp key, lang, text, lang_key;
   int comp_flag;
   int comp_type = 0;
   int ret;
   png_size_t slength, prefix_len, data_len;

   png_debug(1, "in png_handle_iTXt\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before iTXt");

   if (png_ptr->mode & PNG_HAVE_IDAT)
      png_ptr->mode |= PNG_AFTER_IDAT;

#ifdef PNG_MAX_MALLOC_64K
   /* We will no doubt have problems with chunks even half this size, but
      there is no hard and fast rule to tell us where to stop. */
   if (length > (png_uint_32)65535L)
   {
     png_warning(png_ptr,"iTXt chunk too large to fit in memory");
     png_crc_finish(png_ptr, length);
     return;
   }
#endif

   chunkdata = (png_charp)png_malloc_warn(png_ptr, length + 1);
   if (chunkdata == NULL)
   {
     png_warning(png_ptr, "No memory to process iTXt chunk.");
     return;
   }
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)chunkdata, slength);
   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, chunkdata);
      return;
   }

   chunkdata[slength] = 0x00;

   for (lang = chunkdata; *lang; lang++)
      /* empty loop */ ;
   lang++;        /* skip NUL separator */

   /* iTXt must have a language tag (possibly empty), two compression bytes,
      translated keyword (possibly empty), and possibly some text after the
      keyword */

   if (lang >= chunkdata + slength)
   {
      comp_flag = PNG_TEXT_COMPRESSION_NONE;
      png_warning(png_ptr, "Zero length iTXt chunk");
   }
   else
   {
       comp_flag = *lang++;
       comp_type = *lang++;
   }

   for (lang_key = lang; *lang_key; lang_key++)
      /* empty loop */ ;
   lang_key++;        /* skip NUL separator */

   for (text = lang_key; *text; text++)
      /* empty loop */ ;
   text++;        /* skip NUL separator */

   prefix_len = text - chunkdata;

   key=chunkdata;
   if (comp_flag)
       chunkdata = png_decompress_chunk(png_ptr, comp_type, chunkdata,
          (size_t)length, prefix_len, &data_len);
   else
       data_len=png_strlen(chunkdata + prefix_len);
   text_ptr = (png_textp)png_malloc_warn(png_ptr,
      (png_uint_32)png_sizeof(png_text));
   if (text_ptr == NULL)
   {
     png_warning(png_ptr,"Not enough memory to process iTXt chunk.");
     png_free(png_ptr, chunkdata);
     return;
   }
   text_ptr->compression = (int)comp_flag + 1;
   text_ptr->lang_key = chunkdata+(lang_key-key);
   text_ptr->lang = chunkdata+(lang-key);
   text_ptr->itxt_length = data_len;
   text_ptr->text_length = 0;
   text_ptr->key = chunkdata;
   text_ptr->text = chunkdata + prefix_len;

   ret=png_set_text_2(png_ptr, info_ptr, text_ptr, 1);

   png_free(png_ptr, text_ptr);
   png_free(png_ptr, chunkdata);
   if (ret)
     png_error(png_ptr, "Insufficient memory to store iTXt chunk.");
}
#endif