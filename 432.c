static int
one_file(struct global *global, const char *file_name, const char *out_name)
{
   int rc;
   struct control control;
   if (global->verbose)
      fprintf(stderr, "FILE %s -> %s\n", file_name,
         out_name ? out_name : "<none>");
   /* Although control_init can return a failure code the structure is always
    * initialized, so control_end can be used to accumulate any status codes.
    */
   rc = control_init(&control, global, file_name, out_name);
   if (rc == 0)
      rc = read_png(&control);
   rc |= control_end(&control);
   return rc;
}
