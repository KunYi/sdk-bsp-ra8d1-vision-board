
#ifdef __cplusplus

extern "C" {

#include <rtthread.h>
#include <rtdevice.h>
#define DRV_DEBUG
#define LOG_TAG             "gba_porting"
#include <drv_log.h>

}
#endif

#include "gba.h"
#include "globals.h"

extern "C" void InitGba(void);
extern "C" void loadRom(void);
extern "C" void runGBA(void);

static void AllocateBuffer(void)
{
/*
uint8_t *vram;//[0x20000];
uint8_t *workRAM;//[0x40000];
uint8_t *bios;//[0x4000];
uint16_t *pix;//[2 * PIX_BUFFER_SCREEN_WIDTH * 160];
uint8_t *libretro_save_buf;//[0x20000+0x2000];
uint8_t *rom;//[32*1024*1024]
*/
  vram = (uint8_t *)rt_malloc(0x20000);
  workRAM = (uint8_t *)rt_malloc(0x40000);
  bios = (uint8_t *)rt_malloc(0x4000);
  pix = (uint16_t *)rt_malloc(sizeof(uint16_t) * 2 * PIX_BUFFER_SCREEN_WIDTH * 160);
  libretro_save_buf = (uint8_t *)rt_malloc(0x20000+0x2000);

  #define ROM_SIZE  (4*1024*1024)
  uint8_t * rom_a = (uint8_t *)rt_malloc(ROM_SIZE);
  if (rom_a == RT_NULL) {
    LOG_E("Failed allocate buffer for rom_a\n");
  }
  else {
    LOG_I("rom_a: %p", rom_a);
  }

  uint8_t * rom_b = (uint8_t *)rt_malloc(ROM_SIZE);
  if (rom_b == RT_NULL) {
    LOG_E("Failed allocate buffer for rom_b\n");
  }
  else {
    LOG_I("rom_b: %p", rom_b);
  }

  LOG_I("internalRAM: %p, vram: %p, workRAM: %p, bios: %p, pix: %p",
      internalRAM, vram, workRAM, bios, pix);
  LOG_I("libretro_save_buf: %p, rom: %p",
      libretro_save_buf, rom);
}

void InitGba(void) {
  AllocateBuffer();
  CPUSetupBuffers();
  CPUInit(NULL, false);
  CPUReset();
  SetFrameskip(0x1);
}

void loadRom(void) {
    if (CPULoadRom("gba/SuperMary.gba") == 0)
    {
//        LOG_E("vba load error");
    }
}

void runGBA(void) {
    while(1) {
        CPULoop();
        UpdateJoypad();
        rt_thread_mdelay(10);
    }
}

int CPULoadRom(const char * file) {

    return 1;
}

void systemMessage(const char *fmt, ...) {
  char buf[256];
  va_list args;
  va_start(args, fmt);
  rt_snprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  LOG_E("GBA", "%s", buf);
}

void systemDrawScreen(void) {

}

// Sound audio output
void systemOnWriteDataToSoundBuffer(short*, int) {

}
