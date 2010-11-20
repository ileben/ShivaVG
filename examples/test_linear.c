#include "test.h"
#include <ctype.h>
#include <math.h>

VGfloat o[3] = {0.1f, 0.5f, 1.0f};

VGfloat gx1, gy1;
VGfloat gx2, gy2;

VGfloat tx=0.0f, ty=0.0f;
VGfloat sx=1.0f, sy=1.0f;
VGfloat a=0.0f;

VGint sindex = 0;
VGint ssize = 3;
VGint spread[] = {
  VG_COLOR_RAMP_SPREAD_PAD,
  VG_COLOR_RAMP_SPREAD_REPEAT,
  VG_COLOR_RAMP_SPREAD_REFLECT
};

VGfloat sqx = 200;
VGfloat sqy = 200;

VGfloat clickX;
VGfloat clickY;
VGfloat startX;
VGfloat startY;
char mode = 's';

VGPaint linearFill;
VGPaint blackFill;
VGPath start;
VGPath end;
VGPath side1;
VGPath side2;
VGPath unitX;
VGPath unitY;
VGPath p;

VGfloat black[] = {1,1,1, 1};

const char commands[] =
  "Click & drag mouse to change\n"
  "value for current mode\n\n"
  "H - this help\n"
  "TAB - gradient spread mode\n"
  "S - gradient start-point mode\n"
  "E - gradient end-point mode\n"
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
  vgTranslate(tx, tx);
  vgScale(sx, sy);
  vgRotate(a);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  
  vgSetPaint(linearFill, VG_FILL_PATH);
  vgDrawPath(p, VG_FILL_PATH);
  
  vgTranslate(tx, tx);
  vgScale(sx, sy);
  vgRotate(a);
  
  vgSetPaint(blackFill, VG_FILL_PATH | VG_STROKE_PATH);
  vgDrawPath(start, VG_FILL_PATH);
  vgDrawPath(end, VG_FILL_PATH);
  vgDrawPath(unitX, VG_STROKE_PATH);
  vgDrawPath(unitY, VG_STROKE_PATH);
  vgDrawPath(side1, VG_FILL_PATH);
  vgDrawPath(side2, VG_FILL_PATH);
}

void createArrow(VGPath p, VGfloat cx, VGfloat cy, VGfloat dx, VGfloat dy)
{
  VGfloat px = -dy, py = dx;
  testMoveTo(p, cx + 10*dx, cy + 10*dy, VG_ABSOLUTE);
  testLineTo(p, cx +  5*px, cy +  5*py, VG_ABSOLUTE);
  testLineTo(p, cx -  5*px, cy -  5*py, VG_ABSOLUTE);
  testClosePath(p);
}

