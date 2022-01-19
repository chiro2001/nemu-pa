#include <device/map.h>
#include <utils.h>

/* http://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming */
// NOTE: this is compatible to 16550

#define CH_OFFSET 0

static uint8_t *serial_base = NULL;

#define SERIAL_BUF_SIZE 1024

char g_serial_buf[SERIAL_BUF_SIZE];
char *g_serial_p;

void serial_putc_buffed(char ch) {
  // printf("PUTC(%c)\n", ch);
  if (g_serial_p - g_serial_buf < SERIAL_BUF_SIZE)
    *(g_serial_p++) = ch == '\n' ? '\0' : ch;
  if ((ch == '\0' || ch == '\n')) {
    if (*g_serial_buf) {
      printf(ASNI_FMT("%s", ASNI_FG_BLACK ASNI_BG_GREEN) "\n", g_serial_buf);
    } else {
      if (ch != 0xFF)
        printf(ASNI_FMT("%s", ASNI_FG_BLACK ASNI_BG_GREEN) "\n", "[EMPTY STR]");
    }
    g_serial_p = g_serial_buf;
    memset(g_serial_buf, 0, sizeof(g_serial_buf));
  }
}

static void serial_putc(char ch) {
  // MUXDEF(CONFIG_TARGET_AM, putch(ch), putc(ch, stderr));
  MUXDEF(CONFIG_TARGET_AM, putch(ch), serial_putc_buffed(ch));
}

static void serial_io_handler(uint32_t offset, int len, bool is_write) {
  assert(len == 1);
  switch (offset) {
    /* We bind the serial port with the host stderr in NEMU. */
    case CH_OFFSET:
      if (is_write)
        serial_putc(serial_base[0]);
      else
        panic("do not support read");
      break;
    default:
      panic("do not support offset = %d", offset);
  }
}

void init_serial() {
  serial_base = new_space(8);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map("serial", CONFIG_SERIAL_PORT, serial_base, 8, serial_io_handler);
#else
  add_mmio_map("serial", CONFIG_SERIAL_MMIO, serial_base, 8, serial_io_handler);
#endif
  g_serial_p = g_serial_buf;
}
