// Recovers JPEGs from a forensic image

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

const int BLOCK_SIZE = 512;

int main(int argc, char *argv[])
{
    // - The files you generate should each be named ###.jpg, where ### is a three-digit decimal number,
    // starting with 000 for the first image and counting up.
    // Your program, if it uses malloc, must not leak any memory.

    // Remind user of usage if argc != 2 and end the program
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open image and if any errors when opening, inform the user and end the program
    FILE *image = fopen(argv[1], "r");
    if (image == NULL)
    {
        printf("Image could not be opened for reading\n");
        fclose(image);
        return 1;
    }

    BYTE *buffer = malloc(BLOCK_SIZE);
    int jpeg_num = 0;
    char *jpeg_name = malloc(8); // 8 = length of ###.jpg plus NUL character
    FILE *jpeg = NULL; // initialise here for global access (within main).

    // iterate over each BLOCK_SIZE byte block until end of image, where fread doesn't return BLOCK_SIZE.
    while(fread(buffer, 1, BLOCK_SIZE, image) == BLOCK_SIZE)
    {
        // If start of JPEG, whose first four bytes are 0xff 0xd8 0xff 0xe(0-f)
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] >= 0xe0 && buffer[3] <= 0xef)
        {
            // if not the first recovered JPEG, close current JPEG
            if (jpeg_num != 0)
            {
                fclose(jpeg);
            }
            // create new JPEG for writing
            sprintf(jpeg_name, "%03i.jpg", jpeg_num);
            jpeg = fopen(jpeg_name, "w");

            // check a jpeg is open and if so, write to it
            if (jpeg)
            {
                // write block stored in buffer to jpeg
                fwrite(buffer, 1, BLOCK_SIZE, jpeg);
            }
            // increment jpeg_num to avoid overwriting previously recovered JPEGs
            jpeg_num++;
        }
        else // first four bytes of block not start of new JPEG
        {
            // check a jpeg is open and if so, write to it
            if (jpeg)
            {
                fwrite(buffer, 1, BLOCK_SIZE, jpeg);
            }
        }
    }

    // close files and free malloc'd memory
    fclose(jpeg);
    fclose(image);
    free(buffer);
    free(jpeg_name);
}