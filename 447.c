static png_byte
sRGB(double linear /*range 0.0 .. 1.0*/)
{
   return u8d(255 * sRGB_from_linear(linear));
}
