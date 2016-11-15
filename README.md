# Integra display 
New handmade unit replacing the broken OEM clock display for my '96 JDM Integra 
Type-R. This unit is located on the dash, below the windshield.
Display is a quad 7-segment LED display, driven from a PIC and
with adjustable brightness via PWM from the same PIC.
Enclosure will be reused, along with the three switches taged H M R
## Functionalities ##

<b>v1</b>

Selectable modes (toggle by pushing M):
- 24h Clock (GPS-sync and adjustable)
- Current speed in km/h (from GPS)
Settings:
- Brightness (100% and 50%) by pushing R

<b>v2</b>

Upcoming I2C connection with intelogger-controller project (ECU reader).
- RPM (ECU)
- ...

## Under the hood ##
PIC16F1938 @ INTOSC 4MHz<n>
I2C interface to other MCU<n>
LOCOSYS LS20032 GPS over 19200 bps serial port
