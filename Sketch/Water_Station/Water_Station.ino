//Define the device ID
const char myID[5] = "012"; 


//Define the type of device
const char DeviceType = 'W';


const int nbChannel=6;
const int msgLength = 14;
const int interval = 100;
const int sendPin = 2;

const int powerLED = 13;
const int valve[nbChannel] = { 8 , 9 , 10 , 11 , 12 };
 

char macAddr[8];
char receiver[8];
char sender[8];
int channel;
char action;
char data[8];
char dataout[8];
char inStr[msgLength+1];

int i=0;

void setup() {
  //Assign the Power ON LED and switch it on
  pinMode(powerLED, OUTPUT);
  digitalWrite (powerLED, HIGH);  
  
  pinMode(sendPin, OUTPUT);
  digitalWrite (sendPin, LOW);  

  //Declare all channels and assign them at default status
  for(byte j=0; j<nbChannel; j++){
    pinMode(valve[j], OUTPUT);
    digitalWrite (valve[j], 0);
  }


  //Create the Mac Address of the device
  macAddr[0]=DeviceType;
  for(byte j=0; j<3; j++){
     macAddr[j+1]=myID[j];
  }

  
  //Initialize Serial
  Serial.begin(38400); // opens serial port, sets data rate to 9600 bps
  Serial.println(macAddr);
      
}

void loop() {
  
 if (Serial.available()){
    inStr[i] = Serial.read();
    
    //Serial.println(inStr);
    if (inStr[i]=='Z'){ //reset with the input of Z
        resetVariables();  
    } else {
      i++;
      if(i>=msgLength){
        //Serial.println("dealing with incoming message");
        //Serial.println(inStr);
         for(byte k=0; k<4; k++){
          receiver[k]=inStr[k];
          sender[k]=inStr[k+4];
          data[k]=inStr[k+10];
          
        }
        channel = inStr[8] - '0';
        action=inStr[9];
        
        
        if(strcmp(receiver, macAddr)==0){
          //Serial.println("yeah!!!! ");
          switch(action){
            case 'Q':    //Query status
              responseController();
              break;
            case 'A':    //Open valve
              digitalWrite(valve[channel],1);
              responseController();
              break;
            case 'B':    //Close valve
              digitalWrite(valve[channel],0);
              responseController();
              break;
            case 'T':    //Toggle valve
              digitalWrite(valve[channel],!digitalRead(valve[channel]));
              responseController();
              break;
            case 'C':    //Open for XX seconds
              digitalWrite(valve[channel],1);
              responseController();
              delay(atoi(data)*interval); 
              digitalWrite(valve[channel],0);
              responseController();
              break;
            case 'D':    //Close for  for XX seconds
              digitalWrite(valve[channel],0);
              responseController();
              delay(atoi(data)*interval); 
              digitalWrite(valve[channel],1);
              responseController();
              break; 
            default: 
              Serial.println("Unknown Command");   //all others
              break; 
          } 
        } else {
          Serial.println("Not Me!"); //Nothing happens in the case the macAddr is not the right one
         }     

        //put everything back to 0 for next message
        resetVariables();
      }
    }
}   
}

void resetVariables(){
  //digitalWrite (2, HIGH);
  //Serial.println("resetting");
  //digitalWrite (2, LOW);
  i=0;
  for(int j=0; j<msgLength; j++){
    inStr[j]=0;
  }
  channel=0;
  action=0;
  for(byte j=0; j<4; j++){
    receiver[j]=0;
    sender[j]=0;
    data[j]=0;
  }
}


void responseController(){
  char msgOut[msgLength+1]="00000000000000";
  for(byte k=0; k<4; k++){
    msgOut[k]=sender[k];
    msgOut[k+4]=macAddr[k];
  }
  msgOut[8]=inStr[8];
  msgOut[9]='R';
  for(byte k=0; k<4; k++){
    msgOut[k+10]=digitalRead(valve[channel]) + '0';
  }
  digitalWrite (sendPin, HIGH);
  
  delay(10);
  
  String msgSent = "";
  
  for(int k=0; k<msgLength+1; k++){
    msgSent += String(msgOut[k]);
  } 
  Serial.println(msgSent);
  delay(10);
  digitalWrite (sendPin, LOW);
  
}

