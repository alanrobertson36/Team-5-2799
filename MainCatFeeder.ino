#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>
#include <HX711.h>


//Food Scale Info
#define calibration_factor1 440.46
#define DOUT1  23
#define CLK1  25
HX711 foodScale;


//Pet Scale Info
#define calibration_factor2 23.4
#define DOUT2  27
#define CLK2  29
HX711 petScale;


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display


Servo myservo;


const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {22, 24, 26, 28}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {30, 32, 34, 36}; //connect to the column pinouts of the keypad
//LCD set up SDA-A4 and CLK - A5
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int mode = 1;

/*Setup Code*/

void setup(){
  Serial.begin(57600);
    
  foodScale.begin(DOUT1, CLK1);
  foodScale.set_scale(calibration_factor1);
  foodScale.tare();
  petScale.begin(DOUT2, CLK2);
  petScale.set_scale(calibration_factor2);
  petScale.tare();
    
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  
  myservo.attach(9);
}

/*Clear Row Function*/
void clearRow(int a){
  lcd.setCursor(0,a);
  lcd.print("                    ");
}

//Grams Per Feeding Function
float gramsFeed(float cweight, int targetWeight, int kcalPerGram, int feedNum)        //kcal and feedNum is the user input variable
{
    int kcal_Day = (int)(30 * cweight + 70);    //maintain weight
    Serial.print("\n");
    Serial.print(cweight);
    Serial.print("\n");
    Serial.print("is this even doing anything");
    Serial.print("\n");
    Serial.print(kcal_Day);
    Serial.print("\n");
    float gramsMeal = (kcal_Day * (float)(1000/kcalPerGram)) / feedNum;   // grams per feeding
    Serial.print(gramsMeal);
    Serial.print("\n");
    return gramsMeal;      // amount of grams for mainting weight, simply multiply 0.7 to get the grams for loosing weight
}

  float weightCat = 5;
  int targWeight = 3;
  float gramPerFeed = 0;
  int feedPerDay = 3;
  int kcalKg[3] = {3,5,0};
  int kCalKg = 1;
  
