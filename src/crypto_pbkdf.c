#include "crypto.h"
#include "avr-depo.h"

#include <avr-crypto-lib/hmac-sha1/hmac-sha1.h>

static inline uint16_t byte_progress(int32_t bytes, int32_t cplen, int32_t j, uint32_t iter) {
  float block_progress;
  uint16_t block_bits;

  block_progress = j/((float) iter);
  block_bits = block_progress*(cplen << 3);

  return (bytes << 3) + block_bits;
}

/* this function was ported from openssl to use avr-crypto-lib
 * functions. the license header present in that file can be found
 * at the end of this file */                                    
int crypto_pbkdf2(const char *pass, uint16_t passlen,
                  const uint8_t *salt, uint16_t saltlen,
                  uint32_t iter, uint16_t keylen, uint8_t *out,
                  void (*cb)(uint16_t bits, void *user),
                  uint32_t cb_ms_ivl, void *user) {

  uint8_t digtmp[AVR_DEPO_PBKDF2_DIGEST_BYTES];
  uint8_t *p;
  uint8_t itmp[4];
  uint8_t salt_itmp[AVR_DEPO_PBKDF2_MAX_SALT_BYTES + 4];
  int32_t cplen, j, k, tkeylen, mdlen;
  uint32_t i, t;
  hmac_sha1_ctx_t hctx; //HMAC_CTX hctx;

  i = 1;
  if(saltlen > AVR_DEPO_PBKDF2_MAX_SALT_BYTES)
    return -1;

  t = ADP_ts_millis();
  memcpy(salt_itmp, salt, saltlen);
  mdlen = AVR_DEPO_PBKDF2_DIGEST_BYTES;

  /*dont think i need to do this*/ /* init hmac context */ //HMAC_CTX_init(&hctx);
  p = out;
  tkeylen = keylen;
  
  while(tkeylen) {
    if(tkeylen > mdlen)
      cplen = mdlen;
    else
      cplen = tkeylen;
    /* We are unlikely to ever use more than 256 blocks (5120 bits!)
     * but just in case...
     */
    itmp[0] = (uint8_t)((i >> 24) & 0xff);
    itmp[1] = (uint8_t)((i >> 16) & 0xff);
    itmp[2] = (uint8_t)((i >> 8) & 0xff);
    itmp[3] = (uint8_t)(i & 0xff);
    hmac_sha1_init(&hctx, pass, passlen*8); //HMAC_Init_ex(&hctx, pass, passlen, digest, NULL);
    /*HMAC_Update(&hctx, salt, saltlen);
     *HMAC_Update(&hctx, itmp, 4);
     */
    memcpy(salt_itmp + saltlen, itmp, 4);
    hmac_sha1_lastBlock(&hctx, salt_itmp, (saltlen+4)*8);

    /*HMAC_Final(&hctx, digtmp, NULL);
     */
    hmac_sha1_final(digtmp, &hctx);
    
    memcpy(p, digtmp, cplen);

    for(j = 1; j < iter; j++) {
      if(cb != NULL && (ADP_ts_millis() - t >= cb_ms_ivl)) {
        cb(byte_progress(keylen-tkeylen, cplen, j, iter), user);
        t = ADP_ts_millis();
      }
      //HMAC(digest, pass, passlen, digtmp, mdlen, digtmp, NULL);

      hmac_sha1(digtmp, pass, passlen*8, digtmp, mdlen*8);
 
      for(k = 0; k < cplen; k++)
        p[k] ^= digtmp[k];
    }

    tkeylen -= cplen;
    i++;
    p += cplen;
  }
  /* dont think i need to do this */ //HMAC_CTX_cleanup(&hctx);

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
