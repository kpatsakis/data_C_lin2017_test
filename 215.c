static void PNGCBAPI
standard_info(png_structp pp, png_infop pi)
{
   standard_display *dp = voidcast(standard_display*,
      png_get_progressive_ptr(pp));
   /* Call with nImages==1 because the progressive reader can only produce one
    * image.
    */
   standard_info_imp(dp, pp, pi, 1 /*only one image*/);
}
