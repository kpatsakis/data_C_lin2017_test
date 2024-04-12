static size_t
safe_read(png_structp png_ptr, int fd, void *buffer_in, size_t nbytes)
{
   size_t ntotal = 0;
   char *buffer = png_voidcast(char*, buffer_in);
   while (nbytes > 0)
   {
      unsigned int nread;
      int iread;
      /* Passing nread > INT_MAX to read is implementation defined in POSIX
       * 1003.1, therefore despite the unsigned argument portable code must
       * limit the value to INT_MAX!
       */
      if (nbytes > INT_MAX)
         nread = INT_MAX;
      else
         nread = (unsigned int)/*SAFE*/nbytes;
      iread = read(fd, buffer, nread);
      if (iread == -1)
      {
         /* This is the devil in the details, a read can terminate early with 0
          * bytes read because of EINTR, yet it still returns -1 otherwise end
          * of file cannot be distinguished.
          */
         if (errno != EINTR)
         {
            png_warning(png_ptr, "/proc read failed");
            return 0; /* I.e., a permanent failure */
         }
      }
      else if (iread < 0)
      {
         /* Not a valid 'read' result: */
         png_warning(png_ptr, "OS /proc read bug");
         return 0;
      }
      else if (iread > 0)
      {
         /* Continue reading until a permanent failure, or EOF */
         buffer += iread;
         nbytes -= (unsigned int)/*SAFE*/iread;
         ntotal += (unsigned int)/*SAFE*/iread;
      }
      else
         return ntotal;
   }
   return ntotal; /* nbytes == 0 */
}
