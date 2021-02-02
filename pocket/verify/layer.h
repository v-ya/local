#ifndef _pocket_verify_layer_h_
#define _pocket_verify_layer_h_

#include "../pocket-verify.h"

// xor("xor.1", "xor.2", "xor.4", "xor.8")
const pocket_verify_entry_s* pocket_verify$xor$1(void);
const pocket_verify_entry_s* pocket_verify$xor$2(void);
const pocket_verify_entry_s* pocket_verify$xor$4(void);
const pocket_verify_entry_s* pocket_verify$xor$8(void);

// rhash32("*>^~32.4", "*>^~32.8", "*>^~32.16", "*>^~32.32")
const pocket_verify_entry_s* pocket_verify$rhash32$4(void);
const pocket_verify_entry_s* pocket_verify$rhash32$8(void);
const pocket_verify_entry_s* pocket_verify$rhash32$16(void);
const pocket_verify_entry_s* pocket_verify$rhash32$32(void);
// rhash64("*>^~64.4", "*>^~64.8", "*>^~64.16", "*>^~64.32")
const pocket_verify_entry_s* pocket_verify$rhash64$4(void);
const pocket_verify_entry_s* pocket_verify$rhash64$8(void);
const pocket_verify_entry_s* pocket_verify$rhash64$16(void);
const pocket_verify_entry_s* pocket_verify$rhash64$32(void);

#endif
