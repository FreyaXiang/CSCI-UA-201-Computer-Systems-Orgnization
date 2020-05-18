// Xin Xiang xx985

#include "lab3.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) 
{
    // read s E b and file name from command line
    // using getopt()
    int opt;
    int S;
    int s;
    int b;
    int E;
    char *fileName;

    while((opt = getopt(argc, argv, ":s:E:b:t:")) != -1)  
    {  
        switch(opt)  
        {  
            case 's': 
                S=1<<atoi(optarg);   
                s=atoi(optarg);
                break;
            case 'E':  
                E=atoi(optarg);
            case 'b':  
                b=atoi(optarg);  
                break;  
            case 't':  
                fileName=optarg;  
                break;  
            case ':':  
                printf("option needs a value\n");  
                break;  
            case '?':  
                printf("unknown option: %c\n", optopt); 
                break;  
        }  
    }

    // create the cache
    // using malloc
    int *cache=malloc(sizeof(int)*S*E*3); // one for valid bit, one for LRU, one for tag

    // set the valid bits to 0 (0/1)
    // set LRU bits to 0 (0/1)
    int* ptr=cache;
    for (int i=0; i<S*E; i++) {
        *ptr=0;  // valid bits
        *(ptr+1)=0;  // LRU
        ptr+=2;
    }

    // open and read the file
    FILE *fi=fopen(fileName, "r");

    // test for files not existing
    if (fi==NULL) 
    {
        printf("Error: file cannot be opened.\n");
        exit(1);
    } 

    // cache simulator begins

    // counters
    int hit=0;
    int miss=0;
    int eviction=0;

    char line[20]; 
    while ( fgets( line, 20, fi ) != NULL ) 
    { 
        // ignore instruction load
        if (line[0]=='I') {
            continue;
        }

        // store mode: L, S, or M
        char mode=line[1]; 

        // get addresses
        // how many hex 
        int countHEX=0;
        for (int i=3; i<strlen(line); i++) {
            if (line[i]==',') {
                break;
            } else {
                countHEX++;
            }
        }
        
        // convert to int addresses
        char addr_str[countHEX+1];
        memcpy(addr_str,line+3,countHEX);
        addr_str[countHEX]='\0';
        int addr_int=0;
        char *addr_ptr=addr_str;
        while (*addr_ptr) {
            addr_int<<=4;
            if (*addr_ptr>='a' && *addr_ptr<='f') {
                addr_int|=(int)(*addr_ptr-'a')+10;
            } else {
                addr_int|=(int)(*addr_ptr-'0');
            }
            addr_ptr+=1;
        }

        // get tag and index
        int index=(addr_int>>b) & ((1<<s)-1);
        int tag=addr_int>>(s+b);

        // find the set based on the index
        int* block_to_start=cache+index*E*3;

        // cache simulation begins
        int isHit=0;
        int* cmp_ptr=block_to_start; 
        int Mcount=0;
        while (1) {
            for (int i=0; i<E; i++) {
                // if not valid
                if (*cmp_ptr==0) {
                    cmp_ptr+=3;
                    continue;
                } else {
                    // compare tag
                    if (*(cmp_ptr+2)==tag) {
                        // hit!
                        hit++;
                        // decrement/remain same (set other LRU bit in the same set)
                        int *inner_ptr=block_to_start;
                        for (int i=0;i<E;i++) {
                            if (*(inner_ptr+1)>*(cmp_ptr+1)) {
                                (*(inner_ptr+1))--;
                            }
                            inner_ptr+=3;
                        }
                        // set LRU bit to E (most recently)
                        (*(cmp_ptr+1))=E;

                        isHit=1;
                        break;

                    // different tags
                    } else {
                        cmp_ptr+=3;
                    }
                }
            }
            // if miss
            cmp_ptr=block_to_start;
            if (!isHit) {
                miss++;
                int needEvic=1;
                for (int i=0; i<E; i++) {
                    // if the set has empty block, store the address to the empty block
                    if (*cmp_ptr==0) {
                        // set valid
                        *cmp_ptr=1;
                        // decrement/remain same (set other LRU bit in the same set)
                        int *inner_ptr=block_to_start;
                        for (int i=0;i<E;i++) {
                            if (*(inner_ptr+1)>*(cmp_ptr+1)) {
                                (*(inner_ptr+1))--;
                            }
                            inner_ptr+=3;
                        }
                        // set LRU bit to E (most recently)
                        (*(cmp_ptr+1))=E;
                        // store tag
                        *(cmp_ptr+2)=tag;
                        // do not need to evict
                        needEvic=0;
                        break;
                    } else {
                        cmp_ptr+=3;
                    }
                }
                // eviction
                cmp_ptr=block_to_start;
                if (needEvic) {
                    // using LRU to find the block to be evicted
                    for (int i=0; i<E; i++) {
                        if (*(cmp_ptr+1)==1) { // find the block that should be invicted
                            // decrement/remain same (set other LRU bit in the same set)
                            int *inner_ptr=block_to_start;
                            for (int i=0;i<E;i++) {
                                if (*(inner_ptr+1)>*(cmp_ptr+1)) {
                                    (*(inner_ptr+1))--;
                                }
                                inner_ptr+=3;
                            }
                            // set LRU bit to E (most recently)
                            (*(cmp_ptr+1))=E;
                            // tag reset
                            *(cmp_ptr+2)=tag;
                            // increment inviction
                            eviction++;
                            break;
                        }
                        cmp_ptr+=3;
                    }    
                } 
            }

            Mcount++;
            
            if (mode=='L' || mode=='S' || Mcount==2) {
                break;
            } 
        }
    } 

    printSummary(hit, miss, eviction);

    //close the file
    fclose(fi);

    //free memory
    free(cache);

    return 0;
}