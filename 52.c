static void
init_gamma_errors(png_modifier *pm)
{
   /* Use -1 to catch tests that were not actually run */
   pm->error_gray_2 = pm->error_gray_4 = pm->error_gray_8 = -1.;
   pm->error_color_8 = -1.;
   pm->error_indexed = -1.;
   pm->error_gray_16 = pm->error_color_16 = -1.;
}
