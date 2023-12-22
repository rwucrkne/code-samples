#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int count_letters(string text);
int count_words(string text);
int count_sentences(string text);

int main(void)
{
    string text = get_string("Text: ");
    int letters = count_letters(text);
    int words = count_words(text);
    int sentences = count_sentences(text);

    float avg_letters_per_100_words = (float) letters / words * 100;
    float avg_sentences_per_100_words = (float) sentences / words * 100;

    int index = round(0.0588 * avg_letters_per_100_words - 0.296 * avg_sentences_per_100_words - 15.8);
    if (index < 16 && index >= 1)
    {
        printf("Grade %i\n", index);
    }
    else if (index >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Before Grade 1\n");
    }
}

int count_letters(string text)
{
    // count number of alphabetical characters in string
    int count = 0;
    for (int i = 0, length = strlen(text); i < length; i++)
    {
        if (isalpha(text[i]))
        {
            count++;
        }
    }
    return count;
}

int count_words(string text)
{
    // count number of words in string.
    // any sequence of characters separated by a space is considered a word.

    // assumes a sentence:
    // - contains at least one word; and
    // - won't start or end with a space

    int count = 1; // accounts for first word in string
    for (int i = 0, length = strlen(text); i < length; i++)
    {
        if (isblank(text[i]) && !(isblank(text[i + 1]))) // does not count multiple spaces in a row
        {
            count++;
        }
    }
    return count;
}

int count_sentences(string text)
{
    // count number of sentences in string
    // any sequence of characters that ends with a ".", "!", or "?" is considered a sentence
    int count = 0;
    for (int i = 0, length = strlen(text); i < length; i++)
    {
        char c = text[i];
        if (c == '.' || c == '!' || c == '?')
        {
            count++;
        }
    }
    return count;
}