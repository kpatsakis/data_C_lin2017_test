#else /* write or low level APIs not supported */
int main(void)
{
   fprintf(stderr,
      "pngvalid: no low level write support in libpng, all tests skipped\n");
   /* So the test is skipped: */
   return SKIP;
}
