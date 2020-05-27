/*
   Arduino and MPU6050 Accelerometer and Gyroscope Sensor Tutorial
   by Dejan, https://howtomechatronics.com
*/
#include <Wire.h>
#include <avr/wdt.h>

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

void setup() {

  wdt_disable();
  
  Serial.begin(19200);
  Wire.begin();                      // Inicializa la comunicación
  Wire.beginTransmission(MPU);       // Inicia transmisión con MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Habla al registro 6B
  Wire.write(0x00);                  // Crea el reset - pone un 0 en el registro 6B
  Wire.endTransmission(true);        //termina la transmisión

  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
 

  wdt_enable(WDTO_2S);
  
  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
  
}
void loop() {

  wdt_reset();
  // === Lee los datos del acelerómetro === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Comienza con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Lee 6 registros en total, cada valor de eje se almacena en 2 registros
  //Para un rango de +-2g, necesitamos dividir los valores brutos entre 16384, de acuerdo con la hoja de datos
  AccX = (Wire.read() << 8 | Wire.read()) / 16384; // valor del eje X
  AccY = (Wire.read() << 8 | Wire.read()) / 16384; // valor del eje Y
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384; // valor del eje Z
  
  // Calculanado Roll y Pitch a partir de los datos del acelerómetro
  
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX Se corrigen las salidas con los calores de error calculados por la función
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) -AccErrorY; // AccErrorY
  
  // === Lee los datos del giroscopio === //
  previousTime = currentTime;        //  El tiempo anterior se almacena antes del tiempo real leído
  currentTime = millis();            //  Hora actual hora real leída
  elapsedTime = (currentTime - previousTime) / 1000; // Divide entre 1000 para obtener segundos
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Dirección de primer registro de datos de giroscopio 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Leer 4 registros en total, cada valor de eje se almacena en 2 registros
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // Para un rango de 250 grados/s tenemos que dividir primero el valor bruto por 131.0, de acuerdo con la hoja de datos
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  
  // Se corrigen las salidas con los valores de error calculados por la función
  GyroX = GyroX - GyroErrorX; // GyroErrorX 
  GyroY = GyroY - GyroErrorY; // GyroErrorY 
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ 
  
  // Actualmente, los valores brutos están en grados por segundo, grados / s, 
  // por lo que debemos multiplicar por sendonds (s) para obtener el ángulo en grados
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  
  // Filtro complementario: combina los valores del acelerómetro y el ángulo giroscópico
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  roll = -roll;
  
  // Imprime los valores de error en el monitor serie
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);
  
 
}


void calculate_IMU_error() {
  // Podemos llamar a esta función en la sección de setup para calcular el error de datos del acelerómetro y giroscopio. 
  // Desde aquí obtendremos los valores de error utilizados en las ecuaciones anteriores
  // Tener en cuenta que debemos colocar la IMU plana para obtener los valores adecuados, de modo que podamos obtener los valores correctos
  
  // Lee los valores del acelerómetro 200 veces
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    
    // Suma todas las lecturas
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  
  // Divide la suma entre 200 para obtener el valor de error
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  
  // Lee valores de giroscopio 200 veces
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Suma todas las lecturas
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  // Divide la suma entre 200 para obtener el valor de error
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Imprime los valores de error en el monitor serie
//  Serial.print("AccErrorX: ");
//  Serial.println(AccErrorX);
//  Serial.print("AccErrorY: ");
//  Serial.println(AccErrorY);
//  Serial.print("GyroErrorX: ");
//  Serial.println(GyroErrorX);
//  Serial.print("GyroErrorY: ");
//  Serial.println(GyroErrorY);
//  Serial.print("GyroErrorZ: ");
//  Serial.println(GyroErrorZ);
}
