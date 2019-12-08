#include "rtl/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:  rtl_get_OF(&s0); *dest=s0?1:0; break;
    case CC_B:  rtl_get_CF(&s0); *dest=s0?1:0; break;
    case CC_E:  rtl_get_ZF(&s0); *dest=s0?1:0; break;
    case CC_BE: rtl_get_ZF(&s0); rtl_get_CF(&s1); *dest=(s0||s1)?1:0; break;
    case CC_S:  rtl_get_SF(&s0); *dest=s0?1:0; break;
    case CC_L:  rtl_get_SF(&s0); rtl_get_OF(&s1); *dest=(s0!=s1)?1:0; break;
    case CC_LE: rtl_get_SF(&s0); rtl_get_OF(&s1); rtl_get_ZF(&t1); *dest=(t1||(s0!=s1))?1:0; break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}
