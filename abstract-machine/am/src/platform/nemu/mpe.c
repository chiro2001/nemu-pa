#include <am.h>
#include <klib-macros.h>
// #include <stdatomic.h>

bool mpe_init(void (*entry)()) {
  entry();
  panic("MPE entry returns");
}

int cpu_count() { return 1; }

int cpu_current() { return 0; }

int atomic_xchg(int *addr, int newval) {
  // __extension__({
  //   __auto_type __atomic_exchange_ptr = (addr);
  //   __typeof__(*__atomic_exchange_ptr) __atomic_exchange_val = (newval);
  //   __typeof__(*__atomic_exchange_ptr) __atomic_exchange_tmp;
  //   __atomic_exchange(__atomic_exchange_ptr, &__atomic_exchange_val,
  //                     &__atomic_exchange_tmp, (5));
  //   __atomic_exchange_tmp;
  // });
  // return atomic_exchange(addr, newval);
  // TODO: fixme
  return 0;
}
