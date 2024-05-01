#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rt_key_scan.h>


/* 配置 KEY 输入引脚  */
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)      // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)      // PC4 :  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY

// rt_event_t key_event;
rt_align(RT_ALIGN_SIZE)
static uint16_t msgPoolKey[16];
static struct rt_messagequeue mq_key;
static unsigned char  msgKey = 0;

rt_mq_t getKeyMQ(void) {
    return (msgKey > 0) ? (rt_mq_t) &mq_key : (rt_mq_t)RT_NULL;
}

void key_scan_init()
{
    // message queue for key event
    rt_err_t result = rt_mq_init(&mq_key, "queKey",
                    msgPoolKey,
                    sizeof(uint16_t),
                    sizeof(uint16_t)*8,
                    RT_IPC_FLAG_FIFO);

    if (result != RT_EOK) {
        rt_kprintf("Error: create message queue for keyevent\r\n");
    }
    else {
        msgKey = 1;;
    }
}
