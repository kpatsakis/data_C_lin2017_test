static unsigned int
component(png_const_bytep row, png_uint_32 x, unsigned int c,
   unsigned int bit_depth, unsigned int channels)
{
   /* PNG images can be up to 2^31 pixels wide, but this means they can be up to
    * 2^37 bits wide (for a 64-bit pixel - the largest possible) and hence 2^34
    * bytes wide.  Since the row fitted into memory, however, the following must
    * work:
    */
   png_uint_32 bit_offset_hi = bit_depth * ((x >> 6) * channels);
   png_uint_32 bit_offset_lo = bit_depth * ((x & 0x3f) * channels + c);
   row = (png_const_bytep)(((PNG_CONST png_byte (*)[8])row) + bit_offset_hi);
   row += bit_offset_lo >> 3;
   bit_offset_lo &= 0x07;
   /* PNG pixels are packed into bytes to put the first pixel in the highest
    * bits of the byte and into two bytes for 16-bit values with the high 8 bits
    * first, so:
    */
   switch (bit_depth)
   {
      case 1: return (row[0] >> (7-bit_offset_lo)) & 0x01;
      case 2: return (row[0] >> (6-bit_offset_lo)) & 0x03;
      case 4: return (row[0] >> (4-bit_offset_lo)) & 0x0f;
      case 8: return row[0];
      case 16: return (row[0] << 8) + row[1];
      default:
         /* This should never happen, it indicates a bug in this program or in
          * libpng itself:
          */
         fprintf(stderr, "pngpixel: invalid bit depth %u\n", bit_depth);
         exit(1);
   }
}
