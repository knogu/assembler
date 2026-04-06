#include "common.h"

Labels *label_head;

Inst* create_inst(Inst* cur_inst) {
    Inst* inst = calloc(1, sizeof(Inst));
    cur_inst->next = inst;
    return inst;
}

Inst* new_mov_inst(Reg* dst, UnionSrc* src, Inst* cur_inst) {
    Mov* mov = calloc(1, sizeof(Mov));
    mov->src = src;
    mov->dst = dst;
    cur_inst = create_inst(cur_inst);
    cur_inst->mov = mov;
    cur_inst->op = MOV;
    return cur_inst;
}

Inst* new_add_inst(Reg* dst, UnionSrc* src, Inst* cur_inst) {
    Add* add = calloc(1, sizeof(Add));
    add->src = src;
    add->dst = dst;
    cur_inst = create_inst(cur_inst);
    cur_inst->add = add;
    cur_inst->op = ADD;
    return cur_inst;
}

Inst* new_sub_inst(Reg* dst, UnionSrc* src, Inst* cur_inst) {
    Sub* sub = calloc(1, sizeof(Sub));
    sub->src = src;
    sub->dst = dst;
    cur_inst = create_inst(cur_inst);
    cur_inst->sub = sub;
    cur_inst->op = SUB;
    return cur_inst;
}

UnionSrc* parseSrc() {
    UnionSrc* src = calloc(1, sizeof(UnionSrc));
    Reg* reg = consume_reg();
    if (reg != NULL) { // add r32, r32 (kind of 01 /r)
        src->srcOpt = REGISTER;
        src->reg = reg;
        return src;
    }
    if (consume("[")) { // add r32, [r32] = kind of 01 /r
        reg = consume_reg();
        expect("]");
        src->srcOpt = REGISTER_LOOKUP;
        src->reg = reg;
        return src;
    }
    long* imm = consume_num();
    if (imm != NULL) { // interpret as 05 id = add imm32, r32
        src->srcOpt = IMM;
        src->imm = *imm;
        return src;
    }
    exit(32);
}

ParseResult* parse() {
    Labels* labels = calloc(1, sizeof(Labels));
    label_head = labels;
    label_head->next = NULL;
    Labels *cur_label = label_head;

    Inst inst;
    Inst* inst_head = &inst;
    inst_head->next = NULL;
    Inst* cur_inst = inst_head;

    bool is_last_label = false;

    while (token->kind != TK_EOF) {
        char* ident = consume_ident();
        if (ident) { // label:
            expect(":");

            Labels* new_label = calloc(1, sizeof(Labels));
            new_label->name  = ident;
            cur_label->next = new_label;
            // place is filled after parsing
            cur_label = new_label;
            is_last_label = true;
            continue;
        }

        enum OpKind opKind = consume_opcode();
        if (opKind != -1) {
            switch (opKind) {
                case MOV: {
                    Reg *dst = consume_reg();
                    if (dst == NULL) {
                        exit(532);
                    }
                    expect(",");
                    UnionSrc* src = parseSrc();
                    cur_inst = new_mov_inst(dst, src, cur_inst);
                    break;
                }
                case ADD: {
                    Reg *dst = consume_reg();
                    if (dst == NULL) {
                        exit(534);
                    }
                    expect(",");
                    UnionSrc* src = parseSrc();
                    cur_inst = new_add_inst(dst, src, cur_inst);
                    break;
                }
                case JMP_SHORT: {
                    char* dest_label = consume_ident();
                    if (dest_label == NULL) {
                        exit(8);
                    }
                    ShortJmp* shortJmp = calloc(1, sizeof(ShortJmp));
                    shortJmp->label_name = dest_label;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->shortJmp = shortJmp;
                    cur_inst->op = JMP_SHORT;
                    break;
                }
                case CONDITIONAL_JMP: {
                    char* dest_label = consume_ident();
                    if (dest_label == NULL) {
                        exit(8);
                    }
                    ConditinalJmp* jmp = calloc(1, sizeof(ConditinalJmp));
                    jmp->label_name = dest_label;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->cjmp = jmp;
                    cur_inst->op = CONDITIONAL_JMP;
                    break;
                }
                case SUB: {
                    Reg *dst = consume_reg();
                    if (dst == NULL) {
                        exit(53);
                    }
                    expect(",");
                    UnionSrc* src = parseSrc();
                    cur_inst = new_sub_inst(dst, src, cur_inst);
                    break;
                }
                case PUSH: {
                    Push* push = calloc(1, sizeof(Push));
                    Reg* reg = consume_reg();
                    if (reg != NULL) {
                        push->src = reg;
                    }
                    long* imm = consume_num();
                    if (imm != NULL) {
                        push->imm = *imm;
                        push->is_imm = true;
                    }
                    cur_inst = create_inst(cur_inst);
                    cur_inst->push = push;
                    cur_inst->op = PUSH;
                    break;
                }
                case POP: {
                    Pop* pop = calloc(1, sizeof(Pop));
                    Reg* reg = consume_reg();
                    pop->dst = reg;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = POP;
                    cur_inst->pop = pop;
                    break;
                }
                case CALL: {
                    char* dest_label = consume_ident();
                    if (dest_label == NULL) {
                        exit(34);
                    }
                    Call* call = calloc(1, sizeof(Call));
                    call->label_name = dest_label;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->call = call;
                    cur_inst->op = CALL;
                    break;
                }
                case RET: {
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = RET;
                    break;
                }
                case IMUL: {
                    IMul* imul = calloc(1, sizeof(IMul));
                    Reg *dst = consume_reg();
                    if (dst == NULL) {
                        exit(225);
                    }
                    expect(",");
                    Reg *src = consume_reg();
                    if (src == NULL) {
                        exit(235);
                    }
                    imul->dst = dst;
                    imul->src = src;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = IMUL;
                    cur_inst->iMul = imul;
                    break;
                }
                case IDIV: {
                    IDiv* idiv = calloc(1, sizeof(IDiv));
                    Reg *divided = consume_reg();
                    if (divided == NULL) {
                        exit(225);
                    }
                    idiv->divided = divided;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = IDIV;
                    cur_inst->iDiv = idiv;
                    break;
                }
                case CQO: {
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = CQO;
                    break;
                }
                case CMP: {
                    Cmp* cmp = calloc(1, sizeof(Cmp));
                    Reg* arg1 = consume_reg();
                    if (arg1 == NULL) {
                        exit(225);
                    }
                    cmp->arg1 = arg1;
                    expect(",");
                    UnionSrc* arg2 = parseSrc();
                    cmp->arg2 = arg2;
                    cur_inst = create_inst(cur_inst);
                    cur_inst->op = CMP;
                    cur_inst->cmp = cmp;
                    break;
                }
            }

            if (is_last_label) {
                cur_inst->label = cur_label;
            }
            is_last_label = false;
        } else {
            expect_newline();
        }
    }

    ParseResult* result = calloc(1, sizeof(ParseResult));
    result->insts = inst_head->next;
    result->labels = label_head->next;
    return result;
}
