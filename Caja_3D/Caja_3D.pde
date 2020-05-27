/*
    Arduino y MPU6050 IMU - Ejemplo de visualización 3D 
*/

import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String data="";
float roll, pitch,yaw;

void setup() {
  fullScreen(P3D);
  //size (2560, 1440, P3D);
  myPort = new Serial(this, "COM3", 19200); // Inicia la comunicación serial
  myPort.bufferUntil('\n');
}

void draw() {
  translate(width/2, height/2, 0);
  background(0);
  textSize(22);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch) + "      Yaw: " + int(yaw), -100, 265);
  
  // Girar el objeto
  rotateX(radians(-pitch));
  rotateZ(radians(roll));
  rotateY(radians(yaw));
  
  // Objeto 3D
  textSize(30);  
  fill(0, 76, 153);
  box (700, 40, 200); // Draw box
  textSize(25);
  fill(255, 255, 255);
  text("Proyectos Profesionales", -183, 10, 101);
  
  //delay(10);
  //println("ypr:\t" + angleX + "\t" + angleY); 
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
    }
  }
}
