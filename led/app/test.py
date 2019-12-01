import RPi.GPIO as GPIO
import time
   
GPIO.setmode(GPIO.BCM)
   
# Hier werden die Ausgangs-Pin deklariert, an dem die LEDs angeschlossen sind.
LED_ROT = 6
LED_GRUEN = 5
LED_BLAU = 4
 
GPIO.setup(LED_ROT, GPIO.OUT, initial= GPIO.LOW)
GPIO.setup(LED_GRUEN, GPIO.OUT, initial= GPIO.LOW)
GPIO.setup(LED_BLAU, GPIO.OUT, initial= GPIO.LOW)
   
print "LED-Test [druecken Sie STRG+C, um den Test zu beenden]"
  
# Hauptprogrammschleife
try:
        while True:
            print("LED ROT 3 Sekunden an")
            GPIO.output(LED_ROT,GPIO.HIGH) #LED wird eingeschaltet
            GPIO.output(LED_GRUEN,GPIO.LOW) #LED wird eingeschaltet
            GPIO.output(LED_BLAU,GPIO.LOW) #LED wird eingeschaltet
            time.sleep(3) # Wartemodus fuer 4 Sekunden
            print("LED GRUEN 3 Sekunden an") 
            GPIO.output(LED_ROT,GPIO.LOW) #LED wird eingeschaltet
            GPIO.output(LED_GRUEN,GPIO.HIGH) #LED wird eingeschaltet
            GPIO.output(LED_BLAU,GPIO.LOW) #LED wird eingeschaltet
            time.sleep(3) #Wartemodus fuer 3 Sekunden
            print("LED BLAU 3 Sekunden an") 
            GPIO.output(LED_ROT,GPIO.LOW) #LED wird eingeschaltet
            GPIO.output(LED_GRUEN,GPIO.LOW) #LED wird eingeschaltet
            GPIO.output(LED_BLAU,GPIO.HIGH) #LED wird eingeschaltet
            time.sleep(3) #Wartemodus fuer 3 Sekunden
   
# Aufraeumarbeiten nachdem das Programm beendet wurde
except KeyboardInterrupt:
        GPIO.cleanup()