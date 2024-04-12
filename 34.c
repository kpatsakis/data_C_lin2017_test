#if defined(PNG_READ_pCAL_SUPPORTED)
/* read the pCAL chunk (described in the PNG Extensions document) */
//CVE-2007-5269
void /* PRIVATE */
png_handle_pCAL(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_charp purpose;
   png_int_32 X0, X1;
   png_byte type, nparams;
   png_charp buf, units, endptr;
   png_charpp params;
   png_size_t slength;
   int i;

   png_debug(1, "in png_handle_pCAL\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before pCAL");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid pCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_pCAL))
   {
      png_warning(png_ptr, "Duplicate pCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_debug1(2, "Allocating and reading pCAL chunk data (%lu bytes)\n",
      length + 1);
   purpose = (png_charp)png_malloc_warn(png_ptr, length + 1);
   if (purpose == NULL)
     {
       png_warning(png_ptr, "No memory for pCAL purpose.");
       return;
     }
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)purpose, slength);

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, purpose);
      return;
   }

   purpose[slength] = 0x00; /* null terminate the last string */

   png_debug(3, "Finding end of pCAL purpose string\n");
   for (buf = purpose; *buf; buf++)
      /* empty loop */ ;

   endptr = purpose + slength;

   /* We need to have at least 12 bytes after the purpose string
      in order to get the parameter information. */
   if (endptr <= buf + 12)
   {
      png_warning(png_ptr, "Invalid pCAL data");
      png_free(png_ptr, purpose);
      return;
   }

   png_debug(3, "Reading pCAL X0, X1, type, nparams, and units\n");
   X0 = png_get_int_32((png_bytep)buf+1);
   X1 = png_get_int_32((png_bytep)buf+5);
   type = buf[9];
   nparams = buf[10];
   units = buf + 11;

   png_debug(3, "Checking pCAL equation type and number of parameters\n");
   /* Check that we have the right number of parameters for known
      equation types. */
   if ((type == PNG_EQUATION_LINEAR && nparams != 2) ||
       (type == PNG_EQUATION_BASE_E && nparams != 3) ||
       (type == PNG_EQUATION_ARBITRARY && nparams != 3) ||
       (type == PNG_EQUATION_HYPERBOLIC && nparams != 4))
   {
      png_warning(png_ptr, "Invalid pCAL parameters for equation type");
      png_free(png_ptr, purpose);
      return;
   }
   else if (type >= PNG_EQUATION_LAST)
   {
      png_warning(png_ptr, "Unrecognized equation type for pCAL chunk");
   }

   for (buf = units; *buf; buf++)
      /* Empty loop to move past the units string. */ ;

   png_debug(3, "Allocating pCAL parameters array\n");
   params = (png_charpp)png_malloc_warn(png_ptr, (png_uint_32)(nparams
      *png_sizeof(png_charp))) ;
   if (params == NULL)
     {
       png_free(png_ptr, purpose);
       png_warning(png_ptr, "No memory for pCAL params.");
       return;
     }

   /* Get pointers to the start of each parameter string. */
   for (i = 0; i < (int)nparams; i++)
   {
      buf++; /* Skip the null string terminator from previous parameter. */

      png_debug1(3, "Reading pCAL parameter %d\n", i);
      for (params[i] = buf; *buf != 0x00 && buf <= endptr; buf++)
         /* Empty loop to move past each parameter string */ ;

      /* Make sure we haven't run out of data yet */
      if (buf > endptr)
      {
         png_warning(png_ptr, "Invalid pCAL data");
         png_free(png_ptr, purpose);
         png_free(png_ptr, params);
         return;
      }
   }

   png_set_pCAL(png_ptr, info_ptr, purpose, X0, X1, type, nparams,
      units, params);

   png_free(png_ptr, purpose);
   png_free(png_ptr, params);
}
#endif