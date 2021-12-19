#ifdef CONFIG_EXT_PRINT_LOADSAVE
#define print_load(val) Log(ASNI_FMT("load", ASNI_FG_YELLOW ASNI_BG_RED) " M[" FMT_WORD "]" FMT_WORD, *dsrc1 + id_src2->imm, (val))
#define print_save(val) Log(ASNI_FMT("save", ASNI_FG_BLACK ASNI_BG_WHITE) " " FMT_WORD " -> M[" FMT_WORD "]", (val), *dsrc1 + id_src2->imm)
#else
#define print_load(val)
#define print_save(val)
#endif
def_EHelper(lw) {
  print_load(*ddest);
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}
def_EHelper(lb) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
  print_load(*ddest & 0xFF);
  *ddest = imm_sext32(*ddest & 0xFF, 8);
}
def_EHelper(lh) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
  print_load(*ddest & 0xFFFF);
  *ddest = imm_sext32(*ddest & 0xFFFF, 16);
}
def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
  print_load(*ddest & 0xFF);
  *ddest = *ddest & 0xFF;
}
def_EHelper(lhu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
  print_load(*ddest & 0xFFFF);
  *ddest = *ddest & 0xFFFF;
}
def_EHelper(sw) {
  print_save(*ddest);
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}
def_EHelper(sb) {
  static rtlreg_t target;
  target = *ddest & 0xFF;
  print_save(target);
  rtl_sm(s, &target, dsrc1, id_src2->imm, 1);
}
def_EHelper(sh) {
  static rtlreg_t target;
  target = *ddest & 0xFFFF;
  print_save(target);
  rtl_sm(s, &target, dsrc1, id_src2->imm, 2);
}
