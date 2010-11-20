#include "test.h"
/*#include "picu.h"*/

VGPath src;
VGPath dst;
VGPaint srcFill;
VGPaint dstFill;
VGImage isrc;
VGImage idst;

VGfloat srcColor[4] = {0.4, 0.6, 1.0, 1};
VGfloat dstColor[4] = {1, 1, 0.3, 1};

#ifndef IMAGE_DIR
#  define IMAGE_DIR "./"
#endif

VGBlendMode blends[5] = {
  VG_BLEND_SRC,
  VG_BLEND_SRC_OVER,
  VG_BLEND_DST_OVER,
  VG_BLEND_SRC_IN,
  VG_BLEND_DST_IN
};
/*
void createOperands()
{
  PICUImage srcImage, dstImage;
  VGfloat clear[] = {1,0,0,0};

  src = testCreatePath();
  vguEllipse(src, 30,30,40,40);

  dst = testCreatePath();
  vguRect(dst, 0,0,40,30);

  srcFill = vgCreatePaint();
  vgSetParameterfv(srcFill, VG_PAINT_COLOR, 4, srcColor);

  dstFill = vgCreatePaint();
  vgSetParameterfv(dstFill, VG_PAINT_COLOR, 4, dstColor);
  
  picuReadFile(&srcImage, IMAGE_DIR"test_blend_src.png", "PNG");
  picuReadFile(&dstImage, IMAGE_DIR"test_blend_dst.png", "PNG");

  isrc = vgCreateImage(VG_sRGBA_8888, srcImage.width,
                       srcImage.height, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(isrc, srcImage.data, srcImage.stride, VG_sRGBA_8888,
                  0,0, srcImage.width, srcImage.height);

  idst = vgCreateImage(VG_sRGBA_8888, dstImage.width,
                       dstImage.height, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(idst, dstImage.data, dstImage.stride, VG_sRGBA_8888,
                  0,0, dstImage.width, dstImage.height);
}*/

void display(float interval)
{
  VGfloat clear[] = {0,0,0,0};
  
  vgSetfv(VG_CLEAR_COLOR, 4, clear);
  vgClear(0, 0, testWidth(), testHeight());

  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  
  vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
  vgDrawImage(idst);
  vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_IN);
  vgDrawImage(isrc);
}

int main(int argc, char **argv)
{
  /*picuInit();*/

  testInit(argc, argv, 400,400, "ShivaVG: Blending Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testOverlayColor(1,1,1,1);
  testOverlayString("Not implemented yet");

  /*createOperands();*/
  testRun();
  
  return EXIT_SUCCESS;
}
