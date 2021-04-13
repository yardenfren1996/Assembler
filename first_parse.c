#include <stdio.h>
#include <string.h>
#include "utils.h"

char *errors1[] = {
        "Illegal comma", "Illegal label name", "Missing comma", "Argument is not an integer", "Undefined command name",
        "Extraneous text after end of command", "Missing argument", "Multiple consecutive commas", "Incorrect operand",
        "Undefined statement name", "Missing Quote", "Undefined symbol", "Relative use external symbol",
        "Symbol cannot be both entry and extern", "This symbol is already defined","Immediate given is overflow"

};

/*!
 * this function is responsible to parse the file
 * extract the params from the instructions
 * increase the IC and DC according to the instructions
 * write the appropriate data in the data image
 * @param file_name - the name of the file we're working on
* @param IC - the instruction count
 * @param DC - the data count
 * @param table - the symbol table
 * @param data_img - the data image
 * @return 0 if everything is ok , and 1 while error has been occur
 */
bool firstRun(char file_name[], int *IC, int *DC, symbol_table *table, data_word *data_img) {
    FILE *input_file;
    int line_counter = 1;
    int i,error = FALSE;
    char line[MAX_INPUT];
    *IC = 100;                      /* on the first run we want to initiate  IC and DC*/
    *DC = 0;
    table->head = NULL;
    table->counter = 0;
    if (!(input_file = fopen(file_name, "r"))) {
        fprintf(stderr, "cannot open input file: %s\n",file_name);
        return TRUE;
    }
    while (fgets(line, sizeof(line), input_file)) {     /* parsing line by line in the input file*/
        input_attr command;
        command = convertLineToInputAttr(line);
        if (command.error != None) {
            fprintf(stderr, "File : %s\nFound error : %s\nLine: %d\n\n", file_name, errors1[command.error],
                    line_counter++);
            error = TRUE;
            freeUnnecessaryCommandMemory(&command);
            continue;
        } else {
            if (command.line_type == COMMAND) {
                if (command.label_flag) {
                    /** add to symbol table **/
                    addSymbol(&table, command.label_name, *IC, CODE_ATTR, &command.error);
                }
                /** increasing IC according to the operands**/
                switch (command.funct_num) {
                    case MOV:
                    case CMP:
                    case ADD:
                    case SUB:
                    case LEA:
                        *IC += THREE_LINES_INS;
                        break;
                    case CLR:
                    case NOT:
                    case INC:
                    case DEC:
                    case JMP:
                    case BNE:
                    case JSR:
                    case RED:
                    case PRN:
                        *IC += TOW_LINES_INS;
                        break;
                    case RTS:
                    case STOP:
                        *IC += ONE_LINE_INS;
                        break;
                }
            }
            if (command.line_type == STATEMENT) {
                if (command.label_flag && (command.statement == DATA || command.statement == STRING)) {
                    /** add to symbol table **/
                    addSymbol(&table, command.label_name, *DC, convertStatementToAttribute(command.statement),
                              &command.error);
                }
                /** increasing DC according to the data**/
                switch (command.statement) {
                    case DATA:
                        for (i = 0; i < command.data_counter; i++) {
                            data_img[*DC].ARE = ABSOLUTE;
                            data_img[*DC].data = command.param.arr[i];
                            (*DC)++;
                        }
                        break;
                    case STRING:
                        for (i = 0; i <= strlen(command.param.target_op); i++) {
                            data_img[*DC].ARE = ABSOLUTE;
                            data_img[*DC].data = command.param.target_op[i];
                            (*DC)++;
                        }
                        break;
                    case ENTRY:
                        break;
                    case EXTERN:
                        addSymbol(&table, command.param.target_op, 0, convertStatementToAttribute(command.statement),
                                  &command.error);
                        break;
                }

            }
        }

        if (command.error != None) {
            fprintf(stderr, "File : %s\nFound error : %s\nLine: %d\n", file_name, errors1[command.error],
                    line_counter);
            error = TRUE;
            line_counter++;
            freeUnnecessaryCommandMemory(&command);
            continue;

        }
        freeUnnecessaryCommandMemory(&command);
        line_counter++;
    }
    /** CLOSE THE FILE AND FREE **/
    fclose(input_file);
    return error;

}
