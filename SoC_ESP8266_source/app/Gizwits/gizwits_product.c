/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits 控制协议处理,及平台相关的硬件初始化 
* @author       Gizwits
* @date         2016-09-05
* @version      V03010201
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include <stdio.h>
#include <string.h>
#include "gizwits_protocol.h"
#include "driver/hal_key.h"
#include "gagent_soc.h"

/** 用户区当前设备状态结构体*/
extern dataPoint_t currentDataPoint;
uint32_t ntpdelay;
uint32_t delay_time;
_tm ntptimeon;
extern uint8_t switch_flag;
extern uint8_t timeon_flag;

/**@name Gizwits 用户API接口
* @{
*/

/**
* @brief 事件处理接口

* 说明：

* 1.用户可以对WiFi模组状态的变化进行自定义的处理

* 2.用户可以在该函数内添加数据点事件处理逻辑，如调用相关硬件外设的操作接口

* @param[in] info : 事件队列
* @param[in] data : 协议数据
* @param[in] len : 协议数据长度
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t ICACHE_FLASH_ATTR gizwitsEventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;
  uint32_t ntptime;

  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_SWITCHCONTROL:
        currentDataPoint.valueswitchcontrol = dataPointPtr->valueswitchcontrol;
        os_printf("Evt: EVENT_SWITCHCONTROL %d \n", currentDataPoint.valueswitchcontrol);
        if(0x01 == currentDataPoint.valueswitchcontrol)
        {
          //user handle
          GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
        }
        else
        {
          //user handle
          GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
          timeon_flag = 1;
        }
        break;

      case EVENT_TIMEON_HOUR:
        currentDataPoint.valuetimeon_hour = dataPointPtr->valuetimeon_hour;
        os_printf("Evt:EVENT_TIMEON_HOUR %d\n",currentDataPoint.valuetimeon_hour);
        //user handle
        timeon_flag = 0;
        break;
      case EVENT_TIMEON_MIN:
        currentDataPoint.valuetimeon_min = dataPointPtr->valuetimeon_min;
        os_printf("Evt:EVENT_TIMEON_MIN %d\n",currentDataPoint.valuetimeon_min);
        //user handle
        timeon_flag = 0;
        break;
      case EVENT_TIME_DELAY:
        currentDataPoint.valuetime_delay = dataPointPtr->valuetime_delay;
        os_printf("Evt:EVENT_TIME_DELAY %d\n",currentDataPoint.valuetime_delay);
        //user handle
        delay_time = currentDataPoint.valuetime_delay;
        ntpdelay = gizwitsGetTimeStamp();
        if(currentDataPoint.valuetime_delay == 0)
          switch_flag = 0;
        else
          switch_flag = 1;
        os_printf("valuetime_delay ntptime: %d\r\n", ntpdelay);
        break;


      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
        break;
      case WIFI_DISCON_ROUTER:
        break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        os_printf("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        os_printf("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      default:
        break;
    }
  }
  system_os_post(USER_TASK_PRIO_0, SIG_UPGRADE_DATA, 0);
  return 0;
}
/**@} */