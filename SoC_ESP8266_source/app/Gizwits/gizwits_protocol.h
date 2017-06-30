/**
************************************************************
* @file         gizwits_protocol.h
* @brief        对应gizwits_protocol.c的头文件 (包含产品相关定义)
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
#ifndef _GIZWITS_PROTOCOL_H
#define _GIZWITS_PROTOCOL_H
#include <stdint.h>
#include "osapi.h"
#include "user_interface.h"
#include "user_webserver.h"
#include "gizwits_product.h"

/**
* @name 协议版本号
* @{
*/
#define PROTOCOL_VERSION              "00000004"
/**@} */

/**@name P0协议版本号
* @{
*/
#define P0_VERSION                    "00000002"
/**@} */

#define BUFFER_LEN_MAX                900
#define REPORT_TIME_MAX               6000          ///< 6s


/**@name 定时器相关状态
* @{
*/
#define SOC_TIME_OUT                  10
#define MIN_INTERVAL_TIME             2000          ///< The minimum interval for sending
#define MAX_SOC_TIMOUT                1             ///< 1ms
#define TIM_REP_TIMOUT                600000        ///< 600s Regularly report
/**@} */

/**@name task相关定义
* @{
*/
#define SIG_ISSUED_DATA               0x01          ///< 协议处理任务
#define SIG_PASSTHROUGH               0x02          ///< 协议透传任务
#define SIG_IMM_REPORT                0x09          ///< 协议立即上报任务
/**@} */

/** WiFi模组配置参数*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                           ///< WIFI模组复位
  WIFI_SOFTAP_MODE,                                 ///< WIFI模组softAP模式
  WIFI_AIRLINK_MODE,                                ///< WIFI模组AirLink模式
  WIFI_PRODUCTION_TEST,                             ///< MCU请求WiFi模组进入产测模式
  WIFI_NINABLE_MODE,                                ///< MCU请求模组进入可绑定模式
}WIFI_MODE_TYPE_T;

/** WiFi模组工作状态*/
typedef union
{
    uint16_t value;
    struct
    {
        uint16_t            softap:1;               ///< 表示WiFi模组所处的SOFTAP模式状态，类型为bool
        uint16_t            station:1;              ///< 表示WiFi模组所处的STATION模式状态，类型为bool
        uint16_t            onboarding:1;           ///< 表示WiFi模组所处的配置状态，类型为bool
        uint16_t            binding:1;              ///< 表示WiFi模组所处的绑定状态，类型为bool
        uint16_t            con_route:1;            ///< 表示WiFi模组与路由器的连接状态，类型为bool
        uint16_t            con_m2m:1;              ///< 表示WiFi模组与云端m2m的状态，类型为bool
        uint16_t            reserve1:2;             ///< 数据位补齐
        uint16_t            rssi:3;                 ///< 表示路由的信号强度，类型为数值
        uint16_t            app:1;                  ///< 表示WiFi模组与APP端的连接状态，类型为bool
        uint16_t            test:1;                 ///< 表示WiFi模组所处的场测模式状态，类型为bool
        uint16_t            reserve2:3;             ///< 数据位补齐
    }types;
} wifi_status_t;

/** wifi信号强度数值结构体 */
typedef struct {
    uint8_t rssi;                                   ///< WIFI信号强度
}moduleStatusInfo_t;

#pragma pack(1)
/**@name Product Key (产品标识码)
* @{
*/
#define PRODUCT_KEY "abab601b704c46e1a1bb1b62827e2ee4"
/**@} */

/**@name Product Secret (产品密钥)
* @{
*/
#define PRODUCT_SECRET "4578797d463f4c49ba8bd194cb724874"
/**@} */

#define MAX_PACKAGE_LEN    (sizeof(devStatus_t)+sizeof(dataPointFlags_t)+10)                ///< 数据缓冲区最大长度
#define RB_MAX_LEN          (MAX_PACKAGE_LEN*2)     ///< 环形缓冲区最大长度
/**@name 数据点相关定义
* @{
*/
#define SWITCHCONTROL_LEN                           1


#define TIMEON_HOUR_RATIO                         1
#define TIMEON_HOUR_ADDITION                      0
#define TIMEON_HOUR_MIN                           0
#define TIMEON_HOUR_MAX                           23
#define TIMEON_HOUR_LEN                           1
#define TIMEON_MIN_RATIO                         1
#define TIMEON_MIN_ADDITION                      0
#define TIMEON_MIN_MIN                           0
#define TIMEON_MIN_MAX                           59
#define TIMEON_MIN_LEN                           1
#define TIME_DELAY_RATIO                         1
#define TIME_DELAY_ADDITION                      0
#define TIME_DELAY_MIN                           0
#define TIME_DELAY_MAX                           60
#define TIME_DELAY_LEN                           1

/**@} */


/** 可写型数据点 布尔和枚举变量 所占字节大小*/
#define COUNT_BIT 1



/** 可写型数据点 布尔和枚举变量 所占字节大小*/
#define COUNT_W_BIT 1






