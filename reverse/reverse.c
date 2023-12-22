#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./reverse input.wav output.wav\n");
        return 1;
    }

    // Open input file for reading
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        printf("Error opening input file.\n");
        return 1;
    }

    // Read header
    WAVHEADER *inheader = malloc(sizeof(WAVHEADER));
    fread(inheader, sizeof(WAVHEADER), 1, inptr);

    // Use check_format to ensure WAV format
    if (!(check_format(*inheader)))
    {
        printf("Input file is not a WAV file.\n");
        return 1;
    }

    // Open output file for writing
    FILE *outptr = fopen(argv[2], "w");
    if (outptr == NULL)
    {
        printf("Error creating output file.\n");
        return 1;
    }

    // Write header to file
    fwrite(inheader, sizeof(WAVHEADER), 1, outptr);

    // Use get_block_size to calculate size of block
    int block_size = get_block_size(*inheader);
    free(inheader);

    // Write reversed audio to file
    BYTE buffer[block_size];
    fseek(inptr, -block_size, SEEK_END); // start looking at last block of input file

    while (ftell(inptr) >= sizeof(WAVHEADER)) // while file position (byte) more than or equal to 44
    {
        // read block into buffer, moves file position indicator forward by block_size
        fread(buffer, block_size, 1, inptr);

        // write block into output file
        fwrite(buffer, block_size, 1, outptr);

        // move input file position indicator back 2x block size to avoid reading duplicate bytes
        fseek(inptr, -2 * block_size, SEEK_CUR);
    }

}

int check_format(WAVHEADER header)
{
    // access header's format attribute (whose type is BYTE)
    BYTE *format = header.format;

    // if format is WAVE (0x57 0x41 0x56 0x45)
    if (format[0] == 0x57 && format[1] == 0x41 && format[2] == 0x56 && format[3] == 0x45)
    {
        return 1;
    }
    return 0;
}

int get_block_size(WAVHEADER header)
{
    int bytes_per_sample = header.bitsPerSample / 8;
    return header.numChannels * bytes_per_sample;
}