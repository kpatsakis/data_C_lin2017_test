//CVE-2009-5063
#ifdef PNG_WRITE_iCCP_SUPPORTED
/* Write an iCCP chunk */
void /* PRIVATE */
png_write_iCCP(png_structp png_ptr, png_const_charp name, int compression_type,
    png_const_charp profile, int profile_len)
{
   PNG_iCCP;
   png_size_t name_len;
   png_charp new_name;
   compression_state comp;
   int embedded_profile_len = 0;

   png_debug(1, "in png_write_iCCP");

   comp.num_output_ptr = 0;
   comp.max_output_ptr = 0;
   comp.output_ptr = NULL;
   comp.input = NULL;
   comp.input_len = 0;

   if ((name_len = png_check_keyword(png_ptr, name, &new_name)) == 0)
      return;

   if (compression_type != PNG_COMPRESSION_TYPE_BASE)
      png_warning(png_ptr, "Unknown compression type in iCCP chunk");

   if (profile == NULL)
      profile_len = 0;

   if (profile_len > 3)
      embedded_profile_len =
          ((*( (png_const_bytep)profile    ))<<24) |
          ((*( (png_const_bytep)profile + 1))<<16) |
          ((*( (png_const_bytep)profile + 2))<< 8) |
          ((*( (png_const_bytep)profile + 3))    );

   if (embedded_profile_len < 0)
   {
      png_warning(png_ptr,
          "Embedded profile length in iCCP chunk is negative");

      png_free(png_ptr, new_name);
      return;
   }

   if (profile_len < embedded_profile_len)
   {
      png_warning(png_ptr,
          "Embedded profile length too large in iCCP chunk");

      png_free(png_ptr, new_name);
      return;
   }

   if (profile_len > embedded_profile_len)
   {
      png_warning(png_ptr,
          "Truncating profile to actual length in iCCP chunk");

      profile_len = embedded_profile_len;
   }

   if (profile_len)
      profile_len = png_text_compress(png_ptr, profile,
          (png_size_t)profile_len, PNG_COMPRESSION_TYPE_BASE, &comp);

   /* Make sure we include the NULL after the name and the compression type */
   png_write_chunk_start(png_ptr, png_iCCP,
       (png_uint_32)(name_len + profile_len + 2));

   new_name[name_len + 1] = 0x00;

   png_write_chunk_data(png_ptr, (png_bytep)new_name,
       (png_size_t)(name_len + 2));

   if (profile_len)
      png_write_compressed_data_out(png_ptr, &comp);

   png_write_chunk_end(png_ptr);
   png_free(png_ptr, new_name);
}
#endif