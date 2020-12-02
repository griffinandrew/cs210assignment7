//Griffin Heyrich gheyrich@bu.edu
//U42992446
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdint.h>


int sets; //number of sets
int assoc; //allocation
int bytes;

int hits = 0;
int miss = 0;
int evictions =0;


int counter = 0; //update lru with value of counter 
//to evict go throiugh step block aby blovk and evict smallest one 

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



//took this out of main to use in other fucntions //could be cause of seg fault
int set_bit;
int ass; //if you change these to be globals and then send those locals in main to be stored for ref by other programs that might be the move
int off_bits;
//char *tr_file;



void interp_address(unsigned long long address);
void load(unsigned long long address);
void modify(unsigned long long address);
void store(unsigned long long address);

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
//going to need to dealloc this stuff



void read_trace_file(char *trace) //this will need to be called after t in switch statement
{
//char* trace_func = trace_file;
    if(trace == NULL){
        printf("BADDDDDDDD");
    }
//FILE* use fopen with r to read the data
    FILE* trace_p = fopen(trace, "r"); //why is this failing

    if (trace_p == NULL){
        printf("file failed to open\n");
        exit(EXIT_FAILURE);
    }


    unsigned long long address; //WAIT SHOULD BE ADDRESS[]?    //wait address should be unsigned long long 
    char operation;
    int size;

    while(fscanf(trace_p, " %c %llu,%d\n", &operation, &address, &size) > 0){ //space is included in front bc I should be ignored 
//use fscan or gets or getline 
//need to tell it to read until EOF or EOL
   // address = (uint64_t) address;

        switch(operation)
        {
            case 'L':
                load(address);
                break;
            case 'M':
                modify(address);
                //hits++;
                break;
            case 'S':
                store(address);
                break;
            default:
                break;
        }

    }
    fclose(trace_p);
}

void load(unsigned long long address)
{
    
    interp_address(address);

}


void store(unsigned long long address)
{

    interp_address(address);

}


void modify(unsigned long long address)
{
    
    interp_address(address);

}


void interp_address(unsigned long long address){ //m,aybe i should break this up
    int tag;
    //char *addy = &add; //need to solve this 
    //int address = atoi(addy);

    tag = address >> (off_bits + set_bit);

    unsigned int set_indx = (address >> off_bits) & ((1 << set_bit) -1);

    //should i set my_set index to set index?

//need to consider empty block case and how to best use lru
    int bl_empty = -1;
    int bl_evict = 0;
    int i;


    for(i =0; i < ass; i++){ //sets or assoc
        if (my_cache[set_indx].my_set[i].valid_bit){
            if(my_cache[set_indx].my_set[i].tag == tag){ //if hit do that stuff
                hits++;
                //counter++;
                my_cache[set_indx].my_set[i].lru = counter; //or counter 
                counter++;
                //return 0; //or return hits?
                return;
            }
            //counter++;
            //my_cache[set_indx].my_set[i].lru = counter; //i think need to incr so no prob with other
            if(my_cache[set_indx].my_set[i].lru >= my_cache[set_indx].my_set[bl_evict].lru){ //not hit look for lru to evict
                bl_evict = i;
            }
            //else if(!my_cache[set_indx].my_set[i].valid_bit && !my_cache[set_indx].my_set[i].lru){
            //    bl_empty = i;
            //}
       // }
        else if (bl_empty == -1){
           bl_empty = i;
        }
        }
    }
    //miss++;
   // for(int j = 0; j < associativity; j++){
    //    if(my_cache[set_indx].my_set[j].lru ){ //if was most rtesently used evict it 


     //   }
    //}

    if(bl_empty != -1){
       
        miss++;
        //counter++;
        my_cache[set_indx].my_set[bl_empty].tag = tag;
        my_cache[set_indx].my_set[bl_empty].valid_bit = 1; //one to avoid error message
        my_cache[set_indx].my_set[bl_empty].lru = counter;
        counter++;
        return;
        //return 1; 
    }
    
    else{
        miss++;
        
        my_cache[set_indx].my_set[bl_evict].tag = tag;
        my_cache[set_indx].my_set[bl_evict].lru = counter;
        counter++;
        evictions++;
        return;
        //return 2; //bc miss and eviction
    }


}

int main(int argc, char **argv)  //int is number of args char is strings part of that arg list (list of strings bascvially) 
{
    int set_bits = 0;
    int associativity = 0;
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
                set_bit = set_bits;
                break;
            case 'E':
                associativity = atoi(optarg);
                ass = associativity;
                break;
            case 'b':
                offset_bits = atoi(optarg);
                off_bits = offset_bits;
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

    printf("%d, %d, %d, %s\n", set_bits, associativity, offset_bits, trace_file);
    read_trace_file(trace_file);

    printSummary(hits, miss, evictions);
    return 0;
}


//need function to scan and opt through trace file
//functions to modify cache
//insert 
//modify
//delete
//mechanism to keep tack of hits misses and evictions

//need to figure out based on addrerss where it should go and based on size how much size it should take up
