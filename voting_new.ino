//Uses MIFARE RFID card using RFID-RC522 reader
// * Uses MFRC522 - Library
// * -----------------------------------------------------------------------------------------
// *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
// *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
// * Signal      Pin          Pin           Pin       Pin        Pin              Pin
// * -----------------------------------------------------------------------------------------
// * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
// * SPI SS      SDA(SS)      10            53        D10        10               10
// * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
// * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
// * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

// The circuit:
// * LCD RS pin to digital pin 7
// * LCD Enable pin to digital pin 6
// * LCD D4 pin to digital pin 5
// * LCD D5 pin to digital pin 4
// * LCD D6 pin to digital pin 3
// * LCD D7 pin to digital pin 2
// * LCD R/W pin to ground
// * 10K resistor:
// * ends to +5V and ground
// * wiper to LCD VO pin (pin 3)


// include the library code:
#include <LiquidCrystal.h>
#include <GKScroll.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h> //EEPROM Library


#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
int result = A1;
//int closed = A2;
//int total =    A3;
int clearbutton = A0;
int a=A2; //Voter 1
int b=A3; //Voter 2
int c=A4; //Voter 3

//temporary Variables used in program
int vote1;
int vote2;
int vote3;
int cflag;
int v;
int voting_count=0;

//int vote4=0;
int card1_count=0;
int card2_count=0;
int card3_count=0;
int card4_count=0;

//int scrollBuffer = 5;
//String content = "";
//byte letter;
const int buzzer = A5;
const int red = 1;
const int green = 8;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//char receivedChar;
//boolean newData = false;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


//extra
void buzzerOK(){
    //fn. to activate buzzer in a sequence
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW); 
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    
}
void buzzerScan(){
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
}
void buzzerButtonPress(){
    digitalWrite(buzzer,HIGH);
    delay(1000);
    digitalWrite(buzzer,LOW);
}
void buzzerError(){
    //fn. to activate buzzer in a sequence
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
    delay(500);
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
    delay(500);
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
    delay(500);
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW); 
}

void setup() {
  // put your setup code here, to run once:
  cflag = EEPROM.read(0); //read the status of cflag from memory
  vote1 = EEPROM.read(1); // v1 stores the vote count of party A
  vote2 = EEPROM.read(2); //v2 stores the vote cound of party B
  vote3 = EEPROM.read(3); //v3 stores the vote count of party C

  //Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  
  pinMode(a,INPUT);
  pinMode(b,INPUT);
  pinMode(c,INPUT);
  pinMode(result,INPUT);
  pinMode(clearbutton,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  
  //default status of Buttons when not presses is high
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(result,HIGH);
  digitalWrite(clearbutton,HIGH);
  lcd.setCursor(0,0);
  lcd.print("Initialising.");
  delay(1000);
  lcd.clear();
  lcd.print("Initialising..");
  delay(1000);
  lcd.clear();
  lcd.print("Initialising...");
  delay(1000);
  lcd.clear();
  //Serial.print(digitalRead(a));
  //Serial.println(digitalRead(b));
  //Serial.println(digitalRead(c));
  //Serial.println(digitalRead(result));
  //Serial.println(digitalRead(clearbutton));
   if(cflag == 0)
  {
    lcd.print("Authenticated");
    lcd.setCursor(0,1);
    lcd.print("Voting Machine");
    digitalWrite(green,HIGH);
    delay(2000);
    digitalWrite(green,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    Waiting for");
    lcd.setCursor(0,1);
    lcd.print("     Card");
    delay(1000);
  }else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Voting Closed");
    lcd.setCursor(0,1);
    v=vote1+vote2+vote3;
    lcd.print("Total Votes :");
    lcd.print(v);
  }
}

void rpt()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Scan RFID Card");
}

