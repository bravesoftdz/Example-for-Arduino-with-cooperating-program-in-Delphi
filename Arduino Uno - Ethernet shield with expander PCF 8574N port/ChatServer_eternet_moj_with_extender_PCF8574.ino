/*
 Chat Server

 A simple server that distributes any incoming messages to all
 connected clients.  To use, telnet to your device's put IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
  modified 10.04.2017 y
 by JerzySmietanski SP9AUV

Copyright by SP9AUV ham radio  at 03.10.2017 year 
-This sketch ChatServer_eternet_moj_with_extender_PCF8574.ino 
  what have configured socket server(port 23).
  copoerating with a users program in a Delphi
  Cli_and_serv_Socket_with_Arduino_and_ETHERNET_Shield_Chat_Client_Extend_ONE_PCF.exe .
  what have configured one socket client CliSocket ant it cooperated with socket server on Sketch
     on Arduino Uno with shield for ETHERNET .
     Board ARDUINO also have also one  8 port extender PCF8574N , for extend pins port for ARDUINO UNO .
     Only one extender is posibblky , because ARDUINO UNO have only 2 Kbytes for data .

     Why I not preffer www server : Because it is too slow, and you must use  browser
     for example FIREFOX , what is too big tool for this task . With use standard browser it is
     to sloow . Better use protocol TCP and sockets on Sketch Arduino , and in a user's program .

     This example I do only for testing , because for controling antenna system I prefer
     use Wifi , to protect from atmospheric discharge .

    Program in a Delphi working using TWSocket from Overbyte  (not Windows soket or Indy socket-it is bad)
    Also use TMS component for AdvSmoothGauge to dsplay analog value (may try use standard Tgauge)
    and    RVOfficeRadioGroup1  from TMS package

    There are used universal method for cooperating with any ardunino Sketch ,
    what this implies that the states of all pins are sent to Sketch from user's Delphi program,
    and Sketch continuously sends the state of all pins in the loop  to the users in a Delphi programs .
    In this way Sketch and users is simply
 */

#include <SPI.h>
#include <Ethernet.h>
#include <PCF8574.h>
#include <Wire.h>
//#define UNKNOWN_PIN 0xFF

PCF8574 expander;
//PCF8574 expander_1;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192,168,1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

int ActualValuForPin0;
int PreviuosValueForPin0;

String readString; //string for fetching data from address
String BufForAllDigitalLine;
String PreviousBufForAllDigitalLine;
const int NoOfDigitalPinsOnBoard = 14;
const int NoOfDigitalPinsOnExtendedPCF857X = 8;  //7+1  because in a local loop  is neded  8
                                                 // Pins is  from P0   to P7
const char* host = "192.168.1.7";

EthernetServer server(1020);
boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {  
  expander.begin(0x20);
  //expander_1.begin(0x21);
  
  expander.pinMode(0, OUTPUT);
  expander.pinMode(1, OUTPUT);
  expander.pinMode(2, OUTPUT);
  expander.pinMode(3, OUTPUT);

  expander.pinMode(4, OUTPUT);
  expander.pinMode(5, OUTPUT);
  expander.pinMode(6, OUTPUT);
  expander.pinMode(7, OUTPUT);
  
  
  //pinMode(4, OUTPUT); //Configure pin 4 as OUTPUT
  pinMode(6, OUTPUT); //Configure pin 6 as OUTPUT
  pinMode(7, OUTPUT); //Configure pin 7 as OUTPUT
  pinMode(8, OUTPUT); //Configure pin 8 as OUTPUT
  pinMode(9, OUTPUT); //Configure pin 9 as OUTPUT

  pinMode(3, INPUT_PULLUP); //Configure pin 3 as INPUT_PULLUP
  pinMode(4, INPUT_PULLUP); //Configure pin 4 as INPUT_PULLUP
  pinMode(5, INPUT_PULLUP); //Configure pin 5 as INPUT_PULLUP

     
  // initialize the ethernet device
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only

  }
  

  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
   expander.digitalWrite(0, LOW);
   expander.digitalWrite(1, HIGH);
   expander.digitalWrite(2, LOW);

  /*
    for (int DigitalChannel = 2; DigitalChannel < NoOfDigitalPinsOnBoard - 4 ; DigitalChannel++) { 
      
    }
    */
}

