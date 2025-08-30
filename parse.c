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

ByteCode* encode_common_2nd_reg(enum Reg32 reg_1st, ByteCode* cur_bytecode, uint8_t opcode_reg, uint8_t opcode_idx, uint8_t opcode_imm) {
    int reg2 = consume_reg32();
    if (reg2 != -1) { // add r32, r32 (kind of 01 /r)
        cur_bytecode = new_bytecode(cur_bytecode, opcode_reg);
        enum Mod mod = 0b11;
        enum RmReg rm = reg_1st;
        uint8_t mod_rm_val = mod << 6 | reg2 << 3 | rm;
        return new_bytecode(cur_bytecode, mod_rm_val);
    }
    if (consume("[")) { // add r32, [r32] = kind of 01 /r
        int rm_reg = expect_reg32();
        expect("]");
        cur_bytecode = new_bytecode(cur_bytecode, opcode_idx);
        enum Mod mod = 0b00;
        uint8_t mod_rm_val = mod << 6 | reg_1st << 3 | rm_reg;
        return new_bytecode(cur_bytecode, mod_rm_val);
    }
    int* imm = consume_num();
    if (imm != NULL) { // interpret as 05 id = add imm32, r32
        cur_bytecode = new_bytecode(cur_bytecode, opcode_imm);
        for (int i = 0; i<4; i++) {
            cur_bytecode = new_bytecode(cur_bytecode, *imm >> (8 * i));
        }
        return cur_bytecode;
    }
    return NULL;
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
                    if (reg != -1) {
                        expect(",");
                        ByteCode *encoded_2nd = encode_common_2nd_reg(reg, cur_bytecode, 1, 3, 5);
                        if (encoded_2nd) {
                            cur_bytecode = encoded_2nd;
                        } else {
                            exit(11);
                        }
                    }
                    break;
                }
                case MOV: {
                    int reg = consume_reg32();
                    if (reg != -1) {
                        expect(",");
                        ByteCode *encoded_2nd = encode_common_2nd_reg(reg, cur_bytecode, 0x89, 0x8b, 0xb8 + reg);
                        if (encoded_2nd) {
                            cur_bytecode = encoded_2nd;
                        } else {
                            exit(12);
                        }
                    }
                    break;
                }
                case SUB: {
                    int reg = consume_reg32();
                    if (reg != -1) {
                        expect(",");
                        ByteCode *encoded_2nd = encode_common_2nd_reg(reg, cur_bytecode, 0x29, 0x2b, 0x2d);
                        if (encoded_2nd) {
                            cur_bytecode = encoded_2nd;
                        } else {
                            exit(13);
                        }
                    }
                    break;
                }
                case PUSH: {
                    int reg = consume_reg32();
                    if (reg != -1) {
                        cur_bytecode = new_bytecode(cur_bytecode, 0x50 + reg);
                        break;
                    }
                    int* imm = consume_num();
                    if (imm != NULL) {
                        // todo: handle 16 and 32 bit imm
                        cur_bytecode = new_bytecode(cur_bytecode, 0x6a);
                        cur_bytecode = new_bytecode(cur_bytecode, *imm);
                    }
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
