#include <Arduino.h>
#include <MIDIUSB.h>
#include <EEPROM.h>

// --- KONFIGURATION ---
#define DEBUG               // Auskommentieren für maximale Performance im Betrieb
#define EEPROM_VERSION 1    // Erhöhen, um EEPROM-Daten zurückzusetzen
#define SAVE_INTERVAL 30000 // Alle 30 Sek. Kalibrierung speichern (schont EEPROM)

/**
 * ExpressionPedal Klasse (Final Polish Version)
 */
class ExpressionPedal {
  private:
    uint8_t _analogPin, _checkPin, _ccNumber, _midiChannel;
    float _smoothedValue;
    int _minVal, _maxVal;
    int _lastSentMidiValue;
    int _eepromAddr;
    unsigned long _lastSaveTime;
    bool _needsSave;

    const float _alpha = 0.15;
    const int _deadZone = 15;
    const float _hysteresis = 0.6; // Schwellenwert für Wertänderung

  public:
    ExpressionPedal(uint8_t analogPin, uint8_t checkPin, uint8_t cc, int eepromAddr, uint8_t channel = 1)
      : _analogPin(analogPin), _checkPin(checkPin), _ccNumber(cc), _midiChannel(channel),
        _smoothedValue(0), _minVal(1023), _maxVal(0), _lastSentMidiValue(-1),
        _eepromAddr(eepromAddr), _lastSaveTime(0), _needsSave(false) {}

    void begin() {
      pinMode(_checkPin, INPUT_PULLUP);
      loadCalibration();
      _smoothedValue = analogRead(_analogPin);
    }

    void update() {
      if (digitalRead(_checkPin) == LOW) return;

      analogRead(_analogPin); // Double-Reading Fix
      int raw = analogRead(_analogPin);

      // 1. Dynamische Auto-Kalibrierung
      bool rangeChanged = false;
      if (raw < _minVal) { _minVal = raw; rangeChanged = true; }
      if (raw > _maxVal) { _maxVal = raw; rangeChanged = true; }
      
      if (rangeChanged) {
        _needsSave = true;
        _lastSaveTime = millis();
      }

      // 2. Glättung
      _smoothedValue = (_alpha * raw) + ((1.0 - _alpha) * _smoothedValue);

      // 3. Mapping mit Hysterese & Totzonen
      if (_maxVal > _minVal + (_deadZone * 2)) {
        float mapped = mapFloat(_smoothedValue, _minVal + _deadZone, _maxVal - _deadZone, 0.0, 127.0);
        int currentMidiValue = (int)constrain(mapped, 0, 127);

        // Hysterese: Nur senden, wenn die Änderung signifikant ist oder wir an den Enden sind
        if (currentMidiValue != _lastSentMidiValue) {
          float diff = abs(mapped - (float)_lastSentMidiValue);
          if (diff >= _hysteresis || currentMidiValue == 0 || currentMidiValue == 127) {
            sendMidi(currentMidiValue);
            _lastSentMidiValue = currentMidiValue;
          }
        }
      }

      // 4. Periodisches Speichern im EEPROM (schont Schreibzyklen)
      if (_needsSave && (millis() - _lastSaveTime > SAVE_INTERVAL)) {
        saveCalibration();
      }
    }

  private:
    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void sendMidi(uint8_t value) {
      TXLED1;
      midiEventPacket_t event = {0x0B, (uint8_t)(0xB0 | (_midiChannel - 1)), _ccNumber, value};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
      TXLED0;

#ifdef DEBUG
      Serial.print("Pedal (CC "); Serial.print(_ccNumber);
      Serial.print("): "); Serial.println(value);
#endif
    }

    void saveCalibration() {
      EEPROM.put(_eepromAddr, _minVal);
      EEPROM.put(_eepromAddr + sizeof(int), _maxVal);
      _needsSave = false;
#ifdef DEBUG
      Serial.println("-> Kalibrierung im EEPROM gespeichert.");
#endif
    }

    void loadCalibration() {
      int m, x;
      EEPROM.get(_eepromAddr, m);
      EEPROM.get(_eepromAddr + sizeof(int), x);
      
      // Validierung der Daten (wenn EEPROM leer ist, stehen dort oft 0 oder -1)
      if (m >= 0 && m < 1023 && x > 0 && x <= 1023 && x > m) {
        _minVal = m;
        _maxVal = x;
#ifdef DEBUG
        Serial.print("Kalibrierung geladen für CC "); Serial.print(_ccNumber);
        Serial.print(": "); Serial.print(_minVal); Serial.print("-"); Serial.println(_maxVal);
#endif
      }
    }
};

// --- INSTANZEN ---
// Adressen: 0-3 (Version), 10-13 (Pedal1 Min/Max), 20-23 (Pedal2 Min/Max)
ExpressionPedal pedal1(A0, 5, 11, 10, 1);
ExpressionPedal pedal2(A2, 6, 16, 20, 1);

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL_MS = 5;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  
  // EEPROM Version Check (Reset wenn Version sich ändert)
  if (EEPROM.read(0) != EEPROM_VERSION) {
    for (int i = 0 ; i < EEPROM.length() ; i++) EEPROM.write(i, 0xFF);
    EEPROM.write(0, EEPROM_VERSION);
  }

  pedal1.begin();
  pedal2.begin();
  TXLED0;
  RXLED0;
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = now;
    pedal1.update();
    pedal2.update();
  }
}
