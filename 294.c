void PNGCBAPI
count_zero_samples(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
   png_bytep dp = data;
   if (png_ptr == NULL)
      return;
   /* Contents of row_info:
    *  png_uint_32 width      width of row
    *  png_uint_32 rowbytes   number of bytes in row
    *  png_byte color_type    color type of pixels
    *  png_byte bit_depth     bit depth of samples
    *  png_byte channels      number of channels (1-4)
    *  png_byte pixel_depth   bits per pixel (depth*channels)
    */
    /* Counts the number of zero samples (or zero pixels if color_type is 3 */
    if (row_info->color_type == 0 || row_info->color_type == 3)
    {
       int pos = 0;
       png_uint_32 n, nstop;
       for (n = 0, nstop=row_info->width; n<nstop; n++)
       {
          if (row_info->bit_depth == 1)
          {
             if (((*dp << pos++ ) & 0x80) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 2)
          {
             if (((*dp << (pos+=2)) & 0xc0) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 4)
          {
             if (((*dp << (pos+=4)) & 0xf0) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 8)
             if (*dp++ == 0)
                zero_samples++;
          if (row_info->bit_depth == 16)
          {
             if ((*dp | *(dp+1)) == 0)
                zero_samples++;
             dp+=2;
          }
       }
    }
    else /* Other color types */
    {
       png_uint_32 n, nstop;
       int channel;
       int color_channels = row_info->channels;
       if (row_info->color_type > 3)color_channels--;
       for (n = 0, nstop=row_info->width; n<nstop; n++)
       {
          for (channel = 0; channel < color_channels; channel++)
          {
             if (row_info->bit_depth == 8)
                if (*dp++ == 0)
                   zero_samples++;
             if (row_info->bit_depth == 16)
             {
                if ((*dp | *(dp+1)) == 0)
                   zero_samples++;
                dp+=2;
             }
          }
          if (row_info->color_type > 3)
          {
             dp++;
             if (row_info->bit_depth == 16)
                dp++;
          }
       }
    }
}
