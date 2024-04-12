static void
modifier_encoding_iterate(png_modifier *pm)
{
   if (!pm->repeat && /* Else something needs the current encoding again. */
      pm->test_uses_encoding) /* Some transform is encoding dependent */
   {
      if (pm->test_exhaustive)
      {
         if (++pm->encoding_counter >= modifier_total_encodings(pm))
            pm->encoding_counter = 0; /* This will stop the repeat */
      }
      else
      {
         /* Not exhaustive - choose an encoding at random; generate a number in
          * the range 1..(max-1), so the result is always non-zero:
          */
         if (pm->encoding_counter == 0)
            pm->encoding_counter = random_mod(modifier_total_encodings(pm)-1)+1;
         else
            pm->encoding_counter = 0;
      }
      if (pm->encoding_counter > 0)
         pm->repeat = 1;
   }
   else if (!pm->repeat)
      pm->encoding_counter = 0;
}
