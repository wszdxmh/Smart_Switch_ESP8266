/**
************************************************************
* @file         user_main.c
* @brief        SOC版 入口文件
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
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gagent_soc.h"
#include "user_devicefind.h"
#include "user_webserver.h"
#include "gizwits_protocol.h"
#include "driver/hal_key.h"


#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#else
#ifdef CLIENT_SSL_ENABLE
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;
#endif
#endif


/**@name Gizwits模块相关系统任务参数
* @{
*/
#define userQueueLen    200                                                 ///< 消息队列总长度
LOCAL os_event_t userTaskQueue[userQueueLen];                               ///< 消息队列
/**@} */

/**@name 用户定时器相关参数
* @{
*/
#define USER_TIME_MS 1000                                                    ///< 定时时间，单位：毫秒
LOCAL os_timer_t userTimer;                                                 ///< 用户定时器结构体

#define GETNTPTIME 1000
LOCAL os_timer_t getntpTimer;

#define SCAN_TIME_MS 10
LOCAL os_timer_t scanTimer;
/**@} */ 

/**@name 按键相关定义 
* @{
*/
#define GPIO_KEY_NUM                            2                           ///< 定义按键成员总数
#define KEY_0_IO_MUX                            PERIPHS_IO_MUX_GPIO0_U      ///< ESP8266 GPIO 功能
#define KEY_0_IO_NUM                            0                           ///< ESP8266 GPIO 编号
#define KEY_0_IO_FUNC                           FUNC_GPIO0                  ///< ESP8266 GPIO 名称
#define KEY_1_IO_MUX                            PERIPHS_IO_MUX_MTMS_U       ///< ESP8266 GPIO 功能
#define KEY_1_IO_NUM                            14                          ///< ESP8266 GPIO 编号
#define KEY_1_IO_FUNC                           FUNC_GPIO14                 ///< ESP8266 GPIO 名称
LOCAL key_typedef_t * singleKey[GPIO_KEY_NUM];                              ///< 定义单个按键成员数组指针
LOCAL keys_typedef_t keys;                                                  ///< 定义总的按键模块结构体指针    
/**@} */

/** 用户区当前设备状态结构体*/
dataPoint_t currentDataPoint;

/**
* key1按键短按处理
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key1ShortPress(void)
{
    os_printf("#### KEY1 short press ,Production Mode\n");
    gizMSleep();
    gizwitsSetMode(WIFI_PRODUCTION_TEST);
}

/**
* key1按键长按处理
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key1LongPress(void)
{
    os_printf("#### key1 long press, default setup\n");
    gizMSleep();
    gizwitsSetMode(WIFI_RESET_MODE);
}

/**
* key2按键短按处理
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key2ShortPress(void)
{
    os_printf("#### key2 short press, soft ap mode \n");

    gizwitsSetMode(WIFI_SOFTAP_MODE);
}

/**
* key2按键长按处理
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR key2LongPress(void)
{
    os_printf("#### key2 long press, airlink mode\n");

    gizwitsSetMode(WIFI_AIRLINK_MODE);
}

/**
* 按键初始化
* @param none
* @return none
*/
LOCAL void ICACHE_FLASH_ATTR keyInit(void)
{
    singleKey[0] = keyInitOne(KEY_0_IO_NUM, KEY_0_IO_MUX, KEY_0_IO_FUNC,
                                key1LongPress, key1ShortPress);
    singleKey[1] = keyInitOne(KEY_1_IO_NUM, KEY_1_IO_MUX, KEY_1_IO_FUNC,
                                key2LongPress, key2ShortPress);
    keys.singleKey = singleKey;
    keyParaInit(&keys);
}

/**
* 用户数据获取

* 此处需要用户实现除可写数据点之外所有传感器数据的采集,可自行定义采集频率和设计数据过滤算法
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR userTimerFunc(void)
{
        /*

    */   
    system_os_post(USER_TASK_PRIO_0, SIG_UPGRADE_DATA, 0);
}

/**
* @brief 获取网络时间回调函数

* 在该函数中添加相应事件的处理
* @param none
* @return none
*/
uint8_t switch_flag;
uint8_t timeon_flag;
_tm ntptimeStr;
extern uint32_t ntpdelay;
extern uint32_t delay_time;
extern _tm ntptimeon;
void ICACHE_FLASH_ATTR getntpTimerFunc(void)
{
    uint32_t ntptime = 0;
    if(switch_flag == 1)
    {
        ntptime = gizwitsGetTimeStamp();
        os_printf("ntp: %d",ntptime);
        os_printf("data: %s\r\n",sntp_get_real_time(ntptime));
        currentDataPoint.valuetime_delay = delay_time - (ntptime - ntpdelay)/60;
        if(ntptime >= (ntpdelay + delay_time*60))
        {
            GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
            currentDataPoint.valueswitchcontrol = 0;
            currentDataPoint.valuetime_delay = 0;
            switch_flag = 0;
        }
    }
    gagentGetNTP(&ntptimeStr);
    os_printf("gagentntp secon: %d",ntptimeStr.second);
    if((currentDataPoint.valuetimeon_hour != 0) || (currentDataPoint.valuetimeon_min != 0))
    {
        if(ntptimeStr.hour == currentDataPoint.valuetimeon_hour)
            if(ntptimeStr.minute == currentDataPoint.valuetimeon_min)
            {
                if(timeon_flag == 0)
                {
                    GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
                    currentDataPoint.valueswitchcontrol = 1;
                    timeon_flag = 1;
                }
            }
            else
            {
                timeon_flag = 0;
            }
    }
}

