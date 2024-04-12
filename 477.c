static png_uint_32
get_unknown(display *d, png_infop info_ptr, int after_IDAT)
{
   /* Create corresponding 'unknown' flags */
   png_uint_32 flags = 0;
   UNUSED(after_IDAT)
   {
      png_unknown_chunkp unknown;
      int num_unknown = png_get_unknown_chunks(d->png_ptr, info_ptr, &unknown);
      while (--num_unknown >= 0)
      {
         int chunk = findb(unknown[num_unknown].name);
         /* Chunks not known to pngunknown must be validated here; since they
          * must also be unknown to libpng the 'display->keep' behavior should
          * have been used.
          */
         if (chunk < 0) switch (d->keep)
         {
            default: /* impossible */
            case PNG_HANDLE_CHUNK_AS_DEFAULT:
            case PNG_HANDLE_CHUNK_NEVER:
               fprintf(stderr, "%s(%s): %s: %s: unknown chunk saved\n",
                  d->file, d->test, d->keep ? "discard" : "default",
                  unknown[num_unknown].name);
               ++(d->error_count);
               break;
            case PNG_HANDLE_CHUNK_IF_SAFE:
               if (!ancillaryb(unknown[num_unknown].name))
               {
                  fprintf(stderr,
                     "%s(%s): if-safe: %s: unknown critical chunk saved\n",
                     d->file, d->test, unknown[num_unknown].name);
                  ++(d->error_count);
                  break;
               }
               /* FALL THROUGH (safe) */
            case PNG_HANDLE_CHUNK_ALWAYS:
               break;
         }
         else
            flags |= chunk_info[chunk].flag;
      }
   }
   return flags;
}
