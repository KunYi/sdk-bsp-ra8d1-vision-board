
#ifdef __cplusplus

extern "C" {

#include <rtthread.h>
#include <rtdevice.h>
#define DRV_DEBUG
#define LOG_TAG             "gba_porting"
#include <drv_log.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <drv_lcd.h>
}
#endif

#include "gba.h"
#include "globals.h"

extern "C" void InitGba(void);
extern "C" void loadRom(void);
extern "C" void runGBA(void);

#define SDRAM_START (0x68200000UL)
#define SDRAM_SIZE  (30*1024*1024)
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
  vram = (uint8_t *)rt_malloc(0x20000);    // 128KB
  workRAM = (uint8_t *)rt_malloc(0x40000); // 256KB
  bios = (uint8_t *)rt_malloc(0x4000);     // 16KB
  pix = (uint16_t *)rt_malloc(sizeof(uint16_t) * 2 * PIX_BUFFER_SCREEN_WIDTH * 160); // 160KB
  libretro_save_buf = (uint8_t *)rt_malloc(0x20000+0x2000); // 128KB + 8KB = 136KB

// ROM space should be equal 32MB
// but VisionBoard can't support this configuration
// VisionBoard has a 32MB SDRAM, but first 2MB reserve to Display interface
  rom = (uint8_t*)SDRAM_START; // point to SDRAM
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
    if (CPULoadRom("gba/SuperMario.gba") < 0)
    {
      LOG_E("vba load error");
      return;
    }
    else {
      LOG_I("Load ROM complete");
    }
}

void runGBA(void) {
    while(1) {
        CPULoop();
        UpdateJoypad();
        rt_thread_mdelay(10);
    }
}

int CPULoadRom(const char *file) {
  FILE *f = fopen(file, "rb");
  if (!f) {
    LOG_E("Open ROM Failed.");
    return -1;
  }
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (size > SDRAM_SIZE) {
    LOG_E("ROM size over 30MB is not support");
    fclose(f);
    return -2;
  }
  int bytesRead = fread(rom, 1, size, f);
  fclose(f);
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
    //SCB_CleanInvalidateDCache_by_Addr((uint32_t *)fb16_t, sizeof(fb_background[0]));
    lcd_draw_jpg(0, 0, (uint8_t *)pix, PIX_BUFFER_SCREEN_WIDTH , 160);
}

// Sound audio output
void systemOnWriteDataToSoundBuffer(short*, int) {

}


// bit0 -> bit11
// "a", "b", "select", "start", "right", "left", "up", "down",
// "r", "l",  "turbo", "menu"
