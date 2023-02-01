#include "SMSlib.h"
#include "PSGlib.h"
#include "assets.h"
#include "assets.generated.h"

#define H_SCROLL 4u
#define V_SCROLL 0u

unsigned char paused = 0;
unsigned char right_arm = 0;
unsigned char wink = 0;
unsigned char arm_counter = 0;
unsigned char wink_counter = 0;
unsigned char blower = 0;

void build_sprites(void)
{
  SMS_initSprites();
  // santa sprites
  SMS_addSprite(H_SCROLL + 144, V_SCROLL + 40, 32);
  SMS_addSprite(H_SCROLL + 88, V_SCROLL + 48, 33);
  SMS_addSprite(H_SCROLL + 88, V_SCROLL + 56, 34);
  SMS_addSprite(H_SCROLL + 80, V_SCROLL + 64, 35);
  SMS_addSprite(H_SCROLL + 88, V_SCROLL + 64, 36);
  SMS_addSprite(H_SCROLL + 64, V_SCROLL + 72, 37);
  SMS_addSprite(H_SCROLL + 72, V_SCROLL + 72, 38);
  SMS_addSprite(H_SCROLL + 144, V_SCROLL + 72, 39);
  SMS_addSprite(H_SCROLL + 88, V_SCROLL + 80, 40);
  SMS_addSprite(H_SCROLL + 96, V_SCROLL + 80, 41);
  SMS_addSprite(H_SCROLL + 104, V_SCROLL + 80, 42);
  SMS_addSprite(H_SCROLL + 112, V_SCROLL + 80, 43);
  SMS_addSprite(H_SCROLL + 144, V_SCROLL + 80, 44);
  SMS_addSprite(H_SCROLL + 64, V_SCROLL + 88, 45);
  SMS_addSprite(H_SCROLL + 72, V_SCROLL + 88, 46);
  SMS_addSprite(H_SCROLL + 80, V_SCROLL + 88, 47);
  SMS_addSprite(H_SCROLL + 112, V_SCROLL + 88, 48);
  SMS_addSprite(H_SCROLL + 120, V_SCROLL + 88, 49);
  SMS_addSprite(H_SCROLL + 128, V_SCROLL + 88, 50);
  SMS_addSprite(H_SCROLL + 136, V_SCROLL + 88, 51);

  // arm sprites
  if (!right_arm)
  {
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 32, 1);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 32, 2);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 40, 7);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 40, 8);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 48, 13);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 48, 14);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 56, 18);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 56, 19);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 64, 23);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 64, 24);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 72, 28);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 72, 29);
  } else {
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 32, 3);
    SMS_addSprite(H_SCROLL + 167, V_SCROLL + 32, 4);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 40, 9);
    SMS_addSprite(H_SCROLL + 167, V_SCROLL + 40, 10);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 48, 15);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 48, 16);
    SMS_addSprite(H_SCROLL + 167, V_SCROLL + 48, 17);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 56, 20);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 56, 21);
    SMS_addSprite(H_SCROLL + 167, V_SCROLL + 56, 22);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 64, 25);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 64, 26);
    SMS_addSprite(H_SCROLL + 151, V_SCROLL + 72, 30);
    SMS_addSprite(H_SCROLL + 159, V_SCROLL + 72, 31);
  }

  // wink sprites
  if (wink)
  {
    SMS_addSprite(H_SCROLL + 129, V_SCROLL + 47, 5);
    SMS_addSprite(H_SCROLL + 137, V_SCROLL + 47, 6);
    SMS_addSprite(H_SCROLL + 129, V_SCROLL + 55, 11);
    SMS_addSprite(H_SCROLL + 137, V_SCROLL + 55, 12);
  }

  // party blower
  if (!blower)
  {
    SMS_addSprite(H_SCROLL + 128, V_SCROLL + 66, 52);
    SMS_addSprite(H_SCROLL + 128, V_SCROLL + 74, 55);
  }
  else
  {
    SMS_addSprite(H_SCROLL + 127, V_SCROLL + 66, 53);
    SMS_addSprite(H_SCROLL + 127, V_SCROLL + 74, 56);
    SMS_addSprite(H_SCROLL + 135, V_SCROLL + 74, 57);
  }
}

#ifdef TARGET_GG
void EnableGGMode(void)
{
__asm
    ld a, #0xff
    out (#0xdd), a
  __endasm;
}
#endif

void main(void)
{
#ifdef TARGET_GG
  EnableGGMode();
#endif
  SMS_VDPturnOnFeature(VDPFEATURE_LEFTCOLBLANK);
  SMS_setBackdropColor(0);
  SMS_setBGScrollX(H_SCROLL);
#ifdef TARGET_GG
  GG_loadBGPalette(bg_colors);
  GG_loadSpritePalette(sprite_colors);
#else
  SMS_loadBGPalette(bg_colors);
  SMS_loadSpritePalette(sprite_colors);
#endif
  SMS_loadTiles(&bg_tiles, 0, sizeof(bg_tiles));
  SMS_loadTiles(&sprite_tiles, 256, sizeof(sprite_tiles));
  SMS_loadTileMap(0, 0, &screen_map_uncompressed, sizeof(screen_map_uncompressed));

  build_sprites();

  SMS_displayOn();

  PSGPlay(&santa_psg);

  // main game loop
  while (1)
  {
    SMS_waitForVBlank();
    if (!paused)
    {
      PSGFrame();
    }
    SMS_copySpritestoSAT();
#ifndef TARGET_GG
    if (SMS_queryPauseRequested())
    {
      paused = ~paused;
      SMS_resetPauseRequest();
    }
#endif
    if (!paused)
    {
      build_sprites();
      arm_counter++;
      if (arm_counter == 24)
      {
        right_arm = !right_arm;
        arm_counter = 0;
        wink_counter++;
        wink = wink_counter == 15;
        if (wink_counter == 16)
        {
          wink_counter = 0;
        }
      }
      blower = (SMS_getKeysStatus() & (PORT_A_KEY_1 | PORT_A_KEY_2));
    }
  }
}
