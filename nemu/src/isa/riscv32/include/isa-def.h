#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
} riscv32_CPU_state;

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
      uint32_t imm31_12  :20;
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
      uint32_t imm10_1   :10;
      int32_t  simm20    : 1;
    } j;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;

#ifndef RISCV_CSR_REG_COUNT
#define RISCV_CSR_REG_COUNT 32
#endif
typedef struct regs_csr_type {
  uint32_t addr;
  rtlreg_t val;
  char name[32];
} regs_csr_t;
extern regs_csr_t regs_csr_map[RISCV_CSR_REG_COUNT];

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

regs_csr_t *csr_find(uint32_t addr);

#endif
