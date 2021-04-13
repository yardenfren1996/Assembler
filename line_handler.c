#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include "saved_names.h"



bool isEmptyLine(const char *);

bool isComment(const char *);

bool isLabel(char *);

bool isLegalCommand(const char []);

int getLineType(const char *);

int getFunctNum(char *);

params getCommandParams(char *, int);

char *getOperand(char *, int);

void skipSpaces(const char *);

int checkOperandType(char *);

int getRegisterNum(char *);

int getImmediate(const char *);

bool isRegister(char *);

void extraneous_text(const char *str);

int getStatement(char *statement);

params getStatementParams(char *args, int statement_num);

char *getString(char *args);

int *getArray(char *args);

double getScalar(const char *str, int expect_comma);

input_attr command;
int ind;


/*!
 * this function is the "work manager" that gives the little functions to get the information needed
 * @param line_from_file - line from a file as string
 * @return an input attr that represent that command
 */
input_attr convertLineToInputAttr(char line_from_file[]) {
	char *first,*rest;
    command.param.source_op = NULL;
    command.param.target_op = NULL;
    command.param.arr = NULL; 
    ind = 0;
    if (!isEmptyLine(line_from_file)) {
        if (isComment(line_from_file)) {
            command.error = -1;
            command.line_type = COMMENT;
        } else {
            first = strtok(line_from_file,
                           " \t\n");            /* divide the tow components of the command line into first string adn rest string*/
            rest = strtok(NULL, "");
            command.error = None;
            command.label_flag = FALSE;
            if (isLabel(first)) {
                char *label;
                label = first;
                label[strlen(label) - 1] = '\0';
                if (checkLabelName(label)){
                    command.error = ILLEGAL_LABEL_NAME;
                }
                command.label_name = label;
                command.label_flag = TRUE;
                first = strtok(rest, " \t\n");
                rest = strtok(NULL, "");
            }

            command.line_type = getLineType(first);

            if (command.line_type == COMMAND) {
                command.funct_num = getFunctNum(first);
                if (command.error == None) {
                    command.param = getCommandParams(rest, command.funct_num);
                    if (command.error == None) {
                        extraneous_text(rest);
                    }
                }


            }
            if (command.line_type == STATEMENT) {
                command.statement = getStatement(first);
                if (command.error == None) {
                    command.param = getStatementParams(rest, command.statement);
                    if (command.error == None) {
                        extraneous_text(rest);
                    }
                }

            }

        }
        return command;
    } else {                    /* in case of blank line recognized it and ignore it */
        command.error = -1;
        command.line_type = BLANK;
        return command;
    }

}
/*!
 * this function is the "work manager" of the functions that extracts the parameters
 * @param args - the given arguments as a string
 * @param statement_num -integer that represent a statement
 * @return the parameters as params object
 */
params getStatementParams(char *args, int statement_num) {
    params result;
    if (!args) {
        command.error = MISSING_ARG;
		result.target_op = NULL;
		result.arr = NULL;
        return result;
    } else {

        switch (statement_num) {
            case DATA:
                result.arr = getArray(args);
                break;
            case STRING:
                result.target_op = getString(args);
                break;
            case ENTRY:
            case EXTERN:
                result.target_op = getOperand(args, FALSE);
                break;
        }

        return result;
    }
}

/*!
 * this function converts a string that represent an array to array of integers
 * @param args - the string that represent an array
 * @return array of integers
 */
int *getArray(char *args) {
    int i = 0;
    char c;
    int *arr = (int *) calloc(MAX_ARR_SIZE, sizeof(int));
    if (!arr) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    } else {
        arr[i++] = getScalar(args, FALSE);
        while ((c = args[ind]) != '\0' && c != '\n' && command.error == None) {
            if (i < MAX_ARR_SIZE ) {
                arr[i++] = getScalar(args, TRUE);
            }
        }
        command.data_counter = i;
        return arr;
    }
}
/*!
 * this function converts string to scalar
 * @param str - the string given
 * @param expect_comma - boolean integer that tells us if we need t prepare for comma
 * @return the string as integer
 */
