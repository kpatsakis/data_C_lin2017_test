static void
usage(const char *prog)
{
   /* ANSI C-90 limits strings to 509 characters, so use a string array: */
   size_t i;
   static const char *usage_string[] = {
"  Tests, optimizes and optionally fixes the zlib header in PNG files.",
"  Optionally, when fixing, strips ancilliary chunks from the file.",
0,
"OPTIONS",
"  OPERATION",
"      By default files are just checked for readability with a summary of the",
"      of zlib issues founds for each compressed chunk and the IDAT stream in",
"      the file.",
"    --optimize (-o):",
"      Find the smallest deflate window size for the compressed data.",
"    --strip=[none|crc|unsafe|unused|transform|color|all]:",
"        none (default):   Retain all chunks.",
"        crc:    Remove chunks with a bad CRC.",
"        unsafe: Remove chunks that may be unsafe to retain if the image data",
"                is modified.  This is set automatically if --max is given but",
"                may be cancelled by a later --strip=none.",
"        unused: Remove chunks not used by libpng when decoding an image.",
"                This retains any chunks that might be used by libpng image",
"                transformations.",
"        transform: unused+bKGD.",
"        color:  transform+iCCP and cHRM.",
"        all:    color+gAMA and sRGB.",
"      Only ancillary chunks are ever removed.  In addition the tRNS and sBIT",
"      chunks are never removed as they affect exact interpretation of the",
"      image pixel values.  The following known chunks are treated specially",
"      by the above options:",
"        gAMA, sRGB [all]: These specify the gamma encoding used for the pixel",
"            values.",
"        cHRM, iCCP [color]: These specify how colors are encoded.  iCCP also",
"            specifies the exact encoding of a pixel value however in practice",
"            most programs will ignore it.",
"        bKGD [transform]: This is used by libpng transforms."
"    --max=<number>:",
"      Use IDAT chunks sized <number>.  If no number is given the the IDAT",
"      chunks will be the maximum size permitted; 2^31-1 bytes.  If the option",
"      is omitted the original chunk sizes will not be changed.  When the",
"      option is given --strip=unsafe is set automatically, this may be",
"      cancelled if you know that all unknown unsafe-to-copy chunks really are",
"      safe to copy across an IDAT size change.  This is true of all chunks",
"      that have ever been formally proposed as PNG extensions.",
"  MESSAGES",
"      By default the program only outputs summaries for each file.",
"    --quiet (-q):",
"      Do not output the summaries except for files which cannot be read. With",
"      two --quiets these are not output either.",
"    --errors (-e):",
"      Output errors from libpng and the program (except too-far-back).",
"    --warnings (-w):",
"      Output warnings from libpng.",
"  OUTPUT",
"      By default nothing is written.",
"    --out=<file>:",
"      Write the optimized/corrected version of the next PNG to <file>.  This",
"      overrides the following two options",
"    --suffix=<suffix>:",
"      Set --out=<name><suffix> for all following files unless overridden on",
"      a per-file basis by explicit --out.",
"    --prefix=<prefix>:",
"      Set --out=<prefix><name> for all the following files unless overridden",
"      on a per-file basis by explicit --out.",
"      These two options can be used together to produce a suffix and prefix.",
"  INTERNAL OPTIONS",
#if 0 /*NYI*/
#ifdef PNG_MAXIMUM_INFLATE_WINDOW
"    --test:",
"      Test the PNG_MAXIMUM_INFLATE_WINDOW option.  Setting this disables",
"      output as this would produce a broken file.",
#endif
#endif
0,
"EXIT CODES",
"  *** SUBJECT TO CHANGE ***",
"  The program exit code is value in the range 0..127 holding a bit mask of",
"  the following codes.  Notice that the results for each file are combined",
"  together - check one file at a time to get a meaningful error code!",
"    0x01: The zlib too-far-back error existed in at least one chunk.",
"    0x02: At least once chunk had a CRC error.",
"    0x04: A chunk length was incorrect.",
"    0x08: The file was truncated.",
"  Errors less than 16 are potentially recoverable, for a single file if the",
"  exit code is less than 16 the file could be read (with corrections if a",
"  non-zero code is returned).",
"    0x10: The file could not be read, even with corrections.",
"    0x20: The output file could not be written.",
"    0x40: An unexpected, potentially internal, error occured.",
"  If the command line arguments are incorrect the program exits with exit",
"  255.  Some older operating systems only support 7-bit exit codes, on those",
"  systems it is suggested that this program is first tested by supplying",
"  invalid arguments.",
0,
"DESCRIPTION",
"  " PROGRAM_NAME ":",
"  checks each PNG file on the command line for errors.  By default errors are",
"  not output and the program just returns an exit code and prints a summary.",
"  With the --quiet (-q) option the summaries are suppressed too and the",
"  program only outputs unexpected errors (internal errors and file open",
"  errors).",
"  Various known problems in PNG files are fixed while the file is being read",
"  The exit code says what problems were fixed.  In particular the zlib error:",
0,
"        \"invalid distance too far back\"",
0,
"  caused by an incorrect optimization of a zlib stream is fixed in any",
"  compressed chunk in which it is encountered.  An integrity problem of the",
"  PNG stream caused by a bug in libpng which wrote an incorrect chunk length",
"  is also fixed.  Chunk CRC errors are automatically fixed up.",
0,
"  Setting one of the \"OUTPUT\" options causes the possibly modified file to",
"  be written to a new file.",
0,
"  Notice that some PNG files with the zlib optimization problem can still be",
"  read by libpng under some circumstances.  This program will still detect",
"  and, if requested, correct the error.",
0,
"  The program will reliably process all files on the command line unless",
"  either an invalid argument causes the usage message (this message) to be",
"  produced or the program crashes.",
0,
"  The summary lines describe issues encountered with the zlib compressed",
"  stream of a chunk.  They have the following format, which is SUBJECT TO",
"  CHANGE in the future:",
0,
"     chunk reason comp-level p1 p2 p3 p4 file",
0,
"  p1 through p4 vary according to the 'reason'.  There are always 8 space",
"  separated fields.  Reasons specific formats are:",
0,
"     chunk ERR status code read-errno write-errno message file",
"     chunk SKP comp-level file-bits zlib-rc compressed message file",
"     chunk ??? comp-level file-bits ok-bits compressed uncompress file",
0,
"  The various fields are",
0,
"$1 chunk:      The chunk type of a chunk in the file or 'HEAD' if a problem",
"               is reported by libpng at the start of the IDAT stream.",
"$2 reason:     One of:",
"          CHK: A zlib header checksum was detected and fixed.",
"          TFB: The zlib too far back error was detected and fixed.",
"          OK : No errors were detected in the zlib stream and optimization",
"               was not requested, or was not possible.",
"          OPT: The zlib stream window bits value could be improved (and was).",
"          SKP: The chunk was skipped because of a zlib issue (zlib-rc) with",
"               explanation 'message'",
"          ERR: The read of the file was aborted.  The parameters explain why.",
"$3 status:     For 'ERR' the accumulate status code from 'EXIT CODES' above.",
"               This is printed as a 2 digit hexadecimal value",
"   comp-level: The recorded compression level (FLEVEL) of a zlib stream",
"               expressed as a string {supfast,stdfast,default,maximum}",
"$4 code:       The file exit code; where stop was called, as a fairly terse",
"               string {warning,libpng,zlib,invalid,read,write,unexpected}.",
"   file-bits:  The zlib window bits recorded in the file.",
"$5 read-errno: A system errno value from a read translated by strerror(3).",
"   zlib-rc:    A zlib return code as a string (see zlib.h).",
"   ok-bits:    The smallest zlib window bits value that works.",
"$6 write-errno:A system errno value from a write translated by strerror(3).",
"   compressed: The count of compressed bytes in the zlib stream, when the",
"               reason is 'SKP'; this is a count of the bytes read from the",
"               stream when the fatal error was encountered.",
"$7 message:    An error message (spaces replaced by _, as in all parameters),",
"   uncompress: The count of bytes from uncompressing the zlib stream; this",
"               may not be the same as the number of bytes in the image.",
"$8 file:       The name of the file (this may contain spaces).",
};
   fprintf(stderr, "Usage: %s {[options] png-file}\n", prog);
   for (i=0; i < (sizeof usage_string)/(sizeof usage_string[0]); ++i)
   {
      if (usage_string[i] != 0)
         fputs(usage_string[i], stderr);
      fputc('\n', stderr);
   }
   exit(255);
}
