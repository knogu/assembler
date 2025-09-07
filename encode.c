#include "common.h"

int64_t byteCodeLen = 0;

ByteCode* new_bytecode(ByteCode* cur, uint8_t byte) {
    ByteCode* newByte = calloc(1, sizeof(ByteCode));
    newByte->byte = byte;
    cur->next = newByte;
    byteCodeLen += 1;
    return newByte;
}

Labels* get_label(char* name, Labels* labels) {
    while (labels) {
        if (strcmp(name, labels->name) == 0) {
            return labels;
        }
        labels = labels->next;
    }
    return NULL;
}

ByteCode* encodeTwoOperands(ByteCode* cur_bytecode, enum OpKind op, enum Reg32 dst, UnionSrc* src) {
    switch (src->srcOpt) {
        case IMM: {
            switch (op) {
                case MOV: {
                    cur_bytecode = new_bytecode(cur_bytecode, opCodeForImm[op] + dst);
                    break;
                }
                case SUB: {
                    if (dst == EAX) {
                        cur_bytecode = new_bytecode(cur_bytecode, 0x2d);
                    } else {
                        cur_bytecode = new_bytecode(cur_bytecode, 0x81);
                        cur_bytecode = new_bytecode(cur_bytecode, 0b11 << 6 | 0b101 << 3 | dst);
                    }
                    break;
                }
                case ADD: {
                    if (dst == EAX) {
                        cur_bytecode = new_bytecode(cur_bytecode, 0x05);
                    } else {
                        cur_bytecode = new_bytecode(cur_bytecode, 0x81);
                        cur_bytecode = new_bytecode(cur_bytecode, 0b11 << 6 | 0b000 << 3 | dst);
                    }
                    break;
                }
                default: {
                    exit(44);
                }
            }
            for (int i = 0; i<4; i++) {
                cur_bytecode = new_bytecode(cur_bytecode, src->imm >> (8 * i));
            }
            return cur_bytecode;
        }
        case REGISTER: {
            cur_bytecode = new_bytecode(cur_bytecode, opCodeForRegSrc[op]);
            enum Mod mod = 0b11;
            enum RmReg rm = dst;
            uint8_t mod_rm_val = mod << 6 | src->reg << 3 | rm;
            return new_bytecode(cur_bytecode, mod_rm_val);
        }
        case REGISTER_LOOKUP: {
            cur_bytecode = new_bytecode(cur_bytecode, opCodeForLookupByReg[op]);
            enum Mod mod = 0b00;
            uint8_t mod_rm_val = mod << 6 | dst << 3 | src->reg;
            return new_bytecode(cur_bytecode, mod_rm_val);
        }
    }
}

ByteCode* encode(Inst* inst, Labels* labels) {
    ByteCode* headByteCode = calloc(1, sizeof(ByteCode));
    headByteCode->next = NULL;
    ByteCode* cur_bytecode = headByteCode;
    while (inst) {
        switch (inst->op) {
            case MOV: {
                Mov *mov = inst->mov;
                cur_bytecode = encodeTwoOperands(cur_bytecode, MOV, mov->dst, mov->src);
                break;
            }
            case ADD: {
                Add *add = inst->add;
                cur_bytecode = encodeTwoOperands(cur_bytecode, ADD, add->dst, add->src);
                break;
            }
            case JMP_SHORT: {
                ShortJmp *shortJmp = inst->shortJmp;
                cur_bytecode = new_bytecode(cur_bytecode, opCode[JMP_SHORT]);
                Labels *label = get_label(shortJmp->label_name, labels);
                cur_bytecode = new_bytecode(cur_bytecode, label->place - (byteCodeLen + 1));
                break;
            }
            case SUB: {
                Sub *sub = inst->sub;
                cur_bytecode = encodeTwoOperands(cur_bytecode, SUB, sub->dst, sub->src);
                break;
            }
            case PUSH: {
                Push *push = inst->push;
                if (push->is_imm) {
                    cur_bytecode = new_bytecode(cur_bytecode, opCode[PUSH]);
                    cur_bytecode = new_bytecode(cur_bytecode, push->imm);
                } else {
                    cur_bytecode = new_bytecode(cur_bytecode, 0x50 + push->src);
                }
                break;
            }
            case CALL: {
                Call* call = inst->call;
                Labels* label = get_label(call->label_name, labels);
                cur_bytecode = new_bytecode(cur_bytecode, opCode[CALL]);
                for (int i = 0; i<4; i++) {
                    cur_bytecode = new_bytecode(cur_bytecode, label->place >> (8 * i));
                }
                cur_bytecode = cur_bytecode;
            }
            case RET: {
                cur_bytecode = new_bytecode(cur_bytecode, opCode[RET]);
            }
        }
        inst = inst->next;
    }
    return headByteCode->next;
}

int write(ByteCode* cur) {
    uint8_t bytes[byteCodeLen];
    for (int i = 0; i < byteCodeLen; i++) {
        bytes[i] = cur->byte;
        cur = cur->next;
    }
    FILE *fp = fopen("/Users/jp31281/asm/output.bin", "wb");
    if (fp == NULL) {
        perror("ファイルを開けませんでした");
        return 1;
    }
    size_t written = fwrite(bytes, sizeof(uint8_t), byteCodeLen, fp);
    if (written != byteCodeLen) {
        perror("データの書き込みに失敗しました");
        fclose(fp);
        return 1;
    }

    // ファイルを閉じる
    fclose(fp);
    return 0;
}
