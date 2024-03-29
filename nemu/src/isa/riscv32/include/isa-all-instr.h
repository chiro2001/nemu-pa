#include <cpu/decode.h>

#include "../local-include/rtl.h"

#define INSTR_NULLARY(f) f(inv) f(nemu_trap) f(ecall) f(ebreak) f(wfi)
#define INSTR_UNARY(f)
#define INSTR_BINARY(f)                                                     \
  f(auipc) f(mul) f(mulh) f(mulhsu) f(mulhu) f(div) f(divu) f(rem) f(remu)  \
      f(slli) f(srl) f(srli) f(sra) f(srai) f(slt) f(slti) f(sltu) f(sltiu) \
          f(jal) f(jalr) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(lr)  \
              f(fence)
#define INSTR_TERNARY(f)                                                  \
  f(add) f(addi) f(sub) f(lui) f(xor) f(xori) f(or) f(ori) f(and) f(andi) \
      f(sll) f(csrrw) f(csrrs) f(csrrc) f(csrrwi) f(csrrsi) f(csrrci)     \
          f(amoswap) f(amoadd) f(amoxor) f(amoand) f(amoor) f(amomin)     \
              f(amomax) f(amominu) f(amomaxu) f(sc)

#define INSTR_MEM(f) f(lw) f(sw) f(lh) f(lb) f(lhu) f(lbu) f(sh) f(sb)

def_all_EXEC_ID();
