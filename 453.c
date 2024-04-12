static void PNGCBAPI
store_free(png_structp ppIn, png_voidp memory)
{
   png_const_structp pp = ppIn;
   store_pool *pool = voidcast(store_pool*, png_get_mem_ptr(pp));
   store_memory *this = voidcast(store_memory*, memory), **test;
   /* Because libpng calls store_free with a dummy png_struct when deleting
    * png_struct or png_info via png_destroy_struct_2 it is necessary to check
    * the passed in png_structp to ensure it is valid, and not pass it to
    * png_error if it is not.
    */
   if (pp != pool->store->pread && pp != pool->store->pwrite)
      pp = NULL;
   /* First check that this 'memory' really is valid memory - it must be in the
    * pool list.  If it is, use the shared memory_free function to free it.
    */
   --this;
   for (test = &pool->list; *test != this; test = &(*test)->next)
   {
      if (*test == NULL)
      {
         store_pool_error(pool->store, pp, "bad pointer to free");
         return;
      }
   }
   /* Unlink this entry, *test == this. */
   *test = this->next;
   this->next = NULL;
   store_memory_free(pp, pool, this);
}
