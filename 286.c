static void
IDAT_end(struct IDAT **idat_var)
{
   struct IDAT *idat = *idat_var;
   struct file *file = idat->file;
   *idat_var = NULL;
   CLEAR(*idat);
   assert(file->chunk != NULL);
   chunk_end(&file->chunk);
   /* Regardless of why the IDAT was killed set the state back to CHUNKS (it may
    * already be CHUNKS because the state isn't changed until process_IDAT
    * returns; a stop will cause IDAT_end to be entered in state CHUNKS!)
    */
   file->state = STATE_CHUNKS;
}
