#include <Arduino.h>
#include <LiquidCrystal.h>

#include "encoder.h"
#include "button.h"

#include "depo_main.h"

/******************
 * globals
 */
LiquidCrystal g_lcd(9, 8, 7, 6, 5, 4);

/******************
 * avr-depo platform implementations 
 */

void serial_print(const char *s) {
  Serial.print(s);
}

void lcd_set_cursor(uint16_t col, uint16_t row) {
  g_lcd.setCursor(col, row);
}

void lcd_cursor(uint8_t on) {
  if(on)
    g_lcd.cursor();
  else
    g_lcd.noCursor();
}

void lcd_write(char c) {
  g_lcd.write(c);
}

void lcd_clear() {
  g_lcd.clear();
}

/******************
 * main stuff 
 */
void setup()
{
  Serial.begin (115200);
  Serial.println("Start");
  g_lcd.begin(16, 2);

  pinMode(ENCODER_PIN_A, INPUT);
  /* use pullup resistor */
  digitalWrite(ENCODER_PIN_A, HIGH);
  pinMode(ENCODER_PIN_B, INPUT);
  /* use pullup resistor */
  digitalWrite(ENCODER_PIN_B, HIGH);
  attachInterrupt(0, encoder_update, CHANGE);
  attachInterrupt(1, encoder_update, CHANGE);

  pinMode(BUTTON_PIN, INPUT);
  /* use pullup resistor for switch */
  digitalWrite(BUTTON_PIN, HIGH);
}

void loop() {
  /*
  static uint32_t prev_pos = 0;
  uint32_t pos;

  pos = encoder_position();
  if(prev_pos != pos) {
    prev_pos = pos;
    Serial.println(prev_pos);
  }
  */

  depo_main();
  delay(1000);
}
