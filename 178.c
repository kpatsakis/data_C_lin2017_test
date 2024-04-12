}
BOOL PngSaveImage (PTSTR pstrFileName, png_byte *pDiData,
                   int iWidth, int iHeight, png_color bkgColor)
{
    const int           ciBitDepth = 8;
    const int           ciChannels = 3;
    static FILE        *pfFile;
    png_uint_32         ulRowBytes;
    static png_byte   **ppbRowPointers = NULL;
    int                 i;
    /* open the PNG output file */
    if (!pstrFileName)
        return FALSE;
    if (!(pfFile = fopen(pstrFileName, "wb")))
        return FALSE;
    /* prepare the standard PNG structures */
    png_ptr = png_create_write_struct(png_get_libpng_ver(NULL), NULL,
      (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        fclose(pfFile);
        return FALSE;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(pfFile);
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        return FALSE;
    }
    Try
    {
        /* initialize the png structure */
#ifdef PNG_STDIO_SUPPORTED
        png_init_io(png_ptr, pfFile);
#else
        png_set_write_fn(png_ptr, (png_voidp)pfFile, png_write_data, png_flush);
#endif
        /* we're going to write a very simple 3x8-bit RGB image */
        png_set_IHDR(png_ptr, info_ptr, iWidth, iHeight, ciBitDepth,
            PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
            PNG_FILTER_TYPE_BASE);
        /* write the file header information */
        png_write_info(png_ptr, info_ptr);
        /* swap the BGR pixels in the DiData structure to RGB */
        png_set_bgr(png_ptr);
        /* row_bytes is the width x number of channels */
        ulRowBytes = iWidth * ciChannels;
        /* we can allocate memory for an array of row-pointers */
        if ((ppbRowPointers = (png_bytepp) malloc(iHeight * sizeof(png_bytep))) == NULL)
            Throw "Visualpng: Out of memory";
        /* set the individual row-pointers to point at the correct offsets */
        for (i = 0; i < iHeight; i++)
            ppbRowPointers[i] = pDiData + i * (((ulRowBytes + 3) >> 2) << 2);
        /* write out the entire image data in one call */
        png_write_image (png_ptr, ppbRowPointers);
        /* write the additional chunks to the PNG file (not really needed) */
        png_write_end(png_ptr, info_ptr);
        /* and we're done */
        free (ppbRowPointers);
        ppbRowPointers = NULL;
        /* clean up after the write, and free any memory allocated */
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        /* yepp, done */
    }
    Catch (msg)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        if(ppbRowPointers)
            free (ppbRowPointers);
        fclose(pfFile);
        return FALSE;
    }
    fclose (pfFile);
    return TRUE;
}
