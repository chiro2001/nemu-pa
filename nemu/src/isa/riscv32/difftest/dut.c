#include <cpu/difftest.h>
#include <isa.h>
#include <libgen.h>
#include <unistd.h>

#include "../local-include/reg.h"

char cwd[128];
char buf2[512];
char buf3[512];
char buf[1024];
char g_section[64];

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
    char *section = g_section;
    if (img_file) {
      char *c = getcwd(cwd, 128);
      assert(c);
      strcpy(buf2, basename(img_file));
      char *p = buf2;
      while (*p) {
        if (*p == '.') *p = '\0';
        p++;
      }
      FILE *f;
      sprintf(buf, "find $NEMU_HOME/../ -name \"%s.elf\" | head -n 1", buf2);
      // printf("%s\n", buf);
      f = popen(buf, "r");
      int r = fscanf(f, "%s", buf3);
      assert(r);
      pclose(f);
      sprintf(buf,
              "echo @"
              "`riscv64-linux-gnu-readelf %s -s | grep FUNC | tail -n +4 | sort -k 2 "
              "| awk '{print \"0x\"$2 $0}' | awk '{printf \"%%d %%s\\n\", "
              "strtonum($1), $0}' | awk '$1 > " "%u" " {print $0}' | head -n 1 | awk '{ print $NF }'` ; cd %s",
              buf3, ref_r->pc, cwd);
      // printf("%s\n", buf);
      f = popen(buf, "r");
      r = fscanf(f, "%s", section);
      assert(r);
      pclose(f);
    } else {
      section = NULL;
    }
    Log(ASNI_FMT("Difftest failed at " FMT_WORD " : " FMT_WORD "! %s",
                 ASNI_FG_RED),
        cpu.pc, ref_r->pc, section ? section : "");
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
