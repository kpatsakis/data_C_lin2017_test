static int
image_transform_test_counter(png_uint_32 counter, unsigned int max)
{
   /* Test the list to see if there is any point contining, given a current
    * counter and a 'max' value.
    */
   image_transform *next = image_transform_first;
   while (next != &image_transform_end)
   {
      /* For max 0 or 1 continue until the counter overflows: */
      counter >>= 1;
      /* Continue if any entry hasn't reacked the max. */
      if (max > 1 && next->local_use < max)
         return 1;
      next = next->list;
   }
   return max <= 1 && counter == 0;
}
