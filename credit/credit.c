#include <cs50.h>
#include <stdio.h>

int length(long n);
int first_two_digits(long n);
int checksum(long n);

int main(void)
{
    // Prompt for input
    long n = get_long("Number: ");
    // Calculate checksum
    int c = checksum(n);
    // Get length of card number and first two digits
    int l = length(n);
    int d = first_two_digits(n);
    // printf("checksum = %i, length = %i, first two digits = %i\n", c, l, d);

    // Print AMEX, MASTERCARD, VISA, or INVALID
    if (c == 0)
    {
        if (l == 15 && (d == 34 || d == 37)) // AMEX length = 15, begins with 34 or 37
        {
            printf("AMEX\n");
        }
        else if (l == 16 && (51 <= d && d <= 55)) // MASTERCARD length = 16, begins with 51-55
        {
            printf("MASTERCARD\n");
        }
        else if ((l == 13 || l == 16) && (40 <= d && d <= 49)) // VISA length is 13 or 15, begins with 4
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }

}

int length(long n)
{
    // Get length of n
    int len = 0;
    long x = n;
    while (x > 0)
    {
        len++;
        x = x / 10;
    }
    return len;
}

int first_two_digits(long n)
{
    // Get first two digits of n
    int digit1, digit2;
    long x = n;
    while (x > 0)
    {
        digit2 = digit1;
        digit1 = x % 100;
        x = x / 10;
        // printf("digit1 = %i, digit2 = %i, a = %li\n", digit1, digit2, x);
    }
    return digit2;
}

int checksum(long n)
{
    // Calculate checksum using Luhn algorithm
    int sum1 = 0, sum2 = 0;
    // Multiply every other digit by 2, starting with the number’s second-to-last digit,
    // and then add those products’ digits together.
    long x = n;
    x = x / 10; // divide by ten to skip last digit
    while (x > 0)
    {
        int y = 2 * (x % 10); // need to add product's digits, so if more than 9, add each digit individually to sum1
        if (y > 9)
        {
            while (y > 0)
            {
                sum1 += y % 10; // remainder after dividing by 10 always equals last digit in number
                y = y / 10;
            }
        }
        else
        {
            sum1 += y % 10;
        }
        x = x / 100; // divide by 100 to skip every other digit
    }
    // Add the sum to the sum of the digits that weren’t multiplied by 2.
    long i = n;
    while (i > 0)
    {
        sum2 += i % 10;
        i = i / 100; // divide by 100 to skip every other digit
    }

    return (sum1 + sum2) % 10; // zero = valid, non-zero = invalid
}