void loop() {
  
  // wait for a new client:
  EthernetClient client = server.available();
   if (client) {
     if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      //Serial.println("We have a new client");
       alreadyConnected = true;
    }
   
    if (client.available() > 5) {   
    //if (client.available() > 0) {   
      //client.available() give NoOfCharacter send by client
      //At least 5 characters must be from the client
      //Serial.println("NoOfCharacterReceivedFromClient = " + String(client.available())); 
     readString = "" ; 
     client.flush();
     ReadStringFromRemoteClient (client );
     SetStatePins ();
     BufForAllDigitalLine = "";
     SendServerPinsDataToRemoteClient( client);
    }  //if (client.available() > 5)
    
    
     ActualValuForPin0 = analogRead(0); 
    if (abs((ActualValuForPin0 - PreviuosValueForPin0) ) > 10) { 
    //Serial.println("ActualValuForPin0 = " +String(ActualValuForPin0)); 
    //Serial.println("PreviuosValueForPin0 = " +String(PreviuosValueForPin0)); 
         CheckStateAnalogPinsAndSendToRemote  (client);
        //Serial.println("ActualValuForPin0 = " +String(ActualValuForPin0)); 
        //Serial.println("PreviuosValueForPin0 = " +String(PreviuosValueForPin0)); 
        //Serial.println("client_available = " +String(client.available())); 
         }
         

         
 //BufForAllDigitalLine = ""; //cannot be reset this value  , because Sketch reading only pins what is set mode = INPUT_PULLUP , mean 3,4,5 
 //for event who is do at Arduino equipment 
 
 ReadStateOfDigitalPins();        
 //Serial.println("Before comp.ActualDigitalValueFor__Pins = " + BufForAllDigitalLine); 
 //Serial.println("Before comp.PreviuosDigitalValueForPins = " + PreviousBufForAllDigitalLine);

 //in BufForAllDigitalLine  and  PreviousBufForAllDigitalLine is present state pins when 
 //it was set as INPUT_PULLUP mode - sou this value is not too long .because we have only
 //2 KB for values in Arduino UNO
 
 if (PreviousBufForAllDigitalLine  != BufForAllDigitalLine){
    BufForAllDigitalLine = "";
    SendServerPinsDataToRemoteClient(client);
    //Serial.println("After Comparing ");
    //Serial.println("ActualDigitalValueFor__Pins = " + BufForAllDigitalLine); 
    //Serial.println("PreviuosDigitalValueForPins = " + PreviousBufForAllDigitalLine);
    PreviousBufForAllDigitalLine  = BufForAllDigitalLine;
 }

       
const char ci = 'C' ;
 //if (readString  == "CLOSE_SESSION" ){
     //if (readString[1]  == "C" ){ 
     //if (readString[0]  == "C" ){ 
     //if (readString  == 'CLOSE_SESSION' ){
     //if (readString  == CLOSE_SESSION ){
     //if (readString[0]  == ci ){
     if (readString[1]  == ci ){ 
       //Serial.println("String After ReadStringFromRemoteClient = " + readString);
       //Serial.println("Seesion with remote client will be  closed"); 
       client.stop();
       Serial.println("Seesion with remote client was closed"); 
    }
   }  //if (client)
}  //void loop()


 uint8_t getPinMode(uint8_t pin){
 uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg, *out;
  if (port == NOT_A_PIN) return 0xFF;
  reg = portModeRegister(port);
  out = portOutputRegister(port);
 
  //Check for tris (DDxn)
  if( (*reg) & bit ){ //is output
    return OUTPUT;
  }
  else{ //is input
    //Check state (PORTxn)
    if( (*out) & bit ){  //is set
      return INPUT_PULLUP;
    }
    else{ //is clear
      return INPUT;
    }
  }
}