double getScalar(const char *str, int expect_comma) {
    int i = 0, single_minus = TRUE, single_plus = TRUE, flag = FALSE;
    char c;
    char *scalar_as_str;
    double res;
    skipSpaces(str);
    if (expect_comma) {
        if (str[ind] != ',') {
            command.error = MISSING_COMMA;
            flag = TRUE;
        }
        ind++;
        skipSpaces(str);
        if (str[ind] == '\n' && command.statement == DATA && command.error == None) {
            command.error = EXTRANEOUS_TEXT;
            flag = TRUE;
        }
        if (str[ind] == ',') {
            command.error = MULTIPLE_CONSECUTIVE_COMMA;
            flag = TRUE;
        }
    }
    scalar_as_str = (char *) calloc(MAX_INT_SIZE, sizeof(char));
    if (!scalar_as_str) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    }
    while (!flag && (isdigit(c = str[ind]) || c == '-' || c == '+')) {
        if (isdigit(c)) {
            scalar_as_str[i++] = c;
            ind++;
        } else {
            if (c == '-') {
                if (single_minus) {
                    single_minus = FALSE;
                    scalar_as_str[i++] = c;
                    ind++;
                } else {
                    command.error = NOT_INTEGER_NUM;
                    flag = TRUE;
                }
            }
            if (c == '+') {
                if (single_plus) {
                    single_plus = FALSE;
                    ind++;
                } else {
                    command.error = NOT_INTEGER_NUM;
                    flag = TRUE;
                }

            }
        }
    }
    if (isalpha(c)) {
        command.error = NOT_INTEGER_NUM;
        free(scalar_as_str);
        return -1;
    } else {
        scalar_as_str[i] = '\0';
        res = atoi(scalar_as_str);
        free(scalar_as_str);
		if (res < MIN_INTEGER || res > MAX_INTEGER){
			command.error = IMMEDIATE_IS_OVERFLOW;
		}
        return res;
    }

}

/*!
 * this function gets a string and check if he's good to work with
 * @param args - the raw string
 * @return the string after clearence
 */
char *getString(char *args) {
    int i = 0, openQ = FALSE, closeQ = FALSE;
    char c;
    char *res;
    skipSpaces(args);

    res = (char *) calloc(MAX_STR_SIZE, sizeof(char));
    if (!res) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    }
    if (args[ind] != '"') {
        command.error = MISSING_QUOTE;
        free(res);
    } else {
        openQ = TRUE;
        ind++;
        while ((c = args[ind]) != '"' && c != '\n') {
            res[i++] = c;
            ind++;
        }
        if (c != '"') {
            command.error = MISSING_QUOTE;
            free(res);
        } else {
            res[i] = '\0';
            closeQ = TRUE;
            ind++;
        }

    }
    if (openQ && closeQ) {
        return res;
    } else {
        return NULL;
    }
}

/*!
 * this function gets a statement as string and converts it to integer that represents it
 * @param statement - th string
 * @return integer that represent the statement
 */
int getStatement(char *statement) {
    int i;
    for (i = 0; i < NUM_OF_STATEMENTS; i++) {
        if (strcmp(statement, statementsTypeName[i]) == 0)
            return i;
    }
    command.error = UNDEFINED_STATEMENT_NAME;
    return -1;
}

/*!
 * this function is the "work manager" of the functions that extracts the parameters
 * @param args - the given arguments as a string
 * @param funct_num - integer that represent a function
 * @return the parameters as params object
 */
