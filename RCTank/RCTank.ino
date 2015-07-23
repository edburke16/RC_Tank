#include <PS2X_lib.h>  //for v1.6
#include <Servo.h> 

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        6  //14    
#define PS2_CMD        5  //15
#define PS2_SEL        4  //16
#define PS2_CLK        7  //17

#define echoPin 2 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED

Servo Gun; 

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   true
//#define pressures   false
#define rumble      true
//#define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

void setup(){
 
  Gun.attach(10); 
  Gun.write(90); 
  
  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT);  //Initiates Brake Channel A pin
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
  
  Serial.begin(57600);
  Serial.print("Started");
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
//  Serial.print(ps2x.Analog(1), HEX);
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
	case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
   }
}

void loop() {
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 delay(10);
 distance = duration/58.2;
 //Serial.println(distance);
 delay(10);
 
 if(distance <=  15){; 
   digitalWrite(8,HIGH); 
   digitalWrite(9,HIGH); 
   //Serial.println("Close");} 
   
else{
   digitalWrite(8,LOW); 
   digitalWrite(9,LOW); 
   //Serial.println("Far");
 } 
   delay(10);
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  if(error == 1) //skip loop if no controller found
    return; 
  
  if(type == 2){ //Guitar Hero Controller
    ps2x.read_gamepad();          //read controller 
   
    if(ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if(ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if(ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if(ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if(ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed"); 

    if(ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");
    
    if(ps2x.Button(UP_STRUM))          //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if(ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");
 
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");
    
    if(ps2x.Button(ORANGE_FRET)) {     // print stick value IF TRUE
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC); 
    } 
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      //Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      //Serial.println("Select is being held");      

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      //Serial.print("UP held this hard: ");
      //Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      //Serial.print("RIGHT held this hard: ");
      //Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      //Serial.print("LEFT held this hard: ");
      //Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      //Serial.print("DOWN held this hard: ");
      //Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }   

    vibrate = ps2x.Analog(PSAB_TRIANGLE);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        //Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        //Serial.println("R3 pressed");
        
      if(ps2x.ButtonPressed(PSB_R2)) {
        //Serial.println("R2 pressed");
        digitalWrite(12, LOW);  //Establishes backward direction of Channel A
        digitalWrite(9, LOW);   //Disengage the Brake for Channel A
        analogWrite(3, 255);    //Spins the motor on Channel A at full speed
      }
      if(ps2x.ButtonReleased(PSB_R2)) {
        //Serial.println("R2 released");
        digitalWrite(9, HIGH); 
      }
      
      if(ps2x.ButtonPressed(PSB_L2)) {
        //Serial.println("L2 pressed"); 
          //Motor B forward @ full speed
          digitalWrite(13, LOW); //Establishes forward direction of Channel B
          digitalWrite(8, LOW);   //Disengage the Brake for Channel B
          analogWrite(11, 255);   //Spins the motor on Channel B at full speed
        }
      if(ps2x.ButtonReleased(PSB_L2)) {
        //Serial.println("L2 released"); 
        digitalWrite(8, HIGH); 
      }
//      if(ps2x.Button(PSB_TRIANGLE))
//        Serial.println("Triangle pressed");        
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      //Serial.println("Circle just pressed");
    //if(ps2x.ButtonPressed(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      //Serial.println("X just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      //Serial.println("Square just released");     

    if(ps2x.ButtonPressed(PSB_L1)) {
      //Serial.println("L1 pressed"); 
      digitalWrite(13, HIGH); //Establishes forward direction of Channel B
      digitalWrite(8, LOW);   //Disengage the Brake for Channel B
      analogWrite(11, 255);   //Spins the motor on Channel B at full speed
    }
    if(ps2x.ButtonReleased(PSB_L1)){
      //Serial.println("L1 released"); 
      digitalWrite(8, HIGH);
    }
    if(ps2x.ButtonPressed(PSB_R1)) {
      //Serial.println("R1 pressed"); 
      digitalWrite(12, HIGH); //Establishes forward direction of Channel B
      digitalWrite(9, LOW);   //Disengage the Brake for Channel B
      analogWrite(3, 255);   //Spins the motor on Channel B at full speed
    }
    if(ps2x.ButtonReleased(PSB_R1)){
      //Serial.println("R1 released"); 
      digitalWrite(9, HIGH);
    }
    if(ps2x.ButtonPressed(PSB_CROSS)){
      Gun.write(120); 
      //Serial.println("X Pressed"); 
    }
    if(ps2x.ButtonReleased(PSB_CROSS)){
      Gun.write(90);
    }
    if(ps2x.Analog(PSS_RY), DEC) {
      //Serial.print(ps2x.Analog(PSS_LY), DEC);
      //digitalWrite(12, LOW); //Establishes forward direction of Channel B
      //digitalWrite(9, LOW);   //Disengage the Brake for Channel B
      analogWrite(3, ((ps2x.Analog(PSS_RY))-127));
      //digitalWrite(13, LOW); //Establishes forward direction of Channel B
      //digitalWrite(8, LOW);   //Disengage the Brake for Channel B
      analogWrite(11, ((ps2x.Analog(PSS_LY))-127));
      if(((ps2x.Analog(PSS_LY))-127) <= 0) {
        analogWrite(13, LOW); 
      }
      if(((ps2x.Analog(PSS_LY))-127) >= 0) {
        digitalWrite(13, HIGH); 
      }
      if(((ps2x.Analog(PSS_RY))-127) <= 0) {
        digitalWrite(12, LOW);
      }
      if(((ps2x.Analog(PSS_RY))-127) >= 0) {
        digitalWrite(12, HIGH);
      }
    }
//      Serial.print("Stick Values:");
//      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_LX), DEC); 
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_RY), DEC); 
//      Serial.print(",");
//      Serial.println(ps2x.Analog(PSS_RX), DEC); 
    }
  delay(50);  
}
