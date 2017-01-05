# Integra display 
DIY unit that will be replacing the broken OEM clock display for my '96 JDM Integra Type-R.<n> 
This unit is located on the center of the dash, below the windshield.<n> 
Display is a quad 7-segment LED display, driven from a Microchip PIC.<n> 
OEM enclosure is going to be reused, along with the three switches which are going to allow some control of the module.<n> 
Unit will have a permanently connected LOCOSYS LS30032 GPS through EUART port.<n> 

## Functionalities ##

<b>v1</b><n> 
Selectable modes (toggle by pushing M):<n> 
- 24h Clock (GPS-synced every 10 mins)<n> 
- Current speed in km/h (from GPS)<n> 
Settings:<n> 
- Adj. Brightness (default 25%, 50% and 100%)<n> 

<b>v2</b><n> 
I2C connection with intelogger-controller project (ECU reader).<n> 
- RPM (ECU)<n> 
- ...<n> 

## Under the hood ##
PIC16F1938 @ INTOSC 4MHz --- deprecated<n> 
PIC16F1938 @ HC49 XTAL @ 4MHz<n> 
I2C interface to other MCU<n>
LOCOSYS LS20032 GPS @ 19200 bps serial port EUSART<n>
