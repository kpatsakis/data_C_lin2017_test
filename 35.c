//CVE-2009-0040
#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
#if defined(PNG_INFO_IMAGE_SUPPORTED)
void PNGAPI
png_read_png(png_structp png_ptr, png_infop info_ptr,
                           int transforms,
                           voidp params)
{
   int row;

   if (png_ptr == NULL) return;
#if defined(PNG_READ_INVERT_ALPHA_SUPPORTED)
   /* invert the alpha channel from opacity to transparency
    */
   if (transforms & PNG_TRANSFORM_INVERT_ALPHA)
       png_set_invert_alpha(png_ptr);
#endif

   /* png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).
    */
   png_read_info(png_ptr, info_ptr);
   if (info_ptr->height > PNG_UINT_32_MAX/png_sizeof(png_bytep))
      png_error(png_ptr, "Image is too high to process with png_read_png()");

   /* -------------- image transformations start here ------------------- */

#if defined(PNG_READ_16_TO_8_SUPPORTED)
   /* tell libpng to strip 16 bit/color files down to 8 bits per color
    */
   if (transforms & PNG_TRANSFORM_STRIP_16)
       png_set_strip_16(png_ptr);
#endif

#if defined(PNG_READ_STRIP_ALPHA_SUPPORTED)
   /* Strip alpha bytes from the input data without combining with
    * the background (not recommended).
    */
   if (transforms & PNG_TRANSFORM_STRIP_ALPHA)
       png_set_strip_alpha(png_ptr);
#endif

#if defined(PNG_READ_PACK_SUPPORTED) && !defined(PNG_READ_EXPAND_SUPPORTED)
   /* Extract multiple pixels with bit depths of 1, 2, or 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
   if (transforms & PNG_TRANSFORM_PACKING)
       png_set_packing(png_ptr);
#endif

#if defined(PNG_READ_PACKSWAP_SUPPORTED)
   /* Change the order of packed pixels to least significant bit first
    * (not useful if you are using png_set_packing).
    */
   if (transforms & PNG_TRANSFORM_PACKSWAP)
       png_set_packswap(png_ptr);
#endif

#if defined(PNG_READ_EXPAND_SUPPORTED)
   /* Expand paletted colors into true RGB triplets
    * Expand grayscale images to full 8 bits from 1, 2, or 4 bits/pixel
    * Expand paletted or RGB images with transparency to full alpha
    * channels so the data will be available as RGBA quartets.
    */
   if (transforms & PNG_TRANSFORM_EXPAND)
       if ((png_ptr->bit_depth < 8) ||
           (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE) ||
           (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)))
         png_set_expand(png_ptr);
#endif

   /* We don't handle background color or gamma transformation or dithering.
    */

#if defined(PNG_READ_INVERT_SUPPORTED)
   /* invert monochrome files to have 0 as white and 1 as black
    */
   if (transforms & PNG_TRANSFORM_INVERT_MONO)
       png_set_invert_mono(png_ptr);
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED)
   /* If you want to shift the pixel values from the range [0,255] or
    * [0,65535] to the original [0,7] or [0,31], or whatever range the
    * colors were originally in:
    */
   if ((transforms & PNG_TRANSFORM_SHIFT)
       && png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
   {
      png_color_8p sig_bit;

      png_get_sBIT(png_ptr, info_ptr, &sig_bit);
      png_set_shift(png_ptr, sig_bit);
   }
#endif

#if defined(PNG_READ_BGR_SUPPORTED)
   /* flip the RGB pixels to BGR (or RGBA to BGRA)
    */
   if (transforms & PNG_TRANSFORM_BGR)
       png_set_bgr(png_ptr);
#endif

#if defined(PNG_READ_SWAP_ALPHA_SUPPORTED)
   /* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR)
    */
   if (transforms & PNG_TRANSFORM_SWAP_ALPHA)
       png_set_swap_alpha(png_ptr);
#endif

#if defined(PNG_READ_SWAP_SUPPORTED)
   /* swap bytes of 16 bit files to least significant byte first
    */
   if (transforms & PNG_TRANSFORM_SWAP_ENDIAN)
       png_set_swap(png_ptr);
#endif

   /* We don't handle adding filler bytes */

   /* Optional call to gamma correct and add the background to the palette
    * and update info structure.  REQUIRED if you are expecting libpng to
    * update the palette for you (i.e., you selected such a transform above).
    */
   png_read_update_info(png_ptr, info_ptr);

   /* -------------- image transformations end here ------------------- */

#ifdef PNG_FREE_ME_SUPPORTED
   png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);
#endif
   if (info_ptr->row_pointers == NULL)
   {
      info_ptr->row_pointers = (png_bytepp)png_malloc(png_ptr,
         info_ptr->height * png_sizeof(png_bytep));
#ifdef PNG_FREE_ME_SUPPORTED
      info_ptr->free_me |= PNG_FREE_ROWS;
#endif
      for (row = 0; row < (int)info_ptr->height; row++)
      {
         info_ptr->row_pointers[row] = (png_bytep)png_malloc(png_ptr,
            png_get_rowbytes(png_ptr, info_ptr));
      }
   }

   png_read_image(png_ptr, info_ptr->row_pointers);
   info_ptr->valid |= PNG_INFO_IDAT;

   /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
   png_read_end(png_ptr, info_ptr);

   transforms = transforms; /* quiet compiler warnings */
   params = params;

}
#endif /* PNG_INFO_IMAGE_SUPPORTED */
#endif /* PNG_NO_SEQUENTIAL_READ_SUPPORTED */
#endif /* PNG_READ_SUPPORTED */
