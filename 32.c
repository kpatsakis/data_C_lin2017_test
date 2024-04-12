//CVE-2016-10087
int /* PRIVATE */
png_set_text_2(png_const_structrp png_ptr, png_inforp info_ptr,
    png_const_textp text_ptr, int num_text)
{
   int i;

   png_debug1(1, "in %lx storage function", png_ptr == NULL ? "unexpected" :
      (unsigned long)png_ptr->chunk_name);

   if (png_ptr == NULL || info_ptr == NULL || num_text <= 0 || text_ptr == NULL)
      return(0);

   /* Make sure we have enough space in the "text" array in info_struct
    * to hold all of the incoming text_ptr objects.  This compare can't overflow
    * because max_text >= num_text (anyway, subtract of two positive integers
    * can't overflow in any case.)
    */
   if (num_text > info_ptr->max_text - info_ptr->num_text)
   {
      int old_num_text = info_ptr->num_text;
      int max_text;
      png_textp new_text = NULL;

      /* Calculate an appropriate max_text, checking for overflow. */
      max_text = old_num_text;
      if (num_text <= INT_MAX - max_text)
      {
         max_text += num_text;

         /* Round up to a multiple of 8 */
         if (max_text < INT_MAX-8)
            max_text = (max_text + 8) & ~0x7;

         else
            max_text = INT_MAX;

         /* Now allocate a new array and copy the old members in; this does all
          * the overflow checks.
          */
         new_text = png_voidcast(png_textp,png_realloc_array(png_ptr,
            info_ptr->text, old_num_text, max_text-old_num_text,
            sizeof *new_text));
      }

      if (new_text == NULL)
      {
         png_chunk_report(png_ptr, "too many text chunks",
            PNG_CHUNK_WRITE_ERROR);
         return 1;
      }

      png_free(png_ptr, info_ptr->text);

      info_ptr->text = new_text;
      info_ptr->free_me |= PNG_FREE_TEXT;
      info_ptr->max_text = max_text;
      /* num_text is adjusted below as the entries are copied in */

      png_debug1(3, "allocated %d entries for info_ptr->text", max_text);
   }

   for (i = 0; i < num_text; i++)
   {
      size_t text_length, key_len;
      size_t lang_len, lang_key_len;
      png_textp textp = &(info_ptr->text[info_ptr->num_text]);

      if (text_ptr[i].key == NULL)
          continue;

      if (text_ptr[i].compression < PNG_TEXT_COMPRESSION_NONE ||
          text_ptr[i].compression >= PNG_TEXT_COMPRESSION_LAST)
      {
         png_chunk_report(png_ptr, "text compression mode is out of range",
            PNG_CHUNK_WRITE_ERROR);
         continue;
      }

      key_len = strlen(text_ptr[i].key);

      if (text_ptr[i].compression <= 0)
      {
         lang_len = 0;
         lang_key_len = 0;
      }

      else
#  ifdef PNG_iTXt_SUPPORTED
      {
         /* Set iTXt data */

         if (text_ptr[i].lang != NULL)
            lang_len = strlen(text_ptr[i].lang);

         else
            lang_len = 0;

         if (text_ptr[i].lang_key != NULL)
            lang_key_len = strlen(text_ptr[i].lang_key);

         else
            lang_key_len = 0;
      }
#  else /* PNG_iTXt_SUPPORTED */
      {
         png_chunk_report(png_ptr, "iTXt chunk not supported",
            PNG_CHUNK_WRITE_ERROR);
         continue;
      }
#  endif

      if (text_ptr[i].text == NULL || text_ptr[i].text[0] == '\0')
      {
         text_length = 0;
#  ifdef PNG_iTXt_SUPPORTED
         if (text_ptr[i].compression > 0)
            textp->compression = PNG_ITXT_COMPRESSION_NONE;

         else
#  endif
            textp->compression = PNG_TEXT_COMPRESSION_NONE;
      }

      else
      {
         text_length = strlen(text_ptr[i].text);
         textp->compression = text_ptr[i].compression;
      }

      textp->key = png_voidcast(png_charp,png_malloc_base(png_ptr,
          key_len + text_length + lang_len + lang_key_len + 4));

      if (textp->key == NULL)
      {
         png_chunk_report(png_ptr, "text chunk: out of memory",
               PNG_CHUNK_WRITE_ERROR);
         return 1;
      }

      png_debug2(2, "Allocated %lu bytes at %p in png_set_text",
          (unsigned long)(png_uint_32)
          (key_len + lang_len + lang_key_len + text_length + 4),
          textp->key);

      memcpy(textp->key, text_ptr[i].key, key_len);
      *(textp->key + key_len) = '\0';

      if (text_ptr[i].compression > 0)
      {
         textp->lang = textp->key + key_len + 1;
         memcpy(textp->lang, text_ptr[i].lang, lang_len);
         *(textp->lang + lang_len) = '\0';
         textp->lang_key = textp->lang + lang_len + 1;
         memcpy(textp->lang_key, text_ptr[i].lang_key, lang_key_len);
         *(textp->lang_key + lang_key_len) = '\0';
         textp->text = textp->lang_key + lang_key_len + 1;
      }

      else
      {
         textp->lang=NULL;
         textp->lang_key=NULL;
         textp->text = textp->key + key_len + 1;
      }

      if (text_length != 0)
         memcpy(textp->text, text_ptr[i].text, text_length);

      *(textp->text + text_length) = '\0';

#  ifdef PNG_iTXt_SUPPORTED
      if (textp->compression > 0)
      {
         textp->text_length = 0;
         textp->itxt_length = text_length;
      }

      else
#  endif
      {
         textp->text_length = text_length;
         textp->itxt_length = 0;
      }

      info_ptr->num_text++;
      png_debug1(3, "transferred text chunk %d", info_ptr->num_text);
   }

   return(0);
}
#endif