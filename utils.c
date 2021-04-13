#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"

/*!
 * this function converts the instruction to machine code
 * @param command - all the fields of the instruction
 * @param opcode - the opcode desired according to the command
 * @param funct - the funct num desired according to the command
 * @return machine word according to the given fields
 */
union machine_word convertInstructionToCode(input_attr command, int opcode, int funct) {
    union machine_word result;
    code_word *res = (code_word *) malloc(sizeof(code_word));
    if (!res) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    } else {
        res->ARE = ABSOLUTE;
        res->opcode = opcode;
        res->funct = funct;
        res->source = command.param.source_op_type;
        res->target = command.param.target_op_type;
        result.code = res;
        return result;
    }
}
/*!
 * this function converts the source/target ops to machine word code
 * @param command - all the fields of the instruction
 * @param table - the symbol table
 * @param operand - the given operand (source/target)
 * @param ic - current instruction counter
 * @param ext_lst - the list of externals
 * @return machine word according to the given fields
 */
union machine_word
extractOperandsToCode(input_attr *command, symbol_table *table, int operand, int ic, external_list **ext_lst) {
    union machine_word result;
    table_row *symbol;
    data_word *res = (data_word *) malloc(sizeof(data_word));
    if (!res) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    } else {
        switch (operand) {
            case SOURCE:
                switch ((*command).param.source_op_type) {
                    case IMMEDIATE:
                        res->ARE = ABSOLUTE;
                        res->data = (*command).param.source_immediate;
                        break;
                    case DIRECT:
                        symbol = getSymbol(&table, (*command).param.source_op);
                        if (!symbol) {
                            (*command).error = UNDEFINED_SYMBOL;
                            break;
                        } else {
                            if (symbol->is_external) {
                                addExternal(ext_lst, (*command).param.source_op, ic);
                                res->ARE = EXTERNAL;
                            } else {
                                res->ARE = RELOCATABLE;
                            }
                            res->data = symbol->value;
                            break;
                        }
                    case RELATIVE:
                        break;
                    case REGISTER:
                        res->ARE = ABSOLUTE;
                        res->data = pow(2, (((*command).param.source_register) - 1));
                        break;
                }
                break;
            case TARGET:
                switch ((*command).param.target_op_type) {
                    case IMMEDIATE:
                        res->ARE = ABSOLUTE;
                        res->data = (*command).param.target_immediate;
                        break;
                    case DIRECT:
                        symbol = getSymbol(&table, (*command).param.target_op);
                        if (!symbol) {
                            (*command).error = UNDEFINED_SYMBOL;
                            break;
                        } else {
                            if (symbol->is_external) {
                                addExternal(ext_lst, (*command).param.target_op, ic);
                                res->ARE = EXTERNAL;
                            } else {
                                res->ARE = RELOCATABLE;
                            }
                            res->data = symbol->value;
                            break;
                        }
                    case RELATIVE:
                        symbol = getSymbol(&table, (*command).param.target_op + 1);
                        if (!symbol) {
                            (*command).error = UNDEFINED_SYMBOL;
                            break;
                        } else {
                            if (symbol->is_external) {
                                (*command).error = RELATIVE_USE_EXTERNAL;
                            } else {
                                res->ARE = ABSOLUTE;
                                res->data = ((symbol->value) - ic - 100);
                            }
                            break;
                        }
                    case REGISTER:
                        res->ARE = ABSOLUTE;
                        res->data = pow(2, (((*command).param.target_register) - 1));
                        break;
                }
                break;
        }
        result.data = res;
        return result;
    }
}
/*!
 * this function assemble 2 given strings
 * @param s1 - first string
 * @param s2 - second string
 * @return string that include "s1"+"s2"
 */
char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if (!result) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/*!
 * this function converts integer that represent char 'A'/'R'/'E' to the actual letters
 * @param ARE - the integer
 * @return the char 'A'/'R'/'E'
 */
char getLineARE(int ARE) {
    switch (ARE) {
        case ABSOLUTE:
            return 'A';

        case RELOCATABLE:
            return 'R';

        default:
            return 'E';
    }
}

/*!
 * this function adding an external value to the external's list
 * @param ext_lst - the list we want to add to
 * @param name - the name of the external
 * @param IC - the current instruction counter
 */
void addExternal(external_list **ext_lst, char *name, int IC) {
    int empty_list = FALSE;
    external_node *prev, *curr;
    char *new_external_name;
    curr = (*ext_lst)->head;
    if (!curr) {
        empty_list = TRUE;
    }
    while (curr != NULL) {
        prev = curr;
        curr = (external_node *) curr->next;
    }
    curr = (external_node *) malloc(sizeof(external_node));
    if (!curr) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    } else {
        new_external_name = (char *) malloc(MAX_LABEL_SIZE * sizeof(char));
        {
            if (!new_external_name) {
                printf("Failed to allocate memory, Please restart");
                exit(0);
            } else {
                strcpy(new_external_name, name);
                curr->name = new_external_name;
                curr->line_number = IC + 100;
                curr->next = NULL;
                if (empty_list) {
                    (*ext_lst)->head = curr;
                } else {
                    prev->next = (struct external_node *)curr;
                }
            }
        }
    }
}

/*!
 * this function free any allocation memory that allocates for the instruction
 * @param command - the command that contains the memory objects
 */
void freeUnnecessaryCommandMemory(input_attr *command) {
    if ((*command).line_type == COMMAND) {
        if ((*command).param.source_op) {
            free((*command).param.source_op);
        }
        if ((*command).param.target_op) {
            free((*command).param.target_op);
        }
    }
    if ((*command).line_type == STATEMENT) {
        switch ((*command).statement) {
            case DATA:
                if ((*command).param.arr) {
                    free((*command).param.arr);
                }
                break;
            case STRING:
            case ENTRY:
            case EXTERN:
                if ((*command).param.target_op) {
                    free((*command).param.target_op);
                }
                break;
        }
    }

}

/*!
 * this function free any allocation memory that allocates for the main program
 * @param table - the symbol table
 * @param ext_lst - the external's list
 * @param data_img - the data image
 */
void freeAllocatedMemory(symbol_table *table,external_list *ext_lst,data_word *data_img){
    external_node *ext_next, *ext_curr;
    table_row *symbol_curr,*symbol_next;

    /* free the symbol table */
    symbol_curr = (*table).head;
    while (symbol_curr != NULL) {
        symbol_next = (table_row *) symbol_curr->next;
        free(symbol_curr->symbol_name);
        free(symbol_curr);
        symbol_curr = symbol_next;
    }

    /* free the external list */
    ext_curr = (*ext_lst).head;
    while (ext_curr != NULL) {
        ext_next = (external_node *) ext_curr->next;
        free(ext_curr->name);
        free(ext_curr);
        ext_curr = ext_next;
    }

    /* free data_image */
    free(data_img);
}

