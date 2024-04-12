#else /* !PNG_SIMPLIFIED_READ_SUPPORTED */
int main(void)
{
   fprintf(stderr, "pngstest: no read support in libpng, test skipped\n");
   /* So the test is skipped: */
   return 77;
}
