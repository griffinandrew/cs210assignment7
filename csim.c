//Griffin Heyrich gheyrich@bu.edu
//U42992446
//this program is designed to simulate the cache it reads in the relevant information to a trace in main, then the trace file is sent to read tracefile where the reading of each inddivdial operation is read in and sent to that operation for simualtion
//each operation uses cache simulate op to simulate the that operation in the cache if a hit is determined the global counter is incremented and the function returns if it is a miss the set index is sent to find evict idex to determine what index 
//needs to be evicted if the valid bit is 1 eviction is incremented, if its a miss or needs to be evicted then the attributes of the index are updated
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

int sets; //number of sets
int assoc; //allocation
int bytes; //offset

//counters for relevant outcomes
int hits = 0;
int miss = 0;
int evictions = 0;

//lru counter
int counter = 0; //update lru with value of counter 

//struct for each block
typedef struct Block{ //block = line
    unsigned long long int tag;
    int valid_bit;
    int lru; //time stamp //lru will be 1 for most recent used?
    //clock_t time;
} block;

//struct for each set
typedef struct Set{
    unsigned int index; 
    block *my_set; //array of sets in struct
} set;

set *my_cache;

//globals assigned in main to be used in other function for relevant calculations
int set_bit;
int ass; 
int off_bits;

void cache_simulate_op(unsigned long long int address);
void load(unsigned long long int address);
void modify(unsigned long long int address);
void store(unsigned long long int address);
int find_block_to_evict(unsigned long long set_index);



//this functiuon is used to create the cahce for simulation it uses the given information from main to determine the number of sets, associativty and offset in bytes 
//from these features i allocate the relevant size and initialize all set attributes to -1
void create_cache(int set_bits, int assoc, int offset_bits){
    
    sets = 1 << set_bits; //left shift same as mult by 2
    assoc = assoc;
    bytes = 1 << offset_bits;

    my_cache = (set *)malloc(sets * sizeof(set) ); //alloc space for cache

    int i =0;
    int j =0;
    for (i= 0; i < sets; i++)
    {
        my_cache[i].index = i;
        my_cache[i].my_set = (block *) malloc(assoc * sizeof(block)); //allocate my set within each set of size block

        for (j = 0; j < assoc; j ++) //each block within set intialize there starting information to 0
        {
            my_cache[i].my_set[j].tag = 0;
            my_cache[i].my_set[j].valid_bit = 0;
            my_cache[i].my_set[j].lru = 0;
        }
    }
}


/*
void dealloc_cache() {
    int i, j;
    sets = (1 << set_bit);
    for (i =0; i < sets; i++){
        for(j = 0; j < ass; j++){
            void * p = (void*)my_cache[i].my_set[j];
            free(p); 
        }
        
    }
    free(my_cache);
}

*/

//this function takes in the trace file, opens it for reading, and then loops through while scanning in the revelant information from the file 
//it uses a switch statement from the operation to send control to the functions of that relevant operation then closes the file after reading
void read_trace_file(char *trace) //this will need to be called after t in switch statement
{

    unsigned long long int address;
    char operation;
    int size;

    if(trace == NULL){ //if the trace file is null print message and display error
        printf("file is null\n");
         exit(EXIT_FAILURE);
    }
    FILE* trace_p = fopen(trace, "r"); //read in the trace file

    if (trace_p == NULL){//if the file pointer is null print message and display error
        printf("file failed to open\n");
        exit(EXIT_FAILURE);
    }

    while(fscanf(trace_p, " %c %llx,%d\n", &operation, &address, &size) > 0){ //this reads in each given description of the trace and sets the variables to the scanned in values

        switch(operation)   //a switch statement is used here to pass control to each different command read in 
        {
            case 'L':
                load(address); //if load op call load with address
                break;
            case 'M':
                modify(address); //if modify op call modify with address
                break;
            case 'S':
                store(address); //if store op call store with address
                break;
            default: //deafult just break
                break;
        }
    }
    fclose(trace_p); //close the file after all attributes have been read
}

//this fucntion isn meant to simualte the load operation it takes the address and passes it to cache simualate op to be interpreted
void load(unsigned long long int address)
{
    
    cache_simulate_op(address);

}

//this function is meant to take in the given address and simulate the store operation it does so by calling cache simualate op to be interpret and run the op
void store(unsigned long long int address)
{

    cache_simulate_op(address);

}

