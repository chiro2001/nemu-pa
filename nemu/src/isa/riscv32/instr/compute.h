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
def_EHelper(slli) {
  // Log("slli" FMT_WORD ", %d", *id_src1->preg, id_src2->imm);
  rtl_slli(s, ddest, id_src1->preg, id_src2->imm);
  // Log("    -> " FMT_WORD, *ddest);
}
def_EHelper(srl) { rtl_srl(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(srli) { rtl_srli(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(sra) { rtl_sra(s, ddest, id_src1->preg, id_src2->preg); }
def_EHelper(srai) { rtl_srai(s, ddest, id_src1->preg, id_src2->imm); }
def_EHelper(slt) {
  rtl_li(s, ddest, (sword_t)*id_src1->preg < (sword_t)*id_src2->preg ? 1 : 0);
}
def_EHelper(slti) {
  rtl_li(s, ddest, (sword_t)*id_src1->preg < (sword_t)id_src2->imm ? 1 : 0);
}
def_EHelper(sltu) { rtl_li(s, ddest, *id_src1->preg < *id_src2->preg ? 1 : 0); }
def_EHelper(sltiu) { rtl_li(s, ddest, *id_src1->preg < id_src2->imm ? 1 : 0); }
/**
 * @brief  jal x[rd] = pc+4; pc += sext(offset)
 */
def_EHelper(jal) {
  static rtlreg_t t, target;
  static rtlreg_t zero = 0;
  t = (rtlreg_t)(s->pc + 4);
  target = (rtlreg_t)(s->pc + id_src2->imm);
  Log("JUMP  >> " FMT_WORD, target);
  rtl_jr(s, &target);
  rtl_addi(s, ddest, &zero, t);
}

/**
 * @brief  jalr t = pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
 */
def_EHelper(jalr) {
  static rtlreg_t t, target;
  static rtlreg_t zero = 0;
  t = (rtlreg_t)(s->pc + 4);
  target = *id_src1->preg + imm_sext32(id_src2->imm, 12);
  Log("JUMPR >> " FMT_WORD, target);
  rtl_jr(s, &target);
  rtl_addi(s, ddest, &zero, t);
}

/**
 * @brief  beq rs1, rs2, offset   //if (rs1 == rs2) pc += sext(offset)
 */
def_EHelper(beq) {
  static rtlreg_t target;
  // Log("beq: " FMT_WORD ", " FMT_WORD, *id_src1->preg, id_src2->imm);
  if (*id_src1->preg == *ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BEQJ >> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bne rs1, rs2, offset   //if (rs1 =/= rs2) pc += sext(offset)
 */
def_EHelper(bne) {
  static rtlreg_t target;
  if (*id_src1->preg != *ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BNEJ >> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}

/**
 * @brief  blt rs1, rs2, offset    //if (rs1 <s rs2) pc += sext(offset)
 */
def_EHelper(blt) {
  static rtlreg_t target;
  if ((sword_t)*id_src1->preg < (sword_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BLTJ >> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bge rs1, rs2, offset    //if (rs1 >=s rs2) pc += sext(offset)
 */
def_EHelper(bge) {
  static rtlreg_t target;
  if ((sword_t)*id_src1->preg >= (sword_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BGEJ >> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bltu rs1, rs2, offset    //if (rs1 < rs2) pc += sext(offset)
 */
def_EHelper(bltu) {
  static rtlreg_t target;
  if ((word_t)*id_src1->preg < (word_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BLTUJ>> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bgeu rs1, rs2, offset    //if (rs1 >= rs2) pc += sext(offset)
 */
def_EHelper(bgeu) {
  static rtlreg_t target;
  if ((word_t)*id_src1->preg >= (word_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    Log("BGEUJ>> " FMT_WORD, target);
    rtl_jr(s, &target);
  }
}
