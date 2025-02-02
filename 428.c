}
static void gamma_threshold_test(png_modifier *pm, png_byte colour_type,
    png_byte bit_depth, int interlace_type, double file_gamma,
    double screen_gamma)
{
   size_t pos = 0;
   char name[64];
   pos = safecat(name, sizeof name, pos, "threshold ");
   pos = safecatd(name, sizeof name, pos, file_gamma, 3);
   pos = safecat(name, sizeof name, pos, "/");
   pos = safecatd(name, sizeof name, pos, screen_gamma, 3);
   (void)gamma_test(pm, colour_type, bit_depth, 0/*palette*/, interlace_type,
      file_gamma, screen_gamma, 0/*sBIT*/, 1/*threshold test*/, name,
      0 /*no input precision*/,
      0 /*no scale16*/, 0 /*no expand16*/, 0 /*no background*/, 0 /*hence*/,
      0 /*no background gamma*/);
}
