#include "../../png.h"
int main(int argc, const char **argv)
{
   int result = 1;
   if (argc == 3)
   {
      png_image image;
      /* Only the image structure version number needs to be set. */
      memset(&image, 0, sizeof image);
      image.version = PNG_IMAGE_VERSION;
      if (png_image_begin_read_from_file(&image, argv[1]))
      {
         png_bytep buffer;
         /* Change this to try different formats!  If you set a colormap format
          * then you must also supply a colormap below.
          */
         image.format = PNG_FORMAT_RGBA;
         buffer = malloc(PNG_IMAGE_SIZE(image));
         if (buffer != NULL)
         {
            if (png_image_finish_read(&image, NULL/*background*/, buffer,
               0/*row_stride*/, NULL/*colormap for PNG_FORMAT_FLAG_COLORMAP */))
            {
               if (png_image_write_to_file(&image, argv[2],
                  0/*convert_to_8bit*/, buffer, 0/*row_stride*/,
                  NULL/*colormap*/))
                  result = 0;
               else
                  fprintf(stderr, "pngtopng: write %s: %s\n", argv[2],
                      image.message);
               free(buffer);
            }
            else
            {
               fprintf(stderr, "pngtopng: read %s: %s\n", argv[1],
                   image.message);
               /* This is the only place where a 'free' is required; libpng does
                * the cleanup on error and success, but in this case we couldn't
                * complete the read because of running out of memory.
                */
               png_image_free(&image);
            }
         }
         else
            fprintf(stderr, "pngtopng: out of memory: %lu bytes\n",
               (unsigned long)PNG_IMAGE_SIZE(image));
      }
      else
         /* Failed to read the first argument: */
         fprintf(stderr, "pngtopng: %s: %s\n", argv[1], image.message);
   }
   else
      /* Wrong number of arguments */
      fprintf(stderr, "pngtopng: usage: pngtopng input-file output-file\n");
   return result;
}
