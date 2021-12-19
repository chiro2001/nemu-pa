#include <cpu/decode.h>

#include "../local-include/rtl.h"

#define INSTR_NULLARY(f) f(inv) f(nemu_trap)
#define INSTR_UNARY(f)
#define INSTR_BINARY(f)                                                     \
  f(auipc) f(mul) f(mulh) f(mulhsu) f(mulhu) f(div) f(divu) f(rem) f(remu)  \
      f(slli) f(srl) f(srli) f(sra) f(srai) f(slt) f(slti) f(sltu) f(sltiu) \
          f(jal) f(jalr) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu)
#define INSTR_TERNARY(f) \
  f(add) f(addi) f(sub) f(lui) f(xor) f(xori) f(or) f(ori) f(and) f(andi) f(sll)

#define INSTR_MEM(f) f(lw) f(sw) f(lh) f(lb) f(lhu) f(lbu) f(sh) f(sb)

def_all_EXEC_ID();
