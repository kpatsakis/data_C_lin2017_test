static png_uint_32
current_type(struct file *file, int code)
   /* Guess the actual chunk type that causes a stop() */
{
   /* This may return png_IDAT for errors detected (late) in the header; that
    * includes any inter-chunk consistency check that libpng performs.  Assume
    * that if the chunk_type is png_IDAT and the file write count is 8 this is
    * what is happening.
    */
   if (file->chunk != NULL)
   {
      png_uint_32 type = file->chunk->chunk_type;
      /* This is probably wrong for the excess IDATs case, because then libpng
       * whines about too many of them (apparently in some cases erroneously)
       * when the header is read.
       */
      if (code <= LIBPNG_ERROR_CODE && type == png_IDAT &&
         file->write_count == 8)
         type = 0; /* magic */
      return type;
   }
   else
      return file->type;
}
