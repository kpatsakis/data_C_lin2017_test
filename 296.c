static void
transform_row(png_const_structp pp, png_byte buffer[TRANSFORM_ROWMAX],
   png_byte colour_type, png_byte bit_depth, png_uint_32 y)
{
   png_uint_32 v = y << 7;
   png_uint_32 i = 0;
   switch (bit_size(pp, colour_type, bit_depth))
   {
      case 1:
         while (i<128/8) buffer[i] = (png_byte)(v & 0xff), v += 17, ++i;
         return;
      case 2:
         while (i<128/4) buffer[i] = (png_byte)(v & 0xff), v += 33, ++i;
         return;
      case 4:
         while (i<128/2) buffer[i] = (png_byte)(v & 0xff), v += 65, ++i;
         return;
      case 8:
         /* 256 bytes total, 128 bytes in each row set as follows: */
         while (i<128) buffer[i] = (png_byte)(v & 0xff), ++v, ++i;
         return;
      case 16:
         /* Generate all 65536 pixel values in order, which includes the 8 bit
          * GA case as well as the 16 bit G case.
          */
         while (i<128)
         {
            buffer[2*i] = (png_byte)((v>>8) & 0xff);
            buffer[2*i+1] = (png_byte)(v & 0xff);
            ++v;
            ++i;
         }
         return;
      case 24:
         /* 65535 pixels, but rotate the values. */
         while (i<128)
         {
            /* Three bytes per pixel, r, g, b, make b by r^g */
            buffer[3*i+0] = (png_byte)((v >> 8) & 0xff);
            buffer[3*i+1] = (png_byte)(v & 0xff);
            buffer[3*i+2] = (png_byte)(((v >> 8) ^ v) & 0xff);
            ++v;
            ++i;
         }
         return;
      case 32:
         /* 65535 pixels, r, g, b, a; just replicate */
         while (i<128)
         {
            buffer[4*i+0] = (png_byte)((v >> 8) & 0xff);
            buffer[4*i+1] = (png_byte)(v & 0xff);
            buffer[4*i+2] = (png_byte)((v >> 8) & 0xff);
            buffer[4*i+3] = (png_byte)(v & 0xff);
            ++v;
            ++i;
         }
         return;
      case 48:
         /* y is maximum 2047, giving 4x65536 pixels, make 'r' increase by 1 at
          * each pixel, g increase by 257 (0x101) and 'b' by 0x1111:
          */
         while (i<128)
         {
            png_uint_32 t = v++;
            buffer[6*i+0] = (png_byte)((t >> 8) & 0xff);
            buffer[6*i+1] = (png_byte)(t & 0xff);
            t *= 257;
            buffer[6*i+2] = (png_byte)((t >> 8) & 0xff);
            buffer[6*i+3] = (png_byte)(t & 0xff);
            t *= 17;
            buffer[6*i+4] = (png_byte)((t >> 8) & 0xff);
            buffer[6*i+5] = (png_byte)(t & 0xff);
            ++i;
         }
         return;
      case 64:
         /* As above in the 32 bit case. */
         while (i<128)
         {
            png_uint_32 t = v++;
            buffer[8*i+0] = (png_byte)((t >> 8) & 0xff);
            buffer[8*i+1] = (png_byte)(t & 0xff);
            buffer[8*i+4] = (png_byte)((t >> 8) & 0xff);
            buffer[8*i+5] = (png_byte)(t & 0xff);
            t *= 257;
            buffer[8*i+2] = (png_byte)((t >> 8) & 0xff);
            buffer[8*i+3] = (png_byte)(t & 0xff);
            buffer[8*i+6] = (png_byte)((t >> 8) & 0xff);
            buffer[8*i+7] = (png_byte)(t & 0xff);
            ++i;
         }
         return;
      default:
         break;
   }
   png_error(pp, "internal error");
}
