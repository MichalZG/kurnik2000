#include <OneWire.h> 
#include <DallasTemperature.h>
#include "DHT.h"
#include <Wire.h>   
#include <LCD.h>
#include <LiquidCrystal_I2C.h> 

#define DHTPIN 3
#define DHTTYPE DHT11

#define ONE_WIRE_BUS 2 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

#define I2C_ADDR          0x27        
#define BACKLIGHT_PIN      3
#define En_pin             2
#define Rw_pin             1
#define Rs_pin             0
#define D4_pin             4
#define D5_pin             5
#define D6_pin             6
#define D7_pin             7

const int photo_analog_pin = 3;
const int photo_digital_pin = 4;
const int relay_pin1 = 5;
const int relay_pin2 = 6;
const int button_pin1 = 7;

const int array_len = 300;

LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
DHT dht(DHTPIN, DHTTYPE);

int phot_values_array[array_len];
int light_time_len = 180; //min
long timestamp = millis() / 1000;
int global_counter = 0;
int light_start = 0;
int light_flag = 0;
int phot_array_sum = 0;
int button_state1 = 0;
int back_light_state = 0;


void setup(void){ 

  Wire.begin();
  Serial.begin(115200); 
  dht.begin();
  sensors.begin(); 
  

  
  lcd.begin(16,2);   
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LOW);
  pinMode(photo_digital_pin, INPUT); 
  pinMode(button_pin1, INPUT); 

  pinMode(relay_pin1, OUTPUT);
  digitalWrite(relay_pin1, HIGH);
  
  pinMode(relay_pin2, OUTPUT);
  digitalWrite(relay_pin2, LOW);  
} 

void loop(void) 
{ 
  int temp_dht = dht.readTemperature();
  int hum_dth = dht.readHumidity();
  int phot_state = digitalRead(photo_digital_pin);
  int phot_val = analogRead(photo_analog_pin);

  Serial.println(phot_state);
  Serial.println(phot_val);
  
  lcd_display(temp_dht, hum_dth, phot_val, timestamp);
  button_action();
  delay(1000);
  
  if ((millis() / 1000) - timestamp > 1){
    timestamp = millis() / 1000;
    
    phot_array_sum = phot_array_action(phot_state);
  }
  
  light_action(phot_array_sum);
}

void light_action(int phot_array_sum){
  Serial.println("sum:");
  Serial.println(phot_array_sum);
  if ((phot_array_sum == array_len) && (light_flag == 0)){
    light_start = millis() / 1000;
    light_flag = 1;
    digitalWrite(relay_pin1, LOW);
  }
  
  if (phot_array_sum == 0){
    light_flag = 0;
  }
  
  if ((millis() / 1000) - light_start > light_time_len * 60){
    digitalWrite(relay_pin1, HIGH); 
  }
}

void lcd_display(int temp_dht, int hum_dth, int phot_val, int timestamp){

  phot_val = ((1023 - (float) phot_val) / 1023) * 100;
  int light_dur = ((millis() / 1000) - light_start) / 60;

  char phot_val_char[4];
  char light_dur_char[3];
  
  sprintf(phot_val_char, "%03d", phot_val);
  

  lcd.setCursor(0,0); 
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(temp_dht);
  
  lcd.setCursor(5,0);
  lcd.print("H:");
  lcd.setCursor(7,0);
  lcd.print(hum_dth);
  
  
  lcd.setCursor(0,1); 
  lcd.print("LIGHT:");
  lcd.setCursor(6,1); 
  lcd.print(phot_val_char);
  lcd.setCursor(12,1); 
  if (light_flag == 1){
    sprintf(light_dur_char, "%03d", light_dur);
    lcd.print(light_dur_char);
  }
  else{
    lcd.print("X");
  }
}

int phot_array_action(int phot_state){
  if (global_counter > array_len){
    global_counter = 0;
  }
  phot_values_array[global_counter] = phot_state;
  global_counter++;
  
  int i;
  int s = 0;
  for (i = 0; i < array_len; i = i + 1) {
    s += phot_values_array[i];
  }

  return s;
}

void button_action(){
  button_state1 = digitalRead(button_pin1);
  if (button_state1 == HIGH){
    back_light_state = !back_light_state;
    lcd.setBacklight(back_light_state);
  }

}
