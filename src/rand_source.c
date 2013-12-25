#include "rand_source.h"

void rand_source_init(struct rand_source *src,
                      const uint8_t *bytes, uint16_t len) {
  src->idx = 0;
  src->len = len;
  src->bytes = bytes;
}

int rand_source_empty(const struct rand_source *src) {
  return ((src->idx+sizeof(uint32_t)) >= src->len);
}

uint32_t rand_source_uint32(struct rand_source *src) {
  uint32_t v;

  v = *((uint32_t *) (src->bytes+src->idx));
  src->idx += 4;
  return v;
}

/* let 0 < k < max and max >= 2. then this function returns
 * k distinct integers in the inclusive range of 0..(max-1)
 * each with probability k/max.
 *
 * proof by induction on n:
 *   base case:
 *     let max = 2. k must be 1. 
 *     when i = 0, max-i = 2.
 *     if x = 0 < k-j=1, we put 0 in result and are done (1 >= k).
 *     x was 0 with 1/2 = k/max probability so we took result = [0]
 *     with k/max probability.
 *     else we continue with i = 1 => max-i = 1, thus x = 0.
 *     since x = 0 < k-j=1, we put 1 in result and are done (1 >= k).
 *     x was 0 with 1 probability, but the probability of getting
 *     to i = 1 was 1/2, so we took result = [1] with probability
 *     1*(1/2) = 1/2 = k/max probability.
 *
 *     in both cases result was of length k.
 *
 *  induction:
 *    suppose the function satisfies for all max in {2..n}. now
 *    let max=n+1. k must be < n+1.
 *
 *    suppose at i=0, x < k. then we add 0 to the result. if
 *    k=1, then we are done; since x < k with k/n+1 probability,
 *    the function satisfies in this case. if k > 1 then we
 *    move on to the i=1 iteration. at this point (and for
 *    each iteration after this) the term (max-i) on line A
 *    satisfies max-i = (n+1)-i <= n. since j = 1, we are 
 *    comparing x to k-1. thus at this point we are proceeding
 *    in an equivalent manner as if we had called
 *    rand_source_choose_k(src, max-1=n+1-1=n, k-1, result).
 *    note we can take k-1 above because we already showed k>1.
 *    since the function satisfies by assumption for n, k'=(k-1)<n,
 *    the function satisfies in this case (n'=n+1). 
 *
 *    or suppose at i=0, x >= k. then we move on to the next 
 *    iteration with i=1. similarly, at this point and each
 *    iteration afterward, max-i=n+1-i < n. now suppose that
 *    k < n. in this case, the rest of the execution of the
 *    function is equivalent to:
 *    rand_source_choose_k(src, max-1=n+1-1=n, k, result)
 *    this function satisfies for n, k<n by assumption, so
 *    in this case the function satisfies for n+1.
 *    now suppose k >= n. then k=n because k<max=n+1. at
 *    this point i=1, so x is one of
 *    {0,..,(n+1-1)-1} = {0,..,(n-1)}.
 *    but n-1 < n=k, so we will add 1 to the result and
 *    move on to the next iteration where i=2. now x can
 *    only be one of {0,..(n-2=n-i)}, but n-i=<n-1=k-1=k-j.
 *    since n-i<k-j implies we will add i to the result and
 *    increment j, it will be true for the rest of the 
 *    iterations and the result will be [1,..,n].
 *
 *    thus the function satisfies for all cases.
 *
 * algorithm taken from:
 *   http://propersubset.com/2010/04/choosing-random-elements.html
 */
int rand_source_choose_k(struct rand_source *src, uint16_t max,
                         uint16_t k, uint16_t *result) {
  uint16_t i, j, x;

  j = 0;
  for(i = 0; i < max; i++) {
    if(rand_source_empty(src))
      return -1;

    x = rand_source_uint32(src) % (max-i); /* line A */
    if(x < (k-j)) {
      result[j++] = i;
      if(j >= k)
        goto success;
    }
  }

  return -2; /* shouldnt get here */
success:
  return 0;
}

/* fisher-yates shuffle
 * len - 1 iterations implies we will use 4*(len-1) bytes
 * from rand_source.
 */
int rand_source_shuffle(struct rand_source *src, uint8_t *bytes, uint16_t len) {
  uint16_t i, j;
  uint8_t tmp;

  if(len < 1)
    return 0;

  for(i = len - 1; i > 0; i--) {
    if(rand_source_empty(src))
      return -1;

    j = rand_source_uint32(src) % (i + 1);
    tmp = bytes[j];
    bytes[j] = bytes[i];
    bytes[i] = tmp;
  }

  return 0;
}

int rand_source_char_range(struct rand_source *src, uint8_t *out,
                           uint8_t offset, uint8_t range) {
  if(rand_source_empty(src))
    return -1;

  *upper = offset + (rand_source_uint32(src) % range);
  return 0;
}

inline int rand_source_char_upper(struct rand_source *src, uint8_t *upper) {
  return rand_source_char_range(src, upper, 0x41, 25);
}

inline int rand_source_char_lower(struct rand_source *src, uint8_t *lower) {
  return rand_source_char_range(src, upper, 0x61, 25);
}

inline int rand_source_char_numeric(struct rand_source *src, uint8_t *numeric) {
  return rand_source_char_range(src, upper, 0x30, 10);
}

inline int rand_source_char_symbolic(struct rand_source *src, uint8_t *symbolic) {
  uint8_t n;

  if(rand_source_empty(src))
    return -1;

  n = rand_source_uint32(src) % (14 + 6 + 4 + 4);
  if(n < 14) {
    *symbolic = 0x21 + n;
  }
  else if(n < 20) {
    *symbolic = 0x3a + (n-14);
  }
  else if(n < 24) {
    *symbolic = 0x5d + (n-20);
  }
  else {
    *symbolic = 0x7b + (n-24);
  }

  return 0;
}