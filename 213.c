static void
image_transform_png_set_expand_16_mod(PNG_CONST image_transform *this,
    image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
{
   /* Expect expand_16 to expand everything to 16 bits as a result of also
    * causing 'expand' to happen.
    */
   if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
      image_pixel_convert_PLTE(that);
   if (that->have_tRNS)
      image_pixel_add_alpha(that, &display->this);
   if (that->bit_depth < 16)
      that->sample_depth = that->bit_depth = 16;
   this->next->mod(this->next, that, pp, display);
}