params getCommandParams(char *args, int funct_num) {
    params result;
    result.source_op = NULL;
    result.target_op = NULL;
    result.arr = NULL;

    switch (funct_num) {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
			if (!args){
				command.error = MISSING_ARG;
				break;
			}
            result.source_op = getOperand(args, FALSE);
            if (command.error == None) {
                result.source_op_type = checkOperandType(result.source_op);
                if (command.error == None) {
                    switch (result.source_op_type) {
                        case IMMEDIATE:
                            if (funct_num == LEA) {
                                command.error = INCORRECT_OPERAND;
                            } else {
                                result.source_immediate = getImmediate(result.source_op);
                            }
                            break;
                        case DIRECT:
                            break;
                        case RELATIVE:
                            command.error = INCORRECT_OPERAND;
                            break;
                        case REGISTER:
                            if (funct_num == LEA) {
                                command.error = INCORRECT_OPERAND;
                            }
                            result.source_register = getRegisterNum(result.source_op);
                            break;
                    }
                }
                if (command.error == None) {
                    result.target_op = getOperand(args, TRUE);
                    if (command.error == None) {
                        result.target_op_type = checkOperandType(result.target_op);
                        if (command.error == None) {
                            switch (result.target_op_type) {
                                case IMMEDIATE:
                                    if (funct_num != CMP) {
                                        command.error = INCORRECT_OPERAND;
                                    } else {
                                        result.target_immediate = getImmediate(result.target_op);
                                    }
                                    break;
                                case DIRECT:
                                    break;
                                case RELATIVE:
                                    command.error = INCORRECT_OPERAND;
                                    break;
                                case REGISTER:
                                    result.target_register = getRegisterNum(result.target_op);
                                    break;
                            }
                        }
                    }

                }
            }
            break;
        case CLR:
        case NOT:
        case INC:
        case DEC:
			if (!args){
				command.error = MISSING_ARG;
				break;
			}
            result.target_op = getOperand(args, FALSE);
            if (command.error == None) {
                result.target_op_type = checkOperandType(result.target_op);
                if (command.error == None) {
                    switch (result.target_op_type) {
                        case IMMEDIATE:
                            command.error = INCORRECT_OPERAND;
                            break;
                        case DIRECT:
                            break;
                        case RELATIVE:
                            command.error = INCORRECT_OPERAND;
                            break;
                        case REGISTER:
                            result.target_register = getRegisterNum(result.target_op);
                            break;
                    }
                }
            }
            break;
        case JMP:
        case BNE:
        case JSR:
			if (!args){
				command.error = MISSING_ARG;
				break;
			}
            result.target_op = getOperand(args, FALSE);
            if (command.error == None) {
                result.target_op_type = checkOperandType(result.target_op);
                if (command.error == None) {
                    switch (result.target_op_type) {
                        case IMMEDIATE:
                            command.error = INCORRECT_OPERAND;
                            break;
                        case DIRECT:
                        case RELATIVE:
                            break;
                        case REGISTER:
                            command.error = INCORRECT_OPERAND;
                            break;
                    }
                }
            }
            break;
        case RED:
        case PRN:
			if (!args){
				command.error = MISSING_ARG;
				break;
			}
            result.target_op = getOperand(args, FALSE);
            if (command.error == None) {
                result.target_op_type = checkOperandType(result.target_op);
                if (command.error == None) {
                    switch (result.target_op_type) {
                        case IMMEDIATE:
                            if (funct_num == RED) {
                                command.error = INCORRECT_OPERAND;
                            } else {
                                result.target_immediate = getImmediate(result.target_op);
                            }
                            break;
                        case DIRECT:
                            break;
                        case RELATIVE:
                            command.error = INCORRECT_OPERAND;
                            break;
                        case REGISTER:
                            result.target_register = getRegisterNum(result.target_op);
                            break;
                    }
                }
            }
            break;
        case RTS:
        case STOP:
            break;
    }
    return result;
}

/*!
 * this function check if the given string is operand an extract it
 * @param args - the string to extract from
 * @param expect_comma - boolean integer that tells us if we need t prepare for comma
 * @return the operand secluded as string
 */
char *getOperand(char *args, int expect_comma) {
    int i = 0, flag = FALSE;
    char c;
    char *operand_name;
    skipSpaces(args);
    if (expect_comma) {                /* handling the expectation to comma and the cases that there are multiple consecutive commas*/
        if (args[ind] != ',') {
            command.error = MISSING_COMMA;
            flag = TRUE;
        }
        ind++;
        skipSpaces(args);
    } else {
        if (args[ind] == ',') {
            command.error = ILLEGAL_COMMA;
            flag = TRUE;
        }
    }
    operand_name = (char *) calloc(MAX_LABEL_SIZE, sizeof(char));
    if (!operand_name) {
        printf("Failed to allocate memory, Please restart");
        exit(0);
    }
    while ((c = args[ind]) != ' ' && c != '\0' && c != ',' && c != '\t' && c != '\n') {
        operand_name[i++] = c;
        ind++;
    }
    operand_name[i] = '\0';
    if (i == 0 && (command.error == None || command.error == MISSING_COMMA)) {
        if (args[ind] == ',') {
            command.error = MULTIPLE_CONSECUTIVE_COMMA;
            flag = TRUE;
        } else {
            command.error = MISSING_ARG;
            flag = TRUE;
        }
    }
    if (!flag) {
        return operand_name;
    } else {
        free(operand_name);
        return NULL;
    }

}

/*!
 * this function converts string that represent a function to integer to represent a function
 * @param funct_name - the string that represent a function
 * @return integer that represent a function
 */
int getFunctNum(char *funct_name) {
    int i;
    if (!isLegalCommand(funct_name)) {
        command.error = UNDEFINED_COMMAND_NAME;
        return -1;
    }
    for (i = 0; i < NUM_OF_FUNCS; i++) {
        if (strcmp(funct_name, functionName[i]) == 0)
            return i;
    }
    command.error = UNDEFINED_COMMAND_NAME;
    return -1;
}

