//CVE-2007-5269
void /* PRIVATE */
png_handle_sCAL(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_charp buffer, ep;
#ifdef PNG_FLOATING_POINT_SUPPORTED
   double width, height;
   png_charp vp;
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   png_charp swidth, sheight;
#endif
#endif
   png_size_t slength;

   png_debug(1, "in png_handle_sCAL\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sCAL");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sCAL))
   {
      png_warning(png_ptr, "Duplicate sCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_debug1(2, "Allocating and reading sCAL chunk data (%lu bytes)\n",
      length + 1);
   buffer = (png_charp)png_malloc_warn(png_ptr, length + 1);
   if (buffer == NULL)
     {
       png_warning(png_ptr, "Out of memory while processing sCAL chunk");
       return;
     }
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)buffer, slength);

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, buffer);
      return;
   }

   buffer[slength] = 0x00; /* null terminate the last string */

   ep = buffer + 1;        /* skip unit byte */

#ifdef PNG_FLOATING_POINT_SUPPORTED
   width = strtod(ep, &vp);
   if (*vp)
   {
       png_warning(png_ptr, "malformed width string in sCAL chunk");
       return;
   }
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   swidth = (png_charp)png_malloc_warn(png_ptr, png_strlen(ep) + 1);
   if (swidth == NULL)
     {
       png_warning(png_ptr, "Out of memory while processing sCAL chunk width");
       return;
     }
   png_memcpy(swidth, ep, (png_size_t)png_strlen(ep));
#endif
#endif

   for (ep = buffer; *ep; ep++)
      /* empty loop */ ;
   ep++;

#ifdef PNG_FLOATING_POINT_SUPPORTED
   height = strtod(ep, &vp);
   if (*vp)
   {
       png_warning(png_ptr, "malformed height string in sCAL chunk");
       return;
   }
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   sheight = (png_charp)png_malloc_warn(png_ptr, png_strlen(ep) + 1);
   if (swidth == NULL)
     {
       png_warning(png_ptr, "Out of memory while processing sCAL chunk height");
       return;
     }
   png_memcpy(sheight, ep, (png_size_t)png_strlen(ep));
#endif
#endif

   if (buffer + slength < ep
#ifdef PNG_FLOATING_POINT_SUPPORTED
      || width <= 0. || height <= 0.
#endif
      )
   {
      png_warning(png_ptr, "Invalid sCAL data");
      png_free(png_ptr, buffer);
#if defined(PNG_FIXED_POINT_SUPPORTED) && !defined(PNG_FLOATING_POINT_SUPPORTED)
      png_free(png_ptr, swidth);
      png_free(png_ptr, sheight);
#endif
      return;
   }


#ifdef PNG_FLOATING_POINT_SUPPORTED
   png_set_sCAL(png_ptr, info_ptr, buffer[0], width, height);
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
   png_set_sCAL_s(png_ptr, info_ptr, buffer[0], swidth, sheight);
#endif
#endif

   png_free(png_ptr, buffer);
#if defined(PNG_FIXED_POINT_SUPPORTED) && !defined(PNG_FLOATING_POINT_SUPPORTED)
   png_free(png_ptr, swidth);
   png_free(png_ptr, sheight);
#endif
}
#endif