static int
png_pass_row_shift(int pass)
{
   int x, y, base=(-1), inc=8;
   ++pass;
   for (y=0; y<8; ++y) for (x=0; x<8; ++x) if (adam7[y][x] == pass)
   {
      if (base == (-1))
         base = y;
      else if (base == y)
         {}
      else if (inc == y-base)
         base=y;
      else if (inc == 8)
         inc = y-base, base=y;
      else if (inc != y-base)
         return 0xff; /* error - more than one 'inc' value! */
   }
   if (base == (-1)) return 0xfe; /* error - no row in pass! */
   /* The shift is always 1, 2 or 3 - no pass has all the rows! */
   switch (inc)
   {
case 2: return 1;
case 4: return 2;
case 8: return 3;
default: break;
   }
   /* error - unrecognized 'inc' */
   return (inc << 8) + 0xfd;
}
