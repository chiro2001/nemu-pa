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
    Log("CSR Found: " FMT_WORD " %s", t->addr, t->name);
  else
    Log(ASNI_FMT("CSR Not Found: " FMT_WORD, ASNI_FG_BLACK ASNI_BG_RED), addr);
  return t;
}

void init_csr() {
  // Init CSR Regs
  memset(regs_csr_map, 0, sizeof(regs_csr_t) * RISCV_CSR_REG_COUNT);
  CSRA(0x300, 0x1800, mstatus);
  CSRI(0x341, mepc);
  CSRI(0x342, mcause);
  CSRI(0x305, mtvec);
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
  // test
  // regs_csr_t *target = NULL;
  // target = csr_find(0x001);
  // Log("CSR Test: %s", target ? target->name : "404");
  // target = csr_find(0x002);
  // Log("CSR Test: %s", target ? target->name : "404");
  // target = csr_find(0x003);
  // Log("CSR Test: %s", target ? target->name : "404");
  // target = csr_find(0x300);
  // Log("CSR Test: %s", target ? target->name : "404");
  // target = csr_find(0x012);
  // Log("CSR Test: %s", target ? target->name : "404");
  // target = csr_find(0xFFF);
  // Log("CSR Test: %s", target ? target->name : "404");
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  init_csr();

  /* Initialize this virtual computer system. */
  restart();
}