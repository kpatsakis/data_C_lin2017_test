}
static int rpng2_win_create_window()
{
    uch bg_red   = rpng2_info.bg_red;
    uch bg_green = rpng2_info.bg_green;
    uch bg_blue  = rpng2_info.bg_blue;
    uch *dest;
    int extra_width, extra_height;
    ulg i, j;
    WNDCLASSEX wndclass;
    RECT rect;
/*---------------------------------------------------------------------------
    Allocate memory for the display-specific version of the image (round up
    to multiple of 4 for Windows DIB).
  ---------------------------------------------------------------------------*/
    wimage_rowbytes = ((3*rpng2_info.width + 3L) >> 2) << 2;
    if (!(dib = (uch *)malloc(sizeof(BITMAPINFOHEADER) +
                              wimage_rowbytes*rpng2_info.height)))
    {
        return 4;   /* fail */
    }
/*---------------------------------------------------------------------------
    Initialize the DIB.  Negative height means to use top-down BMP ordering
    (must be uncompressed, but that's what we want).  Bit count of 1, 4 or 8
    implies a colormap of RGBX quads, but 24-bit BMPs just use B,G,R values
    directly => wimage_data begins immediately after BMP header.
  ---------------------------------------------------------------------------*/
    memset(dib, 0, sizeof(BITMAPINFOHEADER));
    bmih = (BITMAPINFOHEADER *)dib;
    bmih->biSize = sizeof(BITMAPINFOHEADER);
    bmih->biWidth = rpng2_info.width;
    bmih->biHeight = -((long)rpng2_info.height);
    bmih->biPlanes = 1;
    bmih->biBitCount = 24;
    bmih->biCompression = 0;
    wimage_data = dib + sizeof(BITMAPINFOHEADER);
/*---------------------------------------------------------------------------
    Fill window with the specified background color (default is black), but
    defer loading faked "background image" until window is displayed (may be
    slow to compute).  Data are in BGR order.
  ---------------------------------------------------------------------------*/
    if (bg_image) {   /* just fill with black for now */
        memset(wimage_data, 0, wimage_rowbytes*rpng2_info.height);
    } else {
        for (j = 0;  j < rpng2_info.height;  ++j) {
            dest = wimage_data + j*wimage_rowbytes;
            for (i = rpng2_info.width;  i > 0;  --i) {
                *dest++ = bg_blue;
                *dest++ = bg_green;
                *dest++ = bg_red;
            }
        }
    }
/*---------------------------------------------------------------------------
    Set the window parameters.
  ---------------------------------------------------------------------------*/
    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = rpng2_win_wndproc;
    wndclass.hInstance = global_hInst;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = progname;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wndclass);
/*---------------------------------------------------------------------------
    Finally, create the window.
  ---------------------------------------------------------------------------*/
    extra_width  = 2*(GetSystemMetrics(SM_CXBORDER) +
                      GetSystemMetrics(SM_CXDLGFRAME));
    extra_height = 2*(GetSystemMetrics(SM_CYBORDER) +
                      GetSystemMetrics(SM_CYDLGFRAME)) +
                      GetSystemMetrics(SM_CYCAPTION);
    global_hwnd = CreateWindow(progname, titlebar, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rpng2_info.width+extra_width,
      rpng2_info.height+extra_height, NULL, NULL, global_hInst, NULL);
    ShowWindow(global_hwnd, global_showmode);
    UpdateWindow(global_hwnd);
/*---------------------------------------------------------------------------
    Now compute the background image and display it.  If it fails (memory
    allocation), revert to a plain background color.
  ---------------------------------------------------------------------------*/
    if (bg_image) {
        static const char *msg = "Computing background image...";
        int x, y, len = strlen(msg);
        HDC hdc = GetDC(global_hwnd);
        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);
        x = (rpng2_info.width - len*tm.tmAveCharWidth)/2;
        y = (rpng2_info.height - tm.tmHeight)/2;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        /* this can still begin out of bounds even if x is positive (???): */
        TextOut(hdc, ((x < 0)? 0 : x), ((y < 0)? 0 : y), msg, len);
        ReleaseDC(global_hwnd, hdc);
        rpng2_win_load_bg_image();   /* resets bg_image if fails */
    }
    if (!bg_image) {
        for (j = 0;  j < rpng2_info.height;  ++j) {
            dest = wimage_data + j*wimage_rowbytes;
            for (i = rpng2_info.width;  i > 0;  --i) {
                *dest++ = bg_blue;
                *dest++ = bg_green;
                *dest++ = bg_red;
            }
        }
    }
    rect.left = 0L;
    rect.top = 0L;
    rect.right = (LONG)rpng2_info.width;       /* possibly off by one? */
    rect.bottom = (LONG)rpng2_info.height;     /* possibly off by one? */
    InvalidateRect(global_hwnd, &rect, FALSE);
    UpdateWindow(global_hwnd);                 /* similar to XFlush() */
    return 0;
} /* end function rpng2_win_create_window() */
