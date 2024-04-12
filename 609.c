static void
store_write_reset(png_store *ps)
{
   if (ps->pwrite != NULL)
   {
      anon_context(ps);
      Try
         png_destroy_write_struct(&ps->pwrite, &ps->piwrite);
      Catch_anonymous
      {
         /* memory corruption: continue. */
      }
      ps->pwrite = NULL;
      ps->piwrite = NULL;
   }
   /* And make sure that all the memory has been freed - this will output
    * spurious errors in the case of memory corruption above, but this is safe.
    */
#  ifdef PNG_USER_MEM_SUPPORTED
      store_pool_delete(ps, &ps->write_memory_pool);
#  endif
   store_freenew(ps);
}
