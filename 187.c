static int
read_byte(struct file *file)
{
   int ch = getc(file->file);
   if (ch >= 0 && ch <= 255)
   {
      ++(file->read_count);
      return ch;
   }
   else if (ch != EOF)
   {
      file->status_code |= INTERNAL_ERROR;
      file->read_errno = ERANGE; /* out of range character */
      /* This is very unexpected; an error message is always output: */
      emit_error(file, UNEXPECTED_ERROR_CODE, "file read");
   }
#  ifdef EINTR
      else if (errno == EINTR) /* Interrupted, try again */
      {
         errno = 0;
         return read_byte(file);
      }
#  endif
   else
   {
      /* An error, it doesn't really matter what the error is but it gets
       * recorded anyway.
       */
      if (ferror(file->file))
         file->read_errno = errno;
      else if (feof(file->file))
         file->read_errno = 0; /* I.e. a regular EOF, no error */
      else /* unexpected */
         file->read_errno = EDOM;
   }
   /* 'TRUNCATED' is used for all cases of failure to read a byte, because of
    * the way libpng works a byte read is never attempted unless the byte is
    * expected to be there, so EOF should not occur.
    */
   file->status_code |= TRUNCATED;
   return EOF;
}
