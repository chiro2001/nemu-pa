#include <cpu/ifetch.h>
#include <isa-all-instr.h>

#include "../local-include/reg.h"

def_all_THelper();

static uint32_t get_instr(Decode *s) { return s->isa.instr.val; }

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name)(Decode * s, Operand * op, word_t val, bool flag)

/**
 * @brief 取立即数到op->imm
 * @param val: 值
 */
static def_DopHelper(i) {
  op->imm = val;
  print_Dop(op->str, OP_STR_SIZE, (flag ? "0x%x" : "%d"), op->imm);
}

/**
 * @brief 取寄存器操作信息
 * @param val: 寄存器号
 * @param flag: 是否写入（regWrite）
 */
static def_DopHelper(r) {
  bool is_write = flag;
  // x0寄存器
  static word_t zero_null = 0;
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(val, 4));
}

static def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, imm_sext32(s->isa.instr.i.simm11_0, 12), false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(U) {
  decode_op_i(s, id_src1, imm_sext32(s->isa.instr.u.imm31_12 << 12, 20),
              true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm =
      imm_sext32((s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0, 12);
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

static def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, false);
}

// static def_DHelper(B) {
//   decode_op_r(s, id_src1, s->isa.instr.b.rs1, false);
//   sword_t simm = imm_sext32(
//       (s->isa.instr.b.simm12 << 12) | (s->isa.instr.b.imm11 << 11) |
//           (s->isa.instr.b.imm10_5 << 5) | (s->isa.instr.b.imm4_1 << 1),
//       13);
//   decode_op_i(s, id_src2, simm, false);
//   decode_op_r(s, id_dest, s->isa.instr.b.rs2, false);
// }

// static def_DHelper(J) {
//   sword_t simm = imm_sext32(
//       (s->isa.instr.j.simm20 << 20) | (s->isa.instr.j.imm19_12 << 12) |
//           (s->isa.instr.j.imm11 << 11) | (s->isa.instr.j.imm10_1 << 1),
//       21);
//   decode_op_i(s, id_src2, simm, false);
//   decode_op_r(s, id_dest, s->isa.instr.j.rd, false);
// }

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  return EXEC_ID_inv;
}

def_THelper(jalr) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", jalr);
  return EXEC_ID_inv;
}

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I, load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S, store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U, lui);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R, add);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", R, addi);

  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I, jalr);

  // def_INSTR_TAB("??????? ????? ????? ??? ????? 11010 11", nemu_trap);
  def_INSTR_TAB("00000000000100000000000001110011", nemu_trap);
  return table_inv(s);
};

/**
 * @brief 取指函数
 *
 * @param s
 * @return int g_exec_table[]的索引，指向需要执行的函数
 */
int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
