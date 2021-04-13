#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char *errors2[] = { /* converts the errors to string */
        "Illegal comma", "Illegal label name", "Missing comma", "Argument is not an integer", "Undefined command name",
        "Extraneous text after end of command", "Missing argument", "Multiple consecutive commas", "Incorrect operand",
        "Undefined statement name", "Missing Quote", "Undefined symbol", "Relative use external symbol",
        "Symbol cannot be both entry and extern", "This symbol is already defined","Immediate given is overflow"

};
/*!
 * this function is responsible to convert the instruction into machine word line by line
 * @param file_name - the name of the file we're working on
 * @param IC - the instruction count
 * @param DC - the data count
 * @param table - the symbol table
 * @param data_img - the data image
 * @param ins_img - the instruction image
 * @param ext_lst - the externals list
 * @return 0 if everything is ok , and 1 while error has been occur
 */
bool
secondRun(char file_name[], int *IC, const int *DC, symbol_table *table, data_word *data_img, machine_word *ins_img,
          external_list *ext_lst) {
    FILE *input_file;
    char line[MAX_INPUT];
    int line_counter = 1;
    int i = 0, j, error = FALSE;
    ext_lst->head = NULL;
  

    if (!(input_file = fopen(file_name, "r"))) {
        fprintf(stderr, "cannot open input file: %s\n", file_name);
        return TRUE;
    }
    while (fgets(line, sizeof(line), input_file)) {     /* parsing line by line in the input file*/
        input_attr command;
        command = convertLineToInputAttr(line);

        if (command.line_type == COMMAND && command.error == None) {
            switch (command.funct_num) {
                case MOV:
                    ins_img[i] = convertInstructionToCode(command, ZERO, ZERO);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, SOURCE, i, &ext_lst);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case CMP:
                    ins_img[i] = convertInstructionToCode(command, ONE, ZERO);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, SOURCE, i, &ext_lst);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case ADD:
                    ins_img[i] = convertInstructionToCode(command, TOW, TEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, SOURCE, i, &ext_lst);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case SUB:
                    ins_img[i] = convertInstructionToCode(command, TOW, ELEVEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, SOURCE, i, &ext_lst);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case LEA:
                    ins_img[i] = convertInstructionToCode(command, FOUR, ZERO);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, SOURCE, i, &ext_lst);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case CLR:
                    ins_img[i] = convertInstructionToCode(command, FIVE, TEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case NOT:
                    ins_img[i] = convertInstructionToCode(command, FIVE, ELEVEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case INC:
                    ins_img[i] = convertInstructionToCode(command, FIVE, TWELVE);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case DEC:
                    ins_img[i] = convertInstructionToCode(command, FIVE, THIRTEEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case JMP:
                    ins_img[i] = convertInstructionToCode(command, NINE, TEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case BNE:
                    ins_img[i] = convertInstructionToCode(command, NINE, ELEVEN);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case JSR:
                    ins_img[i] = convertInstructionToCode(command, NINE, TWELVE);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case RED:
                    ins_img[i] = convertInstructionToCode(command, TWELVE, ZERO);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case PRN:
                    ins_img[i] = convertInstructionToCode(command, THIRTEEN, ZERO);
                    i++;
                    ins_img[i] = extractOperandsToCode(&command, table, TARGET, i, &ext_lst);
                    i++;
                    break;
                case RTS:
                    ins_img[i] = convertInstructionToCode(command, FOURTEEN, ZERO);
                    i++;
                    break;
                case STOP:
                    ins_img[i] = convertInstructionToCode(command, FIFTEEN, ZERO);
                    i++;
                    break;
            }
        }
        if (command.line_type == STATEMENT && command.error == None) {
            table_row *symbol;
            switch (command.statement) {
                case DATA:
                case STRING:
                    break;
                case ENTRY:
                    symbol = getSymbol(&table, command.param.target_op);
                    if (symbol->is_external) {
                        command.error = SYMBOL_IS_EXTERNAL_AND_ENTERY;
                    } else {
                        symbol->is_entry = TRUE;
                    }
                    break;
                case EXTERN:
                    break;
            }

        }
        if (command.error != None) {
            if (command.error == SYMBOL_IS_EXTERNAL_AND_ENTERY || command.error == UNDEFINED_SYMBOL) {
                fprintf(stderr, "File : %s\nFound error : %s\nLine: %d\n", file_name, errors2[command.error],
                        line_counter);
            }
            error = TRUE;
            line_counter++;
            freeUnnecessaryCommandMemory(&command);
            continue;
        }
        freeUnnecessaryCommandMemory(&command);
        line_counter++;
    }

    for (j = 0; j < *DC; i++, j++) {
        data_word *res = (data_word *) malloc(sizeof(data_word));
        if (!res) {
            printf("Failed to allocate memory, Please restart");
            exit(0);
        } else {
            res->ARE = data_img[j].ARE;
            res->data = data_img[j].data;
            ins_img[i].data = res;
        }
    }
    *IC += *DC;
    fclose(input_file);
    return error;
}

