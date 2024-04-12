// CVE-2006-5793
#if defined(PNG_sPLT_SUPPORTED)
void PNGAPI
png_set_sPLT(png_structp png_ptr,
             png_infop info_ptr, png_sPLT_tp entries, int nentries)
{
    png_sPLT_tp np;
    int i;

    np = (png_sPLT_tp)png_malloc_warn(png_ptr,
        (info_ptr->splt_palettes_num + nentries) * png_sizeof(png_sPLT_t));
    if (np == NULL)
    {
      png_warning(png_ptr, "No memory for sPLT palettes.");
      return;
    }

    png_memcpy(np, info_ptr->splt_palettes,
           info_ptr->splt_palettes_num * png_sizeof(png_sPLT_t));
    png_free(png_ptr, info_ptr->splt_palettes);
    info_ptr->splt_palettes=NULL;

    for (i = 0; i < nentries; i++)
    {
        png_sPLT_tp to = np + info_ptr->splt_palettes_num + i;
        png_sPLT_tp from = entries + i;

        to->name = (png_charp)png_malloc(png_ptr,
            png_strlen(from->name) + 1);
        /* TODO: use png_malloc_warn */
        png_strcpy(to->name, from->name);
        to->entries = (png_sPLT_entryp)png_malloc(png_ptr,
            from->nentries * png_sizeof(png_sPLT_t));
        /* TODO: use png_malloc_warn */
        png_memcpy(to->entries, from->entries,
            from->nentries * png_sizeof(png_sPLT_t));
        to->nentries = from->nentries;
        to->depth = from->depth;
    }

    info_ptr->splt_palettes = np;
    info_ptr->splt_palettes_num += nentries;
    info_ptr->valid |= PNG_INFO_sPLT;
#ifdef PNG_FREE_ME_SUPPORTED
    info_ptr->free_me |= PNG_FREE_SPLT;
#endif
}
#endif /* PNG_sPLT_SUPPORTED */