#include <stdio.h>
#include <wiringPi.h>
#include <time.h>

int main(void){
    float dist,s,e;
    
    wiringPiSetup();
    
    pinMode(0,OUTPUT);
    pinMode(1,INPUT);
    
    while(1)
    {
        digitalWrite(0,0);
        digitalWrite(0,1);
        delayMicroseconds(10);
        digitalWrite(0,0);
        while(digitalRead(1)==0)
            s = micros();
        while(digitalRead(1)==1)
            e = micros();
        dist = (e-s) / 58;delayMicroseconds(10);
        printf("distance(cm) :%f\n",dist);
        delay(100);
    }
    return 0;
}
