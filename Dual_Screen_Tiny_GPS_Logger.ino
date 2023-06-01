
#include <U8g2lib.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>



static const uint32_t GPSBaud = 9600;
unsigned long previousMillis = 0;
const long interval = 1000; //1 second between logging GPS data to the SD card
Sd2Card card;
SdVolume volume;
SdFile root;
File dataFile;

const int chipSelect = 0; //here I'm using pin 0 for the sd card data line

TinyGPSPlus gps;



U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED_2(U8G2_R0, 1, 2, /* reset=*/ U8X8_PIN_NONE);


// setup the terminal (U8G2LOG) and connect to u8g2 for automatic refresh of the display
// The size (width * height) depends on the selected font and the display
// assume 4x6 font
/*#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 4
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT*10];
U8G2LOG u8g2log;
*/
int gpschar; 

 double lat_val, lng_val, alt_m_val, spd_knts, crs_deg, spd_mps; 
 double prev_lat_val, prev_lng_val,distanceKM;
 double totaldistanceKM = 0;
     //uint32_t date_val;
     uint16_t yr_val;
     uint8_t hr_val, min_val, sec_val, hr_val_jp, sats_val, mnt_val, day_val;
     bool loc_valid, alt_valid, time_valid, sats_valid;

void setup()
{
  Serial.begin(GPSBaud); //uncomment to debug
  Serial1.begin(GPSBaud); //Don't comment this out - it connects the gps data to the QtPy
  

  u8g2.begin();  
  OLED_2.begin();
 /* u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer); //to display raw gps data on the screen
  u8g2log.setLineHeightOffset(1); // set extra space between lines in pixel, this can be negative
  u8g2log.setRedrawMode(0);   // 0: Update screen with newline, 1: Update screen for every char  
*/
  Serial.print("Initializing SD card..."); //for the debug
  if (!SD.begin(0)) {
      Serial.println("initialization failed!");
      while (1);
    }
  Serial.println("initialization done.");
  
}

