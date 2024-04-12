static int
process_iTXt(struct file *file)
{
   /* Like zTXt but more fields. */
   struct chunk *chunk = file->chunk;
   png_uint_32 length;
   png_uint_32 index = 0;
   assert(chunk != NULL && file->idat == NULL);
   length = chunk->chunk_length;
   setpos(chunk);
   while (length >= 5)
   {
      --length;
      ++index;
      if (reread_byte(file) == 0) /* keyword null terminator */
      {
         --length;
         ++index;
         if (reread_byte(file) == 0) /* uncompressed text */
            return 1; /* nothing to check */
         --length;
         ++index;
         (void)reread_byte(file); /* compression method */
         /* Skip the language tag (null terminated). */
         while (length >= 9)
         {
            --length;
            ++index;
            if (reread_byte(file) == 0) /* terminator */
            {
               /* Skip the translated keyword */
               while (length >= 8)
               {
                  --length;
                  ++index;
                  if (reread_byte(file) == 0) /* terminator */
                     return zlib_check(file, index);
               }
            }
         }
         /* Ran out of bytes in the compressed case. */
         break;
      }
   }
   log_error(file, INVALID_ERROR_CODE, "iTXt chunk length");
   return 0; /* skip */
}
