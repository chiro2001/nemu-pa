// #include "../../../../../abstract-machine/am/include/am.h"
#include "isa-def.h"
#include "rtl-basic.h"
def_EHelper(inv) { rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0); }

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0);  // gpr(10) is $a0
}

/**
 * @brief RV Privileged
 */

// t = CSRs[csr]; CSRs[csr] = x[rs1]; x[rd] = t
def_EHelper(csrrw) {
  static rtlreg_t t;
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  t = csr->val;
  csr->val = *dsrc1;
  Log("write: " FMT_WORD, csr->val);
  *ddest = t;
}
// t = CSRs[csr]; CSRs[csr] = t | x[rs1]; x[rd] = t
def_EHelper(csrrs) {
  static rtlreg_t t;
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  t = csr->val;
  csr->val = *dsrc1 | t;
  *ddest = t;
}
// t = CSRs[csr]; CSRs[csr] = t &~x[rs1]; x[rd] = t
def_EHelper(csrrc) {
  static rtlreg_t t;
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  t = csr->val;
  csr->val = ~*dsrc1 & t;
  *ddest = t;
}
// x[rd] = CSRs[csr]; CSRs[csr] = zimm
def_EHelper(csrrwi) {
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  *ddest = csr->val;
  csr->val = *dsrc1;
}
// t = CSRs[csr]; CSRs[csr] = t | zimm; x[rd] = t
def_EHelper(csrrsi) {
  static rtlreg_t t;
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  t = csr->val;
  csr->val = id_src1->imm | t;
  *ddest = t;
}
// t = CSRs[csr]; CSRs[csr] = t &~zimm; x[rd] = t
def_EHelper(csrrci) {
  static rtlreg_t t;
  regs_csr_t* csr = csr_find(id_src2->imm);
  if (!csr) return;
  Assert(csr, "Unsupported CSR: " FMT_WORD, id_src2->imm);
  t = csr->val;
  csr->val = ~id_src1->imm & t;
  *ddest = t;
}

def_EHelper(ecall) {
  static rtlreg_t target;
  Log("ecall now");
  target = isa_raise_intr(EVENT_YIELD, 0);
  rtl_jr(s, &target);
}

def_EHelper(ebreak) {}

def_EHelper(wfi) { panic("No wfi!"); }

def_EHelper(fence) {}

def_EHelper(mret) {
  static rtlreg_t target;
  Log("ecall now");
  target = isa_query_intr();
  rtl_jr(s, &target);
}

/**
 * @brief RVA
 */

// x[rd] = AMO32(M[x[rs1]] + x[rs2])
def_EHelper(amoadd) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  // Log("M[x[rs1]] = M[" FMT_WORD "] = " FMT_WORD, *dsrc1, m);
  // Log("amoadd: x[rd] = AMO32(" FMT_WORD " + " FMT_WORD ")", m, *dsrc2);
  // rtl_li(s, ddest, m + *dsrc2);
  rtl_li(s, ddest, 0);
}

// x[rd] = AMO32(M[x[rs1]] & x[rs2])
def_EHelper(amoand) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, m & *dsrc2);
}

// x[rd] = AMO32(M[x[rs1]] | x[rs2])
def_EHelper(amoor) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, m | *dsrc2);
}

// x[rd] = AMO32(M[x[rs1]] ^ x[rs2])
def_EHelper(amoxor) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, m ^ *dsrc2);
}

#ifndef max_
#define max_(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min_
#define min_(a, b) ((a) < (b) ? (a) : (b))
#endif

// x[rd] = AMO32(M[x[rs1]] MAX x[rs2])
def_EHelper(amomax) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, max_((sword_t)m, (sword_t)*dsrc2));
}

// x[rd] = AMO32(M[x[rs1]] MAXU x[rs2])
def_EHelper(amomaxu) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, max_(m, *dsrc2));
}

// x[rd] = AMO32(M[x[rs1]] MIN x[rs2])
def_EHelper(amomin) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, min_((sword_t)m, (sword_t)*dsrc2));
}

// x[rd] = AMO32(M[x[rs1]] MINU x[rs2])
def_EHelper(amominu) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_li(s, ddest, min_(m, *dsrc2));
}

// x[rd] = AMO32(M[x[rs1]] SWAP x[rs2])
def_EHelper(amoswap) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_sm(s, dsrc2, dsrc1, 0, 4);
  rtl_li(s, ddest, m);
}

// x[rd] = LoadReserved32(M[x[rs1]])
def_EHelper(lr) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  // TODO: regist this addr
  rtl_li(s, ddest, m);
}

// x[rd] = StoreConditonal32(M[x[rs1], x[rs2])
def_EHelper(sc) {
  static rtlreg_t m;
  rtl_lm(s, &m, dsrc1, 0, 4);
  rtl_sm(s, dsrc2, dsrc1, 0, 4);
  // TODO: judge regist
  rtl_li(s, ddest, 0);
}
