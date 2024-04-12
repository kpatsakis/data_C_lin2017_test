static void
file_getpos(struct file *file, fpos_t *pos)
{
   if (fgetpos(file->file, pos))
   {
      /* This is unexpected, so perror it */
      perror(file->file_name);
      stop(file, READ_ERROR_CODE, "fgetpos");
   }
}
