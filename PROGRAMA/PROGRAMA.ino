
#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h> 
#include <WiFiClient.h>
bool errorconexion=false;
WiFiClient client;
MySQL_Connection conn((Client *)&client);
IPAddress server_addr(192, 168 , 128, 37);          // IP del servidor MySQL
char user[] = "super";           
char passwd[] = "12345";      
char BASE_SQL[] = "INSERT INTO datos.temp_agua (content) VALUES (%f)";
char BASE_SQL2[] = "INSERT INTO datos.humedad_suel (content) VALUES (%d)";
char BASE_SQL3[] = "INSERT INTO datos.nivel_agua (content) VALUES (%d)";
char BASE_SQL4[] = "INSERT INTO datos.estado_bomba (content) VALUES (%d)";
char consulta[128];
//const char* ssid  = "CGA2121_yFeVW46";
const char* ssid = "Leyva";
const char* password = "leyva123";
//const char* password  = "3ZtzQcxMDMmNCs4WDQ";
uint8_t newMACAddress[] = {0xa8, 0xdb, 0x03, 0x8e, 0x86, 0x74};
#define trigPin 13
#define echoPin 15
#include "DHT.h"
#define DHTTYPE DHT11
#define dhPin 12
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(16, 5, 4, 0, 2, 14);
DHT dht(dhPin, DHTTYPE);
//const int oneWireBus = 16;  
const float a=0;
const float dt=0;
const int hm =0;
const int ds = 0;
//OneWire oneWire(oneWireBus);
//DallasTemperature sensors(&oneWire);
int numero =0;
void setup() {
  dht.begin();
  Serial.begin(115200);
  delay(10);
   lcd.begin(20, 4);
  //sensors.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(13, OUTPUT);
  WiFi.mode(WIFI_STA);
  Serial.print("[OLD] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // For Soft Access Point (AP) Mode
  //wifi_set_macaddr(SOFTAP_IF, &newMACAddress[0]);
  // For Station Mode
  wifi_set_macaddr(STATION_IF, &newMACAddress[0]);
  Serial.print("[NEW] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  WiFi.begin(ssid, password);
  Serial.println("Conectad¿ndo a :\t");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED){
      delay(200);
      Serial.print('.');
    }
    Serial.println();
    Serial.print("Conectado a:\t");
    Serial.println(WiFi.SSID());
    Serial.print("IP ADDRES:\t");
    Serial.println(WiFi.localIP());
     Serial.println("Conectando a base de datos");
int intentos=0;
    while ((conn.connect(server_addr, 3306, user, passwd) != true)&& (intentos++ < 3)) {  //Realiza varios intentos de conexión a MySQL
      delay(200);
      Serial.print ( "." );
    }
    if (intentos>=4){
    Serial.println("Error al conectar al servidor");
    errorconexion=true;
     }else {
      Serial.println("");
      Serial.println("Conectado al servidor SQL!");  
      }
    }  

void loop() {
  if (!errorconexion){
  float dt = Tempe();
 // float  a =Agua();
  int hm= Humedad();
  int ds = dista();
  int numero = validar(dt, hm, ds);
   Serial.println(numero);
   Serial.println("Guardando datos");
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);  // Inicializa el cursor para la consulta SQL
    sprintf(consulta,BASE_SQL,dt); // Completa la trama de la consulta SQL
    Serial.println(consulta);
    cur_mem->execute(consulta);  // Ejecuta la consulta SQL y escribe el valor de los milisegundos
    sprintf(consulta,BASE_SQL2,hm); // Completa la trama de la consulta SQL
    Serial.println(consulta);
    cur_mem->execute(consulta);  // Ejecuta la consulta SQL y escribe el valor de los milisegundos
    sprintf(consulta,BASE_SQL3,ds); // Completa la trama de la consulta SQL
    Serial.println(consulta);
    cur_mem->execute(consulta);  // Ejecuta la consulta SQL y escribe el valor de los milisegundos
    sprintf(consulta,BASE_SQL4,numero); // Completa la trama de la consulta SQL
    Serial.println(consulta);
    cur_mem->execute(consulta);  // Ejecuta la consulta SQL y escribe el valor de los milisegundos
    delete cur_mem;// Al no haber resultados, elimina el cursor y libera memoria 
   lcd.clear();
   delay(15000);
  }
}
int dista(){
int duration, distance;
digitalWrite(trigPin, HIGH);
delayMicroseconds(1000);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
 distance = (duration/2)/29.1;
  const int dat = distance;
   if (distance >= 200 || distance <= 0){
    Serial.print(distance);
    Serial.print(",");
    lcd.setCursor(0,0);
   lcd.print("FUERA DE RANGO");
   delay(5000);
    lcd.clear();
    delay(3000);
 
  }else{
    Serial.print(distance);
    Serial.print(",");
  lcd.setCursor(0,0);
  lcd.print("DISTANCIA");
  lcd.setCursor(0,1);
  lcd.print(distance);
  delay(5000);
  lcd.clear();
  delay(3000);
    }
    return dat;
  }
