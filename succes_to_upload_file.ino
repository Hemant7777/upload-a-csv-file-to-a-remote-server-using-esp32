#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>
//#include <StreamLib.h>

File myFile;

String post_host = "My.site.in";
const int  post_port  = 80;
String  url = "/upload_file.php";
char server[] ="My.site.in";
const char* ssid4     = "MY-ssid";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password4 ="My-passwoed";
// Emulate Serial1 on pins 6/7 if not present
#if defined(ARDUINO_ARCH_AVR) && !defined(HAVE_HWSERIAL1)
//#include <SoftwareSerial.h>
//SoftwareSerial Serial1(6, 7); // RX, TX
#define AT_BAUD_RATE 9600
#else
#define AT_BAUD_RATE 115200
#endif

//const char* server = "arduino.cc";

WiFiClient client;


//format bytes
String formatBytes(unsigned int bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}
void readFile(fs::FS &fs, String path) {
  Serial.printf("Reading file: %s\n", path);

  File myFile = fs.open(path);
  if (!myFile) {
    Serial.println("Failed to open file for reading");
    return;
  }
}
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial1.begin(AT_BAUD_RATE);
  
  WiFi.begin(ssid4, password4);
 delay(1000);

  // waiting for connection to Wifi network set with the SetupWiFiConnection sketch
  Serial.println("Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connected to WiFi network.");

   //test connect to sd
  Serial.print("Initializing SD card...");
  if (!SD.begin())
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //read file from SD
  //define file
  //myFile = SD.open("/data.wav");
  myFile = SD.open("/S556kb.csv");//100,119,556
  //readFile(SD, "/data.wav");
  readFile(SD, "/S556kb.csv");
  String fileName = myFile.name();
  String fileSize = formatBytes(myFile.size());

  Serial.println();
  Serial.println("file exists");


  if (myFile)
  {
    Serial.println("test file:ok");
    // print content length and host
    Serial.print("contentLength : ");
    Serial.println(fileSize);
    Serial.print("connecting to ");
    Serial.println(post_host);

    // We now create a URI for the request
    Serial.println("Connected to server");
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // Make a HTTP request and add HTTP headers
    //String boundary = "CustomizBoundarye----";
    //change with your content type
    String contentType = "text/csv";//"audio/x-wav";
    String portString = String(post_port);
    String hostString = String(post_host);


 csv
     String requestHead = "--KeepSending\r\nContent-Disposition: form-data; name=\"data\"; filename=\"S556kb.csv\"\r\nContent-Type: text/csv\r\n\r\n";

    String tail = "\r\n--KeepSending--\r\n";


    int contentLength =  requestHead.length() + myFile.size() + tail.length();

    client.connect(server, 80);

    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + post_host);
    client.println("Content-Length: " + String(contentLength, DEC));
    client.println("Content-Type: multipart/form-data; boundary=KeepSending");
    client.println();
    client.print(requestHead );

    // send myFile:
    //this method is for upload data very fast
    //and only work in ESP software version after 2.3.0
    //if your version is lower than please update
    //esp software to last version or use bellow comment code
    client.write(myFile);
    // create file buffer
    const unsigned bufSize = 2048;
    byte clientBuf[bufSize];
    int clientCount = 0;

    while (myFile.available())
    {
      clientBuf [clientCount] = myFile.read ();
      clientCount++;
      if (clientCount > (bufSize - 1))
      {
        client.write((const uint8_t *)clientBuf, bufSize);
        clientCount = 0;
      }

    }

    if (clientCount > 0)
    {
      client.write((const uint8_t *)clientBuf, clientCount);

    }

    // send tail
    //      char charBuf3[tail.length() + 1];
    //      tail.toCharArray(charBuf3, tail.length() + 1);
    client.print(tail);


    //Serial.print(charBuf3);
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening test.WAV");
    Serial.println("Post Failure");
  }



  // Read all the lines of the reply from server and print them to Serial


  Serial.println("request sent");
  String responseHeaders = "";

  while (client.connected() ) {
    //      Serial.println("while client connected");
    String line = client.readStringUntil('\n');
    Serial.println(line);
    responseHeaders += line;
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');

  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

  //close file
  myFile.close();

}
//}
void loop()
{}
