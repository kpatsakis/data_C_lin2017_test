static void
gp_g8(Pixel *p, png_const_voidp pb)
{
   png_const_bytep pp = voidcast(png_const_bytep, pb);
   p->r = p->g = p->b = pp[0];
   p->a = 255;
}