void loop()
{

  u8g2.firstPage();
  //OLED_2.firstPage();
  do {
    u8g2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
    
   

    u8g2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders
    u8g2.drawLine(0, 9, 128,9);
 //   u8g2.drawLine(0, 28, 128,28);
    u8g2.drawStr(3, 7, "TINY GPS LOG");
    
    u8g2.drawLine(0, 54, 128,54);
    u8g2.drawStr(3, 61, "OKUBO HEAVY INDUSTRIES");

 

    //Read GPS Data
    
     while (Serial1.available()>0)  /* Encode data read from GPS while data is available on serial port */
     {  gpschar = Serial1.read(); //read raw gps data to gpschar
        //Serial.write(gpschar);  // uncomment to send raw gps over Serial to debug
       // u8g2log.write(gpschar);   // write raw gps data to u8g2log buffer
        gps.encode(gpschar);      // extract useful info from raw gps data
      }
     
     //u8g2.drawLog(3, 35, u8g2log);     // uncomment to draw the raw gps buffer content in the console
     
   //  double lat_val, lng_val, alt_m_val, spd_knts, crs_deg, spd_mps; 
     //uint32_t date_val;
   //  uint16_t yr_val;
   //  uint8_t hr_val, min_val, sec_val, hr_val_jp, sats_val, mnt_val, day_val;
   //  bool loc_valid, alt_valid, time_valid, sats_valid;
     lat_val = gps.location.lat();  /* Get latitude data */
     loc_valid = gps.location.isValid(); /* Check if valid location data is available */
     lng_val = gps.location.lng(); /* Get longtitude data */
     alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
     alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
     hr_val = gps.time.hour(); /* Get hour */
     min_val = gps.time.minute();  /* Get minutes */
     sec_val = gps.time.second();  /* Get seconds */
     time_valid = gps.time.isValid();  /* Check if valid time data is available */
     sats_valid = gps.satellites.isValid();
     sats_val = gps.satellites.value();
     spd_knts = gps.speed.knots();
     spd_mps = gps.speed.mps();
     crs_deg = gps.course.deg();
     //date_val = gps.date.value();
     yr_val = gps.date.year();
     mnt_val = gps.date.month();
     day_val = gps.date.day();
     // speed.value, speed.knots, speed.mph, speed.mps, speed.kmph are all possible speed formats
     
     if (hr_val < 15) { hr_val_jp = hr_val + 9;  // convert UTC to Japan time zone - change according to your time zone
     //N.B. if you add gps.date.value it will be based on UTC time zone date
     }
     else {hr_val_jp = hr_val -15;
     }

     if (loc_valid){
      distanceKM = TinyGPSPlus::distanceBetween(
      lat_val,
      lng_val,
      prev_lat_val,
      prev_lng_val) / 1000.0;
      if (distanceKM<0.5){
        totaldistanceKM = (totaldistanceKM + distanceKM);
      }
     }
     
   /*  if (!loc_valid)
      {          
      
        u8g2.drawStr(3, 16, "LAT : ********");
        
        u8g2.drawStr(60, 16, "LON : ********");
        
       }
       else
       {
          u8g2.drawStr(3, 16, "LAT :");
          u8g2.setCursor(22, 16);
          u8g2.println(lat_val, 6);
          //Serial.print("Lat = ");
          //Serial.print(lat_val,6);
          //Serial.print(" ");
          
          u8g2.drawStr(60, 16, "LON :");
          u8g2.setCursor(79, 16);
          u8g2.println(lng_val, 6);
          //Serial.print("Lon = ");
          //Serial.print(lng_val,6);
          //Serial.print(" ");
          
        }
        if (!alt_valid)
        {
          
          u8g2.drawStr(3, 24, "ALT : ********");
        }
        else
        {
           
          u8g2.drawStr(3, 24, "ALT :");
          u8g2.setCursor(22, 24);
          u8g2.println(alt_m_val, 2); 
          //Serial.print("Alt = ");
          //Serial.print(alt_m_val, 2);
          //Serial.print(" ");
        }
        
        if (!sats_valid)
        {
          
          u8g2.drawStr(60, 24, "SAT : **");
        }
        else
        {
         
          u8g2.drawStr(60, 24, "SAT :");
          u8g2.setCursor(79, 24);
          u8g2.println(sats_val, 1);   
        }
        */
        if (!time_valid)
        {
          u8g2.drawStr(65, 7, "Time : ********");
         
        }
        else
        {
          char time_string[32];
          //sprintf(time_string, "Time:%02d:%02d:%02d", hr_val_jp, min_val, sec_val);
          sprintf(time_string, "%02d:%02d:%02d", hr_val_jp, min_val, sec_val);
          u8g2.drawStr(65, 7, "Time :");
          u8g2.setCursor(85, 7);
          u8g2.print(time_string); 

          char date_string[32];
          sprintf(date_string, "%02d%02d%02d.txt", yr_val, mnt_val, day_val);
          
          char date_stringx[32];
          sprintf(date_stringx, "%02d-%02d-%02dT", yr_val, mnt_val, day_val);
          //code below is to log data to SD card once every interval
          char time_stringx[32];
          sprintf(time_stringx, "%02d:%02d:%02dZ", hr_val, min_val, sec_val);
          
          unsigned long currentMillis = millis();
          if (currentMillis - previousMillis >= interval) {

          if (SD.exists(date_string)) {
            
          dataFile = SD.open(date_string, FILE_WRITE); //open file on SD card
/*
         // dataFile.print("      <when>");
          dataFile.print(date_stringx);
          dataFile.print(time_stringx);
          //dataFile.print("</when>\n");
         // dataFile.print("      <gx:coord>");
          dataFile.print(lat_val, 6);
          //dataFile.print(" ");
          dataFile.print(lng_val, 6);
         // dataFile.print(" ");
          dataFile.print(alt_m_val, 2);
         // dataFile.print("</gx:coord>\n");
        */  

      if (loc_valid){
      distanceKM = TinyGPSPlus::distanceBetween(
      lat_val,
      lng_val,
      prev_lat_val,
      prev_lng_val) / 1000.0;
      if (distanceKM<0.5){
        totaldistanceKM = (totaldistanceKM + distanceKM);
      }
     }
          
          //dataFile.print(date_val);  //write comma delimited data to SD card file
         // dataFile.print(",");
          dataFile.print(time_string);  
          dataFile.print(",");
          dataFile.print(lat_val, 6);
          dataFile.print(",");
          dataFile.print(lng_val, 6);
          dataFile.print(",");
          dataFile.print(alt_m_val, 2);
          dataFile.print(",");
          dataFile.print(spd_knts, 2);
          dataFile.print(","); 
          dataFile.print(spd_mps, 2);
          dataFile.print(","); 
          dataFile.print(crs_deg, 2);
          dataFile.print(",");
          dataFile.print(distanceKM, 6);
          dataFile.print(",");
          dataFile.print(totaldistanceKM, 2);
          dataFile.print(",");
          dataFile.print(date_stringx);
          dataFile.print(time_stringx);
          prev_lat_val = lat_val; 
          prev_lng_val = lng_val;
          
          dataFile.print("\n"); //add line break at end of each round of gps data
          //*/

          dataFile.close(); //close SD card file
          } else {
            dataFile = SD.open(date_string, FILE_WRITE); //create new file
            //dataFile.print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n<Folder>\n\  <Placemark>\n    <gx:Track>\n");
            dataFile.print("Time,Latitude,Longitude,Altitude,Speed(knots),Speed(m/s),Heading,distance,total distance,When\n");
            dataFile.close(); //close SD card file
          }
          previousMillis = currentMillis;
          }
          
          //Serial.print(time_string); 
        }/*
u8g2.setFont(u8g2_font_6x12_tr);
        if (!sats_valid)
        {
          
          u8g2.drawStr(3, 39, "SPEED : **");
        }
        else
        {
         
          //comment out the next 3 lines if you want to diplay raw gps data in the console
          u8g2.drawStr(3, 39, "SPEED(KNTS):");
          u8g2.setCursor(85, 39);
          u8g2.println(spd_knts, 2); 
          //Serial.print("Spd knts= ");
          //Serial.print(spd_knts, 2);
          //Serial.print(" ");
          //Serial.print("Spd mps= ");
          //Serial.print(spd_mps, 2);
          //Serial.print(" ");
          
        }

        if (!sats_valid)
        {
          
          u8g2.drawStr(3, 51, "HEADING : **");
        }
        else
        {
         
          //comment out the next 3 lines if you want to diplay raw gps data in the console
          u8g2.drawStr(3, 51, "HEADING :");
          u8g2.setCursor(85, 51);
          u8g2.println(crs_deg, 2); 
          //Serial.print("Crs = ");
          //Serial.print(crs_deg, 2);
          //Serial.print("  \n");
        }    */

        u8g2.setFont(u8g2_font_logisoso32_tf);
        
        if (!sats_valid)
        {
          
          u8g2.drawStr(6, 48, "**");
        }
        else
        {
         
          //comment out the next 3 lines if you want to diplay raw gps data in the console
          //u8g2.drawStr(3, 39, "SPEED(KNTS):");
          u8g2.setCursor(6, 48);
          u8g2.println(spd_knts, 1);
          u8g2.setFont(u8g2_font_logisoso16_tf);
          u8g2.print(" knots");
          u8g2.setFont(u8g2_font_t0_13b_tf);
          u8g2.setCursor(68, 26);
          u8g2.println(totaldistanceKM, 1);
          u8g2.print(" km");
          Serial.print("Spd knots= ");
          Serial.print(spd_knts, 2);
          Serial.print(" ");
          Serial.print("distance= ");
          Serial.print(distanceKM, 6);
          Serial.print(" ");
          Serial.print("total distance= ");
          Serial.print(totaldistanceKM, 2);
          Serial.print(" ");
          
          //Serial.print("  \n");
          //Serial.print("Spd mps= ");
          //Serial.print(spd_mps, 2);
          //Serial.print(" ");
          
        }
} while ( u8g2.nextPage() );

    OLED_2.clearBuffer();
    OLED_2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
   OLED_2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders
    OLED_2.drawLine(0, 9, 128,9);
  //  OLED_2.drawLine(0, 28, 128,28);
    //OLED_2.drawStr(2, 7, "TINY GPS LOG Screen 2");
    
    OLED_2.drawLine(0, 54, 128,54);
    //OLED_2.drawStr(2, 61, "OKUBO HEAVY INDUSTRIES");


     if (!loc_valid)
      {          
      
        OLED_2.drawStr(3, 7, "LAT : ********");
        
        OLED_2.drawStr(60, 7, "LON : ********");
        
       }
       else
       {
          OLED_2.drawStr(3, 7, "LAT :");
          OLED_2.setCursor(22, 7);
          OLED_2.println(lat_val, 6);
          //Serial.print("Lat = ");
          //Serial.print(lat_val,6);
          //Serial.print(" ");
          
          OLED_2.drawStr(60, 7, "LON :");
          OLED_2.setCursor(79, 7);
          OLED_2.println(lng_val, 6);
          //Serial.print("Lon = ");
          //Serial.print(lng_val,6);
          //Serial.print(" ");
          
        }
        if (!alt_valid)
        {
          
          OLED_2.drawStr(3, 61, "ALT : ********");
        }
        else
        {
           
          OLED_2.drawStr(3, 61, "ALT :");
          OLED_2.setCursor(22, 61);
          OLED_2.println(alt_m_val, 2); 
          //Serial.print("Alt = ");
          //Serial.print(alt_m_val, 2);
          //Serial.print(" ");
        }
        
        if (!sats_valid)
        {
          
          OLED_2.drawStr(60, 61, "GPS SAT LOCK : **");
        }
        else
        {
         
          OLED_2.drawStr(60, 61, "GPS SAT LOCK :");
          OLED_2.setCursor(117, 61);
          OLED_2.println(sats_val, 1);   
        }
    
    
    OLED_2.setFont(u8g2_font_logisoso32_tf);
        if (!sats_valid)
        {
          
          OLED_2.drawStr(6, 48, "**");
        }
        else
        {
         
          //comment out the next 3 lines if you want to diplay raw gps data in the console
         // u8g2.drawStr(3, 51, "HEADING :");
          OLED_2.setCursor(6, 48);
          OLED_2.println(crs_deg, 0);
        //  int w = u8g2.getStrWidth(crs_deg);
          if ((crs_deg <= 9.5)){
            OLED_2.drawGlyph(30,48,176);
          }
          else if (crs_deg <= 99.5){
            OLED_2.drawGlyph(50,48,176);
          }
          else {
            OLED_2.drawGlyph(70,48,176);
          }
          Serial.print("Crs = ");
          Serial.print(crs_deg, 2);
          Serial.print("  \n");

          OLED_2.setFont(u8g2_font_logisoso16_tf);
          if ((crs_deg >= 348.75) || (crs_deg <= 11.25)){
            OLED_2.drawStr(90, 40, "N");
          }
          if ((crs_deg > 11.25) && (crs_deg <= 33.75)){
            OLED_2.drawStr(90, 40, "NNW");
          }
          if ((crs_deg > 33.75) && (crs_deg <= 56.25)){
            OLED_2.drawStr(90, 40, "NW");
          } 
          if ((crs_deg > 56.25) && (crs_deg <= 78.75)){
            OLED_2.drawStr(90, 40, "WNW");
          } 
          if ((crs_deg > 78.75) && (crs_deg <= 101.25)){
            OLED_2.drawStr(90, 40, "W");
          } 
          if ((crs_deg > 101.25) && (crs_deg <= 123.75)){
            OLED_2.drawStr(90, 40, "WSW");
          } 
          if ((crs_deg > 123.75) && (crs_deg <= 146.25)){
            OLED_2.drawStr(90, 40, "SW");
          } 
          if ((crs_deg > 146.25) && (crs_deg <= 168.75)){
            OLED_2.drawStr(90, 40, "SSW");
          } 
          if ((crs_deg > 168.75) && (crs_deg <= 191.25)){
            OLED_2.drawStr(90, 40, "S");
          } 
          if ((crs_deg > 191.25) && (crs_deg <= 213.75)){
            OLED_2.drawStr(90, 40, "SSE");
          } 
          if ((crs_deg > 213.75) && (crs_deg <= 236.25)){
            OLED_2.drawStr(90, 40, "SE");
          } 
          if ((crs_deg > 236.25) && (crs_deg <= 258.75)){
            OLED_2.drawStr(90, 40, "ESE");
          } 
          if ((crs_deg > 258.75) && (crs_deg <= 281.25)){
            OLED_2.drawStr(90, 40, "E");
          } 
          if ((crs_deg > 281.25) && (crs_deg <= 303.75)){
            OLED_2.drawStr(90, 40, "ENE");
          } 
          if ((crs_deg > 303.75) && (crs_deg <= 326.25)){
            OLED_2.drawStr(90, 40, "NE");
          }
          if ((crs_deg > 326.25) && (crs_deg < 348,75)){
            OLED_2.drawStr(90, 40, "NNE");
          }
           
          
          // OLED_2.sendBuffer();
        }
 // } while ( u8g2.nextPage() );
        //OLED_2.nextPage();
        OLED_2.sendBuffer();

}
