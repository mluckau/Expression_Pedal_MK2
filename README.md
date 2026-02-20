# Expression Pedal MK2 – USB-MIDI Adapter

Ein hochpräziser, zweikanaliger USB-MIDI-Adapter für Expression-Pedale, basierend auf dem Arduino Pro Micro (ATmega32U4). Er wandelt analoge Widerstandswerte herkömmlicher Expression-Pedale in MIDI-Control-Change (CC) Befehle um.

## Features
* **Dual Channel:** Unterstützt zwei unabhängige Expression-Pedale.
* **Plug & Play:** Wird als Standard MIDI-Gerät ("InkoTech Exp MK1") erkannt.
* **Auto-Kalibrierung:** Lernt automatisch den Bewegungsbereich des Pedals.
* **EEPROM-Speicher:** Behält die Kalibrierung auch nach dem Ausstecken bei.
* **Präzisions-Filter:** EMA-Filterung und Hysterese eliminieren Jitter und "Zappeln" der MIDI-Werte komplett.
* **Hardware-Erkennung:** Automatische Erkennung, ob ein Pedal eingesteckt ist (via Schaltbuchsen).
* **Status-LED:** Visuelles Feedback über die Onboard-TX-LED bei MIDI-Aktivität.

## Hardware & Stückliste (BOM)
| Komponente | Anzahl | Beschreibung |
| :--- | :--- | :--- |
| **Arduino Pro Micro** | 1 | ATmega32U4 (5V/16MHz) mit USB-C |
| **Klinkenbuchse 6,35mm** | 2 | TRS (Stereo) mit Schaltkontakt (z.B. REAN NYS216G) |
| **Gehäuse** | 1 | 3D-gedruckt (siehe `Gehäuse/` Ordner) |
| **USB-C Kabel** | 1 | Zur Verbindung mit dem PC |

## Schaltplan (Pinbelegung)
Die Verdrahtung erfolgt direkt am Arduino Pro Micro:

### Expression Pedal 1 (TRS Buchse)
* **Sleeve (S):** GND
* **Ring (R):** 5V (VCC)
* **Tip (T):** Pin **A0** (Wiper des Potis)
* **Schaltkontakt:** Pin **5** (Zieht Pin auf HIGH/LOW zur Erkennung)

### Expression Pedal 2 (TRS Buchse)
* **Sleeve (S):** GND
* **Ring (R):** 5V (VCC)
* **Tip (T):** Pin **A2** (Wiper des Potis)
* **Schaltkontakt:** Pin **6** (Zieht Pin auf HIGH/LOW zur Erkennung)

## Software-Konfiguration
Das Projekt wird mit **PlatformIO** verwaltet.

### Wichtige Parameter in `src/main.cpp`:
* `ALPHA`: Glättungsfaktor (Standard: 0.15). Niedriger = glatter, Höher = schneller.
* `DEADZONE`: Puffer an den Endpunkten (Standard: 15), um sicher 0 und 127 zu erreichen.
* `HYSTERESIS`: Verhindert das Springen zwischen zwei MIDI-Werten (Standard: 0.6).
* `#define DEBUG`: Einkommentieren für Testausgaben im Seriellen Monitor (115200 Baud).

## Bedienung & Kalibrierung
1. Schließe den Adapter an den PC an.
2. Stecke dein Expression-Pedal ein.
3. **Erste Benutzung:** Bewege das Pedal einmal komplett von Anschlag zu Anschlag (Heel to Toe).
4. Der Adapter speichert diese Werte nach 30 Sekunden Stabilität im EEPROM.
5. In deiner DAW (Bitwig, Reaper, etc.) den "InkoTech Exp MK1" auswählen und per "MIDI Learn" zuweisen.

## Lizenz
Dieses Projekt ist Open Source. Viel Spaß beim Nachbauen!
