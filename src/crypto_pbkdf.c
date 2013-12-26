#include "crypto.h"
#include "avr-depo.h"

#include <avr-crypto-lib/hmac-sha1/hmac-sha1.h>

int crypto_pbkdf2_init(struct crypto_pbkdf2 *kdf,
                       uint32_t iters_per_block,
                       const uint8_t *pass, uint16_t passlen,
                       const uint8_t *salt, uint16_t saltlen,
                       void (*cb)(uint32_t bits, void *user),
                       uint32_t cb_ms_ivl, void *user) {
  if(saltlen < 1 || passlen < 1 || iters_per_block < 1)
    return -1;

  kdf->salt_itmp = malloc((saltlen+4)*sizeof(uint8_t));
  if(kdf->salt_itmp == NULL)
    return -1;

  kdf->i = 1;
  kdf->iter = iters_per_block;
  kdf->pass = pass;
  kdf->salt = salt;
  kdf->passlen = passlen;
  kdf->saltlen = saltlen;
  kdf->cb = cb;
  kdf->cb_ms_ivl = cb_ms_ivl;
  kdf->t = ADP_ts_millis();
  kdf->user = user;

  memcpy(kdf->salt_itmp, kdf->salt, kdf->saltlen);

  return 0;
}

void crypto_pbkdf2_free(struct crypto_pbkdf2 *kdf) {
  free(kdf->salt_itmp);
}

inline uint16_t crypto_pbkdf2_blocklen(struct crypto_pbkdf2 *kdf) {
  return AVR_DEPO_PBKDF2_DIGEST_BYTES;
}

static inline uint32_t bit_progress(const struct crypto_pbkdf2 *kdf, uint32_t iter) {
  float block_progress;
  uint32_t block_bits;

  block_progress = iter/((float) kdf->iter);
  block_bits = block_progress*(AVR_DEPO_PBKDF2_DIGEST_BYTES << 3);

  return ((kdf->i-1)*AVR_DEPO_PBKDF2_DIGEST_BYTES << 3) + block_bits;
}

void crypto_pbkdf2_block(struct crypto_pbkdf2 *kdf, uint8_t *out) {
  uint8_t digtmp[AVR_DEPO_PBKDF2_DIGEST_BYTES];
  uint8_t itmp[4];
  int32_t j, k;
  hmac_sha1_ctx_t hctx;

  itmp[0] = (uint8_t)((kdf->i >> 24) & 0xff);
  itmp[1] = (uint8_t)((kdf->i >> 16) & 0xff);
  itmp[2] = (uint8_t)((kdf->i >> 8) & 0xff);
  itmp[3] = (uint8_t)(kdf->i & 0xff);

  hmac_sha1_init(&hctx, kdf->pass, kdf->passlen*8);
  memcpy(kdf->salt_itmp + kdf->saltlen, itmp, 4);
  hmac_sha1_lastBlock(&hctx, kdf->salt_itmp, (kdf->saltlen+4)*8);
  hmac_sha1_final(digtmp, &hctx);
  
  memcpy(out, digtmp, AVR_DEPO_PBKDF2_DIGEST_BYTES);

  for(j = 1; j < kdf->iter; j++) {
    if(kdf->cb != NULL
          && kdf->cb_ms_ivl > 0
          && (ADP_ts_millis() - kdf->t >= kdf->cb_ms_ivl)) {
      kdf->cb(bit_progress(kdf, j), kdf->user);
      kdf->t = ADP_ts_millis();
    }

    hmac_sha1(digtmp, kdf->pass, kdf->passlen*8,
              digtmp, AVR_DEPO_PBKDF2_DIGEST_BYTES*8);
 
    for(k = 0; k < AVR_DEPO_PBKDF2_DIGEST_BYTES; k++)
      out[k] ^= digtmp[k];
  }

  kdf->i++;
}

/* this function was ported from openssl to use avr-crypto-lib
 * functions. the license header present in that file can be found
 * at the end of this file */                                    
int crypto_pbkdf2(const uint8_t *pass, uint16_t passlen,
                  const uint8_t *salt, uint16_t saltlen,
                  uint32_t iter, uint16_t keylen, uint8_t *out,
                  void (*cb)(uint32_t bits, void *user),
                  uint32_t cb_ms_ivl, void *user) {
  struct crypto_pbkdf2 kdf;
  uint8_t buf[AVR_DEPO_PBKDF2_DIGEST_BYTES];
  uint8_t *p;
  int32_t cplen, tkeylen;

  if(crypto_pbkdf2_init(&kdf, iter, pass, passlen, salt, 
                        saltlen, cb, cb_ms_ivl, user) != 0)
    return -1;

  p = out;
  tkeylen = keylen;

  while(tkeylen) {
    if(tkeylen > AVR_DEPO_PBKDF2_DIGEST_BYTES) {
      cplen = AVR_DEPO_PBKDF2_DIGEST_BYTES;
      crypto_pbkdf2_block(&kdf, p);
    }
    else {
      cplen = tkeylen;
      crypto_pbkdf2_block(&kdf, buf);
      memcpy(p, buf, cplen);
    }
    
    tkeylen -= cplen;
    p += cplen;
  }

  return 0;
}

/* original header/license from openssl crypto/evp/p5_crpt2.c */
/**************************************************************/
/* p5_crpt2.c */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999-2006 The OpenSSL Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 * software must display the following acknowledgment:
 * "This product includes software developed by the OpenSSL Project
 * for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 * endorse or promote products derived from this software without
 * prior written permission. For written permission, please contact
 * licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 * nor may "OpenSSL" appear in their names without prior written
 * permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 * "This product includes software developed by the OpenSSL Project
 * for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com). This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
