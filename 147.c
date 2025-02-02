/* Windows open-file functions */
void PngFileInitialize (HWND hwnd)
{
    static TCHAR szFilter[] = TEXT ("PNG Files (*.PNG)\0*.png\0")
        TEXT ("All Files (*.*)\0*.*\0\0");
    ofn.lStructSize       = sizeof (OPENFILENAME);
    ofn.hwndOwner         = hwnd;
    ofn.hInstance         = NULL;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = NULL;          /* Set in Open and Close functions */
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrFileTitle    = NULL;          /* Set in Open and Close functions */
    ofn.nMaxFileTitle     = MAX_PATH;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = NULL;
    ofn.Flags             = 0;             /* Set in Open and Close functions */
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT ("png");
    ofn.lCustData         = 0;
    ofn.lpfnHook          = NULL;
    ofn.lpTemplateName    = NULL;
}