/*Main Code*/
void loop(){
  /*
  long pet = 0;
  petScale.tare();
  while(1){
    pet = petScale.read();
    Serial.print(pet);
    Serial.print('\n');
    if(pet < 630000){
      myservo.write(90);
    }
    if(pet >= 630000){
      myservo.write(0);
    }
  }
  
  long food = 0;
  foodScale.tare();
  while(1){
    food = foodScale.read();
    Serial.print(food);
    Serial.print('\n');
    if(food < 190000){
      myservo.write(0);
    }
    if(food >=190000){
      myservo.write(91);
    }
  }
  */
  int key = keypad.getKey();
  
  switch(mode){
    
    case 1:
      lcd.setCursor(4,0);
      lcd.print("Setup Screen");
      lcd.setCursor(3,1);
      lcd.print("1: Manual Mode");
      lcd.setCursor(2,2);
      lcd.print("2:Automatic Mode");
      
      key = keypad.getKey();
      if(key == 1 + '0'){
        lcd.clear();
        mode = 2;
        break;
      }
      if(key == 2 + '0'){
        lcd.clear();
        lcd.setCursor(4,0);
        mode = 3;
        break; 
      }
      break;
/*
------------------------------------------------------------- 




--------------------------------------------------------------
*/      
    case 2: //manual mode
      lcd.setCursor(0,0);
      lcd.print("       Manual       ");
      lcd.setCursor(0,1);
      lcd.print("   Feedings/Day?:    ");
      lcd.setCursor(0,4);
      lcd.print("      Max of 4      ");
      delay(500);
      /*while(1){
        key =keypad.getKey();
        if(key){
          feedPerDay = key - 48;
          break;
        }
      }*/
      lcd.setCursor(17,1);
      lcd.print(feedPerDay);
      lcd.setCursor(17,3);
      lcd.print("A->");
      while(1){
        key = keypad.getKey();
        if(key == 'A'){
          break; 
         }
      }
      clearRow(0);
      clearRow(1);
      clearRow(3);
      lcd.setCursor(0,1);
      lcd.print("Grams/Feeding?:      ");
      delay(500);
      while(1){
        key =keypad.getKey();
        if(key){
          gramPerFeed = key - 48;
          break;
        }
      }
      lcd.setCursor(15,1);
      lcd.print(gramPerFeed);
      lcd.setCursor(17,3);
      lcd.print("A->");
      while(1){
        key = keypad.getKey();
        if(key == 'A'){
          break; 
         }
      }
      mode =4;
      break;
/*
------------------------------------------------------------- 




--------------------------------------------------------------
*/       
    case 3: //auto mode
      lcd.setCursor(0,0);
      lcd.print("     Automatic     ");
      lcd.setCursor(1,1);
      lcd.print("Place Cat On Scale");
      delay(500);
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      /*
       * 
       * 
       * 
       * 
       * Have them enter target weight
       * 
       * 
       * 
       * 
       * 
       */
      lcd.setCursor(0,1);
      lcd.print("    Set kCal/kg    ");
      
      lcd.setCursor(0,2);
      //user entry
      lcd.print("Enter 1000s Digit:  ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          kcalKg[0] = key - 48;
          break;
        }
      }
      clearRow(3);
      lcd.setCursor(18,2);
      lcd.print(kcalKg[0]);
      delay(500);
      

      clearRow(3);
      lcd.setCursor(0,2);
      lcd.print("100s:               ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          kcalKg[1] = key - 48;
          break;
        }
      }
      lcd.setCursor(5,2);
      lcd.print(kcalKg[1]);
      delay(500);
     

      clearRow(3);
      lcd.setCursor(0,2);
      lcd.print("10s:                ");
      while(1){
        key =keypad.getKey();
        if(key){
          kcalKg[2] = key - 48;
          break;
        }
      }
      delay(200);
      lcd.setCursor(4,2);
      lcd.print(kcalKg[2]);
      delay(500);
      

      clearRow(3);
      lcd.setCursor(0,2);
      lcd.print("1s:                 ");
      while(1){
        key =keypad.getKey();
        if(key){
          kcalKg[3] = key - 48;
          break;
        }
      }
      delay(200);
      lcd.setCursor(3,2);
      lcd.print(kcalKg[3]);
      delay(500);
   
      
      lcd.setCursor(0,2);
      lcd.print("Entered Value:     ");
      lcd.setCursor(14,2);
      kCalKg = kcalKg[0] * 1000 + kcalKg[1] * 100 + kcalKg[2] * 10 + kcalKg[3]; 
      lcd.print(kCalKg); 
      //wait for input to continue
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      mode = 4;
      break;
/*
------------------------------------------------------------- 




--------------------------------------------------------------
*/ 
    case 4:
   
        lcd.setCursor(0,0);
        lcd.print("Weight(Lbs):    ");
        lcd.setCursor(0,1);
        lcd.print("T2F:                ");
        lcd.setCursor(0,2);
        lcd.print("Amt to Feed:        ");
        Serial.print("Cat weight is:");
        Serial.print(weightCat);
        Serial.print("\n");
        Serial.print("Target weight is:");
        Serial.print(targWeight);
        Serial.print("\n");
        Serial.print("kcal per kg is:");
        Serial.print(kCalKg);
        Serial.print("\n");
        Serial.print("feed per day is:");
        Serial.print(feedPerDay);
        Serial.print("\n");
        Serial.print(gramPerFeed);
        Serial.print("\n");
        lcd.setCursor(12,2);
        gramPerFeed = gramsFeed(weightCat, targWeight, kcalKg, feedPerDay); 
        lcd.print(gramPerFeed);
        lcd.setCursor(3,3);
        lcd.print("A:Setup Screen      ");
        if(key == 'A'){
          lcd.clear();
          mode = 1;
          break; 
         }
        break;
      
      }
}
