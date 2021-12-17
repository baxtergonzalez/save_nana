using namespace std;

const int echoPins[3] = {A2, A4, A6};//first pin tested with board, the rest are not
const int trigPins[3] = {A1, A3, A5};//first pin tested with board, the rest are not
const int motorPins[2] = {5,6};//motor pins need pwm
const int ledPins[2] = {7,8};

class Motor{
    public:
        int pin;
        int default_pulse_time;
        //constructor
        Motor(int pinP, int default_pulse_timeP=10){
            pin = pinP;
            default_pulse_time=default_pulse_timeP;
        }
        //pulse_motor at various speeds
        //the higher the frequency, faster the pulses
        void pulse_motor(int frequency){
            float pause = 1.0/frequency;
            
            analogWrite(pin, 255);
            delay(default_pulse_time);
            analogWrite(pin, 0);
            delay(pause);
        }     
};

class Sensor{
    public:
        int echopin;
        int trigpin;
        float distance;
        float minRange = 100;
        int range;
        int intensity = 25;
        Sensor(int ecPin, int trPin){
            echopin = ecPin;
            trigpin = trPin;
        }
       
        float checkDist(){
            digitalWrite(trigpin, LOW);
            delayMicroseconds(2);
            digitalWrite(trigpin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigpin, LOW);

            float duration = pulseIn(echopin, HIGH);
            distance = duration*0.034/2;
            return distance;
        }
        int findRange(){
            if (this->checkDist() < minRange && this->checkDist() > 3*minRange/4){
                range = 1;
            } else if (this->checkDist() < 3*minRange/4 && this->checkDist() > 2*minRange/4){
                range = 2;
            } else if (this->checkDist() < 1*minRange/4){
                range = 3;
            } 
            else{
                range = 0;
            }
            int outputFreq = range*intensity;
            return outputFreq;
        }
};

class LEDS{
  public:
    int pin;
    int brightness;
    int max_light = 255;
    LEDS(int pinP, int brightnessP=255){
      pin = pinP;
      brightness = brightnessP;
      analogWrite(pin, brightness);
    }
    void setLight(int light_mode){
      if (light_mode == 4){
        analogWrite(pin, max_light);
      } else if(light_mode ==3){
        analogWrite(pin, max_light/light_mode);
      } else if(light_mode ==2){
        analogWrite(pin, max_light/3);
      } else if(light_mode ==1){
        analogWrite(pin, max_light/3);
      }else{
        analogWrite(pin, 0);
      }
    }  
};
    //instanciate the sensor and motor objects
    //(apparently this needs to be done outside of the setup function based on the fact that setup returns void)
    Sensor sensL(echoPins[0], trigPins[0]);
    Sensor sensM(echoPins[1], trigPins[1]);
    Sensor sensR(echoPins[2], trigPins[2]);

    Motor motL(motorPins[0]);
    Motor motR(motorPins[1]);

    LEDS leds(ledPins[0]);

void setup(){
    for(int i=0; i<sizeof(trigPins); i++){
        pinMode(trigPins[i], OUTPUT);
    }
    for(int i=0; i<sizeof(echoPins); i++){
        pinMode(echoPins[i], INPUT);
    }
    for(int i=0; i<sizeof(motorPins); i++){
        pinMode(echoPins[i], OUTPUT);
    }
    for(int i=0; i<sizeof(ledPins); i++){
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], HIGH);
    }
}

void loop(){
    sensM.findRange();
    sensR.findRange();
    //pulses the right and left motor with an intensity and pulse rate determined by the
    //range found from their respective side's sensor
    motL.pulse_motor(sensL.findRange());
    motR.pulse_motor(sensR.findRange());
    //because both sensors should activate when something is directly in front of the 
    //sensor array (IE activating the middle sensor)
    motL.pulse_motor(sensM.findRange());
    motR.pulse_motor(sensM.findRange());
}