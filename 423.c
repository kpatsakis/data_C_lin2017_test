static void
make_size_images(png_store *ps)
{
   /* This is in case of errors. */
   safecat(ps->test, sizeof ps->test, 0, "make size images");
   /* Arguments are colour_type, low bit depth, high bit depth
    */
   make_size(ps, 0, 0, WRITE_BDHI);
   make_size(ps, 2, 3, WRITE_BDHI);
   make_size(ps, 3, 0, 3 /*palette: max 8 bits*/);
   make_size(ps, 4, 3, WRITE_BDHI);
   make_size(ps, 6, 3, WRITE_BDHI);
}
