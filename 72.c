static void
global_init(struct global *global)
   /* Call this once (and only once) to initialize the control */
{
   CLEAR(*global);
   /* Globals */
   global->errors        = 0;
   global->warnings      = 0;
   global->quiet         = 0;
   global->verbose       = 0;
   global->idat_max      = 0;         /* no re-chunking of IDAT */
   global->optimize_zlib = 0;
   global->skip          = SKIP_NONE;
   global->status_code   = 0;
   IDAT_list_init(&global->idat_cache);
}
