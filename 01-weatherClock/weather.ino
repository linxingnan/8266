#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266_Seniverse.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>   //  ESP8266WebServer库
#include "myscreen.h"
 
// 心知天气API请求所需信息
// https://www.seniverse.com/docs
String reqUserKey = "S2roGH54aDyMa4iGF";   // 私钥
String reqLocation = "beijing";            // 城市
String reqUnit = "c";                      // 摄氏/华氏
String weatherTemp = "";

ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是 'wifiMulti'
ESP8266WebServer esp8266_server(80);// 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
WeatherNow weatherNow;  // 建立WeatherNow对象用于获取心知天气信息
 
void setup(){
  initScreen();
  testTft();
  Serial.begin(9600);          
  Serial.println("");

  if(SPIFFS.begin()){                       // 启动闪存文件系统
    Serial.println("SPIFFS Started.");
  } else {
    Serial.println("SPIFFS Failed to Start.");
  }     
 
  connectWiFi();    // 连接wifi
 
  // 配置心知天气请求信息
  weatherNow.config(reqUserKey, reqLocation, reqUnit);

}
 
void loop(){
  esp8266_server.handleClient();     // 处理http服务器访问

}
 
// 连接WiFi
void connectWiFi(){

  wifiMulti.addAP("MIX 2S", "00000000"); // 将需要连接的一系列WiFi ID和密码输入这里
  wifiMulti.addAP("TP-LINK_8CEE", "35S235S2."); // ESP8266-NodeMCU再启动后会扫描当前网络
  
  Serial.print("Connecting to ");              // 串口监视器输出网络连接信息
  Serial.print(ssid); Serial.println(" ...");  // 告知用户NodeMCU正在尝试WiFi连接
  
  int i = 0;                                   // 这一段程序语句用于检查WiFi是否连接成功
  while (wifiMulti.run() != WL_CONNECTED) {      // WiFi.status()函数的返回值是由NodeMCU的WiFi连接状态所决定的。 
    delay(1000);                               // 如果WiFi连接成功则返回值为WL_CONNECTED                       
    Serial.print(i++); Serial.print(' ');      // 此处通过While循环让NodeMCU每隔一秒钟检查一次WiFi.status()函数返回值
  }                                            // 同时NodeMCU将通过串口监视器输出连接时长读秒。
                                               // 这个读秒是通过变量i每隔一秒自加1来实现的。                                              
  Serial.println("");                          // WiFi连接成功后
  Serial.println("Connection established!");   // NodeMCU将通过串口监视器输出"连接成功"信息。
  Serial.print("IP address:    ");             // 同时还将输出NodeMCU的IP地址。这一功能是通过调用
  Serial.println(WiFi.localIP());              // WiFi.localIP()函数来实现的。该函数的返回值即NodeMCU的IP地址。  
  
  
  esp8266_server.on("/readLocalWeather", handleLocalTemp);  
  esp8266_server.onNotFound(handleNotFound);// 设置处理404情况的函数'handleNotFound'
  esp8266_server.begin();
}

void handleLocalTemp() {
  
 if(weatherNow.update()){  // 更新天气信息

    weatherTemp += reqLocation;//当前天气字符串
    weatherTemp +=",";
    
    weatherTemp += weatherNow.getServerCode();//当前天气字符串
    weatherTemp +=",";
//    weatherTemp += weatherNow.getWeatherText();// 获取当前天气（整数格式）
//    weatherTemp+=",";
    weatherTemp += weatherNow.getDegree();// 获取当前温度数值
    weatherTemp+=",";
    weatherTemp += weatherNow.getLastUpdate();// 获取服务器更新天气信息时间
    weatherTemp+=",";
    
  } 
  else 
  {    // 更新失败
    // https://docs.seniverse.com/api/start/error.html
    weatherTemp = "更新失败 + weatherNow.getServerCode()";// 心知天气服务器错误代码说明可通过以下网址获取
  }                                             
  
 esp8266_server.send(200, "text/plain", weatherTemp); //发送模拟输入引脚到客户端ajax请求


  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("城市:");
  tft.setTextSize(2);
  tft.println(reqLocation);
  tft.setTextSize(1);
  tft.println("温度:");
  tft.setTextSize(2);
  tft.println(weatherNow.getDegree());
}

// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){                                        // 当浏览器请求的网络资源无法在服务器找到时，
  // 获取用户请求资源(Request Resource）
  String reqResource = esp8266_server.uri();
  Serial.print("reqResource: ");
  Serial.println(reqResource);
  
  // 通过handleFileRead函数处处理用户请求资源
  bool fileReadOK = handleFileRead(reqResource);

  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!fileReadOK){                                                 
    esp8266_server.send(404, "text/plain", "404 Not Found"); 
  }
}

bool handleFileRead(String resource) {            //处理浏览器HTTP访问
  
  if (resource.endsWith("/")) {                   // 如果访问地址以"/"为结尾
    resource = "/index.html";                     // 则将访问地址修改为/index.html便于SPIFFS访问
  } 
  
  String contentType = getContentType(resource);  // 获取文件类型
  
  if (SPIFFS.exists(resource)) {                     // 如果访问的文件可以在SPIFFS中找到
    
    File file = SPIFFS.open(resource, "r");          // 则尝试打开该文件
    esp8266_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  return false;                                  // 如果文件未找到，则返回false
}
// 获取文件类型
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
