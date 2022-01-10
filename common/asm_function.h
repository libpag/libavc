.macro asm_function name
#ifdef __APPLE__
    .globl _\name
    _\name:
#else
    .global \name
    \name:
#endif
.endm