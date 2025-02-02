static png_uint_32
crc_init_4(png_uint_32 value)
{
   /* This is an alternative to the algorithm used in zlib, which requires four
    * separate tables to parallelize the four byte operations, it only works for
    * a CRC of the first four bytes of the stream, but this is what happens in
    * the parser below where length+chunk-name is read and chunk-name used to
    * initialize the CRC.  Notice that the calculation here avoids repeated
    * conditioning (xor with 0xffffffff) by storing the conditioned value.
    */
   png_uint_32 crc = crc_table[(~value >> 24)] ^ 0xffffff;
   crc = crc_table[(crc ^ (value >> 16)) & 0xff] ^ (crc >> 8);
   crc = crc_table[(crc ^ (value >> 8)) & 0xff] ^ (crc >> 8);
   return crc_table[(crc ^ value) & 0xff] ^ (crc >> 8);
}
