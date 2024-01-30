/*
 * @Author: Z丶平凡
 * @Date: 2024-01-30 14:25:07
 * @LastEditTime: 2024-01-30 14:31:53
 * @LastEditors: Z丶平凡
 * @Description: In User Settings Edit
 * @FilePath: \esp32s3\src\Html.h
 */

#include <Arduino.h>
#include <led.h>



void TwinkleLed(uint16_t Mdelay,uint16_t TwinkleNum)
{
  static uint8_t Toggle = 0;

  if (TwinkleNum)
  {
    for (uint8_t i = 0; i < TwinkleNum; i++)
    {
      if (Toggle == 0)
      {
        digitalWrite(8,HIGH);
      }
      else
      {
        digitalWrite(8,LOW);
      }
      Toggle =~Toggle;
      delay(Mdelay);
    }
  }
  else
  {
    if (Toggle == 0)
    {
      digitalWrite(8,HIGH);
    }
    else
    {
      digitalWrite(8,LOW);
    }
    Toggle =~Toggle;
    delay(Mdelay);
  }
}


