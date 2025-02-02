static png_uint_32
transform_height(png_const_structp pp, png_byte colour_type, png_byte bit_depth)
{
   switch (bit_size(pp, colour_type, bit_depth))
   {
      case 1:
      case 2:
      case 4:
         return 1;   /* Total of 128 pixels */
      case 8:
         return 2;   /* Total of 256 pixels/bytes */
      case 16:
         return 512; /* Total of 65536 pixels */
      case 24:
      case 32:
         return 512; /* 65536 pixels */
      case 48:
      case 64:
         return 2048;/* 4 x 65536 pixels. */
#        define TRANSFORM_HEIGHTMAX 2048
      default:
         return 0;   /* Error, will be caught later */
   }
}
