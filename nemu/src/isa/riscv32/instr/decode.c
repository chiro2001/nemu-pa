#include <cpu/ifetch.h>
#include <isa-all-instr.h>

#include "../local-include/reg.h"

def_all_THelper();

static uint32_t get_instr(Decode *s) { return s->isa.instr.val; }

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name)(Decode * s, Operand * op, word_t val, bool flag)

#define imm_expand(val, width_val, width_exp)                             \
  (((((val) >> ((width_val)-1)) & 0x01)                                   \
        ? ((((1llu << (width_exp - 1)) + 1) - ((1 << (width_val)) - 1)) | \
           (val))                                                         \
        : (val)) &                                                        \
   0xFFFFFFFF)

#define imm_expand32(val, width_val) imm_expand(val, width_val, 32)

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
  decode_op_i(s, id_src2, imm_expand32(s->isa.instr.i.simm11_0, 12), false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(U) {
  decode_op_i(s, id_src1, imm_expand32(s->isa.instr.u.imm31_12 << 12, 20),
              true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm =
      imm_expand32((s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0, 12);
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
//   sword_t simm = imm_expand32(
//       (s->isa.instr.b.simm12 << 12) | (s->isa.instr.b.imm11 << 11) |
//           (s->isa.instr.b.imm10_5 << 5) | (s->isa.instr.b.imm4_1 << 1),
//       13);
//   decode_op_i(s, id_src2, simm, false);
//   decode_op_r(s, id_dest, s->isa.instr.b.rs2, false);
// }

// static def_DHelper(J) {
//   sword_t simm = imm_expand32(
//       (s->isa.instr.j.simm20 << 20) | (s->isa.instr.j.imm19_12 << 12) |
//           (s->isa.instr.j.imm11 << 11) | (s->isa.instr.j.imm10_1 << 1),
//       21);
//   decode_op_i(s, id_src2, simm, false);
//   decode_op_r(s, id_dest, s->isa.instr.j.rd, false);
// }

// def_THelper(lw) {
//   return EXEC_ID_lw;
// }

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  // def_INSTR_IDTABW("??????? ????? ????? 010 ????? ????? ??", empty, lw, 0);
  // def_INSTR_raw(pattern_decode, "??????? ????? ????? 010 ????? ????? ??", {
  //   concat(decode_, empty)(s, 0);
  //   return concat(table_, lw)(s);
  // });
  // do {
  //   uint32_t key, mask, shift;
  //   pattern_decode("??????? ????? ????? 010 ????? ????? ??",
  //                  STRLEN("??????? ????? ????? 010 ????? ????? ??"), &key,
  //                  &mask, &shift);
  //   if (((get_instr(s) >> shift) & mask) == key) {
  //     {
  //       decode_empty(s, 0);
  //       return table_lw(s);
  //     };
  //   }
  // } while (0);
  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  return EXEC_ID_inv;
}

/**
 * @brief ALU parts
 */

// def_THelper(add) {
//   return EXEC_ID_add;
// }
// def_THelper(addi) {
//   return EXEC_ID_addi;
// }

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I, load);
  // def_INSTR_IDTABW("??????? ????? ????? ??? ????? 00000 11", I, load, 0);
  // def_INSTR_raw(pattern_decode, "??????? ????? ????? ??? ????? 00000 11", {
  //   concat(decode_, I)(s, 0);
  //   return concat(table_, load)(s);
  // });
  // do {
  //   uint32_t key, mask, shift;
  //   pattern_decode("??????? ????? ????? ??? ????? 00000 11",
  //                  STRLEN("??????? ????? ????? ??? ????? 00000 11"), &key,
  //                  &mask, &shift);
  //   if (((get_instr(s) >> shift) & mask) == key) {
  //     {
  //       decode_I(s, 0);
  //       return table_load(s);
  //     };
  //   }
  // } while (0);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S, store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U, lui);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R, add);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", R, addi);
  def_INSTR_TAB("??????? ????? ????? ??? ????? 11010 11", nemu_trap);
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
