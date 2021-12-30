#include "isa-def.h"
#include "rtl-basic.h"
def_EHelper(inv) { rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0); }

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0);  // gpr(10) is $a0
}

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
  target = isa_raise_intr(1, 0);
  rtl_jr(s, &target);
}

def_EHelper(ebreak) {}