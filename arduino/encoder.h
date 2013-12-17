#ifndef AVR_DEPO_ARDUINO_encoder_h
#define AVR_DEPO_ARDUINO_encoder_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* we assume consecutive pins with interrupts, so PIN_A=2 implies PIN_B=3 */
#define ENCODER_PIN_A_OFFSET 2
/* set to the port in which the 2 consecutive pins reside. we assume they are
 * in the same port */
#define ENCODER_PORT PIND
#define ENCODER_PORT_OFFSET 0
#define ENCODER_PIN_A (ENCODER_PORT_OFFSET + ENCODER_PIN_A_OFFSET)

#define ENCODER_PIN_B (ENCODER_PIN_A+1) 

void encoder_update();
uint32_t encoder_position();
void encoder_set(uint32_t position);

#ifdef __cplusplus
}
#endif

#endif /* AVR_DEPO_ARDUINO_encoder_h */
