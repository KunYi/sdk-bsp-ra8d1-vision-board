#include <stdint.h>
#include <rtthread.h>
#include <tusb.h>
// #include "port/rt_key_scan.h"

static bool xpad_mounted = false;
static uint8_t xpad_dev_addr = 0;
static uint8_t xpad_instance = 0;
static uint8_t motor_left = 0;
static uint8_t motor_right = 0;

static uint16_t keystate = 0;
static uint16_t oldkeystate = 0;

#define PACKET_HEADER_INPUT    (0x20)
#ifndef BIT
#define BIT(x)  (1U<<(x))
#endif

static inline bool is_xpad(uint8_t dev_addr)
{
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  return ( (vid == 0x045e && pid == 0x0b12) );
}

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_xpad_mount_cb(uint8_t dev_addr, uint8_t idx)
{
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  rt_kprintf("XPAD device address = %d, instance = %d is mounted\r\n", dev_addr, idx);
  rt_kprintf("VID = %04x, PID = %04x\r\n", vid, pid);
  //
  if ( is_xpad(dev_addr) )
  {
    if (!xpad_mounted)
    {
      xpad_dev_addr = dev_addr;
      xpad_instance = idx;
      motor_left = 0;
      motor_right = 0;
      xpad_mounted = true;
    }
    // request to receive report
    // tuh_hid_report_received_cb() will be invoked when report is available
    if ( !tuh_xpad_receive_report(dev_addr, idx) )
    {
      rt_kprintf("Error: cannot request to receive report\r\n");
    }
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_xpad_umount_cb(uint8_t dev_addr, uint8_t idx)
{
  rt_kprintf("XPad device address = %d, interface = %d is unmounted\r\n", dev_addr, idx);
  if (xpad_mounted && xpad_dev_addr == dev_addr)
  {
    xpad_mounted = false;
  }
}

extern rt_mq_t getKeyMQ(void);
void process_xpad(uint8_t const* report, uint16_t len)
{
  if (report[0] == PACKET_HEADER_INPUT) {
    rt_mq_t queKey = getKeyMQ();

    if (queKey == RT_NULL)
       return;

    keystate = report[4] + report[5] * 256;
    if (keystate != oldkeystate) {
      oldkeystate = keystate;
      rt_mq_send(queKey, &oldkeystate, sizeof(oldkeystate));
    }
  }
}

void tuh_xpad_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  if ( is_xpad(dev_addr) )
  {
    process_xpad(report, len);
  }

  // continue to request to receive report
  if ( !tuh_xpad_receive_report(dev_addr, instance) )
  {
    rt_kprintf("Error: cannot request to receive report\r\n");
  }
}