float Tempe(){
  float temp = dht.readTemperature();
  delay(5000);
  Serial.print(temp);
  Serial.print(",");
  lcd.setCursor(0,0);
  lcd.print("TEMPERATURA");
  lcd.setCursor(0,1);
  lcd.print(temp);
  delay(5000);
  lcd.clear();
  delay(1000);
  const float dtemp = temp;
  return dtemp;
  }
int Humedad(){
   int humedad=analogRead(A0);
   const int al = humedad;
  Serial.print(humedad);
  Serial.print(",");
  if(humedad >=1000){
   lcd.setCursor(0,0);
   lcd.print("SENSOR EN SUELO");
   lcd.setCursor(0,1);
   lcd.print(humedad);
   delay(5000);
   lcd.clear();
    }
    else if(humedad <1000 && humedad >= 600){
   lcd.setCursor(0,0);
   lcd.print("SUELO SECO");
   lcd.setCursor(0,1);
   lcd.print(humedad);
   delay(5000);
   lcd.clear();
   delay(1000);
      }
      else if (humedad < 600 && humedad >= 370){
           lcd.setCursor(0,0);
          lcd.print("SENSOR HUMEDO");
         lcd.setCursor(0,1);
          lcd.print(humedad);
            delay(5000);
          lcd.clear();
        }
        else if (humedad < 370){
               lcd.setCursor(0,0);
               lcd.print("SENSOR EN SUELO");
                lcd.setCursor(0,1);
               lcd.print(humedad);
                 delay(5000);
                lcd.clear();
                delay(1000);
          }
          return al;
          delay(1000);
          
  }
 /* float Agua(){
  sensors.requestTemperatures(); 
   float  temperatureC = sensors.getTempCByIndex(0);
   Serial.print(temperatureC);
   Serial.print(",");
   lcd.setCursor(0,0);
   lcd.print("TEMP AGUA");
   lcd.setCursor(0,1);
   lcd.print(temperatureC);
  delay(5000);
  lcd.clear();
  delay(1000);
  const float data = temperatureC;
  return data;
  }*/ 
  int validar(float dt, int hm, int ds){
    int Bom = 0;
        if(dt > 25){
          if(hm >= 600 && hm <= 1024){
            if(ds <=13 && ds > 0){
                 Bom = Bom+1;
                //Activo
                Serial.print(Bom);
                digitalWrite(13, HIGH);
                 lcd.setCursor(0,1);
                 lcd.print("Bomba Funcio");
                  delay(2000);
                  digitalWrite(13, LOW);
                  Serial.println("");
                  return Bom; 
              }
              else{
                // No Activo
                 lcd.setCursor(0,1);
                 lcd.print("B no funcio");
                Serial.println(Bom);
                return Bom; 
                }
            }
            else{
                 lcd.setCursor(0,1);
                 lcd.print("B no funcio");
              Serial.println(Bom);
              return Bom; 
              }
          }
          else{
             // No Activo
                lcd.setCursor(0,1);
                 lcd.print("B no funcio");
                Serial.println(Bom);
                return Bom; 
            }
   
    }
