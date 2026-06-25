#include <Wire.h>

#define MY_ADDR         0x68

#define CMD_RECEIVE     0x67
#define CMD_SEND        0x69
#define CMD_SEND_LEN    0x66

uint8_t command = 0x00; // Receive Mode
uint8_t fetch_cmd = true;

int LED = 13;
char rx_buffer[64];
uint32_t cnt =0;
char message[50];

char* message_to_send = "message sent from arduino";

void setup() {

  Serial.begin(9600);
  
  // Start the I2C Bus as Slave on address 0X68
  Wire.begin(MY_ADDR); 
  
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  sprintf(message,"Slave is ready : Address 0x%x",MY_ADDR);
  Serial.println((char*)message );  
  Serial.println("Waiting for data from master");  
}

void loop(void)
{
}

void receiveEvent(int bytes) 
{
  
  if (fetch_cmd)
  {
    // fetching command, perform one read then toss the rest of the data
    command = (uint8_t) Wire.read();
    fetch_cmd = false;

    while( Wire.available() )
    {
      Wire.read();
    }
  } else 
  {
    // currently have command, next data will fetch new command
    fetch_cmd = 1;
    if (command == CMD_RECEIVE)
    {
      // Receiving data
      while( Wire.available() )
      {
        rx_buffer[cnt++] = Wire.read();
      }

      rx_buffer[cnt] = '\0';
      
      Serial.print("Received:");  
      Serial.println((char*)rx_buffer); 

      cnt=0;
    } else 
    {
      // not a rx command, toss out data
      while( Wire.available() )
      {
        Wire.read();
      }
    }
  }
}

void requestEvent()
{
  if (command == CMD_SEND)
  {
    Wire.write(message_to_send);

  } else if (command == CMD_SEND_LEN)
  {
    Wire.write((uint8_t) strlen(message_to_send));

  }
  fetch_cmd = true;
}