static void
standard_info_imp(standard_display *dp, png_structp pp, png_infop pi,
    int nImages)
{
   /* Note that the validation routine has the side effect of turning on
    * interlace handling in the subsequent code.
    */
   standard_info_part1(dp, pp, pi);
   /* And the info callback has to call this (or png_read_update_info - see
    * below in the png_modifier code for that variant.
    */
   if (dp->use_update_info)
   {
      /* For debugging the effect of multiple calls: */
      int i = dp->use_update_info;
      while (i-- > 0)
         png_read_update_info(pp, pi);
   }
   else
      png_start_read_image(pp);
   /* Validate the height, width and rowbytes plus ensure that sufficient buffer
    * exists for decoding the image.
    */
   standard_info_part2(dp, pp, pi, nImages);
}
