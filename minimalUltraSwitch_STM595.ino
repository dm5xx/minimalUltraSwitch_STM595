// cc by DM5XX @ GNU GPLv3
// LLAP! 

#include <Ethernet_STM32.h>
#include "RelayBoard.h"
#include "Boards.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define DHCP
//#define DEBUG



byte mac[] = {0xDE, 0xAD, 0xBE, 0x01, 0x02, 0x01};

//uncomment and change if you need to adress a static ip in your local environment
//IPAddress ip(192, 168, 97, 177);
int myPort = 59;
EthernetServer server(myPort); // (port 80 is default for HTTP)
const uint8_t ETHERNET_SPI_CS_PIN = PB12;

char requestString[100];
bool isLocked = false;

// all files are in their newest version on DM5XX webserver. You have to point this address to your websrver oder local computer, if you want to change something else than labels..
String URLToJS = "h.mmmedia-online.de/minimal63/";

// adjust this URL if you want to change Label.js, GroupDef.js, Lock.js, Disable.js (to customize them). So ALL 4 files must be stored on your own webserver/local computer than, even if you just want to leave one or more default!
//String URLToCustomize = "dl5nam.de/minimal63/";
String URLToCustomize = "h.mmmedia-online.de/minimal63XX/";

// this is the url, where the dashboard will call to reach the switch. can be an internal IP (same as IPADDRESS above!!!) or a dyndns-url, forwarded to IPADDRESS (above).
String SwitchURL = "255.255.255.255";
String MyLocalIP = "192.168.97.177"; 
String ClientIP = "255.255.255.255"; 

uint8_t NUMBEROFBOARDS = 2;
Boards relayArray;


/*********************************************************************************************************************************/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++ THE WEB PAGES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*********************************************************************************************************************************/

void sendEmptyPage(EthernetClient &client)
{
  Send200OK(client);
}

void SendFavicon(EthernetClient &client)
{
   Send200OK(client);
   client.println(F(""));
   client.println(F("<!DOCTYPE html>"));
   client.println(F("<HTML>"));
   client.println(F("<link rel=\"icon\" href=\"data:;base64,=\">"));
   client.println(F("</HTML>"));
}

void GetDataJSON(EthernetClient &client)
{
   client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Access-Control-Allow-Origin: *"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.println(F(""));
  client.print(F("{\"B0\": "));
  
  RelayBoard* boards = relayArray.GetBoards();
  client.print( boards[0].GetStatus());

    if(NUMBEROFBOARDS > 1)
    {
        for(uint8_t c=1; c < NUMBEROFBOARDS; ++c)
        {
            client.print(", \"B");
            client.print(c);
            client.print("\": ");
            client.print(boards[c].GetStatus());
        }
    }

  client.print(F(", \"LockStatus\": "));
  client.print(isLocked);
  client.print(F(", \"Clients\": \""));
  client.print(ClientIP);
  client.print(F("\"}"));
}

void SendLocked(EthernetClient &client)
{
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Access-Control-Allow-Origin: *"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.println(F(""));
  client.print(F("{\"Lockingstatus\": "));
  client.print(isLocked);
  client.print(F("}"));
}
void Send200OK(EthernetClient &client)
{
  client.println(F("HTTP/1.1 200 OK")); //send new page
  client.println(F("Access-Control-Allow-Origin: *"));
  client.println(F(""));
  client.println(F("Content-Type: text/html"));
}

void MainPage(EthernetClient &client)
{
   client.println("HTTP/1.1 200 OK");
   client.println(F("Access-Control-Allow-Origin: *"));
   client.println(F("Content-Type: text/html"));
   client.println("Connection: close");  // the connection will be closed after completion of the response   client.println(F(""));
   client.println(F(""));
   client.println(F("<!DOCTYPE html>"));
   client.println(F("<HTML>"));
   client.println(F("<HEAD>"));
   client.println(F("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"));
   client.println(("<link rel=\"stylesheet\" type=\"text/css\" href=\"http://"+ URLToJS +"style.css\" media=\"screen\"/>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"init.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"ShortCut.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"Custom_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"Profile_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"Disable_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"Label_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"BankDef_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"Globals.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"LockDef_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"Lock.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToCustomize +"GroupDef_c.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"Group.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"UiHandler.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"GetData.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"SetData.js\"></script>"));
   client.println(("<script language=\"javascript\" type=\"text/javascript\" src=\"http://"+ URLToJS +"Helper.js\"></script>"));
   client.print(F(""));
   client.println(F("<!-- Change SwitchURL to the url, where your webswitch is reachable from outside/inside. Dont forget the portforwarding...-->"));
   client.print(F("<script>var url='"));
   client.print((MyLocalIP));
   client.print(":");
   client.print(myPort);
   client.print(F("';\r"));
   client.print(F("var numberOfBoards="));
   client.print(NUMBEROFBOARDS);
   client.print(F(";\r"));
   client.println(F("</script>"));
   client.print(F("<TITLE>"));
   client.print("minimal63 - Remote Switch by DM5XX");
   client.println(F("</TITLE>"));
   client.println(F("</HEAD>"));
   client.println(F("<BODY>"));
   client.println(F("<div class=\"grid-container\" id=\"container\"></div>"));
   client.println(F("</BODY>"));
   client.println(F("<script>(() => { init(); })()</script>"));
   client.println(F("</HTML>"));
}

