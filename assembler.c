/***
maman 23 ex 2
@author Yarden Frenkel
id 315329490

**/

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


int main(int argc, char *argv[]) {
    char *file_name,*as_file_name;
    int status;
    int IC, DC, i;
    union machine_word machine_image[IMAGE_SIZE];
    data_word *data_image;
    symbol_table symbol_table;
    external_list ext_lst;
    for (i = 1; i < argc; i++) {
        status = FALSE;
        file_name = argv[i];
        as_file_name = concat(file_name, ".as");
        data_image = (data_word *) malloc(IMAGE_SIZE * sizeof(data_word));
        if (!data_image) {
            printf("Failed to allocate memory, Please restart");
            exit(0);
        } else {
            status = firstRun(as_file_name, &IC, &DC, &symbol_table, data_image);
            updateSymbolTableValues(&symbol_table, IC);
            status += secondRun(as_file_name, &IC, &DC, &symbol_table, data_image, machine_image, &ext_lst);
            if (!status) {
                createOutputFiles(machine_image, file_name, IC, DC, symbol_table, ext_lst);
            }
            freeAllocatedMemory(&symbol_table, &ext_lst, data_image);
            free(as_file_name);
        }
    }
    return 0;
}
