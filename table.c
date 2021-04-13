#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*!
 * this function gets a symbol in the symbol table and turns his attribute on
 * @param curr - the symbol node
 * @param attribute - the attribute that we want to set on
 */
void setAttributeOn(table_row *curr, int attribute) {
    switch (attribute) {
        case CODE_ATTR:
            curr->is_code = 1;
            break;
        case DATA_ATTR:
            curr->is_data = 1;
            break;
        case ENTRY_ATTR:
            curr->is_entry = 1;
            break;
        case EXTERN_ATTR:
            curr->is_external = 1;
            break;
    }
}

/*!
 * this function resets all of the symbol attrs
 * @param curr - the symbol node
 */
void resetAttributes(table_row *curr) {
    curr->is_code = 0;
    curr->is_data = 0;
    curr->is_entry = 0;
    curr->is_external = 0;
}

/*!
 * this function checks if the symbol is exist in the symbol table
 * case not adding it to the table
 * @param table - the symbol table
 * @param symbol_name - the symbol name as string
 * @param value - the instruction count or data count
 * @param attribute - the symbol attr that apears in the code
 * @param error - if there any error than change it inside the function
 */
void addSymbol(symbol_table **table, char *symbol_name, int value, int attribute, int *error) {
    int flag = 0;
    table_row *prev, *curr;
    char *new_symbol_name;
    curr = (*table)->head;
    while (curr != NULL && !flag) {
        if (strcmp(curr->symbol_name, symbol_name) == 0) {
            flag = 1;
        }
        if (!flag) {
            prev = curr;
            curr = (table_row *) curr->next;
        }
    }
    if (flag) {
		if (attribute != EXTERN){
 			(*error) = SYMBOL_ALREADY_DEFINED;
			}
		else{
				if(curr->is_external){
				return;
				}
				else{
				(*error) = SYMBOL_ALREADY_DEFINED;
				}
			}       
    } else {
        table_row *symbol = (table_row *) malloc(sizeof(table_row));
        if (!symbol) {
            printf("Failed to allocate memory, Please restart");
            exit(0);
        } else {
            new_symbol_name = (char *) malloc(MAX_LABEL_SIZE * sizeof(char));
            {
                if (!new_symbol_name) {
                    printf("Failed to allocate memory, Please restart");
                    exit(0);
                } else {
                    strcpy(new_symbol_name, symbol_name);
                    symbol->symbol_name = new_symbol_name;
                    symbol->value = value;
                    resetAttributes(symbol);
                    setAttributeOn(symbol, attribute);
                    symbol->next = NULL;
                    if ((*table)->counter == 0) {
                        (*table)->head = symbol;
                    } else {
                        prev->next =(struct table_row *) symbol;
                    }

                    (*table)->counter += 1;
                }
            }

        }
    }
}
/*!
 * this function gets a statement integer
 * and if it is code or data we want to address it as a data
 * @param statement
 * @return the statement as int after conversion
 */
int convertStatementToAttribute(int statement) {
    if (statement == CODE_ATTR || statement == DATA_ATTR) {
        return DATA_ATTR;
    } else {
        return statement;
    }
}

/*!
 * this function gets a symbol table and symbol name.
 * and if it founds it in the table returns it
 * @param table - the symbol table
 * @param target_symbol_name - the symbol name as string
 * @return the symbol if found if not return NULL
 */
table_row *getSymbol(symbol_table **table, char *target_symbol_name) {
    int flag = 0;
    table_row *curr;
    curr = (*table)->head;
    while (curr != NULL && !flag) {
        if (strcmp(curr->symbol_name, target_symbol_name) == 0) {
            flag = 1;
        }
        if (!flag) {
            curr = (table_row *) curr->next;
        }
    }
    return curr;
}

/*!
 * this function is update the symbol table values after the first run
 * @param table - the symbol table
 * @param IC - the instruction count after the first run
 */
void updateSymbolTableValues(symbol_table *table, int IC) {
    table_row *curr;
    curr = (*table).head;
    while (curr != NULL) {
        if (curr->is_data) {
            curr->value += IC;
        }
        curr = (table_row *) curr->next;
    }
}

