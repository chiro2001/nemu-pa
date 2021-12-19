def_EHelper(lw) { rtl_lm(s, ddest, dsrc1, id_src2->imm, 4); }

def_EHelper(lb) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
  *ddest = imm_sext32(*ddest & 0xFF, 8);
}
def_EHelper(lh) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
  *ddest = imm_sext32(*ddest & 0xFFFF, 16);
}
def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
  Log("read " FMT_WORD " -> " FMT_WORD, *ddest, *ddest & 0xFF);
  *ddest = *ddest & 0xFF;
}
def_EHelper(lhu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
  Log("read " FMT_WORD " -> " FMT_WORD, *ddest, *ddest & 0xFFFF);
  *ddest = *ddest & 0xFFFF;
}
// sw rs2, offset(rs1)   //M[x[rs1] + sext(offset) = x[rs2][31: 0]
def_EHelper(sw) { rtl_sm(s, ddest, dsrc1, id_src2->imm, 4); }
// sb rs2, offset(rs1)   //M[x[rs1] + sext(offset)] = x[rs2][7: 0]
def_EHelper(sb) {
  static rtlreg_t target;
  target = *ddest & 0xFF;
  rtl_sm(s, &target, dsrc1, id_src2->imm, 1);
}
// def_EHelper(sh) { rtl_sm(s, ddest, dsrc1, id_src2->imm & 0xFFFF, 2); }
def_EHelper(sh) {
  static rtlreg_t target;
  target = *ddest & 0xFFFF;
  rtl_sm(s, &target, dsrc1, id_src2->imm, 2);
}
