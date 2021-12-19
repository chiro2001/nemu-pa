#include "rtl-basic.h"
def_EHelper(lui) { rtl_li(s, ddest, id_src1->imm); }
// auipc rd, immediate    //x[rd] = pc + sext(immediate[31:12] << 12)
def_EHelper(auipc) { rtl_li(s, ddest, imm_sext32(id_src1->imm, 20) + s->pc); }
def_EHelper(add) { rtl_add(s, ddest, dsrc1, dsrc2); }
def_EHelper(addi) { rtl_addi(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(sub) { rtl_sub(s, ddest, dsrc1, dsrc2); }
def_EHelper(xor) { rtl_xor(s, ddest, dsrc1, dsrc2); }
def_EHelper(xori) { rtl_xori(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(or) { rtl_or(s, ddest, dsrc1, dsrc2); }
def_EHelper(ori) { rtl_ori(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(and) { rtl_and(s, ddest, dsrc1, dsrc2); }
def_EHelper(andi) { rtl_andi(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(sll) { rtl_sll(s, ddest, dsrc1, dsrc2); }
def_EHelper(slli) {
  // Log("slli" FMT_WORD ", %d", *dsrc1, id_src2->imm);
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
  // Log("    -> " FMT_WORD, *ddest);
}
def_EHelper(srl) { rtl_srl(s, ddest, dsrc1, dsrc2); }
def_EHelper(srli) { rtl_srli(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(sra) { rtl_sra(s, ddest, dsrc1, dsrc2); }
def_EHelper(srai) { rtl_srai(s, ddest, dsrc1, id_src2->imm); }
def_EHelper(slt) {
  rtl_li(s, ddest, (sword_t)*dsrc1 < (sword_t)*dsrc2 ? 1 : 0);
}
def_EHelper(slti) {
  rtl_li(s, ddest, (sword_t)*dsrc1 < (sword_t)id_src2->imm ? 1 : 0);
}
def_EHelper(sltu) { rtl_li(s, ddest, *dsrc1 < *dsrc2 ? 1 : 0); }
def_EHelper(sltiu) { rtl_li(s, ddest, *dsrc1 < id_src2->imm ? 1 : 0); }
/**
 * @brief  jal x[rd] = pc+4; pc += sext(offset)
 */
def_EHelper(jal) {
  static rtlreg_t t, target;
  t = (rtlreg_t)(s->pc + 4);
  target = (rtlreg_t)(s->pc + id_src2->imm);
  IFDEF(CONFIG_EXT_PRINT_JUMP, Log("JUMP >> " FMT_WORD, target));
  rtl_jr(s, &target);
  rtl_li(s, ddest, t);
}

/**
 * @brief  jalr t = pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
 */
def_EHelper(jalr) {
  static rtlreg_t t, target;
  t = (rtlreg_t)(s->pc + 4);
  target = *dsrc1 + imm_sext32(id_src2->imm, 12);
  IFDEF(CONFIG_EXT_PRINT_JUMP, Log("JUMPR>> " FMT_WORD, target));
  rtl_jr(s, &target);
  rtl_li(s, ddest, t);
}

/**
 * @brief  beq rs1, rs2, offset   //if (rs1 == rs2) pc += sext(offset)
 */
def_EHelper(beq) {
  static rtlreg_t target;
  // Log("beq: " FMT_WORD ", " FMT_WORD, *dsrc1, id_src2->imm);
  if (*dsrc1 == *ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BEQJ >> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bne rs1, rs2, offset   //if (rs1 =/= rs2) pc += sext(offset)
 */
def_EHelper(bne) {
  static rtlreg_t target;
  if (*dsrc1 != *ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BNEJ >> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

/**
 * @brief  blt rs1, rs2, offset    //if (rs1 <s rs2) pc += sext(offset)
 */
def_EHelper(blt) {
  static rtlreg_t target;
  if ((sword_t)*dsrc1 < (sword_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BLTJ >> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bge rs1, rs2, offset    //if (rs1 >=s rs2) pc += sext(offset)
 */
def_EHelper(bge) {
  static rtlreg_t target;
  if ((sword_t)*dsrc1 >= (sword_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BGEJ >> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bltu rs1, rs2, offset    //if (rs1 < rs2) pc += sext(offset)
 */
def_EHelper(bltu) {
  static rtlreg_t target;
  if ((word_t)*dsrc1 < (word_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BLTUJ>> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

/**
 * @brief  bgeu rs1, rs2, offset    //if (rs1 >= rs2) pc += sext(offset)
 */
def_EHelper(bgeu) {
  static rtlreg_t target;
  if ((word_t)*dsrc1 >= (word_t)*ddest) {
    target = s->pc + imm_sext32(id_src2->imm, 13);
    IFDEF(CONFIG_EXT_PRINT_JUMP, Log("BGEUJ>> " FMT_WORD, target));
    rtl_jr(s, &target);
  }
}

def_EHelper(mul) { rtl_mulu_lo(s, ddest, dsrc1, dsrc2); }
def_EHelper(mulh) { rtl_muls_hi(s, ddest, dsrc1, dsrc2); }
def_EHelper(mulhsu) { rtl_muls_hi(s, ddest, dsrc1, dsrc2); }
def_EHelper(mulhu) { rtl_mulu_hi(s, ddest, dsrc1, dsrc2); }
def_EHelper(div) { rtl_divs_q(s, ddest, dsrc1, dsrc2); }
def_EHelper(divu) { rtl_divu_q(s, ddest, dsrc1, dsrc2); }
def_EHelper(rem) { rtl_li(s, ddest, (sword_t)*dsrc1 % (sword_t)*dsrc2); }
def_EHelper(remu) { rtl_li(s, ddest, *dsrc1 % *dsrc2); }