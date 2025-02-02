PNG_FUNCTION(void,PNGAPI
png_err,(png_structp png_ptr),PNG_NORETURN)
{
   if (png_ptr != NULL && png_ptr->error_fn != NULL)
      (*(png_ptr->error_fn))(png_ptr, '\0');

   /* If the custom handler doesn't exist, or if it returns,
      use the default handler, which will not return. */
   png_default_error(png_ptr, '\0');
}