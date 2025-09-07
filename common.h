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
    TK_REG_32,
    TK_REG_64,
    TK_NUM, // imm
    TK_NEWLINE,
    TK_EOF,
} TokenKind;

struct Token {
    TokenKind kind;
    struct Token *next;
    long val;
    int enum_idx;
    char *str;
    int len;
};

typedef struct Token Token;

enum Reg32 { EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, REGISTERS_COUNT };
static char* reg32_names[] = {
        "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};

enum Reg64 { RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, REGISTERS_COUNT_ };
static char* reg64_names[] = {
        "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"};

enum OpKind {MOV, JMP_SHORT, ADD, SUB, PUSH, CALL, RET};
static char* op_names[] = {"mov", "jmp short", "add", "sub", "push", "call", "ret"};

static int opCode[]               = {-1, 0xeb, -1, -1, 0x6a, 0xe8, 0xc3};
static int opCodeForRegSrc[]      = {0x89, -1, 0x01, 0x29, -1, -1, -1};
static int opCodeForLookupByReg[] = {0x8b, -1, 0x03, 0x2b, -1, -1, -1};
static int opCodeForImm[]         = {0xb8, -1, 0x05, 0x2d, -1, -1, -1};


enum Mod { IDX, IDX_DISP8, IDX_DISP32, REG };
enum RmReg { RM_EAX, RM_ECX, RM_EDX, RM_EBX, SIB_OR_ESP, DIS_OR_EBP, RM_ESI, RM_EDI };

typedef struct {
    char* label_name;
} ShortJmp;

enum SrcOpt {
    IMM,
    REGISTER,
    REGISTER_LOOKUP,
};

enum Width {
    w32,
    w64
};

typedef struct {
    union {
        enum Reg32 reg32;
        enum Reg64 reg64;
    };
    enum Width width;
} Reg;

typedef struct {
    union {
        long imm;
        enum Reg32 reg;
    };
    enum SrcOpt srcOpt;
} UnionSrc;

typedef struct {
    Reg* dst; // todo: handle all dst size by adding another field to show size
    enum SrcOpt src_opt;
    int dst_offset;
    int src_offset;
    UnionSrc* src;
} Add;

typedef struct {
    Reg* dst; // todo: handle all dst size by adding another field to show size
    UnionSrc* src;
    int dst_offset;
    int src_offset;
} Sub;

typedef struct {
    Reg* dst;
    UnionSrc* src;
    int dst_offset;
    int src_offset;
} Mov;

typedef struct {
    bool is_imm;
    int imm;
    enum Reg32 src;
} Push;

typedef struct {
    // todo: support more options
    char* label_name;
} Call;

struct Labels {
    char* name;
    int64_t place;
    struct Labels* next;
};

typedef struct Labels Labels;

struct Inst {
    enum OpKind op;
    Labels* label; // NULL if it's not the first inst after a label
    union {
        Mov *mov;
        Add *add;
        Sub *sub;
        ShortJmp *shortJmp;
        Push *push;
        Call *call;
    };
    struct Inst *next;
};

typedef struct Inst Inst;

typedef struct {
    Inst* insts;
    Labels* labels;
} ParseResult;

Token *tokenize(char *p);
enum Reg32 expect_reg32();
enum Reg32 consume_reg32();
Reg* consume_reg();
void expect(char *op);
char *consume_ident();
void expect_newline();
enum OpKind expect_opcode();

//extern Token *token;

struct ByteCode {
    uint8_t byte;
    struct ByteCode *next;
};

typedef struct ByteCode ByteCode;

int write(ByteCode* cur);

enum OpKind consume_opcode();

long* consume_num();

bool consume(char *op);

ParseResult* parse();

void place_label(Inst *inst);

ByteCode* encode(Inst* inst, Labels* labels);

extern Token* token;

//#define NEW_INST(op, new_inst) \
//    Inst* inst = calloc(1, sizeof(Inst)); \
//    cur_inst->next = inst; \
//    cur_inst = inst; \
//    cur_inst->new_inst = new_inst; \
//    cur_inst->op = op;
