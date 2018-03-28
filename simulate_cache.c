/* Simulate the performance of a cache using a reference stream of instruction, and load and store addresses.
 * Types of caches: Unified cache or a separate I-cache and D-cache.
 * Tunable cache parameters: cache size, associativity, and cache-line size.
 * Replacement policy: Least-recently used (LRU).
 * Output: Hit (miss) rate.
 * Date created: 10/28/2017
 * Date modified: 2/27/2018
 */

/*
Assignment completed by: Greg Matthews and Mark Klobukov
Date: 3/14/2018
Class: ECEC 621 with Dr. Anup Das
*/

#define _STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "parse_user_options.h"
#include "cache.h"
#include <unistd.h>

#define DEBUG_FLAG 0

/* Functions are defined here */
void print_usage (void);
extern USER_OPTIONS* parse_user_options (int argc, char **argv);
extern CACHE *create_cache (unsigned int size, unsigned int associativity, unsigned int line_size);
void print_cache (CACHE *);
int access_cache (CACHE *cache, int reference_type, uint64_t memory_address);
void simulate_unified_cache (CACHE *u_cache, FILE *fp);
void simulate_i_d_cache (CACHE *i_cache, CACHE *d_cache, FILE *fp);
void updateLRU(CACHE *cache, int set_index, int line_index);
int getLRUIndex(CACHE *cache);
/* Edit this function to compete the cache simulator. */
int access_cache (CACHE *cache, int reference_type, uint64_t memory_address)
{
    int hit = 0;
    if (DEBUG_FLAG)
    {
        printf ("Accesing cache for memory address %" PRIx64 ", type of reference %c \n", memory_address, reference_type);
    }

    /* Parse the memory address (which is in hex) into the corresponding offset, index, and tag bits. */
    unsigned int index, tag;
    unsigned int offsetBits, indexBits;
    uint64_t address = memory_address;
    //offset bits is log2 of size of cache line
    //index bits is log2 of number of sets
    //everything to the left of index bits is tag
    offsetBits = log2(cache->line_size);
    indexBits = log2(cache->num_sets);

    /*Calculation of offset. Not required for functionality of this assignment,
    but shown anyways*/
    //unsigned int offset;
    // offset = address & (unsigned int)(pow(2, offsetBits) -1);
    address >>= offsetBits;
    index = address & (unsigned int)(pow(2, indexBits) -1);
    address >>= indexBits;
    tag = address; //all remaining bits are tag



    /* Index into the cache set using the index bits. Check if the tag stored in the cache matches the tag
     * correcponding to the memory address. If yes and the valid bit is set to true, then declare a
     * cache hit. Else, declare a cache miss, and update the cache with the specified tag. If all sets are
     * full, then use the LRU algorithm to evict the appropriate cache line. */
     int i;
     for (i = 0; i < cache->associativity; i++) {
       if (cache->cache[index][i].tag == tag && cache->cache[index][i].valid_bit) {
         hit = 1;
         //update LRU matrix
         updateLRU(cache, index, i);
       }
     }

     //if no cache hit, there are two possibilities:
     //1) set is full and LRU replacement is necessary
     //2) set is not full and the line with invalid bit is to be written to
     if (!hit) {
       //if not full, place in the set:
       int needLRU = 1;
       //look for first invalid bit in the set (if it exists):
       for (i = 0; i < cache->associativity; i++) {
         if (!cache->cache[index][i].valid_bit) {
           //place it here, empty spot
           cache->cache[index][i].tag = tag;
           cache->cache[index][i].valid_bit = 1;
           needLRU = 0;
           //update associated LRU matrix
           updateLRU(cache, index, i);
         }
       }

       //if full, remove lru and replace:
       if (needLRU) {
         int lruIndex = getLRUIndex(cache);
         cache->cache[index][lruIndex].tag = tag;
         updateLRU(cache, index, lruIndex);
       }
     }

    return hit;
}

//updates LRU according to assignmenet instructions
void updateLRU(CACHE *cache, int set_index, int line_index) {
  int a = cache->associativity;

  for (int i = 0; i < a; i++) {
    cache->LRU[set_index][line_index][i] = 1;
  }

  for (int i = 0; i < a; i++) {
    cache->LRU[set_index][i][line_index] = 0;
  }
}

