/*
 * @Author: Z丶平凡
 * @Date: 2024-01-30 14:25:07
 * @LastEditTime: 2024-01-30 15:14:10
 * @LastEditors: Z丶平凡
 * @Description: In User Settings Edit
 * @FilePath: \esp32s3\src\main.cpp
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiScan.h>
#include <AsyncTCP.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>
#include <Html.h>
#include <led.h>



#define wifi_ap_ssid  "WIFI Mac Replicator"   /*默认wifi SSID*/
#define wifi_ap_password "12345678"           /*默认wifi 密码*/

#define SaveFlagAddress 0     /*EEPROM 初始化地址*/
#define MacSaveAddress  1     /*18数量MAC地址*/
#define PasswordAddress MacSaveAddress + 18    /*13地址密码数量-0为无密码,>0为有密码-剩下读取数量-密码数量最大16*/
#define SsidAddress     MacSaveAddress + 18 + 17    /*30地址ssid数量-0为无ssid,>0为有ssid-剩下读取数量-ssid数量最大10*/

#define ResetPin 0  /*重置引脚*/


const IPAddress netMsk(255,255,255,0);    //子网掩码
const IPAddress local_ip(192,168,4,100);  //本机IP地址
const IPAddress gateway(192,168,4,1);     //本机网关


uint8_t MacAddressTemp[6]={0};              /*MAC临时地址*/
char macdata1[20] = {0};                    /*MAC数据-EEPROM*/
char passworddata[16]={wifi_ap_password};   /*密码数据-EEPROM*/
char ssiddata[21]={wifi_ap_ssid};           /*SSID数据-EEPROM*/


AsyncWebServer server(80);           // 打开80端口服务器

uint8_t EEPROM_ERROR = 0;   /*EEPROM初始化标志*/
uint8_t OperationFlag =0;   /*存储操作标志-1-有操作-0无操作*/

#define DebugUsart 0