void setup()
{
   // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while ( !Serial ); delay(100);

  relayArray.Init(NUMBEROFBOARDS);

  Serial.println(("*************************************************************"));
  Serial.println(("***** Web server xxx *****"));
  Serial.println(("*************************************************************"));

  SPI.setModule(2);
  // init Ethernet interface
  Ethernet.init(SPI, ETHERNET_SPI_CS_PIN);

  Serial.print("\nGetting IP address using DHCP...");
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");

  IPAddress address = Ethernet.localIP();
  MyLocalIP = String() + address[0] + "." + address[1] + "." + address[2] + "." + address[3];
  
  Serial.println(MyLocalIP);

}


#define BUFSIZE 20

void loop() 
{
  // Create a client connection
  EthernetClient client = server.available();
  char clientline[BUFSIZE];
  int index = 0;
  
  if (client) 
  {
    //IPAddress localClientIP = client.remoteIP();   ?????????????????????????????????????????????????????????
    //ClientIP = String() + localClientIP[0] + "." + localClientIP[1] + "." + localClientIP[2] + "." + localClientIP[3];
  
    byte charIndex = 0;

    // connectLoop controls the hardware fail timeout
    int connectLoop = 0;
    
    while (client.connected()) 
    { 
      connectLoop++;

      if(connectLoop > 10000)
      {
        client.stop();
      };
      
      if(client.available()) 
      {
        char c = client.read();
      
        if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;

          if (index >= BUFSIZE) 
            index = BUFSIZE -1;
          
          continue;
        }
        
        // got a \n or \r new line, which means the string is done
        clientline[index] = 0;
        
        // Print it out for debugging
#ifdef DEBUG
        Serial.println(clientline);
#endif
        
        //if HTTP request has ended
        if (strstr(clientline, "GET /Get/") != 0) 
        {    
#ifdef DEBUG
            Serial.println("Get detected");
#endif
            GetDataJSON(client);
            connectLoop = 0;
            break;
        }

        byte end = 0;            
        for(int r = 11; r < 16; r++)
        {
          if(clientline[r] == ' ')
            end = r;
          else
            continue;
        }

        String sss = clientline;
        String subSss = sss.substring(10, end);
        
        if(strstr(clientline, "GET /Set") != 0)
        {
          if(isLocked)
          {
            SendLocked(client);
            connectLoop = 0;
            break;
          }
          
          int value = subSss.toInt(); 
  
#ifdef DEBUG
          Serial.println("Url called by Set on Number ");
          Serial.println(clientline[8]-'0');
          Serial.println("With value ");
          Serial.println(value);
#endif
          RelayBoard* boards = relayArray.GetBoards();
          boards[clientline[8]-'0'].SetStatus(value);
          relayArray.WriteRegisters();

          Send200OK(client);
          connectLoop = 0;
          break;
        }
        else if(strstr(clientline, "GET /set") != 0)
        {
// ToDo: Implement when needed...
/*          int delim = subSss.indexOf('/');
          int pinNr = subSss.substring(0, delim).toInt();
          int cmd = (subSss[subSss.length()-1])-48;

#ifdef DEBUG
          Serial.println(pinNr);
          Serial.println(cmd);
#endif

          switch (clientline[8])
          {
            case '0':
#ifdef DEBUG
              Serial.println("Manual Set at bank 0");          
#endif
              updatePin(pinNr, cmd, 0);
              break;
            case '1':
#ifdef DEBUG
              Serial.println("Manual Set at bank 1");          
#endif
              updatePin(pinNr, cmd, 1);
              break;
            case '2':
#ifdef DEBUG
              Serial.println("Manual Set at bank 2");          
#endif
               updatePin(pinNr, cmd, 2);
               break;
            case '3':
#ifdef DEBUG
                Serial.println("Manual Set at bank 3");          
#endif
               updatePin(pinNr, cmd, 3);
               break;
          }
          Send200OK(client);
          connectLoop = 0;
          break; */
        }
        else if (strstr(clientline, "GET /favicon") != 0)
        {
#ifdef DEBUG
          Serial.println("Favicon requested");
#endif
          SendFavicon(client);
          connectLoop = 0;
          break;
        }
        else if (strstr(clientline, "GET /Reset") != 0)
        {
          client.println(F(""));
          client.println(F("<!DOCTYPE html>"));
          client.println(F("<HTML><BODY>R-E-S-E-T!!!</BODY></HTML>"));
          delay(50); // 1?  
          client.stop();
          nvic_sys_reset();
          //soft_restart();
        }
        else if (strstr(clientline, "GET /Lock") != 0)
        {
          isLocked = true;
          SendLocked(client);
          connectLoop = 0;
          break;
        }
        else if (strstr(clientline, "GET /UnLock") != 0)
        {
          isLocked = false;
          SendLocked(client);
          connectLoop = 0;
          break;
        }
                  
#ifdef DEBUG
        Serial.println("Call main");
#endif
        MainPage(client);
        connectLoop = 0;
        break;
      }
    }
    delay(10); // 1?
  
    client.stop();
#ifdef DEBUG
    Serial.println("disconnected.");
#endif
  }
}
