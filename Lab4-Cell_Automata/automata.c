// Lab2
// Xin Xiang N12021000
// April 18, 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    // store number of generations
    int num_of_generation=atoi(argv[1]);

    // test for wrong number input
    if (num_of_generation<=0) {
        printf("Error: Wrong Input of Number of Generations");
        exit(1);
    }
    
    // open files
    FILE *fi=fopen(argv[2], "r");
    FILE *fo=fopen(strcat(argv[2],".out"), "w");

    // test for files not existing
    if (fi==NULL || fo==NULL) {
        printf("Error: file cannot be opened.\n");
        exit(1);
    } 

    // cellular automaton begins

    // create two one-dimensional matrices
    char initial[25];
    char result[25];

    // copy the data from the input file into the initial matrix
    for (int i=0; i<5; i++) {
        fscanf(fi,"%c %c %c %c %c\n", &initial[i*5], &initial[i*5+1],  
            &initial[i*5+2], &initial[i*5+3], &initial[i*5+4]);
    }

    // for each generation
    for (int i=0; i<num_of_generation; i++) {

        // for each cell
        for (int j=0; j<25; j++) {

            // check 8 neighbors
            int count=0;

            if (j-6>=0 && j-6<25 && initial[j-6]=='1') {
                count++;
            }

            if (j-5>=0 && j-5<25 && initial[j-5]=='1') {
                count++;
            }

            if (j-4>=0 && j-4<25 && initial[j-4]=='1') {
                count++;
            }

            if (j-1>=0 && j-1<25 && initial[j-1]=='1') {
                count++;
            }

            if (j+1>=0 && j+1<25 && initial[j+1]=='1') {
                count++;
            }

            if (j+4>=0 && j+4<25 && initial[j+4]=='1') {
                count++;
            }

            if (j+5>=0 && j+5<25 && initial[j+5]=='1') {
                count++;
            }

            if (j+6>=0 && j+6<25 && initial[j+6]=='1') {
                count++;
            }

            // decide the state of the cell and update the cell's state

            // cells alive
            if (initial[j]=='1') {
                if (count<2 || count>3) {
                    result[j]='0';
                } else {
                    result[j]='1';
                }
            }

            //cells dead
            if (initial[j]=='0') {
                if (count==3) {
                    result[j]='1';
                } else {
                    result[j]='0';
                }
            }
        }

        // copy data from result to initial
        for (int k=0; k<25; k++) {
            initial[k]=result[k];
        }  
    }

    // write the result matrix data into output file
    for (int i=0; i<25; i++) {
        fprintf(fo, "%c ", result[i]);
        if ((i+1)%5==0) {
            fprintf(fo,"\n");
        }
    }  

    //close the file
    fclose(fi);
    fclose(fo);

    return 0;
}