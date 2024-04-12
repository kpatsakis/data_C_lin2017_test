static size_t
store_read_buffer_size(png_store *ps)
{
   /* Return the bytes available for read in the current buffer. */
   if (ps->next != &ps->current->data)
      return STORE_BUFFER_SIZE;
   return ps->current->datacount;
}
