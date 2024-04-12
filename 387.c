static void
modifier_read_imp(png_modifier *pm, png_bytep pb, png_size_t st)
{
   while (st > 0)
   {
      size_t cb;
      png_uint_32 len, chunk;
      png_modification *mod;
      if (pm->buffer_position >= pm->buffer_count) switch (pm->state)
      {
         static png_byte sign[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
         case modifier_start:
            store_read_imp(&pm->this, pm->buffer, 8); /* size of signature. */
            pm->buffer_count = 8;
            pm->buffer_position = 0;
            if (memcmp(pm->buffer, sign, 8) != 0)
               png_error(pm->this.pread, "invalid PNG file signature");
            pm->state = modifier_signature;
            break;
         case modifier_signature:
            store_read_imp(&pm->this, pm->buffer, 13+12); /* size of IHDR */
            pm->buffer_count = 13+12;
            pm->buffer_position = 0;
            if (png_get_uint_32(pm->buffer) != 13 ||
                png_get_uint_32(pm->buffer+4) != CHUNK_IHDR)
               png_error(pm->this.pread, "invalid IHDR");
            /* Check the list of modifiers for modifications to the IHDR. */
            mod = pm->modifications;
            while (mod != NULL)
            {
               if (mod->chunk == CHUNK_IHDR && mod->modify_fn &&
                   (*mod->modify_fn)(pm, mod, 0))
                  {
                  mod->modified = 1;
                  modifier_setbuffer(pm);
                  }
               /* Ignore removal or add if IHDR! */
               mod = mod->next;
            }
            /* Cache information from the IHDR (the modified one.) */
            pm->bit_depth = pm->buffer[8+8];
            pm->colour_type = pm->buffer[8+8+1];
            pm->state = modifier_IHDR;
            pm->flush = 0;
            break;
         case modifier_IHDR:
         default:
            /* Read a new chunk and process it until we see PLTE, IDAT or
             * IEND.  'flush' indicates that there is still some data to
             * output from the preceding chunk.
             */
            if ((cb = pm->flush) > 0)
            {
               if (cb > st) cb = st;
               pm->flush -= cb;
               store_read_imp(&pm->this, pb, cb);
               pb += cb;
               st -= cb;
               if (st == 0) return;
            }
            /* No more bytes to flush, read a header, or handle a pending
             * chunk.
             */
            if (pm->pending_chunk != 0)
            {
               png_save_uint_32(pm->buffer, pm->pending_len);
               png_save_uint_32(pm->buffer+4, pm->pending_chunk);
               pm->pending_len = 0;
               pm->pending_chunk = 0;
            }
            else
               store_read_imp(&pm->this, pm->buffer, 8);
            pm->buffer_count = 8;
            pm->buffer_position = 0;
            /* Check for something to modify or a terminator chunk. */
            len = png_get_uint_32(pm->buffer);
            chunk = png_get_uint_32(pm->buffer+4);
            /* Terminators first, they may have to be delayed for added
             * chunks
             */
            if (chunk == CHUNK_PLTE || chunk == CHUNK_IDAT ||
                chunk == CHUNK_IEND)
            {
               mod = pm->modifications;
               while (mod != NULL)
               {
                  if ((mod->add == chunk ||
                      (mod->add == CHUNK_PLTE && chunk == CHUNK_IDAT)) &&
                      mod->modify_fn != NULL && !mod->modified && !mod->added)
                  {
                     /* Regardless of what the modify function does do not run
                      * this again.
                      */
                     mod->added = 1;
                     if ((*mod->modify_fn)(pm, mod, 1 /*add*/))
                     {
                        /* Reset the CRC on a new chunk */
                        if (pm->buffer_count > 0)
                           modifier_setbuffer(pm);
                        else
                           {
                           pm->buffer_position = 0;
                           mod->removed = 1;
                           }
                        /* The buffer has been filled with something (we assume)
                         * so output this.  Pend the current chunk.
                         */
                        pm->pending_len = len;
                        pm->pending_chunk = chunk;
                        break; /* out of while */
                     }
                  }
                  mod = mod->next;
               }
               /* Don't do any further processing if the buffer was modified -
                * otherwise the code will end up modifying a chunk that was
                * just added.
                */
               if (mod != NULL)
                  break; /* out of switch */
            }
            /* If we get to here then this chunk may need to be modified.  To
             * do this it must be less than 1024 bytes in total size, otherwise
             * it just gets flushed.
             */
            if (len+12 <= sizeof pm->buffer)
            {
               store_read_imp(&pm->this, pm->buffer+pm->buffer_count,
                   len+12-pm->buffer_count);
               pm->buffer_count = len+12;
               /* Check for a modification, else leave it be. */
               mod = pm->modifications;
               while (mod != NULL)
               {
                  if (mod->chunk == chunk)
                  {
                     if (mod->modify_fn == NULL)
                     {
                        /* Remove this chunk */
                        pm->buffer_count = pm->buffer_position = 0;
                        mod->removed = 1;
                        break; /* Terminate the while loop */
                     }
                     else if ((*mod->modify_fn)(pm, mod, 0))
                     {
                        mod->modified = 1;
                        /* The chunk may have been removed: */
                        if (pm->buffer_count == 0)
                        {
                           pm->buffer_position = 0;
                           break;
                        }
                        modifier_setbuffer(pm);
                     }
                  }
                  mod = mod->next;
               }
            }
            else
               pm->flush = len+12 - pm->buffer_count; /* data + crc */
            /* Take the data from the buffer (if there is any). */
            break;
      }
      /* Here to read from the modifier buffer (not directly from
       * the store, as in the flush case above.)
       */
      cb = pm->buffer_count - pm->buffer_position;
      if (cb > st)
         cb = st;
      memcpy(pb, pm->buffer + pm->buffer_position, cb);
      st -= cb;
      pb += cb;
      pm->buffer_position += cb;
   }
}
