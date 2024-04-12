static int
png_have_neon(png_structp png_ptr)
{
   /* This is a whole lot easier than the linux code, however it is probably
    * implemented as below, therefore it is better to cache the result (these
    * function calls may be slow!)
    */
   PNG_UNUSED(png_ptr)
   return android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
      (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0;
}
