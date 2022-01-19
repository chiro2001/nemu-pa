#include <cpu/cpu.h>
#include <cpu/difftest.h>
#include <cpu/exec.h>
#include <isa-all-instr.h>
#include <locale.h>
#include <pthread.h>
#include <signal.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_instr = 0;
static uint64_t g_timer = 0;  // unit: us
static bool g_print_step = true;
const rtlreg_t rzero = 0;
rtlreg_t tmp_reg[4];

void device_update();
void fetch_decode(Decode *s, vaddr_t pc);

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
#ifndef ITRACE_COND
#define ITRACE_COND "true"
#endif
  if (ITRACE_COND) log_write("%s\n", _this->logbuf);
#endif
  extern bool log_enable();
  if ((g_print_step IFDEF(CONFIG_ITRACE, || true)) && log_enable()) {
    IFDEF(CONFIG_ITRACE, Log("%s", _this->logbuf));
  }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
}

#include <isa-exec.h>

#define FILL_EXEC_TABLE(name) [concat(EXEC_ID_, name)] = concat(exec_, name),
static const void *g_exec_table[TOTAL_INSTR] = {
    MAP(INSTR_LIST, FILL_EXEC_TABLE)};

Decode g_s;
vaddr_t g_s_pc = 0x00000000;
static bool fetch_decode_run = true;
pthread_spinlock_t instr_lock;
static pthread_t instr_thread_id = 0;

static uint32_t instr_missed = 0;
void my_on_exit() {
  if (instr_thread_id) {
    pthread_kill(instr_thread_id, SIGINT);
  } else {
    Log("instr_missed = " FMT_WORD, instr_missed);
  }
}
void sign_handle(int sign) {
  my_on_exit();
  exit(0);
}

void fetch_decode_background() {
  static vaddr_t pc_decoded = 0;
  instr_thread_id = 0;
  while (fetch_decode_run) {
    if (!g_s_pc) continue;
    if (pc_decoded == g_s_pc) {
      instr_missed++;
      continue;
    }
    pthread_spin_lock(&instr_lock);
    vaddr_t pc_target = g_s_pc;
    fetch_decode(&g_s, pc_target);
    pthread_spin_unlock(&instr_lock);
    pc_decoded = pc_target;
  }
}

static void fetch_decode_exec_updatepc(Decode *s) {
#ifdef CONFIG_EXT_MULTI_THREAD
  if (g_s_pc == cpu.pc) {
    pthread_spin_lock(&instr_lock);
    *s = g_s;
    g_s_pc = cpu.pc + 4;
    pthread_spin_unlock(&instr_lock);
  } else {
    pthread_spin_lock(&instr_lock);
    g_s_pc = cpu.pc + 4;
    pthread_spin_unlock(&instr_lock);
    // Log("No instr decode predecode at " FMT_WORD, cpu.pc);
  }
#endif
  fetch_decode(s, cpu.pc);
  s->EHelper(s);
  cpu.pc = s->dnpc;
}

void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%ld", "%'ld")
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_instr);
  if (g_timer > 0)
    Log("simulation frequency = " NUMBERIC_FMT " instr/s",
        g_nr_guest_instr * 1000000 / g_timer);
  else
    Log("Finish running in less than 1 us and can not calculate the simulation "
        "frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

void fetch_decode(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  int idx = isa_fetch_decode(s);
  s->dnpc = s->snpc;
  s->EHelper = g_exec_table[idx];
#ifdef CONFIG_TRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), "%07d " FMT_WORD ":",
                (int)g_nr_guest_instr, s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *instr = (uint8_t *)&s->isa.instr.val;
  for (i = 0; i < ilen; i++) {
    p += snprintf(p, 4, " %02x", instr[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
              MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc),
              (uint8_t *)&s->isa.instr.val, ilen);
#endif
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INSTR_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END:
    case NEMU_ABORT:
      printf(
          "Program execution has ended. To restart the program, exit NEMU and "
          "run again.\n");
      return;
    default:
      nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  signal(SIGINT, sign_handle);

#ifdef CONFIG_EXT_MULTI_THREAD
  // start instr decode thread
  // DEFINE_SPINLOCK(instr_lock);
  int spin_ret = pthread_spin_init(&instr_lock, PTHREAD_PROCESS_SHARED);
  Assert(spin_ret == 0, "Cannot create spinlock!");
  int thread_ret = pthread_create(&instr_thread_id, NULL,
                                  (void *)fetch_decode_background, NULL);

  Assert(thread_ret == 0, "Cannot create instr decode thread!");
#endif
  Decode s;
  for (; n > 0; n--) {
    fetch_decode_exec_updatepc(&s);
    g_nr_guest_instr++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }

#ifdef CONFIG_EXT_MULTI_THREAD
  fetch_decode_run = false;
  pthread_join(instr_thread_id, NULL);
#endif

  IFDEF(CONFIG_DEVICE, serial_putc_buffed(0xFF));

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING:
      nemu_state.state = NEMU_STOP;
      break;

    case NEMU_END:
    case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD " @%s",
          (nemu_state.state == NEMU_ABORT
               ? ASNI_FMT("ABORT", ASNI_FG_RED)
               : (nemu_state.halt_ret == 0
                      ? ASNI_FMT("HIT GOOD TRAP", ASNI_FG_GREEN)
                      : ASNI_FMT("HIT BAD TRAP", ASNI_FG_RED))),
          nemu_state.halt_pc, find_section(nemu_state.halt_pc));
      // fall through
    case NEMU_QUIT:
      statistic();
      my_on_exit();
  }
}
