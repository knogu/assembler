// todo: declare in the same place as the enum for Op
int opCode[]               = {-1, 0xeb, -1, -1, 0x6a, 0xe8, 0xc3};
int opCodeForRegSrc[]      = {0x89, -1, 0x01, 0x29, -1, -1, -1};
int opCodeForLookupByReg[] = {0x8b, -1, 0x03, 0x2b, -1, -1, -1};
int opCodeForImm[]         = {0xb8, -1, 0x05, 0x2d, -1, -1, -1};
