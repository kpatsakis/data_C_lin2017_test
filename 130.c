static unsigned int
random_mod(unsigned int max)
{
   unsigned int x;
   RANDOMIZE(x);
   return x % max; /* 0 .. max-1 */
}
