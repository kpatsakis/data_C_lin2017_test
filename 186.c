     */
static int PNGCBAPI read_user_chunk_callback(png_struct *png_ptr,
   png_unknown_chunkp chunk)
{
   png_uint_32
     *my_user_chunk_data;
   /* Return one of the following:
    *    return (-n);  chunk had an error
    *    return (0);  did not recognize
    *    return (n);  success
    *
    * The unknown chunk structure contains the chunk data:
    * png_byte name[5];
    * png_byte *data;
    * png_size_t size;
    *
    * Note that libpng has already taken care of the CRC handling.
    */
   if (chunk->name[0] == 115 && chunk->name[1] ==  84 &&     /* s  T */
       chunk->name[2] ==  69 && chunk->name[3] ==  82)       /* E  R */
      {
         /* Found sTER chunk */
         if (chunk->size != 1)
            return (-1); /* Error return */
         if (chunk->data[0] != 0 && chunk->data[0] != 1)
            return (-1);  /* Invalid mode */
         my_user_chunk_data=(png_uint_32 *) png_get_user_chunk_ptr(png_ptr);
         my_user_chunk_data[0]=chunk->data[0]+1;
         return (1);
      }
   if (chunk->name[0] != 118 || chunk->name[1] != 112 ||    /* v  p */
       chunk->name[2] !=  65 || chunk->name[3] != 103)      /* A  g */
      return (0); /* Did not recognize */
   /* Found ImageMagick vpAg chunk */
   if (chunk->size != 9)
      return (-1); /* Error return */
   my_user_chunk_data=(png_uint_32 *) png_get_user_chunk_ptr(png_ptr);
   my_user_chunk_data[1]=png_get_uint_31(png_ptr, chunk->data);
   my_user_chunk_data[2]=png_get_uint_31(png_ptr, chunk->data + 4);
   my_user_chunk_data[3]=(png_uint_32)chunk->data[8];
   return (1);
}
