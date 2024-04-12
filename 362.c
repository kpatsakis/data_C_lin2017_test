static void
store_ensure_image(png_store *ps, png_const_structp pp, int nImages,
   png_size_t cbRow, png_uint_32 cRows)
{
   png_size_t cb = nImages * cRows * (cbRow + 5);
   if (ps->cb_image < cb)
   {
      png_bytep image;
      store_image_free(ps, pp);
      /* The buffer is deliberately mis-aligned. */
      image = voidcast(png_bytep, malloc(cb+2));
      if (image == NULL)
      {
         /* Called from the startup - ignore the error for the moment. */
         if (pp == NULL)
            return;
         png_error(pp, "OOM allocating image buffer");
      }
      /* These magic tags are used to detect overwrites above. */
      ++image;
      image[-1] = 0xed;
      image[cb] = 0xfe;
      ps->image = image;
      ps->cb_image = cb;
   }
   /* We have an adequate sized image; lay out the rows.  There are 2 bytes at
    * the start and three at the end of each (this ensures that the row
    * alignment starts out odd - 2+1 and changes for larger images on each row.)
    */
   ps->cb_row = cbRow;
   ps->image_h = cRows;
   /* For error checking, the whole buffer is set to 10110010 (0xb2 - 178).
    * This deliberately doesn't match the bits in the size test image which are
    * outside the image; these are set to 0xff (all 1).  To make the row
    * comparison work in the 'size' test case the size rows are pre-initialized
    * to the same value prior to calling 'standard_row'.
    */
   memset(ps->image, 178, cb);
   /* Then put in the marks. */
   while (--nImages >= 0)
   {
      png_uint_32 y;
      for (y=0; y<cRows; ++y)
      {
         png_bytep row = store_image_row(ps, pp, nImages, y);
         /* The markers: */
         row[-2] = 190;
         row[-1] = 239;
         row[cbRow] = 222;
         row[cbRow+1] = 173;
         row[cbRow+2] = 17;
      }
   }
}
