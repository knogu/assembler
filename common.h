#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_OPECODE,
    TK_REG,
    TK_NUM, // imm
    TK_NEWLINE,
    TK_EOF,
} TokenKind;

struct Token {
    TokenKind kind;
    struct Token *next;
    int val;
    int enum_idx;
    char *str;
    int len;
};

typedef struct Token Token;

enum Reg32 { EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, REGISTERS_COUNT };
static char* reg32_names[] = {
        "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};

enum OpKind {MOV, JMP_SHORT, ADD, SUB, PUSH};
static char* op_names[] = {"mov", "jmp short", "add", "sub", "push"};

enum Mod { IDX, IDX_DISP8, IDX_DISP32, REG };
enum RmReg { RM_EAX, RM_ECX, RM_EDX, RM_EBX, SIB_OR_ESP, DIS_OR_EBP, RM_ESI, RM_EDI };

typedef struct {
    enum Reg32 dst;
    uint32_t imm;
    enum Reg32 src; // todo: make union
    int bytesCnt;
} Mov;

typedef struct {
    char* name;
} Label;

typedef struct {
    char* label;
} ShortJmp;

typedef struct {
    enum Reg32 src;
} AddRm32R32;

typedef enum {
    LABEL,
    OP_MOV,
    OP_JMP_SHORT,
    OP_ADD,
} LineKind;

typedef struct {
    LineKind kind;
    union {
        Mov* mov;
        Label* label;
        ShortJmp* short_jmp;
    };
} Inst;

struct Insts {
    Inst *cur;
    struct Insts *next;
};

typedef struct Insts Insts;

Token *tokenize(char *p);
enum Reg32 expect_reg32();
enum Reg32 consume_reg32();
void expect(char *op);
char *consume_ident();
void expect_newline();
enum OpKind expect_opcode();

extern Token *token;

struct ByteCode {
    uint8_t byte;
    struct ByteCode *next;
};

typedef struct ByteCode ByteCode;

struct Labels {
    char* name;
    int64_t place;
    struct Labels* next;
};

typedef struct Labels Labels;

ByteCode* parse_inst();

int write(ByteCode* cur);

enum OpKind consume_opcode();

int* consume_num();

bool consume(char *op);
