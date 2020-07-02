bool setWaktu = false;
int jam = 11;                                                                                                                                                                                                                                                                                    
int menit = 29;
int detik = 30;
int tanggal = 30;
int bulan = 6;
int tahun = 2020;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <DHT.h>;
DHT dht(8, DHT22);
float hum;
float temp;
int switchDisplay = 0;

char h [5];
char t [5];

const long displayTime = 5000;
long displayLog;

// what's the name of the hardware serial port?
#include <SD.h> 
#include <SPI.h>
File myFile;
int pinCS  = 53;
int chk;

#include <DS3231.h>
DS3231  rtc(SDA, SCL);

//// Connect to the GPS on the hardware port
//  
#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial2);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

void setup() {
  Serial.begin(115200);
  rtc.begin();
  if (setWaktu){
  rtc.setDate(tanggal, bulan, tahun); //hapus
  rtc.setTime(jam, menit, detik);    
  }
  displayLog = displayTime;
   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

//  delay(1000);


//  myFile = SD.open("hasil.txt", FILE_WRITE);
//      if (myFile) {    
//        myFile.print("Tanggal, Jam, Humidity IN, Humdity OUT, Temperature IN, Temperature OUT");
//        myFile.println();
//        myFile.close(); // close the file
//            u8g.firstPage();  
//  do {
//    sdBerhasil();
//  } while( u8g.nextPage() );
//      }
//      // if the file didn't open, print an error:
//      else {
//            u8g.firstPage();  
//  do {
//    sdGagal();
//  } while( u8g.nextPage() );
//
//      }
      
  dht.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setCursor(0,0);    
  display.setTextSize(4);   
  display.setTextColor(SSD1306_WHITE); 
  display.println(" SRS");
  display.display();
  delay(2000);
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
}

void loop() {

    // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  dtostrf(hum, 3, 1, h);
  dtostrf(temp, 3, 1, t);

  // read the input on analog pin 0:
  int lux = analogRead(A0);
  unsigned long currentMillis = millis();
  if (currentMillis >= displayLog & switchDisplay != 2){
       displayLog = displayLog + displayTime;
       switchDisplay = switchDisplay + 1;
  } else if (currentMillis >= displayLog & switchDisplay == 2){
       displayLog = displayLog + displayTime;
       switchDisplay = 0;
  }
  if (switchDisplay == 0){
    display.clearDisplay();
    display.setTextSize(1);   
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.setCursor(0,0);      
    display.print(rtc.getTimeStr());
    display.print(" ");
    display.println(rtc.getDateStr());
    display.setTextSize(1);   
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.print("Temp:");
    display.print(temp);
    display.println("C ");
    display.print("Hum:");
    display.print(hum);
    display.println(" %RH");
    display.setTextSize(1);                             
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println("www.srs-ssms.com");
    display.display();
  }else if(switchDisplay == 1){
    display.clearDisplay();
    display.setCursor(0,0);                              // Start at top-left corner
    display.setTextSize(1);   
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.print(rtc.getTimeStr());
    display.print(" ");
    display.println(rtc.getDateStr());
    display.setTextSize(1);                              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.print("Lux: ");
    display.println(lux);
    display.print("GPS Fix:"); 
    display.print((int)GPS.fix);
    display.print(" | Qual:"); 
    display.println((int)GPS.fixquality);
    display.setTextSize(1);                             
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println("www.srs-ssms.com");
    display.display();
  }else if(switchDisplay == 2 && GPS.fix){
    display.clearDisplay();
    display.setCursor(0,0);                              // Start at top-left corner
    display.setTextSize(1);   
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println(rtc.getTimeStr());
    display.print(" ");
    display.print("Sat: ");
    display.print((int)GPS.satellites);
    display.setTextSize(1);                              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.print("Lat: ");
    display.print(GPS.latitude, 4); 
    display.println(GPS.lat);
    display.print("Lon: ");
    display.print(GPS.longitude, 4); 
    display.println(GPS.lon);
    display.setTextSize(1);                             
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println("www.srs-ssms.com");
    display.display();
  }else if(switchDisplay == 2 && !GPS.fix){
    display.clearDisplay();
    display.setCursor(0,0);                              // Start at top-left corner
    display.setTextSize(1);   
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println("GPS TIDAK DITEMUKAN!");
    display.setTextSize(1);                             
    display.setTextColor(SSD1306_WHITE);                 // Draw white text
    display.println("www.srs-ssms.com");
    display.display();
  }
  Serial.print("millis: ");
  Serial.println(currentMillis);
  Serial.print("log: ");
  Serial.println(displayLog);
  delay(1000);
}
