#include "test.h"

VGPath line;
VGPath polyOpen;
VGPath polyClosed;
VGPath rect;
VGPath rectRound;
VGPath ellipse;
VGPath arcOpen;
VGPath arcChord;
VGPath arcPie;

#define NUM_PRIMITIVES 9
VGPath primitives[NUM_PRIMITIVES];

void display(float interval)
{
  int x,y,p;
  VGfloat white[] = {1,1,1,1};
  
  vgSetfv(VG_CLEAR_COLOR, 4, white);
  vgClear(0, 0, testWidth(), testHeight());
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  
  for (y=0, p=0; y<3; ++y) {
    for (x=0; x<3; ++x, ++p) {
      if (p > NUM_PRIMITIVES) break;
      
      vgLoadIdentity();
      vgTranslate(100 + x*150, 100 + y*150);
      vgDrawPath(primitives[p], VG_STROKE_PATH);
    }
  }
}

void createPrimitives()
{
  VGfloat points[] = {-30,-30, 30,-30, 0,30};
  
  line = testCreatePath();
  vguLine(line, -30,-30,30,30);
  primitives[0] = line;
  
  polyOpen = testCreatePath();
  vguPolygon(polyOpen, points, 3, VG_FALSE);
  primitives[1] = polyOpen;
  
  polyClosed = testCreatePath();
  vguPolygon(polyClosed, points, 3, VG_TRUE);
  primitives[2] = polyClosed;
  
  rect = testCreatePath();
  vguRect(rect, -50,-30, 100,60);
  primitives[3] = rect;
  
  rectRound = testCreatePath();
  vguRoundRect(rectRound, -50,-30, 100,60, 30,30);
  primitives[4] = rectRound;
  
  ellipse = testCreatePath();
  vguEllipse(ellipse, 0,0, 100, 60);
  primitives[5] = ellipse;
  
  arcOpen = testCreatePath();
  vguArc(arcOpen, 0,0, 100,60, 0, 270, VGU_ARC_OPEN);
  primitives[6] = arcOpen;
  
  arcChord = testCreatePath();
  vguArc(arcChord, 0,0, 100,60, 0, 270, VGU_ARC_CHORD);
  primitives[7] = arcChord;
  
  arcPie = testCreatePath();
  vguArc(arcPie, 0,0, 100,60, 0, 270, VGU_ARC_PIE);
  primitives[8] = arcPie;
}

int main(int argc, char **argv)
{
  testInit(argc, argv, 500,500, "ShivaVG: VGU Primitives Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  
  createPrimitives();
  testRun();
  
  return EXIT_SUCCESS;
}
