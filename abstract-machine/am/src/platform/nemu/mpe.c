#include <am.h>
#include <klib-macros.h>
// #include <stdatomic.h>

bool mpe_init(void (*entry)()) {
  entry();
  // panic("MPE entry returns");
  return true;
}

int cpu_count() { return 1; }

int cpu_current() { return 0; }

// TODO: fixme
// int atomic_xchg(atomic_type *addr, int newval) {
int atomic_xchg(int *addr, int newval) {
  // return atomic_exchange(*addr, newval);
  if (*addr && newval) return 1;
  *addr = newval;
  return 0;
}
