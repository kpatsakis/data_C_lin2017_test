static void
image_transform_png_set_rgb_to_gray_mod(PNG_CONST image_transform *this,
    image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
{
   if ((that->colour_type & PNG_COLOR_MASK_COLOR) != 0)
   {
      double gray, err;
      if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
         image_pixel_convert_PLTE(that);
      /* Image now has RGB channels... */
#  if DIGITIZE
      {
         PNG_CONST png_modifier *pm = display->pm;
         const unsigned int sample_depth = that->sample_depth;
         const unsigned int calc_depth = (pm->assume_16_bit_calculations ? 16 :
            sample_depth);
         const unsigned int gamma_depth = (sample_depth == 16 ? 16 :
            (pm->assume_16_bit_calculations ? PNG_MAX_GAMMA_8 : sample_depth));
         int isgray;
         double r, g, b;
         double rlo, rhi, glo, ghi, blo, bhi, graylo, grayhi;
         /* Do this using interval arithmetic, otherwise it is too difficult to
          * handle the errors correctly.
          *
          * To handle the gamma correction work out the upper and lower bounds
          * of the digitized value.  Assume rounding here - normally the values
          * will be identical after this operation if there is only one
          * transform, feel free to delete the png_error checks on this below in
          * the future (this is just me trying to ensure it works!)
          */
         r = rlo = rhi = that->redf;
         rlo -= that->rede;
         rlo = digitize(rlo, calc_depth, 1/*round*/);
         rhi += that->rede;
         rhi = digitize(rhi, calc_depth, 1/*round*/);
         g = glo = ghi = that->greenf;
         glo -= that->greene;
         glo = digitize(glo, calc_depth, 1/*round*/);
         ghi += that->greene;
         ghi = digitize(ghi, calc_depth, 1/*round*/);
         b = blo = bhi = that->bluef;
         blo -= that->bluee;
         blo = digitize(blo, calc_depth, 1/*round*/);
         bhi += that->greene;
         bhi = digitize(bhi, calc_depth, 1/*round*/);
         isgray = r==g && g==b;
         if (data.gamma != 1)
         {
            PNG_CONST double power = 1/data.gamma;
            PNG_CONST double abse = calc_depth == 16 ? .5/65535 : .5/255;
            /* 'abse' is the absolute error permitted in linear calculations. It
             * is used here to capture the error permitted in the handling
             * (undoing) of the gamma encoding.  Once again digitization occurs
             * to handle the upper and lower bounds of the values.  This is
             * where the real errors are introduced.
             */
            r = pow(r, power);
            rlo = digitize(pow(rlo, power)-abse, calc_depth, 1);
            rhi = digitize(pow(rhi, power)+abse, calc_depth, 1);
            g = pow(g, power);
            glo = digitize(pow(glo, power)-abse, calc_depth, 1);
            ghi = digitize(pow(ghi, power)+abse, calc_depth, 1);
            b = pow(b, power);
            blo = digitize(pow(blo, power)-abse, calc_depth, 1);
            bhi = digitize(pow(bhi, power)+abse, calc_depth, 1);
         }
         /* Now calculate the actual gray values.  Although the error in the
          * coefficients depends on whether they were specified on the command
          * line (in which case truncation to 15 bits happened) or not (rounding
          * was used) the maxium error in an individual coefficient is always
          * 1/32768, because even in the rounding case the requirement that
          * coefficients add up to 32768 can cause a larger rounding error.
          *
          * The only time when rounding doesn't occur in 1.5.5 and later is when
          * the non-gamma code path is used for less than 16 bit data.
          */
         gray = r * data.red_coefficient + g * data.green_coefficient +
            b * data.blue_coefficient;
         {
            PNG_CONST int do_round = data.gamma != 1 || calc_depth == 16;
            PNG_CONST double ce = 1. / 32768;
            graylo = digitize(rlo * (data.red_coefficient-ce) +
               glo * (data.green_coefficient-ce) +
               blo * (data.blue_coefficient-ce), gamma_depth, do_round);
            if (graylo <= 0)
               graylo = 0;
            grayhi = digitize(rhi * (data.red_coefficient+ce) +
               ghi * (data.green_coefficient+ce) +
               bhi * (data.blue_coefficient+ce), gamma_depth, do_round);
            if (grayhi >= 1)
               grayhi = 1;
         }
         /* And invert the gamma. */
         if (data.gamma != 1)
         {
            PNG_CONST double power = data.gamma;
            gray = pow(gray, power);
            graylo = digitize(pow(graylo, power), sample_depth, 1);
            grayhi = digitize(pow(grayhi, power), sample_depth, 1);
         }
         /* Now the error can be calculated.
          *
          * If r==g==b because there is no overall gamma correction libpng
          * currently preserves the original value.
          */
         if (isgray)
            err = (that->rede + that->greene + that->bluee)/3;
         else
         {
            err = fabs(grayhi-gray);
            if (fabs(gray - graylo) > err)
               err = fabs(graylo-gray);
            /* Check that this worked: */
            if (err > pm->limit)
            {
               size_t pos = 0;
               char buffer[128];
               pos = safecat(buffer, sizeof buffer, pos, "rgb_to_gray error ");
               pos = safecatd(buffer, sizeof buffer, pos, err, 6);
               pos = safecat(buffer, sizeof buffer, pos, " exceeds limit ");
               pos = safecatd(buffer, sizeof buffer, pos, pm->limit, 6);
               png_error(pp, buffer);
            }
         }
      }
#  else  /* DIGITIZE */
      {
         double r = that->redf;
         double re = that->rede;
         double g = that->greenf;
         double ge = that->greene;
         double b = that->bluef;
         double be = that->bluee;
         /* The true gray case involves no math. */
         if (r == g && r == b)
         {
            gray = r;
            err = re;
            if (err < ge) err = ge;
            if (err < be) err = be;
         }
         else if (data.gamma == 1)
         {
            /* There is no need to do the conversions to and from linear space,
             * so the calculation should be a lot more accurate.  There is a
             * built in 1/32768 error in the coefficients because they only have
             * 15 bits and are adjusted to make sure they add up to 32768, so
             * the result may have an additional error up to 1/32768.  (Note
             * that adding the 1/32768 here avoids needing to increase the
             * global error limits to take this into account.)
             */
            gray = r * data.red_coefficient + g * data.green_coefficient +
               b * data.blue_coefficient;
            err = re * data.red_coefficient + ge * data.green_coefficient +
               be * data.blue_coefficient + 1./32768 + gray * 5 * DBL_EPSILON;
         }
         else
         {
            /* The calculation happens in linear space, and this produces much
             * wider errors in the encoded space.  These are handled here by
             * factoring the errors in to the calculation.  There are two table
             * lookups in the calculation and each introduces a quantization
             * error defined by the table size.
             */
            PNG_CONST png_modifier *pm = display->pm;
            double in_qe = (that->sample_depth > 8 ? .5/65535 : .5/255);
            double out_qe = (that->sample_depth > 8 ? .5/65535 :
               (pm->assume_16_bit_calculations ? .5/(1<<PNG_MAX_GAMMA_8) :
               .5/255));
            double rhi, ghi, bhi, grayhi;
            double g1 = 1/data.gamma;
            rhi = r + re + in_qe; if (rhi > 1) rhi = 1;
            r -= re + in_qe; if (r < 0) r = 0;
            ghi = g + ge + in_qe; if (ghi > 1) ghi = 1;
            g -= ge + in_qe; if (g < 0) g = 0;
            bhi = b + be + in_qe; if (bhi > 1) bhi = 1;
            b -= be + in_qe; if (b < 0) b = 0;
            r = pow(r, g1)*(1-DBL_EPSILON); rhi = pow(rhi, g1)*(1+DBL_EPSILON);
            g = pow(g, g1)*(1-DBL_EPSILON); ghi = pow(ghi, g1)*(1+DBL_EPSILON);
            b = pow(b, g1)*(1-DBL_EPSILON); bhi = pow(bhi, g1)*(1+DBL_EPSILON);
            /* Work out the lower and upper bounds for the gray value in the
             * encoded space, then work out an average and error.  Remove the
             * previously added input quantization error at this point.
             */
            gray = r * data.red_coefficient + g * data.green_coefficient +
               b * data.blue_coefficient - 1./32768 - out_qe;
            if (gray <= 0)
               gray = 0;
            else
            {
               gray *= (1 - 6 * DBL_EPSILON);
               gray = pow(gray, data.gamma) * (1-DBL_EPSILON);
            }
            grayhi = rhi * data.red_coefficient + ghi * data.green_coefficient +
               bhi * data.blue_coefficient + 1./32768 + out_qe;
            grayhi *= (1 + 6 * DBL_EPSILON);
            if (grayhi >= 1)
               grayhi = 1;
            else
               grayhi = pow(grayhi, data.gamma) * (1+DBL_EPSILON);
            err = (grayhi - gray) / 2;
            gray = (grayhi + gray) / 2;
            if (err <= in_qe)
               err = gray * DBL_EPSILON;
            else
               err -= in_qe;
            /* Validate that the error is within limits (this has caused
             * problems before, it's much easier to detect them here.)
             */
            if (err > pm->limit)
            {
               size_t pos = 0;
               char buffer[128];
               pos = safecat(buffer, sizeof buffer, pos, "rgb_to_gray error ");
               pos = safecatd(buffer, sizeof buffer, pos, err, 6);
               pos = safecat(buffer, sizeof buffer, pos, " exceeds limit ");
               pos = safecatd(buffer, sizeof buffer, pos, pm->limit, 6);
               png_error(pp, buffer);
            }
         }
      }
#  endif /* !DIGITIZE */
      that->bluef = that->greenf = that->redf = gray;
      that->bluee = that->greene = that->rede = err;
      /* The sBIT is the minium of the three colour channel sBITs. */
      if (that->red_sBIT > that->green_sBIT)
         that->red_sBIT = that->green_sBIT;
      if (that->red_sBIT > that->blue_sBIT)
         that->red_sBIT = that->blue_sBIT;
      that->blue_sBIT = that->green_sBIT = that->red_sBIT;
      /* And remove the colour bit in the type: */
      if (that->colour_type == PNG_COLOR_TYPE_RGB)
         that->colour_type = PNG_COLOR_TYPE_GRAY;
      else if (that->colour_type == PNG_COLOR_TYPE_RGB_ALPHA)
         that->colour_type = PNG_COLOR_TYPE_GRAY_ALPHA;
   }
   this->next->mod(this->next, that, pp, display);
}