void votedifference() // function to calculate vote difference
      {
        if(vote1>vote2)
        {
          if(vote2>vote3)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("A wins by");
            lcd.setCursor(0,1);
            lcd.print(vote1-vote2);
            lcd.print(" votes");
          }
          else
          {
            if(vote1>vote3)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("A wins by");
              lcd.setCursor(0,1);
              lcd.print(vote1-vote3);
              lcd.print(" votes");
            }
            if(vote1<vote3)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("C wins by");
              lcd.setCursor(0,1);
              lcd.print(vote3-vote1);
              lcd.print(" votes");
            }
          }
        }
        else
        {
          if(vote1>vote3)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("B wins by");
            lcd.setCursor(0,1);
            lcd.print(vote2-vote1);
            lcd.print(" votes");
          }
         
          else
          {
              if(vote2>vote3)
              {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("B wins by");
                lcd.setCursor(0,1);
                lcd.print(vote2-vote3);
                lcd.print(" votes");
              }
              if(vote2<vote3)
              {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("C wins by");
                lcd.setCursor(0,1);
                lcd.print(vote3-vote2);
                lcd.print(" votes");
              }
          }
        }
     }

void other_key()
{
  // if result button is pressed after the close button was pressed
                if(digitalRead(result)==LOW)
                {
                    cflag=1;
                    EEPROM.write(0,cflag);
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Voting Closed!!!");
                    lcd.setCursor(0,1);
                    v=vote1+vote2+vote3;
                    lcd.print(" Total Votes:");
                    lcd.print(v);
                    delay(2000);
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("A:");
                    lcd.setCursor(0,1);
                    lcd.print(vote1);
                    lcd.setCursor(8,0);
                    lcd.print("B:");
                    lcd.setCursor(8,1);
                    lcd.print(vote2);
                    lcd.setCursor(14,0);
                    lcd.print("C:");
                    lcd.setCursor(14,1);
                    lcd.print(vote3);
                    delay(3000);
       
                    // logic for result of voting process
                    if(vote1==vote2 && vote2==vote3)
                    {
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print(" Result Tied!!!");
                      delay(3000);
                    }
   
                    if(vote1==vote2 && vote1>vote3)
                    {
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("Tie b/w A and B");
                      delay(3000);
                    }
     
                    if(vote2==vote3 && vote2>vote1)
                    {
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("Tie b/w B and C");
                      delay(3000);
                    }
                    if(vote1==vote3 && vote1>vote2)
                    {
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("Tie b/w A and C");
                      delay(3000);
                    }
   
                    if(vote1>vote2)
                    {
                      if(vote1>vote3)
                      {
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print(" Party A wins!!!");
                        delay(3000);
                        votedifference();
                      }
                      else if(vote3>vote1)
                      {
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print(" Party C wins!!!");
                        delay(3000);
                        votedifference();
                      }
                    }
                    else
                    {
                        if(vote2>vote3 && vote1!=vote2)
                        {
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print(" Party B wins!!!");
                          delay(3000);
                          votedifference();
                         }
                         else if(vote3>vote2)
                         {
                            lcd.clear();
                            lcd.setCursor(0,0);
                            lcd.print(" Party C wins!!!");
                            delay(3000);
                            votedifference();
                          }
                      }
                  }
    
                  if(digitalRead(clearbutton)==LOW) // if clear button is pressed
                  {
                     for (int i = 0; i < 512; i++)
                    {
                        EEPROM.write(i, 0);
                    }
                    vote1=0;
                    vote2=0;
                    vote3=0;
                    cflag=0;
                    card1_count=0;
                    card2_count=0;
                    card3_count=0;
                    card4_count=0;
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print(" Memory Cleared");
                    lcd.setCursor(0,1);
                    lcd.print("  ***********  ");
                    delay(3000);
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Scan RFID card");

                  }
}
void voting(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Press a");
  lcd.setCursor(0,1);
  lcd.print("Button to Vote");
  
  while(1){
  if(digitalRead(a)==LOW && cflag==0) // if party A button is pressed
              {
                  buzzerButtonPress();
                  voting_count++;
                  vote1=vote1+1;
                  EEPROM.write(1,vote1);
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Vote received...");
                  lcd.setCursor(0,1);
                  lcd.print("  Thank You!!");
                  //digitalWrite(buzzer,LOW);
                  delay(1500);
                  digitalWrite(green,LOW);
                  rpt(); 
                  //while(digitalRead(a) == 0);
                  return;
               }
    
                if(digitalRead(b)==LOW && cflag==0) // if party B button is pressed
                {
                  buzzerButtonPress();
                  voting_count++;
                  vote2=vote2+1;
                  EEPROM.write(2,vote2);
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Vote received...");
                  lcd.setCursor(0,1);
                  lcd.print("  Thank You!!");
                  delay(1500);
                  digitalWrite(green,LOW);
                  rpt();
                  return; 
                }
    
                if(digitalRead(c)==LOW && cflag==0) // if party C button is pressed
                {
                  buzzerButtonPress();
                  voting_count++;
                  vote3=vote3+1;
                  EEPROM.write(3,vote3);
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Vote received...");
                  lcd.setCursor(0,1);
                  lcd.print("  Thank You!!");
                  delay(1500);
                  digitalWrite(green,LOW);
                  rpt(); 
                  return;
                }
  }
      //rpt();
}

