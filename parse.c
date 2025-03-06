#include "common.h"

struct ByteCode* byteCode;
int64_t byteCodeLen = 0;
Labels *label_head;

ByteCode* new_bytecode(ByteCode* cur, uint8_t byte) {
    ByteCode* newByte = calloc(1, sizeof(ByteCode));
    newByte->byte = byte;
    cur->next = newByte;
    byteCodeLen += 1;
    return newByte;
}

int64_t get_label_addr(char* label_name) {
    int len = strlen(label_name);
    for (Labels *cur = label_head->next; cur; cur = cur->next) {
        printf("%s", cur->name);
        if (strlen(cur->name) == len && !memcmp(cur->name, label_name, len))
            return cur->place;
    }

    exit(9);
}


ByteCode* parse_inst() {
    ByteCode bytecode_head;
    bytecode_head.next = NULL;
    ByteCode *cur_bytecode = &bytecode_head;

    Labels labels;
    label_head = &labels;
    label_head->next = NULL;
    Labels *cur_label = label_head;

    while (token->kind != TK_EOF) {
        char* ident = consume_ident();
        if (ident) { // label:
            expect(":");

            Labels* new_label = calloc(1, sizeof(Labels));
            new_label->name  = ident;
            new_label->place = byteCodeLen;
            cur_label->next = new_label;
            cur_label = new_label;

            continue;
        }
        enum OpKind opKind = consume_opcode();
        if (opKind != -1) {
            switch (opKind) {
//        case MOV: {
//            enum Reg32 dstReg;
//            dstReg = expect_reg32();
//            expect(",");
//            if (token->kind == TK_NUM) {
//                mov->imm = token->val;
//                token = token->next;
//                Insts *insts = calloc(1, sizeof(Insts));
//                Inst *cur_inst = calloc(1, sizeof(Inst));
//                cur_inst->mov = mov;
//                cur_inst->kind = OP_MOV;
//                insts->cur = cur_inst;
//                return insts;
//            }
//            mov->src = expect_reg32();
//            break;
//        }
                case JMP_SHORT: {
                    char* dest_label = consume_ident();
                    if (dest_label == NULL) {
                        exit(8);
                    }
                    int64_t dst_addr = get_label_addr(dest_label);
                    cur_bytecode = new_bytecode(cur_bytecode, 0xEB);
                    int64_t offset = dst_addr - (byteCodeLen + 1);
                    cur_bytecode = new_bytecode(cur_bytecode, (int8_t)offset);
                    break;
                }
                case ADD: {
                    int reg = consume_reg32();
                    expect(",");
                    if (reg != -1) {
                        int reg2 = consume_reg32();
                        if (reg2 != -1) { // add r32, r32 (kind of 01 /r)
                            cur_bytecode = new_bytecode(cur_bytecode, (uint8_t) 1);
                            enum Mod mod = 0b11;
                            enum RmReg rm = reg;
                            uint8_t mod_rm_val = mod << 6 | reg2 << 3 | rm;
                            cur_bytecode = new_bytecode(cur_bytecode, mod_rm_val);
                        }
                        int* imm = consume_num();
                        if (imm != NULL) { // interpret as 05 id = add imm32, r32
                            cur_bytecode = new_bytecode(cur_bytecode, (uint8_t) 5);
                            for (int i = 0; i<4; i++) {
                                cur_bytecode = new_bytecode(cur_bytecode, *imm >> (8 * i));
                            }
                        }
                        if (consume("[")) { // add r32, [r32] = kind of 01 /r
                            int rm_reg = expect_reg32();
                            expect("]");
                            cur_bytecode = new_bytecode(cur_bytecode, (uint8_t) 3);
                            enum Mod mod = 0b00;
                            uint8_t mod_rm_val = mod << 6 | reg << 3 | rm_reg;
                            cur_bytecode = new_bytecode(cur_bytecode, mod_rm_val);
                        }
                    }

                    break;
                }
            }
        }

        expect_newline();
    }

    return bytecode_head.next;
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
