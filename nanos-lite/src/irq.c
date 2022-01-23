#include <common.h>

#include "am.h"
#include ARCH_H

static void (*on_yield)(Event, Context*) = NULL;

static Context* do_event(Event e, Context* c) {
  // Log("Event: %d", e.event);
  switch (e.event) {
    case EVENT_YIELD:
      // Log("Meet EVENT_YIELD");
      if (on_yield) on_yield(e, c);
      break;
    case EVENT_SYSCALL:
      Log("Meet EVENT_SYSCALL");
      break;
    default:
      panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}

void init_on_yield(void (*target)(Event, Context*)) { on_yield = target; }