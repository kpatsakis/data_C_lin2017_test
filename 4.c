//CVE-2013-7353
#ifdef PNG_UNKNOWN_CHUNKS_SUPPORTED
void PNGAPI
png_set_unknown_chunks(png_structp png_ptr,
   png_infop info_ptr, png_const_unknown_chunkp unknowns, int num_unknowns)
{
   png_unknown_chunkp np;
   int i;

   if (png_ptr == NULL || info_ptr == NULL || num_unknowns == 0)
      return;

   np = (png_unknown_chunkp)png_malloc_warn(png_ptr,
       (png_size_t)(info_ptr->unknown_chunks_num + num_unknowns) *
       png_sizeof(png_unknown_chunk));

   if (np == NULL)
   {
      png_warning(png_ptr,
          "Out of memory while processing unknown chunk");
      return;
   }

   png_memcpy(np, info_ptr->unknown_chunks,
       (png_size_t)info_ptr->unknown_chunks_num *
       png_sizeof(png_unknown_chunk));

   png_free(png_ptr, info_ptr->unknown_chunks);
   info_ptr->unknown_chunks = NULL;

   for (i = 0; i < num_unknowns; i++)
   {
      png_unknown_chunkp to = np + info_ptr->unknown_chunks_num + i;
      png_const_unknown_chunkp from = unknowns + i;

      png_memcpy(to->name, from->name, png_sizeof(from->name));
      to->name[png_sizeof(to->name)-1] = '\0';
      to->size = from->size;

      /* Note our location in the read or write sequence */
      to->location = (png_byte)(png_ptr->mode & 0xff);

      if (from->size == 0)
         to->data=NULL;

      else
      {
         to->data = (png_bytep)png_malloc_warn(png_ptr,
             (png_size_t)from->size);

         if (to->data == NULL)
         {
            png_warning(png_ptr,
                "Out of memory while processing unknown chunk");
            to->size = 0;
         }

         else
            png_memcpy(to->data, from->data, from->size);
      }
   }

   info_ptr->unknown_chunks = np;
   info_ptr->unknown_chunks_num += num_unknowns;
   info_ptr->free_me |= PNG_FREE_UNKN;
}