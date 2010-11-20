#include "test.h"
#include <math.h>
#include <ctype.h>
#include <jpeglib.h>

#define MYABS(a) (a < 0.0f ? -a : a)

#ifndef IMAGE_DIR
#  define IMAGE_DIR "./"
#endif

typedef struct
{
  VGfloat from[16];
  VGfloat to[16];
  VGfloat value[16];
  VGfloat fromAlpha;
  VGfloat valueAlpha;
  VGfloat toAlpha;
  VGPaint fillStack;
  VGPaint fillWater;
  VGfloat offset;
  VGint play;
} CoverState;

int coversCount = 5;
VGImage covers[5];
CoverState coverStates[5];

int coversCenter = 2;
int drawCenter = 2;
VGfloat swaptime = 0.5;
VGfloat stackAlphaBack = 0.5f;
VGfloat stackAlphaFront = 1.0f;

VGPath frame;
VGPaint frameStroke;
VGPaint frameWaterStroke;
VGfloat white[4] = {1,1,1,1};

VGPath btnPrev;
VGPath btnPrevShine;
VGPath btnPrevArrow;
VGPath btnNext;
VGPath btnNextShine;
VGPath btnNextArrow;
VGfloat btnOffsetY = 100;
int btnPrevOver = 0;
int btnNextOver = 0;

VGPaint btnPaint;
VGPaint btnOverPaint;
VGPaint btnShinePaint;
VGfloat btnColor[4] = {0,0,0,1};
VGfloat btnOverColor[4] = {0.1,0.1,0.1,1};
VGfloat btnShineColor[4] = {0.4,0.4,0.4,0.4};

void copyMatrix(VGfloat *dst, VGfloat *src)
{
  int i;
  for (i=0; i<16; ++i)
    dst[i] = src[i];
}

void copyColor(VGfloat *dst, VGfloat *src)
{
  int i;
  for (i=0; i<4; ++i)
    dst[i] = src[i];
}

