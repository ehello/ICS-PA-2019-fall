#include "nemu.h"
#include <stdlib.h>
#include <time.h>

void isa_reg_display();
uint32_t isa_reg_str2val(const char *s, bool *success);
