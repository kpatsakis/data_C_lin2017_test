//CVE-2012-3425
void /* PRIVATE */
png_push_read_zTXt(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr->buffer_size && png_ptr->current_text_left)
   {
      png_size_t text_size;

      if (png_ptr->buffer_size < (png_uint_32)png_ptr->current_text_left)
         text_size = png_ptr->buffer_size;

      else
         text_size = png_ptr->current_text_left;

      png_crc_read(png_ptr, (png_bytep)png_ptr->current_text_ptr, text_size);
      png_ptr->current_text_left -= text_size;
      png_ptr->current_text_ptr += text_size;
   }
   if (!(png_ptr->current_text_left))
   {
      png_textp text_ptr;
      png_charp text;
      png_charp key;
      int ret;
      png_size_t text_size, key_size;

      if (png_ptr->buffer_size < 4)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      png_push_crc_finish(png_ptr);

      key = png_ptr->current_text;

      for (text = key; *text; text++)
         /* Empty loop */ ;

      /* zTXt can't have zero text */
      if (text >= key + png_ptr->current_text_size)
      {
         png_ptr->current_text = NULL;
         png_free(png_ptr, key);
         return;
      }

      text++;

      if (*text != PNG_TEXT_COMPRESSION_zTXt) /* Check compression byte */
      {
         png_ptr->current_text = NULL;
         png_free(png_ptr, key);
         return;
      }

      text++;

      png_ptr->zstream.next_in = (png_bytep)text;
      png_ptr->zstream.avail_in = (uInt)(png_ptr->current_text_size -
          (text - key));
      png_ptr->zstream.next_out = png_ptr->zbuf;
      png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;

      key_size = text - key;
      text_size = 0;
      text = NULL;
      ret = Z_STREAM_END;

      while (png_ptr->zstream.avail_in)
      {
         ret = inflate(&png_ptr->zstream, Z_PARTIAL_FLUSH);
         if (ret != Z_OK && ret != Z_STREAM_END)
         {
            inflateReset(&png_ptr->zstream);
            png_ptr->zstream.avail_in = 0;
            png_ptr->current_text = NULL;
            png_free(png_ptr, key);
            png_free(png_ptr, text);
            return;
         }

         if (!(png_ptr->zstream.avail_out) || ret == Z_STREAM_END)
         {
            if (text == NULL)
            {
               text = (png_charp)png_malloc(png_ptr,
                   (png_ptr->zbuf_size
                   - png_ptr->zstream.avail_out + key_size + 1));

               png_memcpy(text + key_size, png_ptr->zbuf,
                   png_ptr->zbuf_size - png_ptr->zstream.avail_out);

               png_memcpy(text, key, key_size);

               text_size = key_size + png_ptr->zbuf_size -
                   png_ptr->zstream.avail_out;

               *(text + text_size) = '\0';
            }

            else
            {
               png_charp tmp;

               tmp = text;
               text = (png_charp)png_malloc(png_ptr, text_size +
                   (png_ptr->zbuf_size
                   - png_ptr->zstream.avail_out + 1));

               png_memcpy(text, tmp, text_size);
               png_free(png_ptr, tmp);

               png_memcpy(text + text_size, png_ptr->zbuf,
                   png_ptr->zbuf_size - png_ptr->zstream.avail_out);

               text_size += png_ptr->zbuf_size - png_ptr->zstream.avail_out;
               *(text + text_size) = '\0';
            }

            if (ret != Z_STREAM_END)
            {
               png_ptr->zstream.next_out = png_ptr->zbuf;
               png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
            }
         }
         else
         {
            break;
         }

         if (ret == Z_STREAM_END)
            break;
      }

      inflateReset(&png_ptr->zstream);
      png_ptr->zstream.avail_in = 0;

      if (ret != Z_STREAM_END)
      {
         png_ptr->current_text = NULL;
         png_free(png_ptr, key);
         png_free(png_ptr, text);
         return;
      }

      png_ptr->current_text = NULL;
      png_free(png_ptr, key);
      key = text;
      text += key_size;

      text_ptr = (png_textp)png_malloc(png_ptr,
          png_sizeof(png_text));
      text_ptr->compression = PNG_TEXT_COMPRESSION_zTXt;
      text_ptr->key = key;
      text_ptr->itxt_length = 0;
      text_ptr->lang = NULL;
      text_ptr->lang_key = NULL;
      text_ptr->text = text;

      ret = png_set_text_2(png_ptr, info_ptr, text_ptr, 1);

      png_free(png_ptr, key);
      png_free(png_ptr, text_ptr);

      if (ret)
         png_warning(png_ptr, "Insufficient memory to store text chunk");
   }
}
#endif