LOCAL uint8_t key_scan[2] = {0, 0};
void ICACHE_FLASH_ATTR scanTimerFunc(void)
{
    key_scan[0] = key_scan[1];
    key_scan[1] = GPIO_INPUT_GET(GPIO_ID_PIN(5));
    if(key_scan[0] != key_scan[1])
    {
        if(currentDataPoint.valueswitchcontrol == 0x01)
        {
            currentDataPoint.valueswitchcontrol = 0;
            GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
        }
        else
        {
            currentDataPoint.valueswitchcontrol = 0x01;
            GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
        }
    }
}

/**
* @brief 开关中断函数

* 在该函数中完成中断操作
* @param none
* @return none
*/
static void gpio_interrup_1(void)
{
    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    ETS_GPIO_INTR_DISABLE();
    os_printf("inter interrup GPIO!\r\n");
    if(currentDataPoint.valueswitchcontrol == 0x01)
    {
        currentDataPoint.valueswitchcontrol = 0;
        GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
    }
    else
    {
        currentDataPoint.valueswitchcontrol = 0x01;
        GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
    }
    ETS_GPIO_INTR_ENABLE();
}

/**
* @brief 用户相关系统事件回调函数

* 在该函数中用户可添加相应事件的处理
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR gizwitsUserTask(os_event_t * events)
{
    uint8_t i = 0;
    uint8_t vchar = 0;

    if(NULL == events)
    {
        os_printf("!!! gizwitsUserTask Error \n");
    }

    vchar = (uint8)(events->par);

    switch(events->sig)
    {
    case SIG_UPGRADE_DATA:
        gizwitsHandle((dataPoint_t *)&currentDataPoint);
        break;
    default:
        os_printf("---error sig! ---\n");
        break;
    }
}

/**
* @brief 引脚初始化程序

* 在该函数中完成GPIO的初始化
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR GPIO_Init(void)
{
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    /*开中断，双向沿触发*/
    // PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    // GPIO_DIS_OUTPUT(GPIO_ID_PIN(5));
    // PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    // ETS_GPIO_INTR_DISABLE();
    // ETS_GPIO_INTR_ATTACH(gpio_interrup_1, NULL);
    // gpio_pin_intr_state_set(GPIO_ID_PIN(5), GPIO_PIN_INTR_ANYEDGE);
    // ETS_GPIO_INTR_ENABLE();

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(5));
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    do{
        key_scan[0] = GPIO_INPUT_GET(GPIO_ID_PIN(5));
        key_scan[1] = GPIO_INPUT_GET(GPIO_ID_PIN(5));
    }while(key_scan[0] != key_scan[1]);
    // gpio16_output_conf();
}

/**
* @brief user_rf_cal_sector_set

* 用flash中的636扇区(2544k~2548k)存储RF_CAL参数
* @param none
* @return none
*/
uint32 user_rf_cal_sector_set()
{
    return 636;
}

/**
* @brief 程序入口函数

* 在该函数中完成用户相关的初始化
* @param none
* @return none
*/
void ICACHE_FLASH_ATTR user_init(void)
{
    uint32 system_free_size = 0;

    wifi_station_set_auto_connect(1);
    wifi_set_sleep_type(NONE_SLEEP_T);//set none sleep mode
    espconn_tcp_set_max_con(10);
    uart_init_3(9600,115200);
    UART_SetPrintPort(1);
    os_printf( "---------------SDK version:%s--------------\n", system_get_sdk_version());
    os_printf( "system_get_free_heap_size=%d\n",system_get_free_heap_size());

    struct rst_info *rtc_info = system_get_rst_info();
    os_printf( "reset reason: %x\n", rtc_info->reason);
    if (rtc_info->reason == REASON_WDT_RST ||
        rtc_info->reason == REASON_EXCEPTION_RST ||
        rtc_info->reason == REASON_SOFT_WDT_RST)
    {
        if (rtc_info->reason == REASON_EXCEPTION_RST)
        {
            os_printf("Fatal exception (%d):\n", rtc_info->exccause);
        }
        os_printf( "epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x, depc=0x%08x\n",
                rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr, rtc_info->depc);
    }

    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1)
    {
        os_printf( "---UPGRADE_FW_BIN1---\n");
    }
    else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2)
    {
        os_printf( "---UPGRADE_FW_BIN2---\n");
    }

    //user init
    keyInit();
    GPIO_Init();
    sntp_stop();
    if(true == sntp_set_timezone(-5))
    {
        sntp_init();
    }
    
    //gizwits InitSIG_UPGRADE_DATA
    os_memset((uint8_t *)&currentDataPoint, 0, sizeof(dataPoint_t));
    gizwitsInit();

    system_os_task(gizwitsUserTask, USER_TASK_PRIO_0, userTaskQueue, userQueueLen);

    //user timer 
    os_timer_disarm(&userTimer);
    os_timer_setfn(&userTimer, (os_timer_func_t *)userTimerFunc, NULL);
    os_timer_arm(&userTimer, USER_TIME_MS, 1);
    
    os_timer_disarm(&getntpTimer);
    os_timer_setfn(&getntpTimer, (os_timer_func_t *)getntpTimerFunc, NULL);
    os_timer_arm(&getntpTimer, GETNTPTIME, 1);

    os_timer_disarm(&scanTimer);
    os_timer_setfn(&scanTimer, (os_timer_func_t *)scanTimerFunc, NULL);
    os_timer_arm(&scanTimer, SCAN_TIME_MS, 1);

    os_printf("--- system_free_size = %d ---\n", system_get_free_heap_size());
}
