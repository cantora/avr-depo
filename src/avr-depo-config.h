#ifndef AVR_DEPO_config_h
#define AVR_DEPO_config_h

#include "crypto.h"

#define AVR_DEPO_config_pbkdf2_rounds 1000
#define AVR_DEPO_config_gen_pbkdf2_rounds 1000
#define AVR_DEPO_config_pbkdf2_salt "12345678"
#define AVR_DEPO_config_pbkdf2_saltlen 8

#if (AVR_DEPO_config_pbkdf2_saltlen > AVR_DEPO_PBKDF2_MAX_SALT_BYTES)
#  error maximum pbkdf2 salt length exceeded
#endif

#endif /* AVR_DEPO_config_h */
