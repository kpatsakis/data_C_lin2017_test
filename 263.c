static void
insert_zTXt(png_structp png_ptr, png_infop info_ptr, int nparams,
   png_charpp params)
{
   png_text text;
   check_param_count(nparams, 2);
   clear_text(&text, params[0]);
   text.compression = 0; /* deflate */
   set_text(png_ptr, info_ptr, &text, params[1]);
}
