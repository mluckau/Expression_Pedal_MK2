#include <Arduino.h>
#include <MIDIUSB.h>

/**
 * ExpressionPedal Klasse
 * Kapselt Glättung, Kalibrierung und MIDI-Logik für ein einzelnes Pedal.
 */
class ExpressionPedal {
  private:
    uint8_t _analogPin;
    uint8_t _checkPin;
    uint8_t _ccNumber;
    uint8_t _midiChannel;
    
    float _smoothedValue;
    int _minVal;
    int _maxVal;
    int _lastSentMidiValue;
    
    const float _alpha;
    const int _deadZone;

  public:
    ExpressionPedal(uint8_t analogPin, uint8_t checkPin, uint8_t cc, uint8_t channel = 1)
      : _analogPin(analogPin), 
        _checkPin(checkPin), 
        _ccNumber(cc), 
        _midiChannel(channel),
        _smoothedValue(0),
        _minVal(1023),
        _maxVal(0),
        _lastSentMidiValue(-1),
        _alpha(0.15),
        _deadZone(15) {}

    void begin() {
      pinMode(_checkPin, INPUT_PULLUP);
      // Initial-Messung für den Filter
      _smoothedValue = analogRead(_analogPin);
    }

    void update() {
      // Prüfen, ob Pedal aktiviert ist (HIGH = Aktiv durch Pullup/Switch)
      if (digitalRead(_checkPin) == LOW) return;

      // "Double-Reading" Fix gegen ADC-Crosstalk: 
      // Erster Wert wird verworfen, damit der ADC-Kondensator sich stabilisieren kann.
      analogRead(_analogPin); 
      int raw = analogRead(_analogPin);

      // 1. Auto-Kalibrierung (Lernen der mechanischen Grenzen)
      if (raw < _minVal) _minVal = raw;
      if (raw > _maxVal) _maxVal = raw;

      // 2. Glättung (EMA Filter)
      _smoothedValue = (_alpha * raw) + ((1.0 - _alpha) * _smoothedValue);

      // 3. Mapping & Senden (nur wenn Bereich kalibriert wurde)
      if (_maxVal > _minVal + (_deadZone * 2)) {
        int mapped = map((int)_smoothedValue, _minVal + _deadZone, _maxVal - _deadZone, 0, 127);
        int currentMidiValue = constrain(mapped, 0, 127);

        // Nur senden, wenn sich der MIDI-Wert tatsächlich geändert hat
        if (currentMidiValue != _lastSentMidiValue) {
          sendMidi(currentMidiValue);
          _lastSentMidiValue = currentMidiValue;
        }
      }
    }

  private:
    void sendMidi(uint8_t value) {
      TXLED1; // LED an (Pro Micro spezifisch)
      
      // MIDI Packet: {Header, Message Type | Channel, Data 1 (CC), Data 2 (Value)}
      // Kanal ist 0-basiert in der Library (0 = Channel 1)
      midiEventPacket_t event = {0x0B, (uint8_t)(0xB0 | (_midiChannel - 1)), _ccNumber, value};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
      
      TXLED0; // LED aus

      // Debugging
      Serial.print("Pedal (CC "); Serial.print(_ccNumber);
      Serial.print("): "); Serial.print(value);
      Serial.print(" [Range: "); Serial.print(_minVal);
      Serial.print("-"); Serial.print(_maxVal);
      Serial.println("]");
    }
};

// --- Instanzen & Globale Variablen ---

// Pedal 1: Pin A0, Check-Pin 5, CC 11 (Expression), Kanal 1
ExpressionPedal pedal1(A0, 5, 11, 1);
// Pedal 2: Pin A2, Check-Pin 6, CC 16 (GenPurp 1), Kanal 1
ExpressionPedal pedal2(A2, 6, 16, 1);

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL_MS = 5; // 200Hz Abtastrate

void setup() {
  Serial.begin(115200); // Höhere Baudrate für weniger Latenz im Debugging
  
  pedal1.begin();
  pedal2.begin();

  // Sicherstellen, dass LEDs aus sind
  TXLED0;
  RXLED0;
}

void loop() {
  unsigned long now = millis();

  // Zeitgesteuerte Abfrage (Non-blocking)
  if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = now;
    
    pedal1.update();
    pedal2.update();
  }
}
