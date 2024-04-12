}
static int rpng_win_create_window(HINSTANCE hInst, int showmode)
{
    uch *dest;
    int extra_width, extra_height;
    ulg i, j;
    WNDCLASSEX wndclass;
/*---------------------------------------------------------------------------
    Allocate memory for the display-specific version of the image (round up
    to multiple of 4 for Windows DIB).
  ---------------------------------------------------------------------------*/
    wimage_rowbytes = ((3*image_width + 3L) >> 2) << 2;
    if (!(dib = (uch *)malloc(sizeof(BITMAPINFOHEADER) +
                              wimage_rowbytes*image_height)))
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
    bmih->biWidth = image_width;
    bmih->biHeight = -((long)image_height);
    bmih->biPlanes = 1;
    bmih->biBitCount = 24;
    bmih->biCompression = 0;
    wimage_data = dib + sizeof(BITMAPINFOHEADER);
/*---------------------------------------------------------------------------
    Fill in background color (black by default); data are in BGR order.
  ---------------------------------------------------------------------------*/
    for (j = 0;  j < image_height;  ++j) {
        dest = wimage_data + j*wimage_rowbytes;
        for (i = image_width;  i > 0;  --i) {
            *dest++ = bg_blue;
            *dest++ = bg_green;
            *dest++ = bg_red;
        }
    }
/*---------------------------------------------------------------------------
    Set the window parameters.
  ---------------------------------------------------------------------------*/
    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = rpng_win_wndproc;
    wndclass.hInstance = hInst;
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
      CW_USEDEFAULT, CW_USEDEFAULT, image_width+extra_width,
      image_height+extra_height, NULL, NULL, hInst, NULL);
    ShowWindow(global_hwnd, showmode);
    UpdateWindow(global_hwnd);
    return 0;
} /* end function rpng_win_create_window() */
