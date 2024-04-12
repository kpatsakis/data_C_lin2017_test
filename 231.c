static void
size_row(png_byte buffer[SIZE_ROWMAX], png_uint_32 bit_width, png_uint_32 y)
{
   /* height is in the range 1 to 16, so: */
   y = ((y & 1) << 7) + ((y & 2) << 6) + ((y & 4) << 5) + ((y & 8) << 4);
   /* the following ensures bits are set in small images: */
   y ^= 0xA5;
   while (bit_width >= 8)
      *buffer++ = (png_byte)y++, bit_width -= 8;
   /* There may be up to 7 remaining bits, these go in the most significant
    * bits of the byte.
    */
   if (bit_width > 0)
   {
      png_uint_32 mask = (1U<<(8-bit_width))-1;
      *buffer = (png_byte)((*buffer & mask) | (y & ~mask));
   }
}
