static unsigned int
sample_scale(double sample_value, unsigned int scale)
{
   sample_value = floor(sample_value * scale + .5);
   /* Return NaN as 0: */
   if (!(sample_value > 0))
      sample_value = 0;
   else if (sample_value > scale)
      sample_value = scale;
   return (unsigned int)sample_value;
}