/*!
 * this function gets a line string and decided if its a statement or command
 * @param first - the given line as string
 * @return the type of the line as integer
 */
int getLineType(const char *first) {
    if (first[0] == '.') {
        return STATEMENT;
    } else {
        return COMMAND;
    }
}

/*!
 * this function gets a line and decided if there is a lable
 * @param first - the given line as string
 * @return True if there is a label
 */
bool isLabel(char *first) {
    return (isalpha(first[0]) && first[strlen(first)-1]==':');
}

/*!
 * this function gets a line and decided if its a comment
 * @param line - the given line as string
 * @return True if the line is a comment
 */
bool isComment(const char *line) {
    return (line[0] == ';'); /* if the line started with ; the whole line is a comment */
}

/*!
 * this function gets a line and decided if its an empty line
 * @param line - the given line as string
 * @return True if the line is empty
 */
bool isEmptyLine(const char *line) {
    int i = 0;
    char c;
    while (((c = line[i]) == ' ' || c == '\t') && c != '\0' && c != '\n') {
        i++;
    }
    return (line[i] == '\0' || line[i] == '\n');
}

/*!
 * this function skips white spaces
 * @param str - the white spaces to skip
 */
void skipSpaces(const char *str) {
    char c;
    while (((c = str[ind]) == ' ' || c == '\t') && c != '\0' && c != '\n') {
        ind++;
    }
}

/*!
 * this function checks if there is unnecessary text after the command
 * @param str - the string to check
 */
void extraneous_text(const char *str) {
    if (str) {
        char c;
        while (((c = str[ind]) == ' ' || c == '\t') && c != '\0') {
            ind++;
        }
        if (c != '\0' && c != '\n' && command.error == None) {
            command.error = EXTRANEOUS_TEXT;
        }
    }
}

/*!
 * this function checks if the command given is a legal command
 * @param str - the command as string to check
 * @return True is its legal
 */
bool isLegalCommand(const char str[]) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isalpha(str[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * this function checks the operand type
 * @param operand_name - string that represent the operand name
 * @return integer that represent the type of the operand
 */
int checkOperandType(char *operand_name) {
    if (operand_name[0] == '#') {
        return IMMEDIATE;
    }
    if (operand_name[0] == '%') {
        return RELATIVE;
    }
    if (isRegister(operand_name)) {
        return REGISTER;
    } else {
        return DIRECT;
    }
}

/*!
 * this function checks if the register name is legal
 * @param register_name - string that represent a register
 * @return True if the name is correct
 */
bool isRegister(char *register_name) {
    int i, flag = FALSE;
    for (i = 0; !flag && i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, registerName[i]) == 0)
            flag = TRUE;
    }
    return flag;
}

/*!
 * this function converts string that represent a register to integer
 * @param register_name - string that represent a register
 * @return integer that represent a register
 */
int getRegisterNum(char *register_name) {
    int i, flag = FALSE;
    for (i = 0; !flag && i < NUM_OF_REGISTERS; i++) {
        if (strcmp(register_name, registerName[i]) == 0)
            flag = TRUE;
    }
    return i;
}

/*!
 * this function converts string to integer
 * @param operand_name - string that represent the operand
 * @return the operand as integer
 */
int getImmediate(const char *operand_name) {
    int i = 0 ,result;
    char c;
    char *rest;
    operand_name += 1;
    result = strtol(operand_name, &rest, 10);
    while (isblank(c = rest[i])) {
        i++;
    }
    if (c != '\0') {
        command.error = NOT_INTEGER_NUM;
    }
	if (result < MIN_INTEGER || result > MAX_INTEGER){
		command.error = IMMEDIATE_IS_OVERFLOW;
		}
    return result;
}

/*!
 * this function checks the label name if its legal
 * @param name - string that represent the label
 * @return True if the name is illegal
 */
int checkLabelName(char *name) {
    int i;
    if (strlen(name) > MAX_LABEL_SIZE || !isalpha(name[0]) ){
        return TRUE;
    }

    for (i = 0; i < strlen(name); i++) {
        if (!isalnum(name[i])){
            return TRUE;
        }
    }

    for (i = 0; i < NUM_OF_FUNCS ; i++) {
        if (strcmp(name,functionName[i]) == 0 ){
            return TRUE;
        }
    }
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (strcmp(name,registerName[i])==0){
            return TRUE;
        }
    }
    for (i = 0; i < NUM_OF_STATEMENTS; ++i) {
        if (strcmp(name,statementsTypeName[i]+1)==0){
            return TRUE;
        }
    }
    return FALSE;
}

