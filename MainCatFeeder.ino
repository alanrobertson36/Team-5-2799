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
#define calibration_factor2 22.4
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
boolean toggle4 = LOW;

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
float gramsFeed(float cweight, float targetWeight, float kcalPerGram, float feedNum)        //kcal and feedNum is the user input variable
{
    float kcal_Day = (30 * cweight + 70);    //maintain weight
    
    float gramsMeal = (kcal_Day * (1000/kcalPerGram)) / feedNum;   // grams per feeding

    if(cweight > targetWeight)
      gramsMeal*=.7;
   
    return gramsMeal;      // amount of grams for mainting weight, simply multiply 0.7 to get the grams for loosing weight
    
}
long unsigned prevMill = 0;
long unsigned mill = millis();
long unsigned tempTime = millis();
long unsigned displayHours(long unsigned setTime){
  long unsigned hours =(((setTime - (mill- prevMill))/3600000) % 24);
  return (hours);
}
long unsigned displayMinutes(long unsigned setTime){
  long unsigned minutes =(((setTime - (mill- prevMill) )/60000) % 60); 
  return (minutes);
}
long unsigned displaySeconds(long unsigned setTime){
  long unsigned seconds =(((setTime - (mill- prevMill) )/1000) % 60); 
  return (seconds);
}
//8 hours equals 28800000 
//6 hours equals 21600000
//4 hours equals 14400000
//3 hours equals 10800000
//2 hours equals 7200000
//1 hour equals  3600000



  float weightCat = 3;
  float targWeight = 3;
  float gramPerFeed = 0;
  int feedPerDay = 3;
  int kcalKg[3] = {3,5,0};
  int targWeightArr[3] = {0,5, 1};
  int gramFeedArr[3] = {2,0,0};
  float kCalKg = 1;
  float food = 0;
  long pet = 0;
  int feedFinish = 0;
  int weightReading = 0;
  long unsigned setupMill = 0;
  bool feeding = false;
  float tempW1 = 0;
  int numsRun = 0;
  long unsigned timeLeft = 0;
  
/*Main Code*/
void loop(){
  int key = keypad.getKey();
  int wait = 0;
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
      clearRow(0);
      clearRow(1);
      clearRow(2);
      clearRow(3);
      lcd.setCursor(1,0);
      lcd.print("  Feedings/Day?:   ");
      lcd.setCursor(0,2);
      lcd.print("     (Max of 4)     ");
      delay(500);
      while(wait == 0){
        key =keypad.getKey();
        if(key){
          feedPerDay = key - 48;
          wait = 1;
        }
      }
      lcd.setCursor(10,1);
      lcd.print((int)feedPerDay);
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      
      clearRow(0);
      clearRow(1);
      clearRow(2);
      clearRow(3);
      lcd.setCursor(0,0);
      lcd.print("   Grams/Feeding?   ");
     
      clearRow(3);
      lcd.setCursor(0,1);
      lcd.print("100s:               ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          gramFeedArr[0] = key - 48;
          break;
        }
      }
      lcd.setCursor(5,1);
      lcd.print(gramFeedArr[0]);
      delay(500);
     

      clearRow(3);
      lcd.setCursor(0,1);
      lcd.print("10s:                ");
      while(1){
        key =keypad.getKey();
        if(key){
          gramFeedArr[1] = key - 48;
          break;
        }
      }
      delay(200);
      lcd.setCursor(4,1);
      lcd.print(gramFeedArr[1]);
      delay(500);
      

      clearRow(3);
      lcd.setCursor(0,1);
      lcd.print("1s:                 ");
      while(1){
        key =keypad.getKey();
        if(key){
          gramFeedArr[2] = key - 48;
          break;
        }
      }
      delay(200);
      lcd.setCursor(3,1);
      lcd.print(gramFeedArr[2]);
      delay(500);
   
      
      lcd.setCursor(0,1);
      lcd.print("Entered Value:     ");
      lcd.setCursor(14,1);
      gramPerFeed = gramFeedArr[0] * 100 + gramFeedArr[1] * 10 + gramFeedArr[2]; 
      lcd.print((int)gramPerFeed); 
      //wait for input to continue
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      feedFinish = 0;
      setupMill = millis();
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
      lcd.setCursor(0,2);
      lcd.print("Weight(kg):Reading..");
      weightReading = 0;
      while(weightReading == 0){
        delay(6000);
        petScale.set_scale(calibration_factor2);
        weightCat = (petScale.get_units(5))/1000;
        Serial.print(weightCat);
        Serial.print('\n');
        lcd.setCursor(0,2);
        lcd.print("Weight(kg):         ");
        lcd.setCursor(11, 2);
        lcd.print(weightCat);
        delay(3000);
        weightReading = 1;
      }


      //Enter Target Weight Below
      lcd.setCursor(0,1);
      lcd.print("Set Target Weight-Kg");
      
      lcd.setCursor(0,2);
      //user entry
      lcd.print("Enter 10s Digit:    ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          targWeightArr[0] = key - 48;
          break;
        }
      }
      
      lcd.setCursor(16,2);
      lcd.print(targWeightArr[0]);
      delay(500);
      
      clearRow(2);
      lcd.setCursor(0,2);
      lcd.print("1s:                 ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          targWeightArr[1] = key - 48;
          break;
        }
      }
      lcd.setCursor(3,2);
      lcd.print(targWeightArr[1]);
      delay(500);

       clearRow(2);
      lcd.setCursor(0,2);
      lcd.print(".1s:                ");
      delay(200);
      while(1){
        key =keypad.getKey();
        if(key){
          targWeightArr[2] = key - 48;
          break;
        }
      }
      lcd.setCursor(4,2);
      lcd.print(targWeightArr[2]);
            delay(500);

      lcd.setCursor(0,2);
      lcd.print("Entered Value:     ");
      lcd.setCursor(14,2);
      targWeight = targWeightArr[0] * 10 + targWeightArr[1] + targWeightArr[2] * .1; 
      lcd.print(targWeight);
      lcd.setCursor(0,3);
      lcd.print("                 A->"); 
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      
      
      //Enter Kcals below
      lcd.setCursor(0,3);
      lcd.print("                    ");
      lcd.setCursor(0,1);
      lcd.print("    Set kCal/kg     ");
      
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
      lcd.print((int)kCalKg); 
      //wait for input to continue
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      clearRow(0);
      clearRow(1);
      clearRow(2);
      clearRow(3);
      lcd.setCursor(1,0);
      lcd.print("  Feedings/Day?:   ");
      lcd.setCursor(0,2);
      lcd.print("     (Max of 4)     ");
      delay(500);
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(wait == 0){
        key =keypad.getKey();
        if(key){
          feedPerDay = key - 48;
          wait = 1;
        }
      }
      lcd.setCursor(10,1);
      lcd.print((int)feedPerDay);
      gramPerFeed = gramsFeed(weightCat, targWeight, kCalKg, feedPerDay);
      lcd.setCursor(0,3);
      lcd.print("                 A->");
      while(1){
        key = keypad.getKey();
        if(key == 'A'){
          break;
        }
      }
      feedFinish = 0;
      setupMill = millis();
      mode = 4;
      break;
