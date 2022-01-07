#include <common.h>
#include "am.h"
#include ARCH_H

static Context* do_event(Event e, Context* c) {
  Log("Event: %d", e.event);
  switch (e.event) {
    case EVENT_YIELD:
      Log("Meet EVENT_YIELD");
      break;
    case EVENT_SYSCALL:
      Log("Meet EVENT_SYSCALL");
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