void createLinear()
{
  VGfloat dx = gx2 - gx1;
  VGfloat dy = gy2 - gy1;
  VGfloat n = sqrt(dx*dx + dy*dy);
  VGfloat px = -dy / n;
  VGfloat py = dx / n;
  
  VGfloat stops[] = {
    0.0, 1.0, 0.0, 0.0, 1,
    0.5, 0.0, 1.0, 0.0, 1,
    1.0, 0.0, 0.0, 1.0, 1};
  
  VGint numstops = sizeof(stops) / sizeof(VGfloat);
  
  VGfloat linear[4];
  linear[0] = gx1;
  linear[1] = gy1;
  linear[2] = gx2;
  linear[3] = gy2;
  
  vgSetParameteri(linearFill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, spread[sindex]);
  vgSetParameteri(linearFill, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
  vgSetParameterfv(linearFill, VG_PAINT_LINEAR_GRADIENT, 4, linear);
  vgSetParameterfv(linearFill, VG_PAINT_COLOR_RAMP_STOPS, numstops, stops);
  
  vgClearPath(start, VG_PATH_CAPABILITY_ALL);
  vguEllipse(start, gx1, gy1, 10,10);
  
  vgClearPath(end, VG_PATH_CAPABILITY_ALL);
  createArrow(end, gx2, gy2, dx / n, dy / n);
  
  vgClearPath(unitX, VG_PATH_CAPABILITY_ALL);
  testMoveTo(unitX, gx1, gy1, VG_ABSOLUTE);
  testLineTo(unitX, gx2, gy2, VG_ABSOLUTE);
  
  vgClearPath(unitY, VG_PATH_CAPABILITY_ALL);
  testMoveTo(unitY, gx1 - 50*px, gy1 - 50*py, VG_ABSOLUTE);
  testLineTo(unitY, gx1 + 50*px, gy1 + 50*py, VG_ABSOLUTE);
  
  vgClearPath(side1, VG_PATH_CAPABILITY_ALL);
  createArrow(side1, gx1 + 50*px, gy1 + 50*py, px, py);
  
  vgClearPath(side2, VG_PATH_CAPABILITY_ALL);
  createArrow(side2, gx1 - 50*px, gy1 - 50*py, -px, -py);
}

void updateOverlayString()
{
  switch (mode) {
  case 's':
    testOverlayString("Gradient Start-Point mode"); break;
  case 'e':
    testOverlayString("Gradient End-Point mode"); break;
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
  case 's': {
      VGfloat dgx = (startX + dx) - gx1;
      VGfloat dgy = (startY + dy) - gy1;
      gx1 = startX + dx;
      gy1 = startY + dy;
      gx2 += dgx;
      gy2 += dgy;
      createLinear();
      break;}
  case 'e':
    gx2 = startX + dx;
    gy2 = startY + dy;
    createLinear();
    break;
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
  case 's':
    startX = gx1;
    startY = gy1;
    break;
  case 'e':
    startX = gx2;
    startY = gy2;
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
    case 's': {
        VGfloat tgx1 = gx1;
        VGfloat tgy1 = gy1;
        gx1 = (testWidth() - sqx)/2;
        gy1 = (testHeight() - sqy)/2;
        gx2 += gx1 - tgx1;
        gy2 += gy1 - tgy1;
        createLinear();
        break;}
    case 'e':
      gx2 = gx1 + sqx;
      gy2 = gy1 + sqy;
      createLinear();
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
  case 's':
  case 'e':
  case 'x':
  case 'y':
    break;
    
  case TAB:
    
    /* Cycle spread mode */
    sindex = (sindex+1) % ssize;
    createLinear();
    switch(sindex) {
    case 0: testOverlayString("Gradient Spread mode: PAD\n"); break;
    case 1: testOverlayString("Gradient Spread mode: REPEAT\n"); break;
    case 2: testOverlayString("Gradient Spread mode: REFLECT\n"); break;
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

void createSquare(VGPath p)
{
  testMoveTo(p, (testWidth()-sqx)/2, (testHeight()-sqy)/2, VG_ABSOLUTE);
  testLineTo(p, sqx, 0, VG_RELATIVE);
  testLineTo(p, 0, sqy, VG_RELATIVE);
  testLineTo(p, -sqx, 0, VG_RELATIVE);
  testClosePath(p);
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
  start = testCreatePath();
  end = testCreatePath();
  unitX = testCreatePath();
  unitY = testCreatePath();
  side1 = testCreatePath();
  side2 = testCreatePath();
  
  linearFill = vgCreatePaint();
  blackFill = vgCreatePaint();
  vgSetParameterfv(blackFill, VG_PAINT_COLOR, 4, black);
  
  gx1 = (testWidth() - sqx)/2;
  gy1 = (testHeight() - sqy)/2;
  gx2 = gx1 + sqx;
  gy2 = gy1 + sqy;
  
  createSquare(p);
  createLinear();
  
  testOverlayString("Press H for a list of commands");
  testOverlayColor(1,1,1,1);
  testRun();
  
  return EXIT_SUCCESS;
}
