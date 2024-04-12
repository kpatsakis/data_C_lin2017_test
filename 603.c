static int
find_parameters(png_const_charp what, png_charp param, png_charp *list,
   int nparams)
{
   /* Parameters are separated by '\n' or ':' characters, up to nparams are
    * accepted (more is an error) and the number found is returned.
    */
   int i;
   for (i=0; *param && i<nparams; ++i)
   {
      list[i] = param;
      while (*++param) if (*param == '\n' || *param == ':')
      {
         *param++ = 0; /* Terminate last parameter */
         break;        /* And start a new one. */
      }
   }
   if (*param)
   {
      fprintf(stderr, "--insert %s: too many parameters (%s)\n", what, param);
      exit(1);
   }
   list[i] = NULL; /* terminates list */
   return i; /* number of parameters filled in */
}
