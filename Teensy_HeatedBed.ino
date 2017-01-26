
/*
   100K temp probe in series with 100k resistor where the resistor
   is connected to VCC and temp probe connected to GND.

   Notes:
   - When temp = 511 temp probe resistance is 100k


Example output to serial console:

------------- cnt: 1 -------------
Analog value: 537
i: 23.78 nA
Vo: 2.62 V
Power: 118.77 nW
Rt: 110.49 kOhm
Temperature: 22.94 C

   
*/
// Constants
const float vcc = 5; // This value is likely wrong so don't actually use it
const float r = 100; // kohm 
const float hyst = 1.5;
const int tstate = 32; // Traget state
const int ledPin = 10;
const int anaPin = 0;
// Variables
int ana = 0;
int state = 0;
float temp = 0;
float lasttemp = 0;
float vo = 0;
float rt = 0;
float i = 0;
// Arrays
float rtab[] = {1.6419, 1.8548, 2.1008, 2.3861, 2.7179, 3.1050, 3.5581, 4.0901, 4.7170, 5.4584, 6.3383, 7.3867, 8.6407, 10.147, 11.963, 14.164, 16.841, 20.114, 24.136, 29.100, 35.262, 42.950, 52.598, 64.776, 80.239, 100.00, 125.42, 158.34, 201.27, 257.69, 332.40};
float ttab[] = {150.00, 145.00, 140.00, 135.00, 130.00, 125.00, 120.00, 115.00, 110.00, 105.00, 100.00, 95.000, 90.000, 85.000, 80.000, 75.000, 70.000, 65.000, 60.000, 55.000, 50.000, 45.000, 40.000, 35.000, 30.000, 25.000, 20.000, 15.000, 10.000, 5.0000, 0.0000};
int arrSize = sizeof(rtab) / sizeof(float); // calculate array size
// Count
int cnt = 0;

void setup() {
  Serial.begin(9600); // USB is always 12 Mbit/sec
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

/*
    This function is sourced from:
    http://playground.arduino.cc/Main/MultiMap
*/
// note: the in array should have increasing values
float FmultiMap(float val, float * _in, float * _out, uint8_t size) {
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}


void loop() {
  ana = analogRead(anaPin);

  vo = vcc * ana / 1024; // mV
  i = ((vcc - vo ) / r); // mA
  rt = r * ana / (1023 - ana);
  // rt = vo / (i);         // kohm
  temp = FmultiMap(rt, rtab, ttab, arrSize);
  float pwr = vcc * vcc / (r + rt); // nW


 // Heating element logic
  if (lasttemp == 0) lasttemp = temp;
  if (temp != lasttemp){
    if (state == 0 && temp >= (tstate + hyst)){
      digitalWrite(ledPin, LOW); 
      state = 1;
    }
    else if (state == 1 && temp <= (tstate - hyst)){
      digitalWrite(ledPin, HIGH);
      state = 0;
    }  
  }
  
  lasttemp = temp;
  cnt = cnt + 1;

  Serial.print("------------- ");
  Serial.print("cnt: ");
  Serial.print(cnt);
  Serial.println(" -------------");

  Serial.print("Analog value: ");
  Serial.println(ana);

  Serial.print("i: ");
  Serial.print(i * 1000);
  Serial.println(" nA");

  Serial.print("Vo: ");
  Serial.print(vo);
  Serial.println(" V");

  Serial.print("Power: ");
  Serial.print(pwr * 1000);
  Serial.println(" nW");
  
  Serial.print("Rt: ");
  Serial.print(rt);
  Serial.println(" kOhm");
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  delay(1000);
}