void ReadStateOfDigitalPins(){ 
      //Serial.println("Came to ReadStateOfDigitalPins ");
      //Serial.println();

 // output the value of each digital input pin
   //BufForAllDigitalLine = "";
   //for (int DigitalChannel = 0; DigitalChannel < NoOfDigitalPinsOnBoard; DigitalChannel++) {   
   for (int DigitalChannel = 2; DigitalChannel < NoOfDigitalPinsOnBoard - 4 ; DigitalChannel++) {    

            //const int NoOfDigitalPinsOnBoard = 14;
            //from No pin =2 because pin 0 is used as RX . and pin 1 is used as TX 
            
            //14 - 4 = 10 mean  to pin 9 can set state pins
            //Pin 10,11,12 ,13 is used for SPI for ethernet shield 
            
              int sensorVal = digitalRead(DigitalChannel);
              //BufForAllDigitalLine = BufForAllDigitalLine + " D " + String(DigitalChannel) + " " + String(sensorVal) + " " ;
              //BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
              uint8_t  PinMode ;
              PinMode = getPinMode(DigitalChannel);
              if (PinMode == INPUT_PULLUP )
               {
                BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
               }
               
                       }

//ReadStateOfDigitalPinsExtendedPCF857X
   for (int DigitalChannel = 0; DigitalChannel < NoOfDigitalPinsOnExtendedPCF857X; DigitalChannel++) {         //0   to 7 - it is numbered as P0  P1 P2 P3 P4 P5 P6 P7 
              int sensorVal = expander.digitalRead(DigitalChannel); 
              //BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
              uint8_t  PinMode ;
              PinMode = getPinMode(DigitalChannel);
              if (PinMode == INPUT_PULLUP )
               {
                BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
               }
               
                 }   

              //PreviousBufForAllDigitalLine  = BufForAllDigitalLine;
             }

void CheckStateAnalogPinsAndSendToRemote  ( EthernetClient  cl){
 String BufForOneAnalogLine;     
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
           if (analogChannel == 0)   {
            PreviuosValueForPin0 = sensorReading;
           }
            Serial.print("analog input ");
            Serial.print(analogChannel);
            Serial.print(" is ");
            Serial.print(sensorReading);
            Serial.println("<br />");
            BufForOneAnalogLine = BufForOneAnalogLine + " A " + String(analogChannel) + " " + String(sensorReading) + " " ;
    }
cl.println(BufForOneAnalogLine); 
Serial.println();  
}


