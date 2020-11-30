//Griffin Heyrich
//u42992446
#include "cachelab.h"
#include <stdio.h>
#include<stdlib.h>
#include <getopt.h>


int sets;
int assoc;
int bytes;

//2d array of structs 
//dont need to stroe data bc it is simulator 

typedef struct Block{
    int tag;
    int valid_bit;
    int lru; //time stamp
} block;

typedef struct Set{
    unsigned int index;
    block *my_set; //array of sets in struct
} set;

set *my_cache;

//blocks here is e3ssetianlly a cache line

void create_cache(int set_bits, int assoc, int offset_bits){
    sets = 1 << set_bits; //left shift same as mult by 2
    assoc = assoc;
    bytes = 1 << offset_bits;

    printf("%d, %d, %d\n", sets, assoc, bytes);
    printf("my_cache: %p\n", my_cache);

    my_cache = (set *)malloc(sets* sizeof(set) ); //alloc space for cache
    printf("my_cache: %p\n", my_cache);


    int i =0;
    int j =0;
    for (i= 0; i < sets; i++)
    {
        my_cache[i].index = i;
        my_cache[i].my_set = (block *) malloc(assoc * sizeof(block)); //allocate my set within each set of size block


        for (j = 0; j < assoc; j ++) //each block within set intialize there starting information
        {
            my_cache[i].my_set[j].tag = -1;
            my_cache[i].my_set[j].valid_bit = -1;
            my_cache[i].my_set[j].lru = -1;
        }
    }
}





int main(int argc, char **argv)  //int is number of args char is strings part of that arg list (list of strings bascvially) 
{
    int set_bits = 0;
    int associativity =0;
    int offset_bits = 0;
    char *trace_file = NULL;


    char opt;

    while(1)
    {

        opt = getopt(argc, argv, "vhs:E:b:t:"); //looks at argv and tries to match it with one of those things if found opt gives letter that first found
        

        if (opt == -1)
        {
            break;
        }

        switch(opt)
        {
            case 's':
                set_bits = atoi(optarg); //opt agrv var created wtih get opt, this is string, atoi turns strong to int if possible
                break;
            case 'E':
                associativity = atoi(optarg);
                break;
            case 'b':
                offset_bits = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            default:
                break;
        }

    }



    printf("%d, %d, %d, %s\n", set_bits, associativity, offset_bits, trace_file);

    create_cache(set_bits, associativity, offset_bits);

    printSummary(0, 0, 0);
    return 0;
}


//need function to scan and opt through trace file
//function to modify caache
//insert 
//modify
//delete
//mechanism to keep tack of hits misses and evictions

//need to figure out based on addrerss where it should go and based on size how much size it should take up