 */
BOOL InitBitmap (BYTE *pDiData, int cxWinSize, int cyWinSize)
{
    BYTE *dst;
    int x, y, col;
    /* initialize the background with gray */
    dst = pDiData;
    for (y = 0; y < cyWinSize; y++)
    {
        col = 0;
        for (x = 0; x < cxWinSize; x++)
        {
            /* fill with GRAY */
            *dst++ = 127;
            *dst++ = 127;
            *dst++ = 127;
            col += 3;
        }
        /* rows start on 4 byte boundaries */
        while ((col % 4) != 0)
        {
            dst++;
            col++;
        }
    }
    return TRUE;
}
