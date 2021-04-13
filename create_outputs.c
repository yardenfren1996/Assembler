#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*!
 * this fiction gets all the collected data from the former steps and converts it to the output files
 * @param machine_image - the machine image
 * @param file_name - the name of the file we're working on
* @param IC - the instruction count
 * @param DC - the data count
 * @param table - the symbol table
 * @param ext_lst - the external's list
 */
void createOutputFiles(machine_word *machine_image,char * file_name ,int IC ,int DC, symbol_table table, external_list ext_lst){
    int entry_flag = FALSE;
    table_row *entry;
    external_node *external;
    int i,line_number;
    char ARE;
    FILE  *output_file;
    char *output = concat(file_name, ".ob");
    if (!(output_file = fopen(output, "w"))) {
        fprintf(stderr, "cannot create output file \n");
        free(output);
        exit(0);
    }
    free(output);

    fprintf(output_file,"\t%d\t%d\n",IC-100-DC,DC);

    for (i = 0; i < IC-100 ; i++) {
        line_number = 100 + i;
        ARE = getLineARE(machine_image[i].data->ARE);
        fprintf(output_file,"%04d\t%03X\t\t%c\n",line_number,machine_image[i].data->data,ARE);
    }
    fclose(output_file);

    entry = table.head;
    while (entry != NULL) {
        if (entry->is_entry) {
            if (!entry_flag){
                output = concat(file_name, ".ent");
                if (!(output_file = fopen(output, "w"))) {
                    fprintf(stderr, "cannot create output file \n");
                    free(output);
                    exit(0);
                }
                free(output);
                entry_flag = TRUE;
            }
            fprintf(output_file, "%s\t%04d\n", entry->symbol_name, entry->value);
        }
        entry = (table_row *) entry->next;
    }
    if (entry_flag){
        fclose(output_file);
    }
    if ((external = ext_lst.head)){
        output = concat(file_name, ".ext");
        if (!(output_file = fopen(output, "w"))) {
            fprintf(stderr, "cannot create output file \n");
            free(output);
            exit(0);
        }
        free(output);
        while (external){
            fprintf(output_file,"%s\t%04d\n",external->name,external->line_number);
            external = (external_node *) external->next;
        }
        fclose(output_file);
    }

}
