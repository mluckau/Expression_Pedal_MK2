# Expression-Pedal-Adapter MK2: DIY USB-MIDI Interface (Dual-Channel)

Moin Leute! 👋

Ich wollte euch mal mein neuestes DIY-Projekt vorstellen: Ein **Expression-Pedal-Adapter**, der herkömmliche analoge Pedale in hochpräzise USB-MIDI Controller verwandelt.

### Warum das Ganze?
Die meisten kommerziellen Adapter sind entweder zu teuer, nur einkanalig oder haben Probleme mit "zappelnden" MIDI-Werten (Jitter). Da ich zwei Pedale nutze, musste eine eigene Lösung her!

### Die Highlights (Features):
* **Dual-Channel:** Ich habe zwei Stereo-Klinkenbuchsen verbaut, man kann also zwei Pedale gleichzeitig nutzen (z.B. Wah-Wah und Volume).
* **Auto-Kalibrierung:** Kein mühsames Eintippen von Min/Max-Werten im Code. Einmal das Pedal komplett bewegen, fertig!
* **EEPROM-Gedächtnis:** Die Kalibrierung wird gespeichert. Einmal eingestellt, bleibt sie auch nach dem Ausstecken erhalten.
* **EMA-Filter & Hysterese:** Ich habe einen exponentiellen Filter eingebaut, der das analoge Rauschen der Potis komplett eliminiert. Die MIDI-Werte sind absolut stabil.
* **Hardware-Erkennung:** Das Gerät erkennt automatisch, ob ein Pedal eingesteckt ist (via Schaltkontakt in der Buchse).

### Die Technik:
* **Controller:** Arduino Pro Micro (ATmega32U4) für native USB-MIDI Unterstützung.
* **Gehäuse:** 3D-gedruckt (FreeCAD Design), kompakt und stabil.
* **Code:** Objektorientiertes C++ mit PlatformIO.

### Was ich gelernt habe:
Besonders knifflig war das "Übersprechen" (Crosstalk) zwischen den analogen Eingängen des Arduinos. Ich habe das im Code durch einen "Double-Reading-Fix" gelöst, jetzt sind die Kanäle komplett isoliert.

Ich bin mit dem Ergebnis super zufrieden, die Latenz ist minimal und es fühlt sich in der DAW (Bitwig Studio / Manjaro Linux) richtig "snappy" an.

Falls jemand Interesse am Nachbau hat oder Fragen zum Code/Schaltplan hat, haut einfach in die Tasten! 🎸🎹

[Link zum GitHub Repository / Bilder / Video - Platzhalter]

Beste Grüße,
Michael
