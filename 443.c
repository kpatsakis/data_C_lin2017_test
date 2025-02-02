int
main(int argc, char **argv)
{
   FILE *fp = stdout;
   const char *file_name = NULL;
   int color_type = 8; /* invalid */
   int bit_depth = 32; /* invalid */
   unsigned int colors[5];
   unsigned int filters = PNG_ALL_FILTERS;
   png_fixed_point gamma = 0; /* not set */
   chunk_insert *head_insert = NULL;
   chunk_insert **insert_ptr = &head_insert;
   memset(colors, 0, sizeof colors);
   while (--argc > 0)
   {
      char *arg = *++argv;
      if (strcmp(arg, "--sRGB") == 0)
      {
         gamma = PNG_DEFAULT_sRGB;
         continue;
      }
      if (strcmp(arg, "--linear") == 0)
      {
         gamma = PNG_FP_1;
         continue;
      }
      if (strcmp(arg, "--1.8") == 0)
      {
         gamma = PNG_GAMMA_MAC_18;
         continue;
      }
      if (strcmp(arg, "--nofilters") == 0)
      {
         filters = PNG_FILTER_NONE;
         continue;
      }
      if (strncmp(arg, "--color=", 8) == 0)
      {
          parse_color(arg+8, colors);
          continue;
      }
      if (argc >= 3 && strcmp(arg, "--insert") == 0)
      {
         png_const_charp what = *++argv;
         png_charp param = *++argv;
         chunk_insert *new_insert;
         argc -= 2;
         new_insert = find_insert(what, param);
         if (new_insert != NULL)
         {
            *insert_ptr = new_insert;
            insert_ptr = &new_insert->next;
         }
         continue;
      }
      if (arg[0] == '-')
      {
         fprintf(stderr, "makepng: %s: invalid option\n", arg);
         exit(1);
      }
      if (strcmp(arg, "palette") == 0)
      {
         color_type = PNG_COLOR_TYPE_PALETTE;
         continue;
      }
      if (strncmp(arg, "gray", 4) == 0)
      {
         if (arg[4] == 0)
         {
            color_type = PNG_COLOR_TYPE_GRAY;
            continue;
         }
         else if (strcmp(arg+4, "a") == 0 ||
            strcmp(arg+4, "alpha") == 0 ||
            strcmp(arg+4, "-alpha") == 0)
         {
            color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
            continue;
         }
      }
      if (strncmp(arg, "rgb", 3) == 0)
      {
         if (arg[3] == 0)
         {
            color_type = PNG_COLOR_TYPE_RGB;
            continue;
         }
         else if (strcmp(arg+3, "a") == 0 ||
            strcmp(arg+3, "alpha") == 0 ||
            strcmp(arg+3, "-alpha") == 0)
         {
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            continue;
         }
      }
      if (color_type == 8 && isdigit(arg[0]))
      {
         color_type = atoi(arg);
         if (color_type < 0 || color_type > 6 || color_type == 1 ||
            color_type == 5)
         {
            fprintf(stderr, "makepng: %s: not a valid color type\n", arg);
            exit(1);
         }
         continue;
      }
      if (bit_depth == 32 && isdigit(arg[0]))
      {
         bit_depth = atoi(arg);
         if (bit_depth <= 0 || bit_depth > 16 ||
            (bit_depth & -bit_depth) != bit_depth)
         {
            fprintf(stderr, "makepng: %s: not a valid bit depth\n", arg);
            exit(1);
         }
         continue;
      }
      if (argc == 1) /* It's the file name */
      {
         fp = fopen(arg, "wb");
         if (fp == NULL)
         {
            fprintf(stderr, "%s: %s: could not open\n", arg, strerror(errno));
            exit(1);
         }
         file_name = arg;
         continue;
      }
      fprintf(stderr, "makepng: %s: unknown argument\n", arg);
      exit(1);
   } /* argument while loop */
   if (color_type == 8 || bit_depth == 32)
   {
      fprintf(stderr, "usage: makepng [--sRGB|--linear|--1.8] "
         "[--color=...] color-type bit-depth [file-name]\n"
         "  Make a test PNG file, by default writes to stdout.\n");
      exit(1);
   }
   /* Check the colors */
   {
      const unsigned int lim = (color_type == PNG_COLOR_TYPE_PALETTE ? 255U :
         (1U<<bit_depth)-1);
      unsigned int i;
      for (i=1; i<=colors[0]; ++i)
         if (colors[i] > lim)
         {
            fprintf(stderr, "makepng: --color=...: %u out of range [0..%u]\n",
               colors[i], lim);
            exit(1);
         }
   }
   /* Restrict the filters for more speed to those we know are used for the
    * generated images.
    */
   if (filters == PNG_ALL_FILTERS)
   {
      if ((color_type & PNG_COLOR_MASK_PALETTE) != 0 || bit_depth < 8)
         filters = PNG_FILTER_NONE;
      else if (color_type & PNG_COLOR_MASK_COLOR) /* rgb */
      {
         if (bit_depth == 8)
            filters &= ~(PNG_FILTER_NONE | PNG_FILTER_AVG);
         else
            filters = PNG_FILTER_SUB | PNG_FILTER_PAETH;
      }
      else /* gray 8 or 16-bit */
         filters &= ~PNG_FILTER_NONE;
   }
   {
      int ret = write_png(&file_name, fp, color_type, bit_depth, gamma,
         head_insert, filters, colors);
      if (ret != 0 && file_name != NULL)
         remove(file_name);
      return ret;
   }
}
