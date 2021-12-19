#include <cpu/decode.h>

#include "../local-include/rtl.h"

#define INSTR_NULLARY(f) f(inv) f(nemu_trap)
#define INSTR_UNARY(f)
#define INSTR_BINARY(f)                                                   \
  f(add) f(addi) f(sub) f(lui) f(xor) f(xori) f(or) f(ori) f(and) f(andi) \
      f(sll) f(slli) f(srl) f(srli) f(sra) f(srai) f(slt) f(slti) f(sltu) \
          f(sltiu)
#define INSTR_TERNARY(f) f(lw) f(sw)

def_all_EXEC_ID();
