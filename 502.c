}
static void writepng_error_handler(png_structp png_ptr, png_const_charp msg)
{
    mainprog_info  *mainprog_ptr;
    /* This function, aside from the extra step of retrieving the "error
     * pointer" (below) and the fact that it exists within the application
     * rather than within libpng, is essentially identical to libpng's
     * default error handler.  The second point is critical:  since both
     * setjmp() and longjmp() are called from the same code, they are
     * guaranteed to have compatible notions of how big a jmp_buf is,
     * regardless of whether _BSD_SOURCE or anything else has (or has not)
     * been defined. */
    fprintf(stderr, "writepng libpng error: %s\n", msg);
    fflush(stderr);
    mainprog_ptr = png_get_error_ptr(png_ptr);
    if (mainprog_ptr == NULL) {         /* we are completely hosed now */
        fprintf(stderr,
          "writepng severe error:  jmpbuf not recoverable; terminating.\n");
        fflush(stderr);
        exit(99);
    }
    /* Now we have our data structure we can use the information in it
     * to return control to our own higher level code (all the points
     * where 'setjmp' is called in this file.)  This will work with other
     * error handling mechanisms as well - libpng always calls png_error
     * when it can proceed no further, thus, so long as the error handler
     * is intercepted, application code can do its own error recovery.
     */
    longjmp(mainprog_ptr->jmpbuf, 1);
}
