static void
clear_text(png_text *text, png_charp keyword)
{
   text->compression = -1; /* none */
   text->key = keyword;
   text->text = NULL;
   text->text_length = 0; /* libpng calculates this */
   text->itxt_length = 0; /* libpng calculates this */
   text->lang = NULL;
   text->lang_key = NULL;
}
