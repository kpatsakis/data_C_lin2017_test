static int
uarb_mult32(uarb acc, int a_digits, uarb num, int n_digits, png_uint_32 val)
   /* calculate acc += num * val, 'val' may be any 32-bit value, 'acc' and 'num'
    * may be any value, returns the number of digits in 'acc'.
    */
{
   if (n_digits > 0 && val > 0)
   {
      a_digits = uarb_mult_digit(acc, a_digits, num, n_digits,
         (png_uint_16)(val & 0xffff));
      /* Because n_digits and val are >0 the following must be true: */
      assert(a_digits > 0);
      val >>= 16;
      if (val > 0)
         a_digits = uarb_mult_digit(acc+1, a_digits-1, num, n_digits,
            (png_uint_16)val) + 1;
   }
