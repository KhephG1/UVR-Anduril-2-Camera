void setup(void){
  //configure the serial monitor with the proper baud rate for the runcam
  Serial.begin(115200);
  Serial.print("monitor is working");
  Serial.print("Setup Complete");
}


void loop(void){
//Write a random byte to the default serial port
  Serial.write(0xA3);
 
}//loop   