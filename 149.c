 */
BOOL FillBitmap (
        BYTE *pDiData, int cxWinSize, int cyWinSize,
        BYTE *pbImage, int cxImgSize, int cyImgSize, int cImgChannels,
        BOOL bStretched)
{
    BYTE *pStretchedImage;
    BYTE *pImg;
    BYTE *src, *dst;
    BYTE r, g, b, a;
    const int cDIChannels = 3;
    WORD wImgRowBytes;
    WORD wDIRowBytes;
    int cxNewSize, cyNewSize;
    int cxImgPos, cyImgPos;
    int xImg, yImg;
    int xWin, yWin;
    int xOld, yOld;
    int xNew, yNew;
    if (bStretched)
    {
        cxNewSize = cxWinSize - 2 * MARGIN;
        cyNewSize = cyWinSize - 2 * MARGIN;
        /* stretch the image to it's window determined size */
        /* the following two are mathematically the same, but the first
         * has side-effects because of rounding
         */
/*      if ((cyNewSize / cxNewSize) > (cyImgSize / cxImgSize)) */
        if ((cyNewSize * cxImgSize) > (cyImgSize * cxNewSize))
        {
            cyNewSize = cxNewSize * cyImgSize / cxImgSize;
            cxImgPos = MARGIN;
            cyImgPos = (cyWinSize - cyNewSize) / 2;
        }
        else
        {
            cxNewSize = cyNewSize * cxImgSize / cyImgSize;
            cyImgPos = MARGIN;
            cxImgPos = (cxWinSize - cxNewSize) / 2;
        }
        pStretchedImage = malloc (cImgChannels * cxNewSize * cyNewSize);
        pImg = pStretchedImage;
        for (yNew = 0; yNew < cyNewSize; yNew++)
        {
            yOld = yNew * cyImgSize / cyNewSize;
            for (xNew = 0; xNew < cxNewSize; xNew++)
            {
                xOld = xNew * cxImgSize / cxNewSize;
                r = *(pbImage + cImgChannels * ((yOld * cxImgSize) + xOld) + 0);
                g = *(pbImage + cImgChannels * ((yOld * cxImgSize) + xOld) + 1);
                b = *(pbImage + cImgChannels * ((yOld * cxImgSize) + xOld) + 2);
                *pImg++ = r;
                *pImg++ = g;
                *pImg++ = b;
                if (cImgChannels == 4)
                {
                    a = *(pbImage + cImgChannels * ((yOld * cxImgSize) + xOld)
                        + 3);
                    *pImg++ = a;
                }
            }
        }
        /* calculate row-bytes */
        wImgRowBytes = cImgChannels * cxNewSize;
        wDIRowBytes = (WORD) ((cDIChannels * cxWinSize + 3L) >> 2) << 2;
        /* copy image to screen */
        for (yImg = 0, yWin = cyImgPos; yImg < cyNewSize; yImg++, yWin++)
        {
            if (yWin >= cyWinSize - cyImgPos)
                break;
            src = pStretchedImage + yImg * wImgRowBytes;
            dst = pDiData + yWin * wDIRowBytes + cxImgPos * cDIChannels;
            for (xImg = 0, xWin = cxImgPos; xImg < cxNewSize; xImg++, xWin++)
            {
                if (xWin >= cxWinSize - cxImgPos)
                    break;
                r = *src++;
                g = *src++;
                b = *src++;
                *dst++ = b; /* note the reverse order  */
                *dst++ = g;
                *dst++ = r;
                if (cImgChannels == 4)
                {
                    a = *src++;
                }
            }
        }
        /* free memory */
        if (pStretchedImage != NULL)
        {
            free (pStretchedImage);
            pStretchedImage = NULL;
        }
    }
    /* process the image not-stretched */
    else
    {
        /* calculate the central position */
        cxImgPos = (cxWinSize - cxImgSize) / 2;
        cyImgPos = (cyWinSize - cyImgSize) / 2;
        /* check for image larger than window */
        if (cxImgPos < MARGIN)
            cxImgPos = MARGIN;
        if (cyImgPos < MARGIN)
            cyImgPos = MARGIN;
        /* calculate both row-bytes */
        wImgRowBytes = cImgChannels * cxImgSize;
        wDIRowBytes = (WORD) ((cDIChannels * cxWinSize + 3L) >> 2) << 2;
        /* copy image to screen */
        for (yImg = 0, yWin = cyImgPos; yImg < cyImgSize; yImg++, yWin++)
        {
            if (yWin >= cyWinSize - MARGIN)
                break;
            src = pbImage + yImg * wImgRowBytes;
            dst = pDiData + yWin * wDIRowBytes + cxImgPos * cDIChannels;
            for (xImg = 0, xWin = cxImgPos; xImg < cxImgSize; xImg++, xWin++)
            {
                if (xWin >= cxWinSize - MARGIN)
                    break;
                r = *src++;
                g = *src++;
                b = *src++;
                *dst++ = b; /* note the reverse order  */
                *dst++ = g;
                *dst++ = r;
                if (cImgChannels == 4)
                {
                    a = *src++;
                }
            }
        }
    }
    return TRUE;
}