/*
------------------------------------------------------------- 
--------------------------------------------------------------
*/ 
    case 4:
        feedFinish = 0;
        mill = millis()- setupMill;
        Serial.print(mill);
        Serial.print('\n');
        if( ((numsRun % feedPerDay) == 0) && (numsRun != 0) ){
          timeLeft = 43200000;
        }
        if( ((numsRun % feedPerDay) != 0) || (numsRun == 0) ){
          timeLeft = 120000/feedPerDay;
        }
        lcd.setCursor(0,0);
        lcd.print("Weight(Kgs):        ");
        lcd.setCursor(12,0);
        lcd.print(weightCat);

        petScale.set_scale(calibration_factor2);
        foodScale.set_scale(calibration_factor1);
        tempW1 = foodScale.get_units(5);
        if( ( (foodScale.get_units(2) >= (tempW1 + 10)) || (foodScale.get_units(2) <= (tempW1 - 10)) ) 
                          && (feeding == false) ){
          lcd.setCursor(12,0);
          lcd.print("----");
          weightCat = petScale.get_units(10)/1000;
          gramPerFeed = gramsFeed(weightCat, targWeight, kCalKg, feedPerDay);
        }
        
        lcd.setCursor(0,1);
        lcd.print("T2F:                ");
        lcd.setCursor(4,1);
        lcd.print(displayHours(timeLeft));
        lcd.setCursor(6,1);
        lcd.print(":");
        lcd.setCursor(7,1);
        lcd.print(displayMinutes(timeLeft));
        lcd.setCursor(0,2);
        lcd.setCursor(9,1);
        lcd.print(":");
        lcd.setCursor(10,1);
        lcd.print(displaySeconds(timeLeft));
        lcd.setCursor(13,1);
        lcd.print("BL:");

        //checking battery life
        if(millis()>=11520000){
         lcd.setCursor(16,1);
         lcd.print("LOW "); 
        }
        if((millis()<11520000)&& (millis()>=5760000)){
         lcd.setCursor(16,1);
         lcd.print("MED "); 
        }
        if((millis()<5760000)){
         lcd.setCursor(16,1);
         lcd.print("HIGH"); 
        }
        lcd.setCursor(0,2);
        lcd.print("Amt to Feed:        ");
        lcd.setCursor(12,2); 
        lcd.print(gramPerFeed);
        lcd.setCursor(3,3);
        lcd.print("A:Setup Screen   ");
        if(key == 'A'){
          lcd.clear();
          mode = 1;
          break; 
         }

        for(int n = 0; n < feedPerDay; n++){
         if( (displaySeconds(timeLeft) <= 0) && (displayMinutes(timeLeft) <= 0) && (displayHours(timeLeft) <= 0) ){
          feeding = true;
          while(feedFinish == 0){
              foodScale.set_scale(calibration_factor1);
              food = foodScale.get_units(5);
              if(food<gramPerFeed){
                myservo.write(80);
              }
              if(food>=gramPerFeed){
                myservo.write(90);
                timeLeft = 120000/feedPerDay;
                prevMill = mill;
                feeding = false;
                numsRun++;
                feedFinish = 1;
              }
          }
          
        }
       }
       break;
      
      }
      
}
