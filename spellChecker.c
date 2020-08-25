#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    file = fopen("dictionary.txt", "r");

    while (!feof(file))
    {
        char *word = nextWord(file);
        if (word)
        {
            hashMapPut(map, word, 1);
        }
    }

    fclose(file);
}

/* returns the minimum of three integer values, a, b, and c */
int minimum(int a, int b, int c)
{
    if (a < b && a < c) {
        return a;
    }
    else if(c < b && c < a){
        return c;
    }
    else return b;
}

//source: https://rosettacode.org/wiki/Levenshtein_distance#C
/* s, t: two strings; len_s, len_t: their respective length */
int LevenshteinDistance(const char *s, int len_s, const char *t, int len_t)
{
    int cost;

    /* if either string is empty, difference is inserting all chars 
     * from the other
     */
    if (!len_s) return len_t;
    if (!len_t) return len_s;

    /* test if last characters are the same */
    if (s[len_s-1] == t[len_t-1])
        cost = 0;
    else
        cost = 1;

    /* recursive step, return minimum of delete char from s, delete char from t, and delete char from both */
    return minimum(LevenshteinDistance(s, len_s - 1, t, len_t    ) + 1,
                   LevenshteinDistance(s, len_s    , t, len_t - 1) + 1,
                   LevenshteinDistance(s, len_s - 1, t, len_t - 1) + cost);
}


/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..
         if (!hashMapContainsKey(map, inputBuffer))
        {
            printf("%s is not a word\n", inputBuffer);

            HashLink *suggest[5];

            int count = 0;

            for (int i = 0; i < hashMapCapacity(map); i++)
            {
                HashLink *dictionaryLink = map->table[i];

                char *key = nextWord(file);

                if (LevenshteinDistance(inputBuffer, strlen(inputBuffer), key, strlen(key)) < strlen(inputBuffer)) {
                    suggest[count]->key = dictionaryLink->key;
                    suggest[count]->value = dictionaryLink->value;
                    suggest[count]->next = dictionaryLink->next;
                    count++;
                    printf("Did you mean %s?\n", key);
                }
                if (count > 5)
                {
                    break;
                }
            }
        }
        else {
            printf("The inputted word %s is spelled correctly \n", inputBuffer);
        }


        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}
