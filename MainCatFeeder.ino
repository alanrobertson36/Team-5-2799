#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

char mode = 'S';
char kcalKg[3] = {'3','5','0'};

byte pin_rows[ROW_NUM] = {12, 11, 10, 9}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
//LCD set up SDA-A4 and CLK - A5

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );



void setup(){
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
}

void loop(){
  char key = keypad.getKey();
  switch(mode){
    
    case 'S':
      lcd.setCursor(4,0);
      lcd.print("Setup Screen");
      lcd.setCursor(3,1);
      lcd.print("1: Manual Mode");
      lcd.setCursor(3,2);
      lcd.print("2:Automatic Mode");
      key = keypad.getKey();
      if(key == 1 + '0'){
        lcd.clear();
        mode = 'M';
        break;
      }
      if(key == 2 + '0'){
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("It Changes");
        mode = 'A';
        break; 
      }
      
    case 'M': //manual mode
      lcd.setCursor(4,0);
      lcd.print("Manual Mode");
      lcd.setCursor(1,1);
      lcd.print("Feedings/Day?");
      //save scale reading to variable
      delay(2000);
      lcd.setCursor(1,1);
      lcd.print("Grams/Feeding?");
      //save scale reading to variable
      delay(2000);
      
      break;
      
    case 'A': //auto mode
      lcd.setCursor(2,0);
      lcd.print("Automatic Mode");
      lcd.setCursor(1,1);
      lcd.print("Place Cat On Scale");
      //save scale reading to variable
      delay(2000);
      lcd.setCursor(1,1);
      lcd.print("Set kCal/kg");
      
      lcd.setCursor(1,1);
      //user entry
      lcd.print("Enter 1000s Digit ");
      kcalKg[0] = keypad.getKey();
      delay(3000);
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      
      lcd.setCursor(1,1);
      lcd.print("Enter 100s Digit  ");
      kcalKg[1] = keypad.getKey();
      delay(3000);
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      
      lcd.setCursor(1,1);
      lcd.print("Enter 10s Digit  ");
      kcalKg[2] = keypad.getKey();
      delay(3000);
      while(1){
        if(keypad.getKey()){
          break;
        }
      }

      lcd.setCursor(1,1);
      lcd.print("Enter 1s Digit   ");
      kcalKg[3] = keypad.getKey();
      while(1){
        if(keypad.getKey()){
          break;
        }
      }
      
      lcd.setCursor(1,1);
      lcd.print("Entered Value Is");
      lcd.setCursor(4,2);
      lcd.print(kcalKg);
    
      mode = 'D';
      break;
      

   case 'D':
   
        lcd.setCursor(4,0);
        lcd.print("Weight of Cat:");
        lcd.setCursor(4,1);
        lcd.print("T2F:");
        lcd.setCursor(4,2);
        lcd.print("Amt to Feed:");
        lcd.setCursor(4,3);
        lcd.print("Battery Life:");
        break;
      
     
      }
}
