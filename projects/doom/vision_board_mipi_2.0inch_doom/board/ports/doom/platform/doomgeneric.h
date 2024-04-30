#ifndef DOOM_GENERIC
#define DOOM_GENERIC

#include <stdlib.h>
#include <stdint.h>

#include <rtthread.h>

void DG_Init();
void DG_DrawFrame();
void DG_SleepMs(uint32_t ms);
uint32_t DG_GetTicksMs();
int DG_GetKey(uint16_t *keystate);
void DG_SetWindowTitle(const char *title);
rt_mq_t DG_getKeyMsgQue(void);

#endif // DOOM_GENERIC
