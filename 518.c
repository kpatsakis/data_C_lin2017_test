} /* end function rpng2_x_reload_bg_image() */
static int is_number(char *p)
{
    while (*p) {
        if (!isdigit(*p))
            return FALSE;
        ++p;
    }
    return TRUE;
}
