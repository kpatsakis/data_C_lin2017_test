static int
is_combo(int transforms)
{
   return transforms & (transforms-1); /* non-zero if more than one set bit */
}
