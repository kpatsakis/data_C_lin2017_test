}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam,
        LPARAM lParam)
{
    static HINSTANCE          hInstance ;
    static HDC                hdc;
    static PAINTSTRUCT        ps;
    static HMENU              hMenu;
    static BITMAPFILEHEADER  *pbmfh;
    static BITMAPINFOHEADER  *pbmih;
    static BYTE              *pbImage;
    static int                cxWinSize, cyWinSize;
    static int                cxImgSize, cyImgSize;
    static int                cImgChannels;
    static png_color          bkgColor = {127, 127, 127};
    static BOOL               bStretched = TRUE;
    static BYTE              *pDib = NULL;
    static BYTE              *pDiData = NULL;
    static TCHAR              szImgPathName [MAX_PATH];
    static TCHAR              szTitleName [MAX_PATH];
    static TCHAR             *pPngFileList = NULL;
    static int                iPngFileCount;
    static int                iPngFileIndex;
    BOOL                      bOk;
    switch (message)
    {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
        PngFileInitialize (hwnd);
        strcpy (szImgPathName, "");
        /* in case we process file given on command-line */
        if (szCmdFileName[0] != '\0')
        {
            strcpy (szImgPathName, szCmdFileName);
            /* read the other png-files in the directory for later */
            /* next/previous commands */
            BuildPngList (szImgPathName, &pPngFileList, &iPngFileCount,
                          &iPngFileIndex);
            /* load the image from file */
            if (!LoadImageFile (hwnd, szImgPathName,
                &pbImage, &cxImgSize, &cyImgSize, &cImgChannels, &bkgColor))
                return 0;
            /* invalidate the client area for later update */
            InvalidateRect (hwnd, NULL, TRUE);
            /* display the PNG into the DIBitmap */
            DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
                pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
        }
        return 0;
    case WM_SIZE:
        cxWinSize = LOWORD (lParam);
        cyWinSize = HIWORD (lParam);
        /* invalidate the client area for later update */
        InvalidateRect (hwnd, NULL, TRUE);
        /* display the PNG into the DIBitmap */
        DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
            pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
        return 0;
    case WM_INITMENUPOPUP:
        hMenu = GetMenu (hwnd);
        if (pbImage)
            EnableMenuItem (hMenu, IDM_FILE_SAVE, MF_ENABLED);
        else
            EnableMenuItem (hMenu, IDM_FILE_SAVE, MF_GRAYED);
        return 0;
    case WM_COMMAND:
        hMenu = GetMenu (hwnd);
        switch (LOWORD (wParam))
        {
        case IDM_FILE_OPEN:
            /* show the File Open dialog box */
            if (!PngFileOpenDlg (hwnd, szImgPathName, szTitleName))
                return 0;
            /* read the other png-files in the directory for later */
            /* next/previous commands */
            BuildPngList (szImgPathName, &pPngFileList, &iPngFileCount,
                          &iPngFileIndex);
            /* load the image from file */
            if (!LoadImageFile (hwnd, szImgPathName,
                &pbImage, &cxImgSize, &cyImgSize, &cImgChannels, &bkgColor))
                return 0;
            /* invalidate the client area for later update */
            InvalidateRect (hwnd, NULL, TRUE);
            /* display the PNG into the DIBitmap */
            DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
                pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
            return 0;
        case IDM_FILE_SAVE:
            /* show the File Save dialog box */
            if (!PngFileSaveDlg (hwnd, szImgPathName, szTitleName))
                return 0;
            /* save the PNG to a disk file */
            SetCursor (LoadCursor (NULL, IDC_WAIT));
            ShowCursor (TRUE);
            bOk = PngSaveImage (szImgPathName, pDiData, cxWinSize, cyWinSize,
                  bkgColor);
            ShowCursor (FALSE);
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            if (!bOk)
                MessageBox (hwnd, TEXT ("Error in saving the PNG image"),
                szProgName, MB_ICONEXCLAMATION | MB_OK);
            return 0;
        case IDM_FILE_NEXT:
            /* read next entry in the directory */
            if (SearchPngList (pPngFileList, iPngFileCount, &iPngFileIndex,
                NULL, szImgPathName))
            {
                if (strcmp (szImgPathName, "") == 0)
                    return 0;
                /* load the image from file */
                if (!LoadImageFile (hwnd, szImgPathName, &pbImage,
                        &cxImgSize, &cyImgSize, &cImgChannels, &bkgColor))
                    return 0;
                /* invalidate the client area for later update */
                InvalidateRect (hwnd, NULL, TRUE);
                /* display the PNG into the DIBitmap */
                DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
                    pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
            }
            return 0;
        case IDM_FILE_PREVIOUS:
            /* read previous entry in the directory */
            if (SearchPngList (pPngFileList, iPngFileCount, &iPngFileIndex,
                szImgPathName, NULL))
            {
                if (strcmp (szImgPathName, "") == 0)
                    return 0;
                /* load the image from file */
                if (!LoadImageFile (hwnd, szImgPathName, &pbImage, &cxImgSize,
                    &cyImgSize, &cImgChannels, &bkgColor))
                    return 0;
                /* invalidate the client area for later update */
                InvalidateRect (hwnd, NULL, TRUE);
                /* display the PNG into the DIBitmap */
                DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
                    pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
            }
            return 0;
        case IDM_FILE_EXIT:
            /* more cleanup needed... */
            /* free image buffer */
            if (pDib != NULL)
            {
                free (pDib);
                pDib = NULL;
            }
            /* free file-list */
            if (pPngFileList != NULL)
            {
                free (pPngFileList);
                pPngFileList = NULL;
            }
            /* let's go ... */
            exit (0);
            return 0;
        case IDM_OPTIONS_STRETCH:
            bStretched = !bStretched;
            if (bStretched)
                CheckMenuItem (hMenu, IDM_OPTIONS_STRETCH, MF_CHECKED);
            else
                CheckMenuItem (hMenu, IDM_OPTIONS_STRETCH, MF_UNCHECKED);
            /* invalidate the client area for later update */
            InvalidateRect (hwnd, NULL, TRUE);
            /* display the PNG into the DIBitmap */
            DisplayImage (hwnd, &pDib, &pDiData, cxWinSize, cyWinSize,
                pbImage, cxImgSize, cyImgSize, cImgChannels, bStretched);
            return 0;
        case IDM_HELP_ABOUT:
            DialogBox (hInstance, TEXT ("AboutBox"), hwnd, AboutDlgProc) ;
            return 0;
        } /* end switch */
        break;
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        if (pDib)
            SetDIBitsToDevice (hdc, 0, 0, cxWinSize, cyWinSize, 0, 0,
                0, cyWinSize, pDiData, (BITMAPINFO *) pDib, DIB_RGB_COLORS);
        EndPaint (hwnd, &ps);
        return 0;
    case WM_DESTROY:
        if (pbmfh)
        {
            free (pbmfh);
            pbmfh = NULL;
        }
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}
