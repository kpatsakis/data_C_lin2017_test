static void
modifier_crc(png_bytep buffer)
{
   /* Recalculate the chunk CRC - a complete chunk must be in
    * the buffer, at the start.
    */
   uInt datalen = png_get_uint_32(buffer);
   uLong crc = crc32(0, buffer+4, datalen+4);
   /* The cast to png_uint_32 is safe because a crc32 is always a 32 bit value.
    */
   png_save_uint_32(buffer+datalen+8, (png_uint_32)crc);
}
