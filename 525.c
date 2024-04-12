}
LRESULT CALLBACK rpng2_win_wndproc(HWND hwnd, UINT iMsg, WPARAM wP, LPARAM lP)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    int rc;
    switch (iMsg) {
        case WM_CREATE:
            /* one-time processing here, if any */
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            rc = StretchDIBits(hdc, 0, 0, rpng2_info.width, rpng2_info.height,
                                    0, 0, rpng2_info.width, rpng2_info.height,
                                    wimage_data, (BITMAPINFO *)bmih,
                                    0, SRCCOPY);
            EndPaint(hwnd, &ps);
            return 0;
        /* wait for the user to tell us when to quit */
        case WM_CHAR:
            switch (wP) {       /* only need one, so ignore repeat count */
                case 'q':
                case 'Q':
                case 0x1B:      /* Esc key */
                    PostQuitMessage(0);
            }
            return 0;
        case WM_LBUTTONDOWN:    /* another way of quitting */
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, iMsg, wP, lP);
}
