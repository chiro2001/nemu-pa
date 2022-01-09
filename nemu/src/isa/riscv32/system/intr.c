#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  /*
  riscv32触发异常后硬件的响应过程如下:
  1.  将当前PC值保存到mepc寄存器
  2.  在mcause寄存器中设置异常号
  3.  从mtvec寄存器中取出异常入口地址
  4.  跳转到异常入口地址
  */
  csr_mepc->val = cpu.pc;
  // Log("save: " FMT_WORD, csr_mepc->val);
  csr_mcause->val = NO;
  // Log("load: " FMT_WORD, csr_mtvec->val);
  return csr_mtvec->val;
}

word_t isa_query_intr() {
  // return INTR_EMPTY;
  /*
  riscv32从异常中恢复:
  1.  从mepc读取之前的pc
  */
  return csr_mepc->val;
}
