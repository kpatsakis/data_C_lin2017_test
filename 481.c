static png_voidp PNGCBAPI
store_malloc(png_structp ppIn, png_alloc_size_t cb)
{
   png_const_structp pp = ppIn;
   store_pool *pool = voidcast(store_pool*, png_get_mem_ptr(pp));
   store_memory *new = voidcast(store_memory*, malloc(cb + (sizeof *new) +
      (sizeof pool->mark)));
   if (new != NULL)
   {
      if (cb > pool->max)
         pool->max = cb;
      pool->current += cb;
      if (pool->current > pool->limit)
         pool->limit = pool->current;
      pool->total += cb;
      new->size = cb;
      memcpy(new->mark, pool->mark, sizeof new->mark);
      memcpy((png_byte*)(new+1) + cb, pool->mark, sizeof pool->mark);
      new->pool = pool;
      new->next = pool->list;
      pool->list = new;
      ++new;
   }
   else
   {
      /* NOTE: the PNG user malloc function cannot use the png_ptr it is passed
       * other than to retrieve the allocation pointer!  libpng calls the
       * store_malloc callback in two basic cases:
       *
       * 1) From png_malloc; png_malloc will do a png_error itself if NULL is
       *    returned.
       * 2) From png_struct or png_info structure creation; png_malloc is
       *    to return so cleanup can be performed.
       *
       * To handle this store_malloc can log a message, but can't do anything
       * else.
       */
      store_log(pool->store, pp, "out of memory", 1 /* is_error */);
   }
   return new;
}
