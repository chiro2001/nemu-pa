#include <isa.h>
#include <memory/paddr.h>

#include "isa-def.h"

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img[] = {
    0x800002b7,  // lui t0,0x80000
    0x0002a023,  // sw  zero,0(t0)
    0x0002a503,  // lw  a0,0(t0)
    0x0000006b,  // nemu_trap
};

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  cpu.gpr[0]._32 = 0;
}

regs_csr_t regs_csr_map[RISCV_CSR_REG_COUNT];
regs_csr_t *csr_mcause, *csr_mepc, *csr_mtvec, *csr_mstatus;
int regs_csr_tail = 0;

void csr_insert(regs_csr_t t) {
  // Log("CSR Init: " FMT_WORD " " FMT_WORD " %s", t.addr, t.val, t.name);
  memcpy(&regs_csr_map[regs_csr_tail++], &t, sizeof(regs_csr_t));
}

#define CSRA(addr, val, name) csr_insert((regs_csr_t){addr, val, #name})
#define CSRI(addr, name) CSRA(addr, 0, name)

regs_csr_t *csr_find_inner(uint32_t addr, int left, int right) {
  // Log("[%d, %d]", left, right);
  if (addr < regs_csr_map[left].addr) return NULL;
  if (addr > regs_csr_map[right].addr) return NULL;
  if (left == right) return &regs_csr_map[left];
  if (left + 1 == right) {
    if (addr == regs_csr_map[left].addr) return &regs_csr_map[left];
    if (addr == regs_csr_map[right].addr) return &regs_csr_map[right];
    return NULL;
  }
  int mid = (left + right) / 2;
  if (addr == regs_csr_map[mid].addr) return &regs_csr_map[mid];
  if (addr > regs_csr_map[mid].addr)
    return csr_find_inner(addr, mid, right);
  else
    return csr_find_inner(addr, left, mid);
}

regs_csr_t *csr_find(uint32_t addr) {
  regs_csr_t *t =
      csr_find_inner(addr, 0, regs_csr_tail > 0 ? regs_csr_tail - 1 : 0);
  if (t)
    Log("CSR Found: " FMT_WORD "[" FMT_WORD "]" " %s", t->addr, t->val, t->name);
  else
    Log(ASNI_FMT("CSR Not Found: " FMT_WORD, ASNI_FG_BLACK ASNI_BG_RED), addr);
  return t;
}

void init_csr() {
  // Init CSR Regs
  memset(regs_csr_map, 0, sizeof(regs_csr_t) * RISCV_CSR_REG_COUNT);
  
  CSRI(0x001, fflags);
  CSRI(0x002, frm);
  CSRI(0x003, fcsr);

  CSRI(0x100, sstatus);
  CSRI(0x104, sie);
  CSRI(0x144, sip);

  CSRA(0x300, 0x1800, mstatus);
  CSRI(0x301, misa);
  CSRI(0x304, mie);
  CSRI(0x305, mtvec);
  CSRI(0x340, mscratch);
  CSRI(0x341, mepc);
  CSRI(0x342, mcause);
  CSRI(0x343, mtval);
  CSRI(0x344, mip);
  
  CSRI(0xb00, mcycle);
  CSRI(0xb80, mcycleh);
  CSRI(0xb82, minstret);
  
  CSRI(0xf11, mvendorid);
  CSRI(0xf12, marchid);
  CSRI(0xf13, mmimpid);
  CSRI(0xf14, mhartid);
  // sort for index
  regs_csr_t t;
  for (int i = 0; i < regs_csr_tail; i++) {
    for (int j = 0; j < i; j++) {
      if (regs_csr_map[i].addr < regs_csr_map[j].addr) {
        t = regs_csr_map[i];
        regs_csr_map[i] = regs_csr_map[j];
        regs_csr_map[j] = t;
      }
    }
  }
  csr_mstatus = csr_find(0x300);
  csr_mepc = csr_find(0x341);
  csr_mcause = csr_find(0x342);
  csr_mtvec = csr_find(0x305);
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  init_csr();

  /* Initialize this virtual computer system. */
  restart();
}
