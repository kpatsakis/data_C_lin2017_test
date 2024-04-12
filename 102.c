static void
image_pixel_convert_PLTE(image_pixel *this)
{
   if (this->colour_type == PNG_COLOR_TYPE_PALETTE)
   {
      if (this->have_tRNS)
      {
         this->colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
         this->have_tRNS = 0;
      }
      else
         this->colour_type = PNG_COLOR_TYPE_RGB;
      /* The bit depth of the row changes at this point too (notice that this is
       * the row format, not the sample depth, which is separate.)
       */
      this->bit_depth = 8;
   }
}