void updateStackFill(VGPaint f, float alpha)
{
  VGfloat radial[] = {200,300,200,300,250};
  
  VGfloat stops[] = {
    0.0, 1.0, 1.0, 1.0, 1.0,
    0.5, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0 };
  
  VGint stopsize = sizeof(stops) / sizeof(VGfloat);
  VGint numstops = stopsize / 5;
  
  VGint s, c;
  for (s=0; s<numstops; ++s)
    for (c=1; c<=3; ++c)
      stops[s * 5 + c] = alpha;
  
  stops[14] = alpha;
  
  vgSetParameteri(f, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
  vgSetParameterfv(f, VG_PAINT_RADIAL_GRADIENT, 5, radial);
  vgSetParameterfv(f, VG_PAINT_COLOR_RAMP_STOPS, stopsize, stops);
}

void updateWaterFill(VGPaint f, float alpha)
{
  VGfloat linear[] = {0,0,0,190};
  
  VGfloat stops[] = {
    0.0, 1.0, 1.0, 1.0, 1.0f,
    1.0, 1.0, 1.0, 1.0, 0.0f};
  
  VGint numstops = sizeof(stops) / sizeof(VGfloat);
  
  stops[4] = alpha;
  vgSetParameteri(f, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
  vgSetParameterfv(f, VG_PAINT_LINEAR_GRADIENT, 4, linear);
  vgSetParameterfv(f, VG_PAINT_COLOR_RAMP_STOPS, numstops, stops);
}

void interpolateCoverState(CoverState *c, float interval)
{
  int i;
  VGfloat d;
  VGfloat o;
  
  c->offset += interval * 2;
  
  if (c->offset >= 1.0f) {
    c->offset = 1.0f;
    c->play = 0;
  }
  
  o = sqrt(c->offset);
  
  for (i=0; i<16; ++i) {
    d = c->to[i] - c->from[i];
    c->value[i] = c->from[i] + o * d;
  }
  
  d = c->toAlpha - c->fromAlpha;
  c->valueAlpha = c->fromAlpha + o * d;
  
  updateStackFill(c->fillStack, c->valueAlpha);
  updateWaterFill(c->fillWater, c->valueAlpha*0.5f);
  
  drawCenter = coversCenter;
}

void updateCoverStates(int init)
{
  int ci; CoverState *c;
  VGfloat alpha = 1.0f;
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  
  for (ci=0; ci<coversCount; ++ci) {
    c = &coverStates[ci];
    
    vgLoadIdentity();
    vgTranslate(testWidth()/2, testHeight()/2);
    
    if (ci == coversCenter) {
      alpha = stackAlphaFront;
    }else{
      VGfloat ds = ci - coversCenter;
      VGfloat d = MYABS(ds);
      VGfloat sign = ds / d;
      VGfloat s = 1 - 0.1 - d*0.05;
      VGfloat t = sign * (200 + (d-1) * 50);
      vgTranslate(t, 0);
      vgScale(s,s);
      alpha = stackAlphaBack - d*0.05;
    }
    
    if (init) {
      c->play = 0;
      c->offset = 0.0f;
      vgGetMatrix(c->value);
      c->valueAlpha = alpha;
      c->fillStack = vgCreatePaint();
      c->fillWater = vgCreatePaint();
      updateStackFill(c->fillStack, alpha);
      updateWaterFill(c->fillWater, alpha*0.5f);
    }else{
      c->play = 1;
      c->offset = 0.0f;
      copyMatrix(c->from, c->value);
      vgGetMatrix(c->to);
      c->fromAlpha = c->valueAlpha;
      c->toAlpha = alpha;
    }
  }
}

void drawCover(int c)
{
  /* Position image on stack */
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgLoadMatrix(coverStates[c].value);

  /* Center image and draw */
  vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
  vgSetPaint(coverStates[c].fillStack, VG_FILL_PATH);
  vgTranslate(-200,-200);
  vgDrawImage(covers[c]);
  
  /* Clear reflection below */
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadMatrix(coverStates[c].value);
  vgTranslate(-200,-200);
  vgScale(1, -1);
  vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
  vgDrawPath(frame, VG_FILL_PATH);
  
  /* Mirror water image */
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
  vgSetPaint(coverStates[c].fillWater, VG_FILL_PATH);
  vgScale(1, -1);
  vgDrawImage(covers[c]);
  
  /* Draw frame */ 
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadMatrix(coverStates[c].value);
  vgTranslate(-200.5,-200.5);
  vgSetPaint(frameStroke, VG_STROKE_PATH);
  vgSetf(VG_STROKE_LINE_WIDTH, 1.0f);
  vgDrawPath(frame, VG_STROKE_PATH);
  
  /* Mirror water frame */
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_STROKE_PAINT_TO_USER);
  vgLoadIdentity();
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgSetPaint(frameWaterStroke, VG_STROKE_PATH);
  vgScale(1, -1);
  vgDrawPath(frame, VG_STROKE_PATH);
}

void display(float interval)
{
  VGfloat white[] = {0,0,0,1};
  VGint c;
  
  vgSetfv(VG_CLEAR_COLOR, 4, white);
  vgClear(0, 0, testWidth(), testHeight());
  
  for (c=0; c<coversCount; ++c)
    if (coverStates[c].play)
      interpolateCoverState(&coverStates[c], interval);
  
  for (c=0; c!=drawCenter; ++c)
    drawCover(c);
  
  for (c=coversCount-1; c!=drawCenter; --c)
    drawCover(c);
  
  drawCover(drawCenter);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgTranslate(testWidth()/2, btnOffsetY);
  
  vgSetPaint(frameStroke, VG_STROKE_PATH);
  vgSetPaint(btnPrevOver ? btnOverPaint : btnPaint, VG_FILL_PATH);
  vgDrawPath(btnPrev, VG_FILL_PATH | VG_STROKE_PATH);
  vgSetPaint(btnNextOver ? btnOverPaint : btnPaint, VG_FILL_PATH);
  vgDrawPath(btnNext, VG_FILL_PATH | VG_STROKE_PATH);
  
  vgSetPaint(btnShinePaint, VG_FILL_PATH);
  vgDrawPath(btnPrevShine, VG_FILL_PATH);
  vgDrawPath(btnNextShine, VG_FILL_PATH);
  
  vgSetPaint(frameStroke, VG_FILL_PATH);
  vgDrawPath(btnPrevArrow, VG_FILL_PATH);
  vgDrawPath(btnNextArrow, VG_FILL_PATH);
}

void specialKey(int key, int x, int y)
{
  switch(key)
  {
  case GLUT_KEY_LEFT:
    if (coversCenter > 0) --coversCenter;
    updateCoverStates(0);
    break;
  case GLUT_KEY_RIGHT:
    if (coversCenter < coversCount-1) ++coversCenter;
    updateCoverStates(0);
    break;
  }
}

int isPointInBoundbox(VGPath p, float x, float y)
{
  VGfloat minX=0.0f, minY=0.0f;
  VGfloat width=0.0f, height=0.0f;
  vgPathTransformedBounds(p, &minX, &minY, &width, &height);
  return (x >= minX && x <= minX + width &&
          y >= minY && y <= minY + height);
}

void click(int button, int state, int x, int y)
{
  if (state == GLUT_UP) return;
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgTranslate(testWidth()/2, btnOffsetY);
 
  y = testHeight() - y;
  if (isPointInBoundbox(btnPrev, (float)x, (float)y))
    specialKey(GLUT_KEY_LEFT, x, y);
  else if (isPointInBoundbox(btnNext, (float)x, (float)y))
    specialKey(GLUT_KEY_RIGHT, x, y);
}

void move(int x, int y)
{
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgTranslate(testWidth()/2, btnOffsetY);
 
  y = testHeight() - y;
  btnPrevOver = 0;
  btnNextOver = 0;

  if (isPointInBoundbox(btnPrev, (float)x, (float)y))
    btnPrevOver = 1;
  else if (isPointInBoundbox(btnNext, (float)x, (float)y))
    btnNextOver = 1;
}

VGImage createImageFromJpeg(const char *filename)
{
  FILE *infile;
  struct jpeg_decompress_struct jdc;
  struct jpeg_error_mgr jerr;
  JSAMPARRAY buffer;  
  unsigned int bstride;
  unsigned int bbpp;

  VGImage img;
  VGubyte *data;
  unsigned int width;
  unsigned int height;
  unsigned int dstride;
  unsigned int dbpp;
  
  VGubyte *brow;
  VGubyte *drow;
  unsigned int x;
  unsigned int lilEndianTest = 1;
  VGImageFormat rgbaFormat;

  /* Check for endianness */
  if (((unsigned char*)&lilEndianTest)[0] == 1)
    rgbaFormat = VG_lABGR_8888;
  else rgbaFormat = VG_lRGBA_8888;
  
  /* Try to open image file */
  infile = fopen(filename, "rb");
  if (infile == NULL) {
    printf("Failed opening '%s' for reading!\n", filename);
    return VG_INVALID_HANDLE; }
  
  /* Setup default error handling */
  jdc.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&jdc);
  
  /* Set input file */
  jpeg_stdio_src(&jdc, infile);
  
  /* Read header and start */
  jpeg_read_header(&jdc, TRUE);
  jpeg_start_decompress(&jdc);
  width = jdc.output_width;
  height = jdc.output_height;
  
  /* Allocate buffer using jpeg allocator */
  bbpp = jdc.output_components;
  bstride = width * bbpp;
  buffer = (*jdc.mem->alloc_sarray)
    ((j_common_ptr) &jdc, JPOOL_IMAGE, bstride, 1);
  
  /* Allocate image data buffer */
  dbpp = 4;
  dstride = width * dbpp;
  data = (VGubyte*)malloc(dstride * height);
  
  /* Iterate until all scanlines processed */
  while (jdc.output_scanline < height) {
    
    /* Read scanline into buffer */
    jpeg_read_scanlines(&jdc, buffer, 1);    
    drow = data + (height-jdc.output_scanline) * dstride;
    brow = buffer[0];
    
    /* Expand to RGBA */
    for (x=0; x<width; ++x, drow+=dbpp, brow+=bbpp) {
      switch (bbpp) {
      case 4:
        drow[0] = brow[0];
        drow[1] = brow[1];
        drow[2] = brow[2];
        drow[3] = brow[3];
        break;
      case 3:
        drow[0] = brow[0];
        drow[1] = brow[1];
        drow[2] = brow[2];
        drow[3] = 255;
        break; }
    }
  }
  
  /* Create VG image */
  img = vgCreateImage(rgbaFormat, width, height, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(img, data, dstride, rgbaFormat, 0, 0, width, height);
  
  /* Cleanup */
  jpeg_destroy_decompress(&jdc);
  fclose(infile);
  free(data);
  
  return img;
}

void createButton(VGPath b)
{
  testMoveTo(b, 0,0, VG_ABSOLUTE);
  testLineTo(b, -15,0, VG_RELATIVE);
  testArcTo(b,20,20,0,0,-40, VG_SCCWARC_TO, VG_RELATIVE);
  testLineTo(b, 15,0, VG_RELATIVE);
  testClosePath(b);
}

void createArrow(VGPath a)
{
  testMoveTo(a, -12,-12, VG_ABSOLUTE);
  testLineTo(a, -8,-8, VG_RELATIVE);
  testLineTo(a, 8, -8, VG_RELATIVE);
  testClosePath(a);
}

void createButtons()
{
  btnPrev = testCreatePath();
  createButton(btnPrev);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgScale(-1,1);
  
  btnNext = testCreatePath();
  vgTransformPath(btnNext,btnPrev);
  
  vgLoadIdentity();
  vgTranslate(0,-1);
  vgScale(0.9,0.5);
  
  btnPrevShine = testCreatePath();
  vgTransformPath(btnPrevShine, btnPrev);
  
  btnNextShine = testCreatePath();
  vgTransformPath(btnNextShine, btnNext);
  
  btnPrevArrow = testCreatePath();
  createArrow(btnPrevArrow);
  
  btnPaint = vgCreatePaint();
  vgSetParameterfv(btnPaint, VG_PAINT_COLOR, 4, btnColor);

  btnOverPaint = vgCreatePaint();
  vgSetParameterfv(btnOverPaint, VG_PAINT_COLOR, 4, btnOverColor);

  btnShinePaint = vgCreatePaint();
  vgSetParameterfv(btnShinePaint, VG_PAINT_COLOR, 4, btnShineColor);
  
  vgLoadIdentity();
  vgScale(-1,1);
  
  btnNextArrow = testCreatePath();
  vgTransformPath(btnNextArrow, btnPrevArrow);
}

int main(int argc, char **argv)
{
  testInit(argc, argv, 1000,800, "ShivaVG: Image Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_SPECIALKEY, (CallbackFunc)specialKey);
  testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc)click);
  testCallback(TEST_CALLBACK_MOVE, (CallbackFunc)move);
  testOverlayColor(1,1,1,1);
  
  covers[0] = createImageFromJpeg(IMAGE_DIR"test_img_guitar.jpg");
  covers[1] = createImageFromJpeg(IMAGE_DIR"test_img_piano.jpg");
  covers[2] = createImageFromJpeg(IMAGE_DIR"test_img_violin.jpg");
  covers[3] = createImageFromJpeg(IMAGE_DIR"test_img_flute.jpg");
  covers[4] = createImageFromJpeg(IMAGE_DIR"test_img_sax.jpg");
  
  frame = testCreatePath();
  vguRect(frame,0.0,0.0,400,400);
  
  frameStroke = vgCreatePaint();
  vgSetParameterfv(frameStroke, VG_PAINT_COLOR, 4, white);

  frameWaterStroke = vgCreatePaint();
  updateWaterFill(frameWaterStroke, 1);
  
  createButtons();
 
  updateCoverStates(1);
  testRun();
  
  return EXIT_SUCCESS;
}
