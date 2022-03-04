#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
int sniffed_macs_global_counter;
String Sniffed_Mac_Addresses[150];
int rssi[150]; //collect the rssi data on each peripheral
String Internet_String="";
String incoming="";
volatile bool finshed_scan;

float latitude;
float longitude;
String Tym;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if(sniffed_macs_global_counter<150)
        {
          //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
          Sniffed_Mac_Addresses[sniffed_macs_global_counter]=advertisedDevice.getAddress().toString().c_str();
          //Serial.print(" RSSI: ");
          //Serial.println(advertisedDevice.getRSSI());
          rssi[sniffed_macs_global_counter] = advertisedDevice.getRSSI();
          sniffed_macs_global_counter++;
        }
    }
};
 
static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  sniffed_macs_global_counter = 0;
  Serial.println("Scanning...");
  finshed_scan=false;
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
//  while (ss.available() > 0)
//    if (gps.encode(ss.read()))
//      displayInfo();
//
//  if (millis() > 5000 && gps.charsProcessed() < 10)
//  {
//    Serial.println(F("No GPS detected: check wiring."));
//    while(true);
//  }      

///////////////////////////////////////////////////////////////////////////////////////////////
  //put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, true);
  Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  Serial.println(sniffed_macs_global_counter);
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  Serial.println("===========================================================================");
  Serial.println("Now scan for duplicates");
  LowEnergyCheckForDuplicates();
  Serial.println("===========================================================================");
  Serial.println("===========================================================================");
    finshed_scan=true;
  if(finshed_scan==true)
  {
      while (ss.available() > 0)
      {
        if (gps.encode(ss.read()))
        {
              displayInfo();
        }
      }
  
    if (millis() > 2000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      while(true);
    }
    finshed_scan=false;
  }
  
   AlldomumentingJson();
  delay(500);
  sniffed_macs_global_counter=0;
 
}

void displayInfo()
{
    Serial.println("===========================================================================");
  Serial.println("===========================================================================");
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {

    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.print(F("Latitude: "));
    Serial.print(latitude, 6);
    Serial.print(F("Longitude: "));
    Serial.println(longitude, 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  current_time();

  Serial.println("===========================================================================");
  Serial.println("===========================================================================");
  Serial.println();
}

void current_time()
{
  String _tym_;

  //Serial.print(F("Time: "));
  if (gps.date.isValid())
  {
    _tym_ += gps.date.month();
    _tym_ += "/";
    _tym_ += gps.date.day();
    _tym_ += "/";
    _tym_ += gps.date.year();
  }
  else
  {
    _tym_ = "DATE INVALID";
  }

  _tym_ += " ";
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.hour();
    _tym_ += ":";
    if (gps.time.minute() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.minute();
    _tym_ += ":";
    if (gps.time.second() < 10) {
    _tym_ += "0";}
    _tym_ += gps.time.second();
  }
  else
  {
    _tym_ = "TIME INVALID";
  }
 
 // Serial.print("Inside gps functon the returned tym ->");
  //Serial.println(_tym_);
  Tym =_tym_;
  _tym_ ="";
  //Serial.println();    

 return;
}

void LowEnergyCheckForDuplicates()
{
    int i, j, k;
    int old_number_of_macs = sniffed_macs_global_counter;

    //lets sort them and remove the duplicates

    for( i=0; i<sniffed_macs_global_counter; i++)
    {
        for( j=i+1; j<sniffed_macs_global_counter; j++)
        {
            /* If any duplicate found */
            if ((strcmp(((Sniffed_Mac_Addresses[i].c_str())), ((Sniffed_Mac_Addresses[j].c_str()))) == 0))
            {
                /* Delete the current duplicate element */
                for(k=j; k<sniffed_macs_global_counter-1; k++)
                {
                    Sniffed_Mac_Addresses[k] = Sniffed_Mac_Addresses[k+1];
                    rssi[k]= rssi[k+1];
                }
                /* Decrement size after removing duplicate element */
                sniffed_macs_global_counter--;

                /* If shifting of elements occur then don't increment j */
                j--;
            }
        }
    }

    Serial.println("Function: LowEnergyCheckForDuplicates");
    Serial.print("Old Unsorted Array had: "); Serial.print(old_number_of_macs); Serial.println(" Elements");
    Serial.print("New Sorted Array has: "); Serial.print(sniffed_macs_global_counter); Serial.println(" Elements");
    Serial.println(F("========================================"));

    for(int m = 0; m< sniffed_macs_global_counter; m++)
    {
        Serial.print(Sniffed_Mac_Addresses[m]);
        Serial.print(" RSSI=");
        Serial.println(rssi[m]);
    }
    Serial.println(F("========================================"));
    Serial.println(F("========================================"));
    //delay(100);
    return;
}

void AlldomumentingJson()
{
  Serial.println("======================START==========================");
  Serial.println("=====================================================");
  Serial.print("ALL DOCUMENTING FUNCTION");
 
  //calculate the capacity of json document
  uint16_t capa = 128*sniffed_macs_global_counter;
  if(capa<=1024)
  {
    capa=1024;
  }
  Serial.print("The global count is:");
  Serial.println(sniffed_macs_global_counter);
  Serial.print("The created capacity is:");
  Serial.println(capa);
  Serial.println("=====================================================");
 
  DynamicJsonDocument doc(capa); //this was the original before trying to fix the output issue

  Serial.println("The documentingJson function");
 

    doc["time"].set(Tym);
    doc["lat"] = latitude;
    doc["lon"] = longitude;
    JsonArray macs = doc.createNestedArray("macs");

    for(int i =0; i<sniffed_macs_global_counter; i++)
    {
      JsonObject rooot = macs.createNestedObject();
      rooot["addr"].set(Sniffed_Mac_Addresses[i]);
      rooot["rssi"].set(rssi[i]);
    }

    serializeJson(doc, Serial);
    Serial.println();
    Serial.println("==============================NEW DATA STRUCTURE======================================");

    //show the serialized json string
    //format 2 fails to print when there is too many passengers data
    Serial.println("The following is the serialized output string for POST");
    serializeJson(doc, Internet_String);
    Serial.println(Internet_String);
   
    //clear document for reuse
    Serial.println("======================domumentingJson Finished==========================");
    doc.clear();

    for (int e = 0; e < 150; e++)
    {
      (Sniffed_Mac_Addresses[e]) = "";
      (rssi[e]) = '\0';
    }        

    capa=0;
    Tym="";
    latitude=0;
    longitude=0;
   
    return;
 
}
