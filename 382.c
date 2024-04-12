static void
standard_palette_init(standard_display *dp)
{
   store_palette_entry *palette = store_current_palette(dp->ps, &dp->npalette);
   /* The remaining entries remain white/opaque. */
   if (dp->npalette > 0)
   {
      int i = dp->npalette;
      memcpy(dp->palette, palette, i * sizeof *palette);
      /* Check for a non-opaque palette entry: */
      while (--i >= 0)
         if (palette[i].alpha < 255)
            break;
#     ifdef __GNUC__
         /* GCC can't handle the more obviously optimizable version. */
         if (i >= 0)
            dp->is_transparent = 1;
         else
            dp->is_transparent = 0;
#     else
         dp->is_transparent = (i >= 0);
#     endif
   }
}
