#include "common.h"

Token *token;

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_alnum(char c) {
    return is_alpha(c) || ('0' <= c && c <= '9');
}

int get_match_idx(char *p, int candidates_len, char *candidates[]) {
    for (int i = 0; i < candidates_len; i++) {
        int len = strlen(candidates[i]);
        if (startswith(p, candidates[i]) && !is_alnum(p[len])) {
            return i;
        }
    }
    return -1;
}

enum Reg32 check_reg32(char *p) {
    return get_match_idx(p, sizeof(reg32_names) / sizeof(reg32_names[0]),reg32_names);
}

enum OpKind check_op(char *p) {
    return get_match_idx(p, sizeof(op_names) / sizeof(op_names[0]), op_names);
}

//char *starts_with_reserved(char *p) {
//    // Keyword
//    static char *kw[] = {"mov", "jmp short", "jmp", "eax" };
//
//    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
//        int len = strlen(kw[i]);
//        if (startswith(p, kw[i]) && !is_alnum(p[len]))
//            return kw[i];
//    }
//
//    // Multi-letter punctuator
//    static char *ops[] = {"==", "!=", "<=", ">="};
//
//    for (int i = 0; i < sizeof(ops) / sizeof(*ops); i++)
//        if (startswith(p, ops[i]))
//            return ops[i];
//
//    return NULL;
//}

// Create a new token and add it as the next token of `byte`.
Token *new_token(TokenKind kind, Token *cur, char *str, int len, int idx) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    tok->enum_idx = idx;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (*p == '\n') {
            cur = new_token(TK_NEWLINE, cur, p++, 1, -1);
            continue;
        }

        enum OpKind op_idx = check_op(p);
        if (0 <= (int)op_idx) {
            int len = strlen(op_names[(int)op_idx]);
            cur = new_token(TK_OPECODE, cur, p, len, op_idx);
            p += len;
            continue;
        }

        enum Reg32 reg32_idx = check_reg32(p);
        if (0 <= (int)reg32_idx) {
            int len = strlen(reg32_names[(int)reg32_idx]);
            cur = new_token(TK_REG, cur, p, len, reg32_idx);
            p += len;
            continue;
        }

        if (isspace(*p)) {
            p++;
            continue;
        }

//        char *kw = starts_with_reserved(p);
//        if (kw) {
//            int len = strlen(kw);
//            byte = new_token(TK_RESERVED, byte, p, len);
//            p += len;
//            continue;
//        }

        if (strchr(":,[]", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1, -1);
            continue;
        }

        if (is_alpha(*p)) {
            char *q = p++;
            while (is_alnum(*p))
                p++;
            cur = new_token(TK_IDENT, cur, q, p - q, -1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0, -1);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        exit(3);
    }

    new_token(TK_EOF, cur, p, 0, -1);
    return head.next;
}

enum Reg32 expect_reg32() {
    if (token->kind != TK_REG) {
        exit(5);
    }
    enum Reg32 reg = (enum Reg32) token->enum_idx;
    token = token->next;
    return reg;
}

enum Reg32 consume_reg32() {
    if (token->kind != TK_REG) {
        return -1;
    }
    enum Reg32 reg = (enum Reg32) token->enum_idx;
    token = token->next;
    return reg;
}

int* consume_num() {
    if (token->kind != TK_NUM) {
        return NULL;
    }
    int* ret = &token->val;
    token = token->next;
    return ret;
}

enum OpKind consume_opcode() {
    if (token->kind != TK_OPECODE) {
        return -1;
    }
    enum OpKind opKind = (enum OpKind) token->enum_idx;
    token = token->next;
    return opKind;
}

enum OpKind expect_opcode() {
    if (token->kind != TK_OPECODE) {
        exit(10);
    }
    return (enum OpKind) token->enum_idx;
}

char* consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    char* name = token->str;
    int len = token->len;
    token = token->next;
    return strndup(name, len);
}

void expect_newline() {
    if (token->kind != TK_NEWLINE) {
        exit(7);
    }
    token = token->next;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        exit(6);
    token = token->next;
}

bool consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}
