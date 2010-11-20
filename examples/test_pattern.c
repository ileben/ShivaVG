#include "test.h"
#include <ctype.h>
#include <math.h>
#include <jpeglib.h>

#ifndef IMAGE_DIR
#  define IMAGE_DIR "./"
#endif

VGfloat tx=0.0f, ty=0.0f;
VGfloat sx=1.0f, sy=1.0f;
VGfloat a=45.0f;

VGint tindex = 3;
VGint tsize = 4;
VGint tile[] = {
  VG_TILE_FILL,
  VG_TILE_PAD,
  VG_TILE_REPEAT,
  VG_TILE_REFLECT
};

VGfloat sqx = 200;
VGfloat sqy = 200;

VGfloat clickX;
VGfloat clickY;
VGfloat startX;
VGfloat startY;
char mode = 't';

VGImage backImage;
VGImage patternImage;
VGPaint patternFill;
VGPaint blackFill;
VGPath org;
VGPath p;

VGfloat black[] = {1,1,1, 1};

const char commands[] =
  "Click & drag mouse to change\n"
  "value for current mode\n\n"
  "H - this help\n"
  "TAB - pattern tiling mode\n"
  "T - translate mode\n"
  "X - scale X mode\n"
  "Y - scale Y mode\n"
  "BACKSPACE - reset value\n";

#if defined(__APPLE__)
#  define BACKSPACE 127
#else
#  define BACKSPACE 8
#endif

#define TAB 9

void display(float interval)
{
  VGfloat cc[] = {0,0,0,1};
  
  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
  vgLoadIdentity();
  vgTranslate(tx, ty);
  vgScale(sx, sy);
  vgRotate(a);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
  vgLoadIdentity();
  
  vgSetPaint(patternFill, VG_FILL_PATH);
  /*vgDrawPath(p, VG_FILL_PATH);*/
  vgDrawImage(backImage);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgTranslate(tx, ty);
  vgScale(sx, sy);
  vgRotate(a);
  
  vgSetPaint(blackFill, VG_FILL_PATH | VG_STROKE_PATH);
  vgDrawPath(org, VG_FILL_PATH);
}

void createPattern()
{
  VGfloat tileFill[] = {0,0,1,1};
  vgSetParameteri(patternFill, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
  vgSetParameteri(patternFill, VG_PAINT_PATTERN_TILING_MODE, tile[tindex]);
  vgSetfv(VG_TILE_FILL_COLOR, 4, tileFill);
  vgPaintPattern(patternFill, patternImage);
}

void updateOverlayString()
{
  switch (mode) {
  case 't':
    testOverlayString("Translate mode: %f,%f", tx,ty); break;
  case 'x':
    testOverlayString("Scale X mode: %f", sx); break;
  case 'y':
    testOverlayString("Scale Y mode: %f", sy); break;
  }
}

void drag(int x, int y)
{
  VGfloat dx, dy;
  y = testHeight() - y;
  dx = x - clickX;
  dy = y - clickY;
  
  switch(mode) {
  case 't': {
      tx = startX + dx;
      ty = startY + dy;
      break;}
  case 'x':
    sx = startY + dy * 0.01;
    break;
  case 'y':
    sy = startY + dy * 0.01;
    break;
  }
  
  updateOverlayString();
  glutPostRedisplay();
}

void click(int button, int state, int x, int y)
{
  y = testHeight() - y;
  clickX = x; clickY = y;
  
  switch (mode)
  {
  case 't':
    startX = tx;
    startY = ty;
    break;
  case 'x':
    startY = sx;
    break;
  case 'y':
    startY = sy;
    break;
  }
  
  glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
  glutPostRedisplay();
  
  if (key == BACKSPACE) {
    /* Reset value */
    switch (mode) {
    case 't':
      tx = 0;
      ty = 0;
      break;
    case 'x':
      sx = 1.0f;
      break;
    case 'y':
      sy = 1.0f;
      break;
    }
    updateOverlayString();
    return;
  }
    

  switch (tolower(key)) {
  case 't':
  case 'x':
  case 'y':
    break;
    
  case TAB:
    
    /* Cycle spread mode */
    tindex = (tindex+1) % tsize;
    createPattern();
    switch(tindex) {
    case 0: testOverlayString("Pattern Tiling mode: FILL\n"); break;
    case 1: testOverlayString("Pattern Tiling mode: PAD\n"); break;
    case 2: testOverlayString("Pattern Tiling mode: REPEAT\n"); break;
    case 3: testOverlayString("Pattern Tiling mode: REFLECT\n"); break;
    }return;
    
  case 'h':
    /* Show help */
    testOverlayString(commands);
    return;
    
  default:
    return;
  }
  
  /* Switch mode */
  mode = tolower(key);
  updateOverlayString();
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

void createSquare(VGPath p)
{
  testMoveTo(p, (testWidth()-sqx)/2, (testHeight()-sqy)/2, VG_ABSOLUTE);
  testLineTo(p, sqx, 0, VG_RELATIVE);
  testLineTo(p, 0, sqy, VG_RELATIVE);
  testLineTo(p, -sqx, 0, VG_RELATIVE);
  testClosePath(p);
}

void createOrigin(VGPath p)
{
  vguEllipse(p, 0,0,10,10);
}

void reshape(int x, int y)
{
  vgClearPath(p, VG_PATH_CAPABILITY_ALL);
  createSquare(p);
}

int main(int argc, char **argv)
{
  testInit(argc, argv, 400,400, "ShivaVG: Linear Gradient Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc)click);
  testCallback(TEST_CALLBACK_DRAG, (CallbackFunc)drag);
  testCallback(TEST_CALLBACK_KEY, (CallbackFunc)key);
  testCallback(TEST_CALLBACK_RESHAPE, (CallbackFunc)reshape);
  
  p = testCreatePath();
  org = testCreatePath();
  
  blackFill = vgCreatePaint();
  vgSetParameterfv(blackFill, VG_PAINT_COLOR, 4, black);
  
  backImage = createImageFromJpeg(IMAGE_DIR"test_img_violin.jpg");
  patternImage = createImageFromJpeg(IMAGE_DIR"test_img_shivavg.jpg");
  patternFill = vgCreatePaint();
  
  createSquare(p);
  createOrigin(org);
  createPattern();
  
  testOverlayString("Press H for a list of commands");
  testOverlayColor(1,1,1,1);
  testRun();
  
  return EXIT_SUCCESS;
}
