//Griffin Heyrich gheyrich@bu.edu
//U42992446
#include "cachelab.h"
#include <stdio.h>
#include<stdlib.h>
#include <getopt.h>
#include <time.h>


int sets;
int assoc;
int bytes;

int hits = 0;
int miss = 0;
int evictions =0;

//2d array of structs 
//dont need to stroe data bc it is simulator 

typedef struct Block{ //block = line
    int tag;
    int valid_bit;
    int lru; //time stamp //lru will be 1 for most recent used?
} block;

typedef struct Set{ //pointe to line 
    unsigned int index; 
    block *my_set; //array of sets in struct
} set;

set *my_cache;



//took this out of main to use in other fucntions
int set_bits = 0;
int associativity = 0;
int offset_bits = 0;
char *trace_file = NULL;



//blocks here is essetianlly a cache line

void create_cache(int set_bits, int assoc, int offset_bits){
    sets = 1 << set_bits; //left shift same as mult by 2
    assoc = assoc;
    bytes = 1 << offset_bits;

    printf("%d, %d, %d\n", sets, assoc, bytes);
    printf("my_cache: %p\n", my_cache);

    my_cache = (set *)malloc(sets * sizeof(set) ); //alloc space for cache
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




void read_trace_file(char* trace_func) //this will need to be called after t in switch statement
{
//FILE* use fopen with r to read the data
FILE* trace_p = fopen(trace_func, "r");

    if (trace_p == NULL){
        printf("file failed to open\n");
        exit(EXIT_FAILURE);
    }


    char address;
    char operation;

    while(fscanf(trace_p, " %c %s", &operation, &address) > 0){ //space is included in front bc I should be ignored 
//use fscan or gets or getline 
//need to tell it to read until EOF or EOL
        
        switch(operation)
        {
            case 'L':
                //call to function called load 
                break;
            case 'M':
                //call to function data modify
                break;
            case 'S':
                //call a fucntion to data store
                //store
                break;
            default:
                break;
        }

    }
    fclose(trace_p);
}

void load(char address)
{
    


}


void store(char address)
{
    


}


void modify(char address)
{
    


}


int interp_address(char add){
    int tag;
    //int address;
    address = atoi(add);

    tag = address >> (offset_bits + set_bits);

    unsigned int set_indx = address >> offset_bits & ((1 << set_bits) -1);

    //should i set my_set index to set index?

//need to consider empty block case and how to best use lru
int bl_empty = -1;
int bl_evict = 0;



    for(int i =0; i < associativity; i++){
        if (my_cache[set_indx].my_set[i].valid_bit){
            if(my_cache[set_indx].my_set[i].tag == tag){
                hits++;
                my_cache[set_indx].my_set[i].lru = 1;
                return 0; //or return hits?
            }
            if(my_cache[set_indx].my_set[i].lru > my_cache[set_indx].my_set[bl_evict].lru){
                bl_evict = i;
            }
        }
        else{
            bl_empty = i;
        }
    }
    miss++;
   // for(int j = 0; j < associativity; j++){
    //    if(my_cache[set_indx].my_set[j].lru ){ //if was most rtesently used evict it 


     //   }
    //}

    if(bl_empty != -1){
       
       
        my_cache[set_indx].my_set[bl_empty].tag = tag;
        my_cache[set_indx].my_set[bl_empty].valid_bit = valid_bit;
        my_cache[set_indx].my_set[bl_empty].lru = 1;
        
        return 1; 
    }
    else{

        my_cache[set_indx].my_set[bl_evict].tag = tag;
        my_cache[set_indx].my_set[bl_evict].lru = 1;

        evictions++;
    }


}

int main(int argc, char **argv)  //int is number of args char is strings part of that arg list (list of strings bascvially) 
{
   // int set_bits = 0;
   // int associativity = 0;
   // int offset_bits = 0;
   // char *trace_file = NULL;


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
//functions to modify cache
//insert 
//modify
//delete
//mechanism to keep tack of hits misses and evictions

//need to figure out based on addrerss where it should go and based on size how much size it should take up
