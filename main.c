#include "common.h"

// Returns the contents of a given file.
char *read_file(char *path) {
    // Open and read the file.
    FILE *fp = fopen(path, "r");
    if (!fp) exit(1);

    int filemax = 10 * 1024 * 1024;
    char *buf = malloc(filemax);
    int size = fread(buf, 1, filemax - 2, fp);
    if (!feof(fp)) exit(2);

    // Make sure that the string ends with "\n\0".
    if (size == 0 || buf[size - 1] != '\n')
        buf[size++] = '\n';
    buf[size] = '\0';
    return buf;
}

void dump_insts(Insts* insts) {
    while (insts != NULL) {
        switch (insts->cur->kind) {
            case LABEL: {
                Label* label = insts->cur->label;
                printf("%s:\n", label->name);
                break;
            }
            case OP_MOV : {
                Mov* mov = insts->cur->mov;
                printf("mov %s, %u\n", reg32_names[(int)mov->dst], mov->imm);
                break;
            }
            case OP_JMP_SHORT: {
                printf("jmp short ");
                ShortJmp* short_jmp = insts->cur->short_jmp;
                printf("%s:\n", short_jmp->label);
                break;
            }
        }
        insts = insts->next;
    }
}

int main(int argc, char **argv) {
    char* p = read_file(argv[1]);
    token = tokenize(p);

    ByteCode* bytecode = parse_inst();
    write(bytecode);

    return 0;
}
