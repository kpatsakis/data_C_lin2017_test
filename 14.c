//CVE-2011-3464
void
png_formatted_warning(png_structp png_ptr, png_warning_parameters p,
   png_const_charp message)
{
   /* The internal buffer is just 128 bytes - enough for all our messages,
    * overflow doesn't happen because this code checks!
    */
   size_t i;
   char msg[128];

   for (i=0; i<(sizeof msg)-1 && *message != '\0'; ++i)
   {
      if (*message == '@')
      {
         int parameter = -1;
         switch (*++message)
         {
            case '1':
               parameter = 0;
               break;

            case '2':
               parameter = 1;
               break;

            case '\0':
               continue; /* To break out of the for loop above. */

            default:
               break;
         }

         if (parameter >= 0 && parameter < PNG_WARNING_PARAMETER_COUNT)
         {
            /* Append this parameter */
            png_const_charp parm = p[parameter];
            png_const_charp pend = p[parameter] + (sizeof p[parameter]);

            /* No need to copy the trailing '\0' here, but there is no guarantee
             * that parm[] has been initialized, so there is no guarantee of a
             * trailing '\0':
             */
            for (; i<(sizeof msg)-1 && parm != '\0' && parm < pend; ++i)
               msg[i] = *parm++;

            ++message;
            continue;
         }

         /* else not a parameter and there is a character after the @ sign; just
          * copy that.
          */
      }

      /* At this point *message can't be '\0', even in the bad parameter case
       * above where there is a lone '@' at the end of the message string.
       */
      msg[i] = *message++;
   }

   /* i is always less than (sizeof msg), so: */
   msg[i] = '\0';

   /* And this is the formatted message: */
   png_warning(png_ptr, msg);
}
#endif /* PNG_WARNINGS_SUPPORTED */
