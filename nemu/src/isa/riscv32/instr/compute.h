#include "rtl-basic.h"
def_EHelper(lui) { rtl_li(s, ddest, id_src1->imm); }
def_EHelper(add) { rtl_add(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(addi) { rtl_addi(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(sub) { rtl_sub(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(xor) { rtl_xor(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(xori) { rtl_xori(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(or) { rtl_or(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(ori) { rtl_ori(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(and) { rtl_and(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(andi) { rtl_andi(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(sll) { rtl_sll(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(slli) { rtl_slli(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(srl) { rtl_srl(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(srli) { rtl_srli(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(sra) { rtl_sra(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(srai) { rtl_srai(s, ddest, id_src1->preg, id_src2->imm); }
/**
 * @brief  jalr t = pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
 */
def_EHelper(jalr) {
  // static rtlreg_t t, target;
  // static rtlreg_t zero = 0;
  // Log("JUMP >> " FMT_WORD, target);
  
  // t = (rtlreg_t)(s->pc + 4);
  // target = *id_src1->preg + imm_sext32(id_src2->imm, 12);
  // rtl_jr(s, &target);
  // rtl_addi(s, ddest, &zero, t);
}
