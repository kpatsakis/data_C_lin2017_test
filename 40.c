//CVE-2015-8540
#if defined(PNG_WRITE_TEXT_SUPPORTED) || defined(PNG_WRITE_pCAL_SUPPORTED) || \
    defined(PNG_WRITE_iCCP_SUPPORTED) || defined(PNG_WRITE_sPLT_SUPPORTED)
/* Check that the tEXt or zTXt keyword is valid per PNG 1.0 specification,
 * and if invalid, correct the keyword rather than discarding the entire
 * chunk.  The PNG 1.0 specification requires keywords 1-79 characters in
 * length, forbids leading or trailing whitespace, multiple internal spaces,
 * and the non-break space (0x80) from ISO 8859-1.  Returns keyword length.
 *
 * The new_key is allocated to hold the corrected keyword and must be freed
 * by the calling routine.  This avoids problems with trying to write to
 * static keywords without having to have duplicate copies of the strings.
 */
png_size_t /* PRIVATE */
png_check_keyword(png_structp png_ptr, png_const_charp key, png_charpp new_key)
{
   png_size_t key_len;
   png_const_charp ikp;
   png_charp kp, dp;
   int kflag;
   int kwarn=0;

   png_debug(1, "in png_check_keyword");

   *new_key = NULL;

   if (key == NULL || (key_len = png_strlen(key)) == 0)
   {
      png_warning(png_ptr, "zero length keyword");
      return ((png_size_t)0);
   }

   png_debug1(2, "Keyword to be checked is '%s'", key);

   *new_key = (png_charp)png_malloc_warn(png_ptr, (png_uint_32)(key_len + 2));

   if (*new_key == NULL)
   {
      png_warning(png_ptr, "Out of memory while procesing keyword");
      return ((png_size_t)0);
   }

   /* Replace non-printing characters with a blank and print a warning */
   for (ikp = key, dp = *new_key; *ikp != '\0'; ikp++, dp++)
   {
      if ((png_byte)*ikp < 0x20 ||
         ((png_byte)*ikp > 0x7E && (png_byte)*ikp < 0xA1))
      {
         PNG_WARNING_PARAMETERS(p)

         png_warning_parameter_unsigned(p, 1, PNG_NUMBER_FORMAT_02x,
            (png_byte)*ikp);
         png_formatted_warning(png_ptr, p, "invalid keyword character 0x@1");
         *dp = ' ';
      }

      else
      {
         *dp = *ikp;
      }
   }
   *dp = '\0';

   /* Remove any trailing white space. */
   kp = *new_key + key_len - 1;
   if (*kp == ' ')
   {
      png_warning(png_ptr, "trailing spaces removed from keyword");

      while (*kp == ' ')
      {
         *(kp--) = '\0';
         key_len--;
      }
   }

   /* Remove any leading white space. */
   kp = *new_key;
   if (*kp == ' ')
   {
      png_warning(png_ptr, "leading spaces removed from keyword");

      while (*kp == ' ')
      {
         kp++;
         key_len--;
      }
   }

   png_debug1(2, "Checking for multiple internal spaces in '%s'", kp);

   /* Remove multiple internal spaces. */
   for (kflag = 0, dp = *new_key; *kp != '\0'; kp++)
   {
      if (*kp == ' ' && kflag == 0)
      {
         *(dp++) = *kp;
         kflag = 1;
      }

      else if (*kp == ' ')
      {
         key_len--;
         kwarn = 1;
      }

      else
      {
         *(dp++) = *kp;
         kflag = 0;
      }
   }
   *dp = '\0';
   if (kwarn)
      png_warning(png_ptr, "extra interior spaces removed from keyword");

   if (key_len == 0)
   {
      png_free(png_ptr, *new_key);
      png_warning(png_ptr, "Zero length keyword");
   }

   if (key_len > 79)
   {
      png_warning(png_ptr, "keyword length must be 1 - 79 characters");
      (*new_key)[79] = '\0';
      key_len = 79;
   }

   return (key_len);
}
#endif