//converts each row of LRU matrix to a number
//and find index corresponding to the smallest one
int getLRUIndex(CACHE *cache) {
  int a = cache->associativity;

  int min = pow(2, a+1);
  int index;
  //for each row
  for (int i = 0; i < a; i++) {
    int sum = 0;
    for (int j = 0; j < a; j++) {
      sum += pow(2, a-1-j) * (*cache->LRU[i][j]);
    }
    if (sum < min) {
      index = i;
    }
  }
  return index;
}

/* Simulates a unified cache for instructions and data. */
void simulate_unified_cache (CACHE *u_cache, FILE *fp)
{
    int num_i_hits = 0, num_d_hits = 0;
    int num_instructions = 0, num_stores = 0, num_loads = 0;
    char reference_type;
    uint64_t memory_address;
    int status;

    while (1){
        /* Obtain the type of reference: instruction fetch, load, or store, and the memory address. */
        status = fscanf (fp, " %c %" SCNx64, &reference_type, &memory_address);
        if (status == EOF)
            break;

        /* Simulate the cache. */
         switch (reference_type) {
            case 'L':
                /* NO NEED TO IMPLEMENT THIS. */
                break;

             case 'S':
                /* NO NEED TO IMPLEMENT THIS. */
                break;

             case 'I':
             // printf ("%" PRIu64 "\n", memory_address);
                num_instructions++;
                /* FIXME: IMPLEMENT CACHE FUNCTIONALITY. */
                num_i_hits += access_cache (u_cache, reference_type, memory_address);
                break;

             default:
                break;
        }
    }

    /* Print some statistics. */
    printf("Total number of references to the cache: %d. \n", num_instructions + num_stores + num_loads);
    printf("Hit rate: %f. \n", (float)(num_i_hits + num_d_hits)/(float)(num_instructions + num_stores + num_loads));
}


int main (int argc, char **argv)
{
    FILE *input_fp;
    CACHE *u_cache = NULL;
    // CACHE *i_cache = NULL, *d_cache = NULL;

    /* Parse command line parameters. */
    USER_OPTIONS *user_options = parse_user_options (argc, argv);
    if (user_options == NULL){
        printf ("Error parsing command line arguments. \n");
        print_usage ();
        exit (0);
    }

    if (user_options->u_flag == 1){
        printf ("Creating unified cache; size: %dK, associativity: %d, cache line: %d bytes \n", user_options->u_cache_size,
                                                                                                user_options->u_cache_associativity,
                                                                                                user_options->u_cache_line_size);

        u_cache = create_cache (user_options->u_cache_size,
                                user_options->u_cache_associativity,
                                user_options->u_cache_line_size);


        if (u_cache == NULL){
            printf ("Error creating cache; parameters must be powers of 2. \n");
            fflush(stdout);
            exit (0);
        }
        if (DEBUG_FLAG) {
        print_cache (u_cache);
        }
    }

    printf ("Simulating the cache using trace file: %s. \n", user_options->trace_file_name);
    input_fp = fopen (user_options->trace_file_name, "r");
    if(input_fp == NULL){
        printf ("Error opening trace file. Exiting. \n");
        fflush(stdout);
        exit (-1);
    }

    if (user_options->u_flag == 1) {
        simulate_unified_cache (u_cache, input_fp);
        // free ((void *)u_cache);  //THROWS AN ERROR!
    }

    fclose (input_fp);
    free ((void *)user_options);

    exit (0);
}

void print_usage (void)
{
    printf ("*****************USAGE************************ \n");
    printf ("To simulate a unified cache use the -U option: \n");
    printf ("./simulate_cache -U <cache size in Kilobytes> <set associativity> <cache line size in bytes> -f <trace file> \n");
    printf ("For example, ./simulate_cache -U 512 4 128 -f gcc_trace, simulates a 512 KB cache with \n");
    printf ("set accociativity of 4 and a cache line size of 128 bytes using the trace from gcc_trace \n \n");


    printf ("To simulate a system with separate I- and D-caches: \n");
    printf ("./simulate_cache -I <cache size> <set associativity> <cache line size> -D <cache size> <set associativity> <cache line size> -f <trace file> \n");
    printf ("For example, ./simulate_cache -I 128 1 64 -D 512 4 128 -f gcc_trace, simulates a 128 KB I cache with \n");
    printf ("set accociativity of 1 (direct mapped) and a cache line size of 64 bytes, and a 512 KB D cache with \n");
    printf ("set asscociativity of 4 and a cache line size of 128 bytes using the trace from gcc_trace \n");
}
