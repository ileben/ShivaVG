#include "test.h"
#include <ctype.h>

VGPaint testStroke;
VGPaint testFill;
VGPath testPath;
VGfloat phase=0.0f;

VGint jindex = 0;
VGint jsize = 3;
VGint joins[] = {
  VG_JOIN_MITER,
  VG_JOIN_BEVEL,
  VG_JOIN_ROUND
};

VGint cindex = 2;
VGint csize = 3;
VGint caps[] = {
  VG_CAP_BUTT,
  VG_CAP_SQUARE,
  VG_CAP_ROUND
};

VGfloat sx=1.0f, sy=1.0f;

VGfloat startX=0.0f;
VGfloat startY=0.0f;
VGfloat clickX=0.0f;
VGfloat clickY=0.0f;
char mode='d';

const char commands[] =
  "Click & drag mouse to change\n"
  "value for current mode\n\n"
  "H - this help\n"
  "D - dash phase mode\n"
  "C - dash caps cycle\n"
  "J - dash joins cycle\n"
  "X - scale X mode\n"
  "Y - scale Y mode\n";

void display(float interval)
{
  VGfloat cc[] = {0,0,0,1};
  VGfloat dash[] = {10,15,0,15};

  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  
  vgSetfv(VG_STROKE_DASH_PATTERN, sizeof(dash)/sizeof(float), dash);
  vgSeti(VG_STROKE_DASH_PHASE_RESET, VG_TRUE);
  vgSetf(VG_STROKE_DASH_PHASE, phase);
  
  vgSetf(VG_STROKE_LINE_WIDTH, 10.0f);
  vgSetf(VG_STROKE_JOIN_STYLE, joins[jindex]);
  vgSetf(VG_STROKE_CAP_STYLE, caps[cindex]);
  
  vgLoadIdentity();
  vgTranslate(testWidth()/2,testHeight()/2);
  vgScale(3 * sx, 3 * sy);
  vgDrawPath(testPath, VG_FILL_PATH);
  vgDrawPath(testPath, VG_STROKE_PATH);
}

void createStar()
{
  VGubyte segs[] = {
    VG_MOVE_TO, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL,
    VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL,
    VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
  
  VGfloat data[] = {
    0,50, 15,-40, 45,0, -35,-20,
    15,-40, -40,30, -40,-30, 15,40,
    -35,20, 45,0 };
  
  VGfloat cstroke[] = {0.5,0.2,0.8, 0.6};
  VGfloat cfill[] = {0.3,1.0,0.0, 0.6};
  
  testPath = testCreatePath();
  vgAppendPathData(testPath, sizeof(segs), segs, data);
  
  testStroke = vgCreatePaint();
  vgSetParameterfv(testStroke, VG_PAINT_COLOR, 4, cstroke);
  vgSetPaint(testStroke, VG_STROKE_PATH);
  
  testFill = vgCreatePaint();
  vgSetParameterfv(testFill, VG_PAINT_COLOR, 4, cfill);
  vgSetPaint(testFill, VG_FILL_PATH);
}

void updateOverlayString()
{
  switch(mode)
  {
  case 'd':
    testOverlayString("Dash Phase mode"); break;
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
  
  switch (mode) {
  case 'd':
    phase = startY + dy * 0.1;
    break;
  case 'x':
    sx = startY + dy * 0.01;
    break;
  case 'y':
    sy = startY + dy * 0.01;
    break;
  }
  
  updateOverlayString();
}

void click(int button, int state, int x, int y)
{
  y = testHeight() - y;
  clickX = x; clickY = y;
  
  switch (mode) {
  case 'd':
    startY = phase;
    break;
  case 'x':
    startY = sx;
    break;
  case 'y':
    startY = sy;
    break;
  }
}

void key(unsigned char code, int x, int y)
{
  switch (tolower(code)) {
  case 'd':
  case 'x':
  case 'y':
    break;
    
  case 'c':
    /* Cycle caps type */
    cindex = (cindex+1) % csize;
    switch(caps[cindex]) {
    case VG_CAP_BUTT: testOverlayString("Dash caps: BUTT\n"); break;
    case VG_CAP_SQUARE: testOverlayString("Dash caps: SQUARE\n"); break;
    case VG_CAP_ROUND: testOverlayString("Dash caps: ROUND\n"); break; }
    return;
    
  case 'j':
    /* Cycle joins type */
    jindex = (jindex+1) % jsize;
    switch(joins[jindex]) {
    case VG_JOIN_MITER: testOverlayString("Dash joins: MITER\n"); break;
    case VG_JOIN_BEVEL: testOverlayString("Dash joins: BEVEL\n"); break;
    case VG_JOIN_ROUND: testOverlayString("Dash joins: ROUND\n"); break; }
    return;
    
  case 'h':
    /* Show help */
    testOverlayString(commands);
    return;
    
  default:
    return;
  }
  
  /* Switch mode */
  mode = tolower(code);
  updateOverlayString();
}

int main(int argc, char **argv)
{
  testInit(argc, argv, 500,500, "ShivaVG: Dash Pattern Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_KEY, (CallbackFunc)key);
  testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc)click);
  testCallback(TEST_CALLBACK_DRAG, (CallbackFunc)drag);
  
  createStar();
  
  testOverlayString("Press H for a list of commands");
  testOverlayColor(1,1,1,1);
  testRun();
  
  return EXIT_SUCCESS;
}