void setup() 
{
  pinMode(8,OUTPUT);
  pinMode(0,INPUT);
  TwinkleLed(50,20);
  #if (DebugUsart)
    Serial.begin(115200);
  #endif


  if (!EEPROM.begin(512))   //初始化EEPROM
  {
    #if (DebugUsart)
      Serial.println("Failed to initialise EEPROM");
    #endif
    EEPROM_ERROR = 1;
  }
  else
  {
    #if (DebugUsart)
      Serial.println("Success to initialise EEPROM");
    #endif


    if (EEPROM.read(SaveFlagAddress) == 0xAF)   /*EEPROM已经初始化完成*/
    {
      uint8_t length = EEPROM.read(MacSaveAddress);
      Serial.println("MAC length:");
      Serial.println(length);

      
      if (length > 0)
      {
        for (uint8_t i = 0; i < length; i++)
        {
          macdata1[i] = EEPROM.read(MacSaveAddress + 1 + i);
        }
        sscanf(macdata1,"%02x:%02x:%02x:%02x:%02x:%02x",MacAddressTemp,MacAddressTemp+1,MacAddressTemp+2,MacAddressTemp+3,MacAddressTemp+4,MacAddressTemp+5);
        
        #if (DebugUsart)
          Serial.println("MAC Address:");
          for(uint8_t i = 0 ; i < 6; i++)
          {
            Serial.println(MacAddressTemp[i]);
          }  
        #endif
      }

      length = EEPROM.read(PasswordAddress);

      #if (DebugUsart)
        Serial.println("password length:");
        Serial.println(length);
      #endif

      if (length > 0)
      {
        for (uint8_t i = 0; i < length; i++)
        {
          passworddata[i] = EEPROM.read(PasswordAddress + 1 + i);
        }
          passworddata[length]=0;
      }

      length = EEPROM.read(SsidAddress);

      #if (DebugUsart)
        Serial.println("ssid length:");
        Serial.println(length);
      #endif

      if (length > 0)
      {
        for (uint8_t i = 0; i < length; i++)
        {
          ssiddata[i] = EEPROM.read(SsidAddress + 1 + i);
        }
          ssiddata[length]=0;
      }
    }
    else  /*EEPROM 未初始化*/
    {
      EEPROM.write(MacSaveAddress, 0); 
      EEPROM.commit();

      EEPROM.write(PasswordAddress, 0); 
      EEPROM.commit();

      EEPROM.write(SsidAddress, 0); 
      EEPROM.commit();

      EEPROM.write(SaveFlagAddress, 0xAF); 
      EEPROM.commit();
      #if (DebugUsart)
        Serial.println("EEPROM VALUE Init--OK");
      #endif
    }

    WiFi.softAPConfig(local_ip, gateway, netMsk);
    WiFi.softAP(ssiddata, passworddata);
    if(MacAddressTemp[0] != 0)
      esp_wifi_set_mac(WIFI_IF_AP, MacAddressTemp);
  }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      String Html_index1(Html_index);

      Html_index1.replace("%SSID%",ssiddata);
      Html_index1.replace("%PASSWORD%",passworddata);
      Html_index1.replace("%WIFIMAC%",macdata1);

      request->send(200, "text/html", Html_index1);
    });

    server.on("/macsave", HTTP_POST, [](AsyncWebServerRequest *request)
    {
      String macdata = request->arg("macdata");
      String ssiddata1 = request->arg("ssiddata");
      String password = request->arg("password");
      String HtmlStr(HtmlHead);


        if(strcmp(ssiddata1.c_str(),ssiddata) != 0)
        {
          if (ssiddata1.length() > 0 && ssiddata1.length() < 20)
          {
            if (!EEPROM_ERROR)
            {
              uint8_t length = ssiddata1.length();
              EEPROM.write(SsidAddress, length); 
              EEPROM.commit();

              for (uint8_t i = 0; i < length; i++)
              {
                EEPROM.write(SsidAddress + 1 + i,ssiddata1[i]);
                EEPROM.commit();
              }
              OperationFlag |= 1<<0;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();
            }
          }
          else if(ssiddata1.length() > 20)
          {
            HtmlStr += R"rawliteral(alert("SSID长度应在20个字符以下(不支持中文SSID)"); window.history.back();)rawliteral";
            HtmlStr += HtmlTail ;
            request->send(200, "text/html", HtmlStr);
          }
          else  /*无SSID*/
          {
            if (!EEPROM_ERROR)
            {
              EEPROM.write(SsidAddress, 0); 
              EEPROM.commit();
              OperationFlag |= 1<<0;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();
            }
          }
        }

        if(strcmp(password.c_str(),passworddata) != 0)
        {
          if (password.length() >= 8 && password.length() < 16)
          {
            #if (DebugUsart)
              Serial.println("WritePasswordTest\r\n");
              Serial.println(password);
            #endif
            if (!EEPROM_ERROR)
            {
              uint8_t length = password.length();
              EEPROM.write(PasswordAddress, length); 
              EEPROM.commit();

              for (uint8_t i = 0; i < length; i++)
              {
                EEPROM.write(PasswordAddress + 1 + i,password[i]);
                EEPROM.commit();
              }
              OperationFlag |= 1<<1;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();
            }
          }
          else if(password.length() > 0 && password.length() < 8)
          {
            HtmlStr += R"rawliteral(alert("密码长度应在8个字符以上16个字符以下。"); window.history.back();)rawliteral";
            HtmlStr += HtmlTail ;
            request->send(200, "text/html", HtmlStr);
          }
          else   /*无密码*/
          {
            if (!EEPROM_ERROR)
            {
              EEPROM.write(PasswordAddress, 0); 
              EEPROM.commit();
              OperationFlag |= 1<<1;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();            
            }
          }
        }

        if(strcmp(macdata.c_str(),macdata1) != 0)
        {
          if (macdata.length() == 17)
          {
            if (!EEPROM_ERROR)
            {
              uint8_t length = macdata.length();
              #if (DebugUsart)
                Serial.println("WriteMacTest\r\n");
                Serial.println(length);
              #endif
              
              EEPROM.write(MacSaveAddress, length); 
              EEPROM.commit();

              for (uint8_t i = 0; i < length; i++)
              {
                EEPROM.write(MacSaveAddress + 1 + i,macdata[i]);
                EEPROM.commit();
              }
              OperationFlag |= 1<<2;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();   
            }
          }
          else if(macdata.length() > 0 && macdata.length() < 17)
          {
            HtmlStr += R"rawliteral(alert("MAC地址长度不正确,MAC格式应为AA:BB:CC:DD:EE:FF(使用英文的:符号)"); window.history.back();)rawliteral";
            HtmlStr += HtmlTail ;
            request->send(200, "text/html", HtmlStr);
          }
          else
          {
            if (!EEPROM_ERROR)
            {
              EEPROM.write(MacSaveAddress, 0); 
              EEPROM.commit();
              OperationFlag |= 1<<2;
            }
            else
            {
              HtmlStr += R"rawliteral(alert("数据保存失败! FLASH ERROR! Reboot..."); window.history.back();)rawliteral";
              HtmlStr += HtmlTail ;
              request->send(200, "text/html", HtmlStr);
              delay(2000);
              ESP.restart();    
            }
          }
        }



        if (OperationFlag > 0)  
        {
            OperationFlag = 0;
            HtmlStr += R"rawliteral(alert("数据保存成功! Reboot..."); window.history.back();)rawliteral";
            HtmlStr += HtmlTail ;
            request->send(200, "text/html", HtmlStr);
            delay(2000);
            ESP.restart();
        }
        else
        {
            OperationFlag = 0;
            HtmlStr += R"rawliteral(alert("数据无修改!"); window.history.back();)rawliteral";
            HtmlStr += HtmlTail ;
            request->send(200, "text/html", HtmlStr);
        }
  });
    server.begin(); // 启动服务器
}




void loop() 
{
 static uint8_t CountTime = 0;

  TwinkleLed(500,0);
  if (digitalRead(ResetPin) == 0)
  {
    if (++CountTime >= 10)
    {
      CountTime = 0;                    /*恢复出厂设置*/
      EEPROM.write(MacSaveAddress, 0); 
      EEPROM.commit();

      EEPROM.write(PasswordAddress, 0); 
      EEPROM.commit();

      EEPROM.write(SsidAddress, 0); 
      EEPROM.commit();
      digitalWrite(8,LOW);            /*操作完成-LED常亮*/
      while (!digitalRead(ResetPin))  /*等待复位按键释放*/
      delay(500);
      ESP.restart();
    }
  }
  else
  {
    CountTime = 0;
  }
}






