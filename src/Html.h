/*
 * @Author: Z丶平凡
 * @Date: 2024-01-30 14:25:07
 * @LastEditTime: 2024-01-30 15:46:12
 * @LastEditors: Z丶平凡
 * @Description: In User Settings Edit
 * @FilePath: \WIFI_MacReplicator\src\Html.h
 */

#ifndef _Html_H_
#define _Html_H_

#include <pgmspace.h>


const char Html_index[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta charset="utf-8">
</head>
<body>
  <form method='POST' action='/macsave'>
    <div>
      <h3>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;WIFI MAC 测试工具</h3>
      	  <h5>警告:请勿用于非法用途!您只能在自己的网络和设备上使用它!</h5>
    </div>
  <div>
	<label for='mac-input'>设置SSID:&nbsp;</label> <input type='text' name='ssiddata' id='mac-input' value='%SSID%' placeholder='请输入非中文SSID,选填'> 
	<br>
	<label for='mac-input'>设置密码 :&nbsp;</label> <input type='text' name='password' id='mac-input' value='%PASSWORD%' placeholder='请输入当前WIFI密码,选填'>
	<br>
  <label for='mac-input'>设置MAC:&nbsp;</label> <input type='text' name='macdata' id='mac-input' value='%WIFIMAC%' placeholder='请输入克隆目标MAC地址'> 
  <br>
  </div>
  <br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type='submit' value='保存设置'>
  </form>
</body>
)rawliteral";


const char HtmlHead[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta charset="utf-8">
</head>
<body>
    <script>
)rawliteral";


const char HtmlTail[] PROGMEM = R"rawliteral(
  </script>
</body>
)rawliteral";



#endif 
