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

void lcd_blink(uint8_t on) {
  if(on)
    g_lcd.blink();
  else
    g_lcd.noBlink();
}

void lcd_write(char c) {
  g_lcd.write(c);
}

void lcd_clear() {
  g_lcd.clear();
}

void arduino_delay(uint32_t m) {
  delay(m);
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
  static int once = 0;
/*
  static uint32_t prev_pos = 0;
  uint32_t pos;
  char buf[16];
  static int btn0 = 99;
  int btn;


  pos = ADP_selector_position();
  if(prev_pos != pos) {
    prev_pos = pos;
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(prev_pos);
  }

  btn = button_check();
  if(btn != btn0) {
    btn0 = btn;
    Serial.print("btn: ");
    Serial.println(btn0);
  }
*/


  if(once == 0) {
    once = 1;
    depo_main();
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print("fail");
  }
}
