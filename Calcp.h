#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#define bool int
#define true 1
#define false 0
#define TOKEN_BAD 'b'
#define TOKEN_END 'e'
#define TOKEN_NUMBER 'n'

#include <stdlib.h>
#include <stdint.h>
#include "vmstdlib.h"

typedef uint8_t u8;
typedef double f64;

void f(VMINT state, VMWSTR text);
void f1(VMINT state, VMWSTR text);
bool next_token();
bool token_is(int kind);
bool match_token(int kind);
f64 parse_level_three();
f64 parse_level_two();
f64 parse_level_one();
f64 parse_level_zero();
f64 parse_string(char * string);

#endif