/** 事件枚举*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                               ///< WiFi SOFTAP 配置事件
  WIFI_AIRLINK,                                     ///< WiFi模块 AIRLINK 配置事件
  WIFI_STATION,                                     ///< WiFi模块 STATION 配置事件
  WIFI_OPEN_BINDING,                                ///< WiFi模块开启绑定事件
  WIFI_CLOSE_BINDING,                               ///< WiFi模块关闭绑定事件
  WIFI_CON_ROUTER,                                  ///< WiFi模块已连接路由事件
  WIFI_DISCON_ROUTER,                               ///< WiFi模块已断开连接路由事件
  WIFI_CON_M2M,                                     ///< WiFi模块已连服务器M2M事件
  WIFI_DISCON_M2M,                                  ///< WiFi模块已断开连服务器M2M事件
  WIFI_OPEN_TESTMODE,                               ///< WiFi模块开启测试模式事件
  WIFI_CLOSE_TESTMODE,                              ///< WiFi模块关闭测试模式事件
  WIFI_CON_APP,                                     ///< WiFi模块连接APP事件
  WIFI_DISCON_APP,                                  ///< WiFi模块断开APP事件
  WIFI_RSSI,                                        ///< WiFi模块信号事件
  WIFI_NTP,                                         ///< 网络时间事件
  TRANSPARENT_DATA,                                 ///< 透传事件
  EVENT_SWITCHCONTROL,
  EVENT_TIMEON_HOUR,
  EVENT_TIMEON_MIN,
  EVENT_TIME_DELAY,
  EVENT_TYPE_MAX                                    ///< 枚举成员数量计算 (用户误删)
} EVENT_TYPE_T;


/** 用户区设备状态结构体*/
typedef_t struct {
  bool valueswitchcontrol;
  uint32_t valuetimeon_hour;
  uint32_t valuetimeon_min;
  uint32_t valuetime_delay;
} dataPoint_t;

/** 用户区设备状态结构体*/
typedef_t struct {
  uint8_t flagswitchcontrol:1;


  uint8_t flagtimeon_hour:1;
  uint8_t flagtimeon_min:1;
  uint8_t flagtime_delay:1;



} dataPointFlags_t;

#define DATAPOINT_FLAG_LEN sizeof(dataPointFlags_t)//所有数据点FLAG占用的最大字节数

/** 对应协议“4.9 设备MCU向WiFi模组主动上报当前状态”中的设备状态"dev_status(11B)" */ 
typedef_t struct {
  uint8_t bitFeildBuf[COUNT_BIT];

  uint8_t valuetimeon_hour;
  uint8_t valuetimeon_min;
  uint8_t valuetime_delay;



} devStatus_t;                                      

/** P0 command 命令码*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x11,             ///< 协议 WiFi模组控制设备 WiFi模组发送
    ACTION_READ_DEV_STATUS      = 0x12,             ///< 协议 WiFi模组读取设备的当前状态 WiFi模组发送
    ACTION_READ_DEV_STATUS_ACK  = 0x13,             ///< 协议 WiFi模组读取设备的当前状态 设备MCU回复
    ACTION_REPORT_DEV_STATUS    = 0X14,             ///< 协议 设备MCU向WiFi模组主动上报当前状态 设备MCU发送
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi到设备MCU透传
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< 设备MCU到WiFi透传
} actionType_t;

typedef_t struct {                           
    dataPointFlags_t  devDatapointFlag;             ///< 数据点占位Flag
    devStatus_t devStatus;                          ///< 设备全数据点状态数据
}gizwitsElongateP0Form_t;  

typedef_t struct {                           
    uint8_t action;
    uint8_t data[sizeof(gizwitsElongateP0Form_t)];
}gizwitsP0Max_t;                                  
                                                    
/** 事件队列结构体 */                               
typedef_t struct {                           
    uint8_t num;                                    ///< 队列成员个数
    uint8_t event[EVENT_TYPE_MAX];                  ///< 队列成员事件内容
}eventInfo_t;


/** 协议处理相关结构体 */
typedef_t struct
{
    uint32_t timerMsCount;                          ///< 时间计数(Ms)
    uint8_t transparentBuff[BUFFER_LEN_MAX];        ///< 透传数据存储区
    uint32_t transparentLen;                        ///< 透传数据长度

    dataPointFlags_t  waitReportDatapointFlag;      ///< 存放待上报的数据点FLAG
    uint8_t reportData[sizeof(gizwitsElongateP0Form_t)];    ///< 协议上报实际数据
    uint32_t reportDataLen;                         ///< 协议上报实际数据长度

    
    dataPoint_t gizCurrentDataPoint;                ///< 当前设备状态数据
    dataPoint_t gizLastDataPoint;                   ///< 上次上报的设备状态数据

    eventInfo_t issuedProcessEvent;                 ///< 控制事件
}gizwitsProtocol_t;

#pragma pack()

/**@name Gizwits 用户API接口
* @{
*/
void gizwitsSetMode(uint8_t mode);
uint32_t gizwitsGetTimeStamp(void);
void gizwitsInit(void);
int8_t gizwitsHandle(dataPoint_t *dataPoint);
int32_t gizwitsPassthroughData(uint8_t * data, uint32_t len);
/**@} */

#endif