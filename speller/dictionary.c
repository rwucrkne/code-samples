// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 5369;

// Hash table
node *table[N];

// Keep track of number of words loaded
unsigned int words_loaded = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // Hash word
    unsigned int index = hash(word);

    // Create temporary pointer
    node *ptr = table[index];

    // while not at end of linked list or not in empty array index
    while (ptr != NULL)
    {
        // if word in dictionary
        if (strcasecmp(word, ptr->word) == 0)
        {
            return true;
        }
        else
        {
            // look at next linked list element
            ptr = ptr->next;
        }
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    return strlen(word) * tolower(word[0]) - tolower(word[0]);
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // open dictionary file
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        printf("Could not open dictionary.\n");
        return false;
    }

    // read strings from file one at a time
    char word[LENGTH + 1];
    while (fscanf(dict, "%s", word) != EOF)
    {
        // create new node
        node *new_node = malloc(sizeof(node));
        if (new_node == NULL)
        {
            printf("Could not create new node.\n");
            return false;
        }

        // copy word into node
        strcpy(new_node->word, word);

        // use hash function on word
        int index = hash(word);

        // insert node into hash table
        new_node->next = table[index];
        table[index] = new_node;

        // increment words_loaded
        words_loaded++;
    }

    // Close file
    fclose(dict);

    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return words_loaded;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // for each set of linked lists
    for (int i = 0; i < N; i++)
    {
        // create two pointers: one to point to next linked list element; and another to remember the current element
        node *ptr = table[i];
        node *tmp = ptr;
        while (ptr != NULL)
        {
            ptr = ptr->next;
            free(tmp);
            tmp = ptr;
        }
    }
    return true;
}
