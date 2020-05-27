/*
    Arduino y MPU6050 IMU - Ejemplo de visualización 3D 
*/

import guru.ttslib.*;
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String data="";
float roll, pitch,yaw;
int centradoYaw = 0, centradoPitch = 0, centradoRoll = 0;
TTS tts;

void setup() {
  fullScreen(P3D);
  //size (2560, 1440, P3D);
  myPort = new Serial(this, "COM3", 19200); // Inicia la comunicación serial
  myPort.bufferUntil('\n');
  tts = new TTS();
 
}


void draw() {
  translate(width/2, height/2, 0);
  background(0);
  textSize(22);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch) + "      Yaw: " + int(yaw), -100, 265);
}

// Lee los datos del puerto serial
void serialEvent (Serial myPort) { 
  // lee los datos del puerto serial hasta el carácter '.' y lo pone en la variable de cadena "data".
  data = myPort.readStringUntil('\n');
  
  // si tienes otros bytes que no sean el salto de línea:
  if (data != null) {
    data = trim(data);
    // divide la cadena en "/"
    String items[] = split(data, '/');
    if (items.length > 1) {
      
      //--- Roll,Pitch & Yaw en grados
      roll = float(items[0]);
      pitch = float(items[1]);
      yaw = float(items[2]);
      
      
     
      
      if(yaw == 0){
        centradoYaw = 1;
      }
      
      if(centradoYaw == 1){
        float giroYaw = 0 + yaw;
        
        if(giroYaw > 0){
          if(abs(giroYaw) > 15){
            //Giro a la izquierda
            tts.speak("Body turns left");
            centradoYaw = 0;
          }
        }else if(giroYaw < 0){
          if(abs(giroYaw) > 15){
            //Giro a la derecha
            tts.speak("Body turns right");
            centradoYaw = 0;
          }
        }
      }
      
      
      if(pitch == 0){
        centradoPitch = 1;
      }
      
      if(centradoPitch == 1){
        float giroPitch = 0 + pitch;
        
        if(giroPitch > 0){
          if(abs(giroPitch) > 15){
            //Giro hacia arriba
            tts.speak("Body turns up");
            centradoPitch = 0;
          }
        }else if(giroPitch < 0){
          if(abs(giroPitch) > 15){
            //Giro hacia abajo
            tts.speak("Body turns down");
            centradoPitch = 0;
          }
        }
      }
      
      
      if(roll == 0){
        centradoRoll = 1;
      }
      
      if(centradoRoll == 1){
        float giroRoll = 0 + roll;
        
        if(giroRoll > 0){
          if(abs(giroRoll) > 15){
            //Inclinación a la derecha
            tts.speak("Body tilts right");
            centradoRoll = 0;
          }
        }else if(giroRoll < 0){
          if(abs(giroRoll) > 15){
            //Inclinación a la izquierda
            tts.speak("Body tilts left");
            centradoRoll = 0;
          }
        }
      }
      
    }
  }
}

void keyPressed() {
  
  exit();
}
