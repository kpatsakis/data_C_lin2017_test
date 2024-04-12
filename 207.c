}
static int mytime(struct timespec *t)
{
   /* Do the timing using clock_gettime and the per-process timer. */
   if (!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t))
      return 1;
   perror("CLOCK_PROCESS_CPUTIME_ID");
   fprintf(stderr, "timepng: could not get the time\n");
   return 0;
}
