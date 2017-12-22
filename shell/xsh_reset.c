#include <xinu.h>

#define KEYB_CTLR_IO_ADDR 0X64
#define KEYB_RESET_CMD 0XFE

shellcmd xsh_reset(int nargs, char *args[]){
  printf("\ngoing to reboot...\n");
  outb(KEYB_CTLR_IO_ADDR,KEYB_RESET_CMD);
  return 0;
}
