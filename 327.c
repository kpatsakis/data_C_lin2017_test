static void
modifier_set_encoding(png_modifier *pm)
{
   /* Set the encoding to the one specified by the current encoding counter,
    * first clear out all the settings - this corresponds to an encoding_counter
    * of 0.
    */
   pm->current_gamma = 0;
   pm->current_encoding = 0;
   pm->encoding_ignored = 0; /* not ignored yet - happens in _ini functions. */
   /* Now, if required, set the gamma and encoding fields. */
   if (pm->encoding_counter > 0)
   {
      /* The gammas[] array is an array of screen gammas, not encoding gammas,
       * so we need the inverse:
       */
      if (pm->encoding_counter <= pm->ngammas)
         pm->current_gamma = 1/pm->gammas[pm->encoding_counter-1];
      else
      {
         unsigned int i = pm->encoding_counter - pm->ngammas;
         if (i >= pm->nencodings)
         {
            i %= pm->nencodings;
            pm->current_gamma = 1; /* Linear, only in the 16 bit case */
         }
         else
            pm->current_gamma = pm->encodings[i].gamma;
         pm->current_encoding = pm->encodings + i;
      }
   }
}
