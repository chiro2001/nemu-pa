#include <cpu/difftest.h>
#include <isa.h>
#include <libgen.h>
#include <unistd.h>

#include "../local-include/reg.h"


bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool reg_diffs[32];
  bool same = cpu.pc == ref_r->pc;
  memset(reg_diffs, 0, sizeof(bool) * 32);
  for (size_t i = 0; i < 32; i++) {
    if (ref_r->gpr[i]._32 != gpr(i)) {
      same = false;
      reg_diffs[i] = true;
    }
  }
  if (!same) {
    const char *section = find_section(ref_r->pc);
    Log(ASNI_FMT("Difftest failed at " FMT_WORD " : " FMT_WORD " ! @%s",
                 ASNI_FG_RED),
        cpu.pc, ref_r->pc, section);
    for (size_t i = 0; i < 32; i++) {
      if (reg_diffs[i]) {
        Log("REG DIFF %s: " FMT_WORD " : " FMT_WORD, reg_name(i, 0), gpr(i),
            ref_r->gpr[i]._32);
      }
    }
  }
  return same;
}

void isa_difftest_attach() {}
