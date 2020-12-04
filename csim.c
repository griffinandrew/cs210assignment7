//Griffin Heyrich gheyrich@bu.edu
//U42992446
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>


int sets; //number of sets
int assoc; //allocation
int bytes;

int hits = 0;
int miss = 0;
int evictions = 0;


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


 
void interp_address(unsigned long long int address);
void load(unsigned long long int address);
void modify(unsigned long long int address);
void store(unsigned long long int address);

//blocks here is essetianlly a cache line


//this functiuon is used to create the cahce for simulation it uses the given information from main to determine the number of sets, associativty and offset in bytes 
//from these features i allocate the relevant size and initialize all set attributes to -1
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


/*
void dealloc_cache() {
    int i; //, j;
    sets = (1 << set_bit);
    for (i =0; i < sets; i++){
        //for( j=0; j < ass; j++){
            free((void *)my_cache[i]); //.my_set[j]);
       // }
        //free(my_cache[i])
    }
    free(my_cache);
   // free(my_set);
}

*/

//this function takes in the trace file, opens it for reading, and then loops through while scanning in the revelant information from the file 
//it uses a switch statement from the operation to send control to the functions of that relevant operation then closes the file after reading
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


    unsigned long long int address; //WAIT SHOULD BE ADDRESS[]?    //wait address should be unsigned long long 
    char operation;
    int size;

    while(fscanf(trace_p, " %c %llu,%d\n", &operation, &address, &size) > 0){ //space is included in front bc I should be ignored 
//use fscan or gets or getline 
//need to tell it to read until EOF or EOL
   // address = (uint64_t) address;

        switch(operation)
        {
            case 'L':
                //printf("%c %llu,%d\n", operation, address, size);
                load(address);
                printf("%c %llu,%d\n", operation, address, size);
                break;
            case 'M':
               // printf("%c %llu,%d\n", operation, address, size);
                modify(address);
                printf("%c %llu,%d\n", operation, address, size);
                break;
            case 'S':
                //printf("%c %llu,%d\n", operation, address, size);
                store(address);
                printf("%c %llu,%d\n", operation, address, size);
                break;
            default:
                break;
        }

    }
    fclose(trace_p);
}

void load(unsigned long long int address)
{
    
    interp_address(address);

}


void store(unsigned long long int address)
{

    interp_address(address);

}

//this function is meant to take in the given address and simulate the modify operation by calling twice 
void modify(unsigned long long int address)
{
    
    interp_address(address);
    interp_address(address);

}


void interp_address(unsigned long long int address){ //maybe i should break this up
   // printf("address: %llu      ", address);
    unsigned long long int tag;
    //char *addy = &add; //need to solve this 
    //int address = atoi(addy);

    tag = address >> (off_bits + set_bit);
    printf("tag: %llu      ", tag);
    unsigned long long int set_indx = (address >> off_bits) & ((1 << set_bit) -1);
    printf("indx: %llu      ", set_indx);
    //should i set my_set index to set index?

//need to consider empty block case and how to best use lru
    //int bl_empty = -1;
    int bl_evict = 0;
    int i, j;

//this needs to be slightly diff for modify
    for(i = 0; i < ass; i++){ //sets or assoc
        if (my_cache[set_indx].my_set[i].valid_bit){
            if(my_cache[set_indx].my_set[i].tag == tag){ //if hit do that stuff
                hits++;
                printf("hit    ");
                //counter++;
                my_cache[set_indx].my_set[i].lru = counter++; //or counter 
                //counter++;
                //return 0; //or return hits?
                return;
            }
        }
    }
    miss++;
    printf("miss    ");
    int lru_to_evict = INT_MAX; //just so it saves it for first case
    for(j = 0; j < ass; j++){ //bc ass is 1?
        if(my_cache[set_indx].my_set[j].lru  < lru_to_evict){
            bl_evict = j;
            printf("blk: %d one to evict     ", bl_evict);
            lru_to_evict = my_cache[set_indx].my_set[j].lru; //im setting t
        }
    }
    if(my_cache[set_indx].my_set[bl_evict].valid_bit){ //having == 1 causing error
        evictions++;
        printf("eviction    ");
    }
    my_cache[set_indx].my_set[bl_evict].tag = tag;
    my_cache[set_indx].my_set[bl_evict].lru = counter++;
    my_cache[set_indx].my_set[bl_evict].valid_bit = 1;
    
}


    //miss++;
   // for(int j = 0; j < associativity; j++){
    //    if(my_cache[set_indx].my_set[j].lru ){ //if was most rtesently used evict it 


     //   }
    //}
/*
    if(bl_empty > -1){ 
       
        miss++;
        //counter++;
        my_cache[set_indx].my_set[bl_empty].tag = tag;
        my_cache[set_indx].my_set[bl_empty].valid_bit = 1; //one to avoid error message
        my_cache[set_indx].my_set[bl_empty].lru = counter;
        counter++;
        return;
        //return 1; 
    }
    
    else{ //set is full need to evict
        miss++;
        my_cache[set_indx].my_set[bl_evict].tag = tag;
        my_cache[set_indx].my_set[bl_evict].lru = counter;
        counter++;
        evictions++;
        //return;
        //return 2; //bc miss and eviction
    }


}

*/


//this is the main control center of the program, intially the getopt function is used to determine and set the relevant information for the file that will be read in 
//after that the values read in are sent to create cache to form a cache specific to these requirements, after that the tracefile is sent to 
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
