static void
gpc_g16(Pixel *out, const Pixel *in, const Background *back)
{
   (void)back;
   out->r = out->g = out->b = u16d(YfromRGBint(in->r, in->g, in->b));
   out->a = 65535;
}
