static int global_showmode;
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PSTR cmd, int showmode)
{
    char *args[1024];                 /* arbitrary limit, but should suffice */
    char **argv = args;
    char *p, *q, *bgstr = NULL;
    int argc = 0;
    int rc, alen, flen;
    int error = 0;
    int timing = FALSE;
    int have_bg = FALSE;
    double LUT_exponent;              /* just the lookup table */
    double CRT_exponent = 2.2;        /* just the monitor */
    double default_display_exponent;  /* whole display system */
    MSG msg;
    /* First initialize a few things, just to be sure--memset takes care of
     * default background color (black), booleans (FALSE), pointers (NULL),
     * etc. */
    global_hInst = hInst;
    global_showmode = showmode;
    filename = (char *)NULL;
    memset(&rpng2_info, 0, sizeof(mainprog_info));
#ifndef __CYGWIN__
    /* Next reenable console output, which normally goes to the bit bucket
     * for windowed apps.  Closing the console window will terminate the
     * app.  Thanks to David.Geldreich@realviz.com for supplying the magical
     * incantation. */
    AllocConsole();
    freopen("CONOUT$", "a", stderr);
    freopen("CONOUT$", "a", stdout);
#endif
    /* Set the default value for our display-system exponent, i.e., the
     * product of the CRT exponent and the exponent corresponding to
     * the frame-buffer's lookup table (LUT), if any.  This is not an
     * exhaustive list of LUT values (e.g., OpenStep has a lot of weird
     * ones), but it should cover 99% of the current possibilities.  And
     * yes, these ifdefs are completely wasted in a Windows program... */
#if defined(NeXT)
    /* third-party utilities can modify the default LUT exponent */
    LUT_exponent = 1.0 / 2.2;
    /*
    if (some_next_function_that_returns_gamma(&next_gamma))
        LUT_exponent = 1.0 / next_gamma;
     */
#elif defined(sgi)
    LUT_exponent = 1.0 / 1.7;
    /* there doesn't seem to be any documented function to
     * get the "gamma" value, so we do it the hard way */
    infile = fopen("/etc/config/system.glGammaVal", "r");
    if (infile) {
        double sgi_gamma;
        fgets(tmpline, 80, infile);
        fclose(infile);
        sgi_gamma = atof(tmpline);
        if (sgi_gamma > 0.0)
            LUT_exponent = 1.0 / sgi_gamma;
    }
#elif defined(Macintosh)
    LUT_exponent = 1.8 / 2.61;
    /*
    if (some_mac_function_that_returns_gamma(&mac_gamma))
        LUT_exponent = mac_gamma / 2.61;
     */
#else
    LUT_exponent = 1.0;   /* assume no LUT:  most PCs */
#endif
    /* the defaults above give 1.0, 1.3, 1.5 and 2.2, respectively: */
    default_display_exponent = LUT_exponent * CRT_exponent;
    /* If the user has set the SCREEN_GAMMA environment variable as suggested
     * (somewhat imprecisely) in the libpng documentation, use that; otherwise
     * use the default value we just calculated.  Either way, the user may
     * override this via a command-line option. */
    if ((p = getenv("SCREEN_GAMMA")) != NULL)
        rpng2_info.display_exponent = atof(p);
    else
        rpng2_info.display_exponent = default_display_exponent;
    /* Windows really hates command lines, so we have to set up our own argv.
     * Note that we do NOT bother with quoted arguments here, so don't use
     * filenames with spaces in 'em! */
    argv[argc++] = PROGNAME;
    p = cmd;
    for (;;) {
        if (*p == ' ')
            while (*++p == ' ')
                ;
        /* now p points at the first non-space after some spaces */
        if (*p == '\0')
            break;    /* nothing after the spaces:  done */
        argv[argc++] = q = p;
        while (*q && *q != ' ')
            ++q;
        /* now q points at a space or the end of the string */
        if (*q == '\0')
            break;    /* last argv already terminated; quit */
        *q = '\0';    /* change space to terminator */
        p = q + 1;
    }
    argv[argc] = NULL;   /* terminate the argv array itself */
    /* Now parse the command line for options and the PNG filename. */
    while (*++argv && !error) {
        if (!strncmp(*argv, "-gamma", 2)) {
            if (!*++argv)
                ++error;
            else {
                rpng2_info.display_exponent = atof(*argv);
                if (rpng2_info.display_exponent <= 0.0)
                    ++error;
            }
        } else if (!strncmp(*argv, "-bgcolor", 4)) {
            if (!*++argv)
                ++error;
            else {
                bgstr = *argv;
                if (strlen(bgstr) != 7 || bgstr[0] != '#')
                    ++error;
                else {
                    have_bg = TRUE;
                    bg_image = FALSE;
                }
            }
        } else if (!strncmp(*argv, "-bgpat", 4)) {
            if (!*++argv)
                ++error;
            else {
                pat = atoi(*argv) - 1;
                if (pat < 0 || pat >= num_bgpat)
                    ++error;
                else {
                    bg_image = TRUE;
                    have_bg = FALSE;
                }
            }
        } else if (!strncmp(*argv, "-timing", 2)) {
            timing = TRUE;
        } else {
            if (**argv != '-') {
                filename = *argv;
                if (argv[1])   /* shouldn't be any more args after filename */
                    ++error;
            } else
                ++error;   /* not expecting any other options */
        }
    }
    if (!filename)
        ++error;
    /* print usage screen if any errors up to this point */
    if (error) {
#ifndef __CYGWIN__
        int ch;
#endif
        fprintf(stderr, "\n%s %s:  %s\n\n", PROGNAME, VERSION, appname);
        readpng2_version_info();
        fprintf(stderr, "\n"
          "Usage:  %s [-gamma exp] [-bgcolor bg | -bgpat pat] [-timing]\n"
          "        %*s file.png\n\n"
          "    exp \ttransfer-function exponent (``gamma'') of the display\n"
          "\t\t  system in floating-point format (e.g., ``%.1f''); equal\n"
          "\t\t  to the product of the lookup-table exponent (varies)\n"
          "\t\t  and the CRT exponent (usually 2.2); must be positive\n"
          "    bg  \tdesired background color in 7-character hex RGB format\n"
          "\t\t  (e.g., ``#ff7700'' for orange:  same as HTML colors);\n"
          "\t\t  used with transparent images; overrides -bgpat option\n"
          "    pat \tdesired background pattern number (1-%d); used with\n"
          "\t\t  transparent images; overrides -bgcolor option\n"
          "    -timing\tenables delay for every block read, to simulate modem\n"
          "\t\t  download of image (~36 Kbps)\n"
          "\nPress Q, Esc or mouse button 1 after image is displayed to quit.\n"
#ifndef __CYGWIN__
          "Press Q or Esc to quit this usage screen. ",
#else
          ,
#endif
          PROGNAME,
#if (defined(__i386__) || defined(_M_IX86) || defined(__x86_64__)) && \
    !(defined(__CYGWIN__) || defined(__MINGW32__))
          (int)strlen(PROGNAME), " ",
#endif
          (int)strlen(PROGNAME), " ", default_display_exponent, num_bgpat);
        fflush(stderr);
#ifndef __CYGWIN__
        do
            ch = _getch();
        while (ch != 'q' && ch != 'Q' && ch != 0x1B);
#endif
        exit(1);
    }
    if (!(infile = fopen(filename, "rb"))) {
        fprintf(stderr, PROGNAME ":  can't open PNG file [%s]\n", filename);
        ++error;
    } else {
        incount = fread(inbuf, 1, INBUFSIZE, infile);
        if (incount < 8 || !readpng2_check_sig(inbuf, 8)) {
            fprintf(stderr, PROGNAME
              ":  [%s] is not a PNG file: incorrect signature\n",
              filename);
            ++error;
        } else if ((rc = readpng2_init(&rpng2_info)) != 0) {
            switch (rc) {
                case 2:
                    fprintf(stderr, PROGNAME
                      ":  [%s] has bad IHDR (libpng longjmp)\n", filename);
                    break;
                case 4:
                    fprintf(stderr, PROGNAME ":  insufficient memory\n");
                    break;
                default:
                    fprintf(stderr, PROGNAME
                      ":  unknown readpng2_init() error\n");
                    break;
            }
            ++error;
        }
        if (error)
            fclose(infile);
    }
    if (error) {
#ifndef __CYGWIN__
        int ch;
#endif
        fprintf(stderr, PROGNAME ":  aborting.\n");
#ifndef __CYGWIN__
        do
            ch = _getch();
        while (ch != 'q' && ch != 'Q' && ch != 0x1B);
#endif
        exit(2);
    } else {
        fprintf(stderr, "\n%s %s:  %s\n", PROGNAME, VERSION, appname);
#ifndef __CYGWIN__
        fprintf(stderr,
          "\n   [console window:  closing this window will terminate %s]\n\n",
          PROGNAME);
#endif
        fflush(stderr);
    }
    /* set the title-bar string, but make sure buffer doesn't overflow */
    alen = strlen(appname);
    flen = strlen(filename);
    if (alen + flen + 3 > 1023)
        sprintf(titlebar, "%s:  ...%s", appname, filename+(alen+flen+6-1023));
    else
        sprintf(titlebar, "%s:  %s", appname, filename);
    /* set some final rpng2_info variables before entering main data loop */
    if (have_bg) {
        unsigned r, g, b;   /* this approach quiets compiler warnings */
        sscanf(bgstr+1, "%2x%2x%2x", &r, &g, &b);
        rpng2_info.bg_red   = (uch)r;
        rpng2_info.bg_green = (uch)g;
        rpng2_info.bg_blue  = (uch)b;
    } else
        rpng2_info.need_bgcolor = TRUE;
    rpng2_info.state = kPreInit;
    rpng2_info.mainprog_init = rpng2_win_init;
    rpng2_info.mainprog_display_row = rpng2_win_display_row;
    rpng2_info.mainprog_finish_display = rpng2_win_finish_display;
    /* OK, this is the fun part:  call readpng2_decode_data() at the start of
     * the loop to deal with our first buffer of data (read in above to verify
     * that the file is a PNG image), then loop through the file and continue
     * calling the same routine to handle each chunk of data.  It in turn
     * passes the data to libpng, which will invoke one or more of our call-
     * backs as decoded data become available.  We optionally call Sleep() for
     * one second per iteration to simulate downloading the image via an analog
     * modem. */
    for (;;) {
        Trace((stderr, "about to call readpng2_decode_data()\n"))
        if (readpng2_decode_data(&rpng2_info, inbuf, incount))
            ++error;
        Trace((stderr, "done with readpng2_decode_data()\n"))
        if (error || incount != INBUFSIZE || rpng2_info.state == kDone) {
            if (rpng2_info.state == kDone) {
                Trace((stderr, "done decoding PNG image\n"))
            } else if (ferror(infile)) {
                fprintf(stderr, PROGNAME
                  ":  error while reading PNG image file\n");
                exit(3);
            } else if (feof(infile)) {
                fprintf(stderr, PROGNAME ":  end of file reached "
                  "(unexpectedly) while reading PNG image file\n");
                exit(3);
            } else /* if (error) */ {
                /* will print error message below */
            }
            break;
        }
        if (timing)
            Sleep(1000L);
        incount = fread(inbuf, 1, INBUFSIZE, infile);
    }
    /* clean up PNG stuff and report any decoding errors */
    fclose(infile);
    Trace((stderr, "about to call readpng2_cleanup()\n"))
    readpng2_cleanup(&rpng2_info);
    if (error) {
        fprintf(stderr, PROGNAME ":  libpng error while decoding PNG image\n");
        exit(3);
    }
    /* wait for the user to tell us when to quit */
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    /* we're done:  clean up all image and Windows resources and go away */
    Trace((stderr, "about to call rpng2_win_cleanup()\n"))
    rpng2_win_cleanup();
    return msg.wParam;
}
