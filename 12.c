//CVE-2011-3328
#ifdef PNG_READ_cHRM_SUPPORTED
void /* PRIVATE */
png_handle_cHRM(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_byte buf[32];
   png_fixed_point x_white, y_white, x_red, y_red, x_green, y_green, x_blue,
      y_blue;

   png_debug(1, "in png_handle_cHRM");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before cHRM");

   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid cHRM after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }

   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Missing PLTE before cHRM");

   if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_cHRM)
#  ifdef PNG_READ_sRGB_SUPPORTED
       && !(info_ptr->valid & PNG_INFO_sRGB)
#  endif
      )
   {
      png_warning(png_ptr, "Duplicate cHRM chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (length != 32)
   {
      png_warning(png_ptr, "Incorrect cHRM chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 32);

   if (png_crc_finish(png_ptr, 0))
      return;

   x_white = png_get_fixed_point(NULL, buf);
   y_white = png_get_fixed_point(NULL, buf + 4);
   x_red   = png_get_fixed_point(NULL, buf + 8);
   y_red   = png_get_fixed_point(NULL, buf + 12);
   x_green = png_get_fixed_point(NULL, buf + 16);
   y_green = png_get_fixed_point(NULL, buf + 20);
   x_blue  = png_get_fixed_point(NULL, buf + 24);
   y_blue  = png_get_fixed_point(NULL, buf + 28);

   if (x_white == PNG_FIXED_ERROR ||
       y_white == PNG_FIXED_ERROR ||
       x_red   == PNG_FIXED_ERROR ||
       y_red   == PNG_FIXED_ERROR ||
       x_green == PNG_FIXED_ERROR ||
       y_green == PNG_FIXED_ERROR ||
       x_blue  == PNG_FIXED_ERROR ||
       y_blue  == PNG_FIXED_ERROR)
   {
      png_warning(png_ptr, "Ignoring cHRM chunk with negative chromaticities");
      return;
   }

#ifdef PNG_READ_sRGB_SUPPORTED
   if ((info_ptr != NULL) && (info_ptr->valid & PNG_INFO_sRGB))
   {
      if (PNG_OUT_OF_RANGE(x_white, 31270,  1000) ||
          PNG_OUT_OF_RANGE(y_white, 32900,  1000) ||
          PNG_OUT_OF_RANGE(x_red,   64000L, 1000) ||
          PNG_OUT_OF_RANGE(y_red,   33000,  1000) ||
          PNG_OUT_OF_RANGE(x_green, 30000,  1000) ||
          PNG_OUT_OF_RANGE(y_green, 60000L, 1000) ||
          PNG_OUT_OF_RANGE(x_blue,  15000,  1000) ||
          PNG_OUT_OF_RANGE(y_blue,   6000,  1000))
      {
         PNG_WARNING_PARAMETERS(p)

         png_warning_parameter_signed(p, 1, PNG_NUMBER_FORMAT_fixed, x_white);
         png_warning_parameter_signed(p, 2, PNG_NUMBER_FORMAT_fixed, y_white);
         png_warning_parameter_signed(p, 3, PNG_NUMBER_FORMAT_fixed, x_red);
         png_warning_parameter_signed(p, 4, PNG_NUMBER_FORMAT_fixed, y_red);
         png_warning_parameter_signed(p, 5, PNG_NUMBER_FORMAT_fixed, x_green);
         png_warning_parameter_signed(p, 6, PNG_NUMBER_FORMAT_fixed, y_green);
         png_warning_parameter_signed(p, 7, PNG_NUMBER_FORMAT_fixed, x_blue);
         png_warning_parameter_signed(p, 8, PNG_NUMBER_FORMAT_fixed, y_blue);

         png_formatted_warning(png_ptr, p,
             "Ignoring incorrect cHRM white(@1,@2) r(@3,@4)g(@5,@6)b(@7,@8) "
             "when sRGB is also present");
      }
      return;
   }
#endif /* PNG_READ_sRGB_SUPPORTED */

#ifdef PNG_READ_RGB_TO_GRAY_SUPPORTED
   /* Store the _white values as default coefficients for the rgb to gray
    * operation if it is supported.
    */
   if ((png_ptr->transformations & PNG_RGB_TO_GRAY) == 0)
   {
      /* png_set_background has not been called, the coefficients must be in
       * range for the following to work without overflow.
       */
      if (y_red <= (1<<17) && y_green <= (1<<17) && y_blue <= (1<<17))
      {
         /* The y values are chromaticities: Y/X+Y+Z, the weights for the gray
          * transformation are simply the normalized Y values for red, green and
          * blue scaled by 32768.
          */
         png_uint_32 w = y_red + y_green + y_blue;

         png_ptr->rgb_to_gray_red_coeff   = (png_uint_16)(((png_uint_32)y_red *
            32768)/w);
         png_ptr->rgb_to_gray_green_coeff = (png_uint_16)(((png_uint_32)y_green
            * 32768)/w);
         png_ptr->rgb_to_gray_blue_coeff  = (png_uint_16)(((png_uint_32)y_blue *
            32768)/w);
      }
   }
#endif

   png_set_cHRM_fixed(png_ptr, info_ptr, x_white, y_white, x_red, y_red,
      x_green, y_green, x_blue, y_blue);
}
#endif