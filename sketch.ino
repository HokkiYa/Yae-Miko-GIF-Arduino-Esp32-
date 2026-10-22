#include <SPI.h>
#include <TFT_eSPI.h>
#include <AnimatedGIF.h>
#include "animation.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
AnimatedGIF gif;

int gifW = 0;
int gifH = 0;

// Callback: Copy nguyên cả dòng vào buffer của Sprite thay vì vẽ từng pixel lẻ
void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette;
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;
  s = pDraw->pPixels;

  if (pDraw->ucDisposalMethod == 2) {
    for (x = 0; x < iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) s[x] = 0;
    }
  }

  // Lấy con trỏ trực tiếp đến dòng y trong bộ nhớ RAM của Sprite
  uint16_t *linePtr = (uint16_t *)spr.getPointer() + (y * gifW) + pDraw->iX;

  for (x = 0; x < iWidth; x++) {
    uint8_t c = *s++;
    if (c != pDraw->ucTransparent) {
      linePtr[x] = usPalette[c];
    }
  }
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  gif.begin(BIG_ENDIAN_PIXELS);
  if (gif.open((uint8_t *)gif_data, gif_size, GIFDraw)) {
    gifW = gif.getCanvasWidth();
    gifH = gif.getCanvasHeight();
    gif.close();
  }

  spr.setColorDepth(16);
  spr.createSprite(gifW, gifH);
}

void loop() {
  if (gif.open((uint8_t *)gif_data, gif_size, GIFDraw)) {
    int posX = (240 - gifW) / 2;
    int posY = (320 - gifH) / 2;

    // Đổi tham số đầu thành false để BỎ QUA delay gốc của GIF, tự ép tốc độ tối đa
    while (gif.playFrame(false, NULL)) {
      spr.pushSprite(posX, posY);
      // Có thể thêm delay(10) nếu thấy nó chạy quá nhanh
    }
    gif.close();
  }
}