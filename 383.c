static void
perform_size_test(png_modifier *pm)
{
   /* Test each colour type over the valid range of bit depths (expressed as
    * log2(bit_depth) in turn, stop as soon as any error is detected.
    */
   if (!test_size(pm, 0, 0, READ_BDHI))
      return;
   if (!test_size(pm, 2, 3, READ_BDHI))
      return;
   /* For the moment don't do the palette test - it's a waste of time when
    * compared to the grayscale test.
    */
#if 0
   if (!test_size(pm, 3, 0, 3))
      return;
#endif
   if (!test_size(pm, 4, 3, READ_BDHI))
      return;
   if (!test_size(pm, 6, 3, READ_BDHI))
      return;
}