void loop() {

  // put your main code here, to run repeatedly:
  //lcd.clear();
  // Look for new cards
  //delay(2000);
    other_key();
    if (mfrc522.PICC_IsNewCardPresent() ){
      //true,if RFID tag/card is present) PICC = Proximity Integrated Circuit Card
        if(mfrc522.PICC_ReadCardSerial()){
          //True if RFID tag/card was read.
          unsigned long UID_unsigned;
    UID_unsigned = mfrc522.uid.uidByte[0] << 24;
    UID_unsigned += mfrc522.uid.uidByte[1] << 16;
    UID_unsigned += mfrc522.uid.uidByte[2] << 8;
    UID_unsigned += mfrc522.uid.uidByte[3];
//    if(showCardData==1){
//        Serial.println("UID Unsigned int");
//        Serial.println(UID_unsigned);
//        String UID_string = (String)UID_unsigned;
//        long UID_LONG=(long)UID_unsigned;
//        Serial.println("UID Long :");
//        Serial.println(UID_LONG);
//        Serial.println("UID String :");
//        Serial.println(UID_string);
//    } // print card data to the serial monitor
          int UID_int = (int) UID_unsigned;
          if(UID_int == -741){
            if(card1_count>0)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("    Voting Done!!");
              lcd.setCursor(0,1);
              lcd.print("Please Leave!!");
              digitalWrite(red,HIGH);
              buzzerError();
              delay(2000);
              digitalWrite(red,LOW);
              rpt();
              return;
            }
            else
            {
              voting_count=0;
              buzzerScan();
              card1_count=card1_count+1;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Authorised User");
              delay(200);
              digitalWrite(green,HIGH);
              delay(2000);
              voting(); 
              return;
            }
          }else if(UID_int == 19742)
          {
            if(card2_count>0)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("    Voting Done!!");
              lcd.setCursor(0,1);
              lcd.print("Please Leave!!");
              digitalWrite(red,HIGH);
              buzzerError();
              delay(2000);
              digitalWrite(red,LOW);
              rpt();
              return;
            }
            else
            {
              voting_count=0;
              buzzerScan();
              card2_count=card2_count+1;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Authorised User");
              delay(200);
              digitalWrite(green,HIGH);
              delay(2000);
              voting(); 
              return;
            }
            }
            else if(UID_int == 16158){
              if(card3_count>0)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("    Voting Done!!");
              lcd.setCursor(0,1);
              lcd.print("Please Leave!!");
              digitalWrite(red,HIGH);
              buzzerError();
              delay(2000);
              digitalWrite(red,LOW);
              rpt();
              return;
            }
            else
            {
              voting_count=0;
              buzzerScan();
              card3_count=card3_count+1;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Authorised User");
              delay(200);
              digitalWrite(green,HIGH);
              delay(2000);
              voting(); 
              return;
            }
            }
            else if(UID_int == -18918){
              if(card4_count>0)
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("    Voting Done!!");
              lcd.setCursor(0,1);
              lcd.print("Please Leave!!");
              digitalWrite(red,HIGH);
              buzzerError();
              delay(2000);
              digitalWrite(red,LOW);
              rpt();
              return;
            }
            else
            {
              voting_count=0;
              buzzerScan();
              card4_count=card4_count+1;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Authorised User");
              delay(200);
              digitalWrite(green,HIGH);
              delay(2000);
              voting(); 
              return;
            }
            }
            else{
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("   UnAuthorised");
              lcd.setCursor(0,1);
              lcd.print("     User!!");
              digitalWrite(red,HIGH);
              buzzerError();
              delay(2000);
              digitalWrite(red,LOW);
              rpt();
            }
          }
}
}