//this function is meant to take in the given address and simulate the modify operation by calling twice to cache simulate op to simulate a load then store 
void modify(unsigned long long int address)
{
    
    cache_simulate_op(address);
    cache_simulate_op(address);

}

//this function is meant to simulate all the relavent operations for a cache. it does so first by calculating the tag and set index from the address as well as set bits and offset bits using bit manipulation
//it then loops through trying to that index trying to identify a valid bit and tag match if it does so a hit is recorded the lru counter incremented and the function returns
//if a hit is not found a miss has occured, the set idx is sent to find block to evict to determine proper eviction block if that block has a valid bit an eviction takes place if not no eviction, in any case the attributes of that idx are updated
void cache_simulate_op(unsigned long long int address){
    
    int i; 
    int evict_idx;
    unsigned long long int tag;
    unsigned long long int set_indx;

    tag = address >> (off_bits + set_bit); //calcualte the tag 

    set_indx = (address >> off_bits) & ((1 << set_bit) -1); //calcualte the set index
    
   for(i = 0; i < ass; i++){ 
        if (my_cache[set_indx].my_set[i].valid_bit == 1){
            if(my_cache[set_indx].my_set[i].tag == tag){ //if the block has a valid bit and the tags match, a hit takes place
                hits++; //increment hits
                my_cache[set_indx].my_set[i].lru = counter++; //increment the counter and assign it to that hits lru
                return;
            }
        }
    }
    miss++; //if a hit was not found increment miss 

    evict_idx = find_block_to_evict(set_indx); //send the set index to the idx to evict to determine what idx needs to be evicted using lru

    if(my_cache[set_indx].my_set[evict_idx].valid_bit ==1 ){ //if the idx to evict has a valid bit evict 
        evictions++; //increment evictions 
    }
    
    my_cache[set_indx].my_set[evict_idx].tag = tag;   //assign tag, counter incremented and valid bit to 1 for the idx to be evicted
    my_cache[set_indx].my_set[evict_idx].lru = counter++;
    my_cache[set_indx].my_set[evict_idx].valid_bit = 1;
    
}



//the purpose of this function is to determine the idxk to be evicted given the set index the function loops through and determines the idx with the lowest possible counter value or lru and returns the index of that value
int find_block_to_evict(unsigned long long set_index){

unsigned long long set_indx = set_index;
int lru_to_evict = INT_MAX;
int evict_idx = 0;
int j;

    for(j = 0; j < ass; j++){ //get the idx in that set with the lowest possible counter 
        if(lru_to_evict > my_cache[set_indx].my_set[j].lru){  //if lru to evict is greater than j lru value 
            evict_idx = j; //the one to evict is j
            lru_to_evict = my_cache[set_indx].my_set[j].lru;  
        }
    }
    return evict_idx; //return index to be evicted
}

//this is the main control center of the program, intially the getopt function is used to determine and set the relevant information for the file that will be read in 
//after that the values read in are sent to create cache to form a cache specific to these requirements, after that the tracefile is sent to 
int main(int argc, char **argv)
{
    //attributes to be read in
    int set_bits = 0;
    int associativity = 0;
    int offset_bits = 0;
    char *trace_file = NULL;
    char opt;

    while(1)
    {

        opt = getopt(argc, argv, "vhs:E:b:t:"); //looks at argv and tries to match it with one of those things if found opt gives letter that first found
        
        if (opt == -1) //if it fails break loop
        {
            break;
        }

        switch(opt)
        {
            case 's':
                set_bits = atoi(optarg); //atoi turns strong to int if possible
                set_bit = set_bits; //assign to global var for use outside of main
                break;
            case 'E':
                associativity = atoi(optarg);//atoi turns strong to int if possible
                ass = associativity;//assign to global var for use outside of main
                break;
            case 'b':
                offset_bits = atoi(optarg);//atoi turns strong to int if possible
                off_bits = offset_bits;//assign to global var for use outside of main
                break;
            case 't':
                trace_file = optarg;
                break;
            default:
                break;
        }

    }


    create_cache(set_bits, associativity, offset_bits); //initialize the cache with the read in requirements

    read_trace_file(trace_file); //send the tracefile to read trace

    printSummary(hits, miss, evictions); //print the results
    return 0;
}

