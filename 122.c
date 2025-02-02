static void
standard_text_validate(standard_display *dp, png_const_structp pp,
   png_infop pi, int check_end)
{
   png_textp tp = NULL;
   png_uint_32 num_text = png_get_text(pp, pi, &tp, NULL);
   if (num_text == 2 && tp != NULL)
   {
      standard_check_text(pp, tp, "image name", dp->ps->current->name);
      /* This exists because prior to 1.5.18 the progressive reader left the
       * png_struct z_stream unreset at the end of the image, so subsequent
       * attempts to use it simply returns Z_STREAM_END.
       */
      if (check_end)
         standard_check_text(pp, tp+1, "end marker", "end");
   }
   else
   {
      char msg[64];
      sprintf(msg, "expected two text items, got %lu",
         (unsigned long)num_text);
      png_error(pp, msg);
   }
}