void SendServerPinsDataToRemoteClient( EthernetClient  cl){
      Serial.println("Came to SendServerPinsDataToRemoteClient ");
      Serial.println();
      String BufForDigitalLine = "";  
    // output the value of each digital input pin
  //for (int DigitalChannel = 0; DigitalChannel < NoOfDigitalPinsOnBoard; DigitalChannel++) {
    for (int DigitalChannel = 2; DigitalChannel < NoOfDigitalPinsOnBoard - 4 ; DigitalChannel++) {    
            //const int NoOfDigitalPinsOnBoard = 14;
            //from No pin =2 because pin 0 is used as RX . and pin 1 is used as TX 
            //pin 0 and 1 is used as RX and TX for USB serial   
            //14 - 4 = 10 mean  to pin 9 can set state pins
            //Pin 10,11,12 ,13 is used for SPI for ethernet shield 
            
    
      //from No pin =2 because pin 0 is used as RX . and pin 1 is used as TX   
              int sensorVal = digitalRead(DigitalChannel);
              Serial.print("digital input  D ");
              Serial.print(DigitalChannel);
              Serial.print(" is ");                      
              Serial.print(sensorVal);
              Serial.println("<br />");
              BufForDigitalLine = BufForDigitalLine + " D " + String(DigitalChannel) + " " + String(sensorVal) + " " ;
              //beause working in a Arduino enviroment take atention that all value in a runningo procedure 
              //cannot exceed allow 2 KB for value in  memory             
             // Serial.println(BufForAllDigitalLine); 
              //BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
              uint8_t  PinMode ;
              PinMode = getPinMode(DigitalChannel);
              if (PinMode == INPUT_PULLUP )
               {
                BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
               } 
              
                       }
   cl.println(BufForDigitalLine);
  
   BufForDigitalLine = "";
   Serial.println();
   //PCF8574 expander; 
    for (int DigitalChannel = 0; DigitalChannel < NoOfDigitalPinsOnExtendedPCF857X; DigitalChannel++) {  //0   to 7 - it is numbered as P0  P1 P2 P3 P4 P5 P6 P7 
              int sensorVal =  expander.digitalRead(DigitalChannel);
              Serial.print("digital input  P ");
              Serial.print(DigitalChannel);
              Serial.print(" is ");                      
              Serial.print(sensorVal);
              Serial.println("<br />"); 
              BufForDigitalLine = BufForDigitalLine + " P " + String(DigitalChannel) + " " + String(sensorVal) + " " ;
              //BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
              
              uint8_t  PinMode ;
              PinMode = getPinMode(DigitalChannel);
              if (PinMode == INPUT_PULLUP )
               {
                BufForAllDigitalLine = BufForAllDigitalLine  + String(sensorVal) ;
               }  
               
                  }   

cl.println(BufForDigitalLine);
//Serial.println("Send to client = "+BufForAllDigitalLine);
//BufForAllDigitalLine = BufForAllDigitalLine + BufForDigitalLine;
//Serial.println("Inside = "+ BufForAllDigitalLine);
BufForDigitalLine = "";
Serial.println();

}

void SetStatePins (){
  //Serial.println("Came to SetStatePins ");
  //Serial.println(readString);
  //Serial.println("String= " + readString);
//const char Zero = "0" ; //dla char z jednym znakiem nie mozna stosowac "" , a tylko  ''
    //patrz strona http://forum.arduino.cc/index.php?topic=126574.0
    const char Zero = '0' ;
    const char Jedn = '1' ;
    //for (int DigitalChannel = 0; DigitalChannel < NoOfDigitalPinsOnBoard; DigitalChannel++) {
     for (int DigitalChannel = 2; DigitalChannel < NoOfDigitalPinsOnBoard - 4 ; DigitalChannel++) {    

            //const int NoOfDigitalPinsOnBoard = 14;
            //from No pin =2 because pin 0 is used as RX . and pin 1 is used as TX 
            //pin 0 and 1 is used as RX and TX for USB serial   
            //14 - 4 = 10 mean  to pin 9 can set state pins
            //Pin 10,11,12 ,13 is used for SPI for ethernet shield 
         
      char c;
      c = readString[DigitalChannel+1];
      //c = readString[DigitalChannel];
      if (c == Zero )  {  
        digitalWrite(DigitalChannel, LOW); //Wyłączenie diody
      }
         
     if (c == Jedn)  {  
          digitalWrite(DigitalChannel, HIGH); //Włączenie diody
    }
     }

  //first PCF8574
  //Serial.println("Came to pcf8574 "); 
  for (int DigitalChannel = NoOfDigitalPinsOnBoard; DigitalChannel < (NoOfDigitalPinsOnBoard + NoOfDigitalPinsOnExtendedPCF857X) ; DigitalChannel++) 
 {
      char c;
      //c = readString[DigitalChannel];
      c = readString[DigitalChannel+1];
      if (c == Zero )  {  
         expander.digitalWrite(DigitalChannel - NoOfDigitalPinsOnBoard , LOW); //Switching  OFF led
     
       }
         
     if (c == Jedn)  {  
          expander.digitalWrite(DigitalChannel - NoOfDigitalPinsOnBoard , HIGH); //Switching  OFF led         
         }
        }
  
}

void ReadStringFromRemoteClient (EthernetClient cl ){  
     //Serial.println("Came to  ReadStringFromRemoteClient ");
      readString = cl.readStringUntil('\r');
      Serial.println(readString);    
}





