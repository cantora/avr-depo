#ifndef AVR_DEPO_config_h
#define AVR_DEPO_config_h

#include "crypto.h"

#define AVR_DEPO_config_pbkdf2_rounds 3000
#define AVR_DEPO_config_pbkdf2_salt "12345678"
#define AVR_DEPO_config_pbkdf2_saltlen 8

#define stringify(s) stringify_int(s)
#define stringify_int(s) #s

#if (AVR_DEPO_config_pbkdf2_saltlen > AVR_DEPO_PBKDF2_MAX_SALT_BYTES)
#  error maximum pbkdf2 salt length exceeded
#endif

#endif /* AVR_DEPO_config_h */
