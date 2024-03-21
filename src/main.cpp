#include <Arduino.h>
#include <MIDIUSB.h>

int EXPRESSION_PEDAL_1 = A0;
int EXPRESSION_PEDAL_2 = A2;
int EXP_P1_CHECK = 5;
int EXP_P2_CHECK = 6;
int pedal1enable = 0;
int pedal2enable = 0;
int previousExpressionPedal1Value = -1;
int previousExpressionPedal2Value = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(EXP_P1_CHECK, INPUT_PULLUP);
  pinMode(EXP_P2_CHECK, INPUT_PULLUP);
}

void pedal1()
{
  uint8_t expressionPedal1 = map(analogRead(EXPRESSION_PEDAL_1), 0, 1023, 0, 127);
  if (expressionPedal1 != previousExpressionPedal1Value)
  {
    midiEventPacket_t cc = {0x0B, 0xB0 | 0, 11, expressionPedal1};
    MidiUSB.sendMIDI(cc);
    MidiUSB.flush();
    Serial.print("Pedal_1 send: ");
    Serial.print(expressionPedal1);
    Serial.print("\n");
  }
  previousExpressionPedal1Value = expressionPedal1;
}

void pedal2()
{
  uint8_t expressionPedal2 = map(analogRead(EXPRESSION_PEDAL_2), 0, 1023, 0, 127);
  if (expressionPedal2 != previousExpressionPedal2Value)
  {
    midiEventPacket_t cc = {0x0B, 0xB0 | 0, 16, expressionPedal2};
    MidiUSB.sendMIDI(cc);
    MidiUSB.flush();
    Serial.print("Pedal_2 send: ");
    Serial.print(expressionPedal2);
    Serial.print("\n");
  }
  previousExpressionPedal2Value = expressionPedal2;
}

void loop()
{
  pedal1enable = digitalRead(EXP_P1_CHECK);
  pedal2enable = digitalRead(EXP_P2_CHECK);

  if (pedal1enable == HIGH)
  {
    pedal1();
  }

  if (pedal2enable == HIGH)
  {
    pedal2();
  }
}