#include "title.h"
#include <stdio.h>
void title(void){
    FILE *infile;
    char input[255];
    infile = fopen("design.txt", "r");
    char status;
    /*fscanf(infile, "%d", &myint);*/
    if (infile == NULL){
        printf("cannot open file");
    } else {
        do {
            status = fgets(input, sizeof(input), infile);
            if (status != 0){
                printf("%s\n", input);
            }
        } while (status);

        }
    fclose(infile);
    return;
}
