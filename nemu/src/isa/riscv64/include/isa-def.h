#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>

typedef struct {
  union {
    uint64_t _64;
  } gpr[32];

  vaddr_t pc;
} riscv64_CPU_state;

// decode
typedef struct {
  union {
    // 指令格式数据位置解码
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      int32_t  simm31_12 :20;
    } u;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11     : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      int32_t  simm12    : 1;
    } b;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm19_12  : 8;
      uint32_t imm11     : 1;
      uint32_t imm_10_1  :10;
      int32_t  simm20    : 1;
    } j;
    uint32_t val;
  } instr;
} riscv64_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
