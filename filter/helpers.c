#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int avg = round((image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0);
            image[i][j].rgbtBlue = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtRed = avg;
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int original_red = image[i][j].rgbtRed;
            int original_green = image[i][j].rgbtGreen;
            int original_blue = image[i][j].rgbtBlue;

            int sepia_red = round(.393 * original_red + .769 * original_green + .189 * original_blue);
            int sepia_green = round(.349 * original_red + .686 * original_green + .168 * original_blue);
            int sepia_blue = round(.272 * original_red + .534 * original_green + .131 * original_blue);

            if (sepia_red > 255)
            {
                image[i][j].rgbtRed = 255;
            }
            else
            {
                image[i][j].rgbtRed = sepia_red;
            }

            if (sepia_green > 255)
            {
                image[i][j].rgbtGreen = 255;
            }
            else
            {
                image[i][j].rgbtGreen = sepia_green;
            }

            if (sepia_blue > 255)
            {
                image[i][j].rgbtBlue = 255;
            }
            else
            {
                image[i][j].rgbtBlue = sepia_blue;
            }
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE *a = &image[i][j];
            RGBTRIPLE *b = &image[i][width - 1 - j];

            RGBTRIPLE tmp = *a;
            *a = *b;
            *b = tmp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Copy original image
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            copy[i][j] = image[i][j];
        }
    }

    // Blur image
    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column++)
        {
            float pixels = 0;
            float red = 0;
            float green = 0;
            float blue = 0;

            // iterate over 3x3 grid surrounding current pixel
            for (int r = -1; r < 2; r++)
            {
                for (int c = -1; c < 2; c++)
                {
                    // if pixel outside image bounds, skip it
                    if (row + r < 0 || row + r >= height || column + c < 0 || column + c >= width)
                    {
                        continue;
                    }

                    // add pixel's RGB values
                    red += copy[row + r][column + c].rgbtRed;
                    green += copy[row + r][column + c].rgbtGreen;
                    blue += copy[row + r][column + c].rgbtBlue;
                    pixels++;
                }
            }

            // average collected RGB values and apply to original image
            image[row][column].rgbtRed = round(red / pixels);
            image[row][column].rgbtGreen = round(green / pixels);
            image[row][column].rgbtBlue = round(blue / pixels);

        }
    }
    return;
}
