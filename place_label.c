#include "common.h"

int bytesCountFromSrc(UnionSrc* src) {
    switch (src->srcOpt) {
        case IMM:
            return 5;
        case REGISTER:
        case REGISTER_LOOKUP:
            return 2;
    }
}

int inst_bytes_count(Inst *inst) {
    switch (inst->op) {
        case MOV:
            return bytesCountFromSrc(inst->mov->src);
        case ADD:
            return bytesCountFromSrc(inst->add->src);
        case SUB:
            return bytesCountFromSrc(inst->sub->src);
        case JMP_SHORT:
            return 2;
        case PUSH: {
            return inst->push->is_imm ? 2 : 1;
        }
        case CALL:
            return 5;
    }
}

void place_label(Inst *inst) {
    long cur_bytes_count = 0;
    while (inst) {
        Labels *label = inst->label;
        if (label) {
            label->place = cur_bytes_count;
        }
        cur_bytes_count += inst_bytes_count(inst);
        inst = inst->next;
    }
}
