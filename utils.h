
#ifndef CPROJECT_UTILS_H
#define CPROJECT_UTILS_H


#include <stdbool.h>

#define NUM_OF_FUNCS 16
#define NUM_OF_REGISTERS 8
#define NUM_OF_STATEMENTS 4
#define MAX_ARR_SIZE 80
#define MAX_STR_SIZE 80
#define MAX_INT_SIZE 31
#define MAX_LABEL_SIZE 31
#define MAX_INPUT 80
#define IMAGE_SIZE 500
#define THREE_LINES_INS 3
#define TOW_LINES_INS 2
#define ONE_LINE_INS 1
#define MAX_INTEGER 2047
#define MIN_INTEGER (-2048)


enum ARE {
    ABSOLUTE, RELOCATABLE, EXTERNAL
};

enum CODES {
    ZERO, ONE, TOW, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN
};

enum OPS {
    SOURCE, TARGET
};


enum FUNCS {
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, JSR, RED, PRN, RTS, STOP
};


enum STATEMENTS {
    DATA, STRING, ENTRY, EXTERN
};

enum ADDRESS_MODE {
    IMMEDIATE, DIRECT, RELATIVE, REGISTER
};
enum LINE_OPTIONS {
    BLANK, COMMENT, COMMAND, STATEMENT
};

enum ATTRS {
    CODE_ATTR, DATA_ATTR, ENTRY_ATTR, EXTERN_ATTR
};


enum BOOL {
    FALSE, TRUE
};
enum ERRORS {
    None = -1,
    ILLEGAL_COMMA,
    ILLEGAL_LABEL_NAME,
    MISSING_COMMA,
    NOT_INTEGER_NUM,
    UNDEFINED_COMMAND_NAME,
    EXTRANEOUS_TEXT,
    MISSING_ARG,
    MULTIPLE_CONSECUTIVE_COMMA,
    INCORRECT_OPERAND,
    UNDEFINED_STATEMENT_NAME,
    MISSING_QUOTE,
    UNDEFINED_SYMBOL,
    RELATIVE_USE_EXTERNAL,
    SYMBOL_IS_EXTERNAL_AND_ENTERY,
    SYMBOL_ALREADY_DEFINED,
	IMMEDIATE_IS_OVERFLOW
};

/* command parameters */
typedef struct {
    char *source_op;
    unsigned int source_op_type: 2;
    unsigned int source_register: 4;
    char *target_op;
    unsigned int target_op_type: 2;
    unsigned int target_register: 4;
    int source_immediate;
    int target_immediate;
    int *arr;
} params;

/* command input after analyzed */
typedef struct {
    unsigned int label_flag: 1;
    unsigned int line_type: 2;
    unsigned int statement: 2;
    unsigned int funct_num: 4;
    params param;
    int error;
    char *label_name;
    int data_counter;
} input_attr;

/* single symbol */
typedef struct {
    char *symbol_name;
    int value;
    unsigned int is_code: 1;
    unsigned int is_data: 1;
    unsigned int is_entry: 1;
    unsigned int is_external: 1;
    struct table_row *next;
} table_row;


/* symbol table */
typedef struct {
    table_row *head;
    int counter;
} symbol_table;

/* code word as machine word */
typedef struct code_word {
    unsigned int ARE: 2;
    unsigned int target: 2;
    unsigned int source: 2;
    unsigned int funct: 4;
    unsigned int opcode: 4;
} code_word;

/* date word as machine word */
typedef struct data_word {
    unsigned int ARE: 2;
    unsigned int data: 12;
} data_word;

/* contains one of the both above */
typedef union machine_word {
    data_word *data;
    code_word *code;
} machine_word;

/* single external node and his appearance in the code */
typedef struct {
    char *name;
    int line_number;
    struct external_node *next;
} external_node;

/* externals list */
typedef struct {
    external_node *head;
} external_list;

input_attr convertLineToInputAttr(char []);

machine_word convertInstructionToCode(input_attr, int, int);

union machine_word extractOperandsToCode(input_attr *, symbol_table *, int, int, external_list **);

char *concat(const char *, const char *);

char getLineARE(int);

void createOutputFiles(machine_word *, char *, int, int, symbol_table, external_list);

void addExternal(external_list **, char *, int);


void addSymbol(symbol_table **, char *, int, int, int *);

int convertStatementToAttribute(int);

table_row *getSymbol(symbol_table **, char *);

void updateSymbolTableValues(symbol_table *, int IC);

bool secondRun(char [], int *, const int *, symbol_table *, data_word *, machine_word *, external_list *);

bool firstRun(char [], int *, int *, symbol_table *, data_word *);

int checkLabelName(char *);

void freeUnnecessaryCommandMemory(input_attr *);

void freeAllocatedMemory(symbol_table *,external_list *,data_word *);


#endif
