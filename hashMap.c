/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Jacob Mast
 * Date: 12/04/2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement

    struct HashLink *current;

    for (int i = 0; i < map->capacity; i++)
    {
        current = map->table[i];
        while(current != 0)
        {
            hashLinkDelete(current);
            current = current->next;
        }

        map->table[i]= 0;
    }

    free(map->table);
    map->capacity = 0;
    map->size = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    int idx = HASH_FUNCTION(key) % map->capacity;

    if (idx < 0)
    {
        idx += map->capacity;
    }

    HashLink *current = map->table[idx];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0){
            return &current->value;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement

    HashMap *newMap = hashMapNew(capacity);
    hashMapInit(map, capacity);
    HashLink *current;
    for(int i = 0; i < hashMapCapacity(map); i++)
    {
        current = map->table[i];
        while(current != NULL){   
            newMap->table[i] = map->table[i];
            current = current->next;
        }
    }

    hashMapCleanUp(map);
    map->capacity = newMap->capacity;
    map->table = newMap->table;
    map->size = newMap->size;
    hashMapCleanUp(newMap);

}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement

    int idx = HASH_FUNCTION(key) % map->capacity;
    struct HashLink *current = map->table[idx];

    if(idx < 0)
    {
        idx += map->capacity;
    }

    if(current != NULL)
    {
        current->value = value;
        map->size++;
        if (hashMapTableLoad(map) >= (float) MAX_TABLE_LOAD) {
            resizeTable(map, map->capacity*2);
        }
        return;
    }
    else {
        current = hashLinkNew(key, value, 0);
        map->table[idx] = current;
        map->size++;
        if (hashMapTableLoad(map) >= (float) MAX_TABLE_LOAD) {
            resizeTable(map, map->capacity*2);
        }
        return;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement

    assert(map != 0);
    assert(map->table > 0);

    int idx = HASH_FUNCTION(key) % map->capacity;

    if (idx < 0)
    {
        idx += map->capacity;
    }

    HashLink *current = map->table[idx];
    HashLink *prev = map->table[idx];

    if (current != NULL) {
        if(strcmp(current->key, key) == 0) {
            HashLink *next = current->next;
            hashLinkDelete(current);
            map->table[idx] = next;
            map->size--;
        } else {
            prev = current;
            current = current->next;
        }
    }

    while (current != NULL)
    {
        if(strcmp(current->key, key) == 0)
        {
            prev->next = current->next;
            hashLinkDelete(current);
            map->size--;
            return;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    int idx = HASH_FUNCTION(key) % map->capacity;
    HashLink *current = map->table[idx];

    if (idx < 0) {
            idx += map->capacity;
    }

    while (current != NULL) {
        if (strcmp(current->key, key) == 0){
            return 1;
        }
        else {
            current = current->next;
        }
    } 
    
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    int empty = 0;
    for (int i = 0; i < map->capacity; i++) {
        if (map->table[i] == 0) {
            empty++;
        }
    }
    return empty;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    if(map->capacity != 0) {
        float load = map->size / map->capacity;
        return load;
    } else return 0;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    // FIXME: implement
    struct HashLink *print;

    for (int i = 0; i < map->capacity; i++)
    {
        print = map->table[i];
        if (print != 0) {
            printf("\nBucket index: %d -> ", i);
        }

        while (print != 0)
        {
            printf("\nKey: %s", print->key);
            printf(" -> ");
            printf("Value: %d", print->value);
            print = print->next;
        }
    }
   
}
