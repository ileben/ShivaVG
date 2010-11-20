#include "test.h"
#include <ctype.h>

VGfloat cx, cy;
VGfloat fx, fy;
VGfloat r;

VGfloat sx=1.0f, sy=1.0f;
VGfloat tx=0.0f, ty=0.0f;
VGfloat ang=15.0f;

VGint sindex = 0;
VGint ssize = 3;
VGint spread[] = {
  VG_COLOR_RAMP_SPREAD_PAD,
  VG_COLOR_RAMP_SPREAD_REPEAT,
  VG_COLOR_RAMP_SPREAD_REFLECT
};

VGfloat sqx = 200;
VGfloat sqy = 200;

VGPaint radialFill;
VGPaint blackFill;
VGPath center;
VGPath focus;
VGPath radius;
VGPath p;

VGfloat clickX;
VGfloat clickY;
VGfloat startX;
VGfloat startY;
char mode = 'c';

VGfloat black[] = {1,1,1,1};

const char commands[] =
  "Click & drag mouse to change\n"
  "value for current mode\n\n"
  "H - this help\n"
  "TAB - gradient spread mode\n"
  "C - gradient center mode\n"
  "F - gradient focus mode\n"
  "R - gradient radius mode\n"
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
  vgRotate(ang);
  vgScale(sx, sy);
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  
  vgSetPaint(radialFill, VG_FILL_PATH);
  vgDrawPath(p, VG_FILL_PATH);
  
  vgTranslate(tx, ty);
  vgRotate(ang);
  vgScale(sx, sy);
  
  vgSetPaint(blackFill, VG_FILL_PATH | VG_STROKE_PATH);
  vgDrawPath(radius, VG_STROKE_PATH);
  vgDrawPath(center, VG_STROKE_PATH);
  vgDrawPath(focus, VG_FILL_PATH);
}

void createCross(VGPath p, VGfloat x, VGfloat y)
{
  VGfloat D=20;
  testMoveTo(p, x-D/2,     y, VG_ABSOLUTE);
  testLineTo(p,     D,     0, VG_RELATIVE);
  testMoveTo(p,     x, y-D/2, VG_ABSOLUTE);
  testLineTo(p,     0,     D, VG_RELATIVE);
}

void createArrow(VGPath p, VGfloat cx, VGfloat cy, VGfloat dx, VGfloat dy)
{
  VGfloat px = -dy, py = dx;
  testMoveTo(p, cx + 10*dx, cy + 10*dy, VG_ABSOLUTE);
  testLineTo(p, cx +  5*px, cy +  5*py, VG_ABSOLUTE);
  testLineTo(p, cx -  5*px, cy -  5*py, VG_ABSOLUTE);
  testClosePath(p);
}

void createRadial()
{
  VGfloat stops[] = {
    0.0, 1.0, 0.0, 0.0, 1,
    0.5, 0.0, 1.0, 0.0, 1,
    1.0, 0.0, 0.0, 1.0, 1};
  
  VGint numstops = sizeof(stops) / sizeof(VGfloat);
  
  VGfloat radial[5];
  radial[0] = cx;
  radial[1] = cy;
  radial[2] = fx;
  radial[3] = fy;
  radial[4] = r;
  
  vgSetParameteri(radialFill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, spread[sindex]);
  vgSetParameteri(radialFill, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
  vgSetParameterfv(radialFill, VG_PAINT_RADIAL_GRADIENT, 5, radial);
  vgSetParameterfv(radialFill, VG_PAINT_COLOR_RAMP_STOPS, numstops, stops);

  vgClearPath(center, VG_PATH_CAPABILITY_ALL);
  createCross(center, cx, cy);
  
  vgClearPath(focus, VG_PATH_CAPABILITY_ALL);
  vguEllipse(focus, fx, fy, 8,8);
  
  vgClearPath(radius, VG_PATH_CAPABILITY_ALL);
  vguEllipse(radius, cx, cy, r*2, r*2);
}

void createSquare(VGPath p)
{
  testMoveTo(p, (testWidth()-sqx)/2, (testHeight()-sqy)/2, VG_ABSOLUTE);
  testLineTo(p, sqx, 0, VG_RELATIVE);
  testLineTo(p, 0, sqy, VG_RELATIVE);
  testLineTo(p, -sqx, 0, VG_RELATIVE);
  testClosePath(p);
}

void updateOverlayString()
{
  switch (mode) {
  case 'c':
    testOverlayString("Gradient Center mode"); break;
  case 'f':
    testOverlayString("Gradient Focus mode"); break;
  case 'r':
    testOverlayString("Gradient Radius mode"); break;
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
  case 'c': {
    VGfloat dcx, dcy;
    dcx = startX + dx - cx;
    dcy = startY + dy - cy;
    cx = startX + dx;
    cy = startY + dy;
    fx += dcx;
    fy += dcy;
    createRadial();
    break; }
  case 'f':
    fx = startX + dx;
    fy = startY + dy;
    createRadial();
    break;
  case 'r':
    r = startY + dy;
    createRadial();
    break;
  case 'x':
    sx = startY + dy * 0.01f;
    break;
  case 'y':
    sy = startY + dy * 0.01f;
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
  case 'c':
    startX = cx;
    startY = cy;
    break;
  case 'f':
    startX = fx;
    startY = fy;
    break;
  case 'r':
    startY = r;
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
    switch(mode) {
    case 'c':
      cx = testWidth()/2;
      cy = testHeight()/2;
      fx = cx; fy = cy;
      createRadial();
      break;
    case 'f':
      fx = cx; fy = cy;
      createRadial();
      break;
    case 'r':
      r = sqx;
      createRadial();
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
  case 'c':
  case 'f':
  case 'r':
  case 'x':
  case 'y':
    break;
    
  case TAB:
    
    /* Cycle spread mode */
    sindex = (sindex+1) % ssize;
    createRadial();
    switch(sindex) {
    case 0: testOverlayString("Gradient Spread mode: PAD"); break;
    case 1: testOverlayString("Gradient Spread mode: REPEAT"); break;
    case 2: testOverlayString("Gradient Spread mode: REFLECT"); break;
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

int main(int argc, char **argv)
{
  testInit(argc, argv, 400,400, "ShivaVG: Radial Gradient Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc)click);
  testCallback(TEST_CALLBACK_DRAG, (CallbackFunc)drag);
  testCallback(TEST_CALLBACK_KEY, (CallbackFunc)key);
  
  p = testCreatePath();
  center = testCreatePath();
  focus = testCreatePath();
  radius = testCreatePath();
  
  cx = testWidth()/2;
  cy = testHeight()/2;
  fx = cx;
  fy = cy;
  r = sqx/2;
  
  radialFill = vgCreatePaint();
  blackFill = vgCreatePaint();
  vgSetParameterfv(blackFill, VG_PAINT_COLOR, 4, black);
  
  createSquare(p);
  createRadial();
  
  testOverlayString("Press H for a list of commands");
  testOverlayColor(1,1,1,1);
  testRun();
  
  return EXIT_SUCCESS;
}
