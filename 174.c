static int
file_end(struct file *file)
{
   int rc;
   /* If either of the chunk pointers are set end them here, the IDAT structure
    * must be deallocated first as it may deallocate the chunk structure.
    */
   if (file->idat != NULL)
      IDAT_end(&file->idat);
   if (file->chunk != NULL)
      chunk_end(&file->chunk);
   rc = file->status_code;
   if (file->file != NULL)
      (void)fclose(file->file);
   if (file->out != NULL)
   {
      /* NOTE: this is bitwise |, all the following functions must execute and
       * must succeed.
       */
      if (ferror(file->out) | fflush(file->out) | fclose(file->out))
      {
         perror(file->out_name);
         emit_error(file, READ_ERROR_CODE, "output write error");
         rc |= WRITE_ERROR;
      }
   }
   /* Accumulate the result codes */
   file->global->status_code |= rc;
   CLEAR(*file);
   return rc; /* status code: non-zero on read or write error */
}
