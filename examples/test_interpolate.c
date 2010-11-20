#include "test.h"
#include <math.h>
#define PI 3.141592654f

VGPath iApple;
VGPath iPear;
VGPath iMorph;
VGfloat angle=3*PI/2;
VGfloat amount=0.0f;
VGPaint fill;
VGfloat white[] = {1,1,1,1};

void createMorph()
{
  vgClearPath(iMorph, VG_PATH_CAPABILITY_ALL);
  vgInterpolatePath(iMorph, iApple, iPear, amount);
}

void display(float interval)
{
  VGfloat cc[] = {0,0,0,1};
  
  angle += interval * 0.4 * PI;
  if (angle > 2*PI) angle -= 2*PI;
  amount = (sin(angle) + 1) * 0.5f;
  createMorph();
  
  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  
  vgLoadIdentity();
  vgTranslate(testWidth()/2, testHeight()/2);
  vgDrawPath(iMorph, VG_FILL_PATH);
}

void createApple(VGPath p)
{
  VGPath temp;
  
  VGubyte segs[] = {
    VG_MOVE_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS,
    VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS,
    VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CLOSE_PATH,
    VG_MOVE_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CLOSE_PATH };
  
  VGfloat data[] = {
    1.53125,-44.681982, -3.994719,-44.681982, -8.0085183,-50.562501,
    -26.5625,-50.562501, -42.918439,-50.562501, -56.46875,-34.239393,
    -56.46875,-12.187501, -56.46875,26.520416, -34.65822,61.731799,
    -16.84375,61.812499, -7.1741233,61.812499, -2.9337937,55.656199,
    4.15625,55.656199, 11.746294,55.656199, 17.981627,62.281199,
    25.4375,62.281199, 33.88615,62.281199, 50.53251,44.282999,
    58.75,15.718799, 47.751307,9.086518, 40.999985,-0.228074,
    41,-13.046574, 41,-27.849147, 46.64686,-34.763001,
    52.4375,-39.937501, 46.111827,-47.219094, 39.0413,-50.503784,
    29.09375,-50.446384, 11.146487,-50.342824, 8.6341912,-44.681982,
    1.53125,-44.681982,
    
    0.23972344,-52.075169, -2.8344902,-69.754133, 5.9303785,-81.915323,
    24.152707,-86.881406, 23.71828,-70.367255, 15.114064,-58.365865,
    0.23972344,-52.075169 };
  
  temp = testCreatePath();
  vgAppendPathData(temp, sizeof(segs), segs, data);

  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgScale(1,-1);
  vgTransformPath(p, temp);
  vgDestroyPath(temp);
}

void createPear(VGPath p)
{
  VGPath temp;
  
  VGubyte segs[] = {
    VG_MOVE_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS,
    VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS,
    VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CLOSE_PATH,
    VG_MOVE_TO_ABS, VG_CUBIC_TO_ABS, VG_CUBIC_TO_ABS, VG_CLOSE_PATH };
  
  VGfloat data[] = {
    0.0625,-90.625001, -29.44062,-89.191161, -23.07159,-32.309301,
    -30.5625,-14.062501, -38.29681,4.7771994, -56.8077,20.767199,
    -56.46875,42.812499, -56.1298,64.502999, -40.15822,79.731799,
    -22.34375,79.812499, -4.17446,79.893199, -1.93369,71.113999,
    4.15625,71.156199, 10.49619,71.198499, 13.70293,80.336799,
    30.4375,80.281199, 42.49257,80.241199, 53.53251,70.782999,
    58.75,58.218799, 47.0442,54.768499, 38.5,43.943499,
    38.5,31.124999, 38.50001,22.754099, 42.14686,15.236999,
    47.9375,10.062499, 42.2834,1.5737994, 36.5413,-6.6199006,
    34.09375,-14.062501, 28.48694,-31.111801, 32.99356,-90.265511,
    1.5,-90.625001,
    
    5.1056438,-97.8762, -12.766585,-99.48239, -22.244878,-111.09615,
    -22.325466,-129.98288, -6.486451,-125.28908, 2.8790668,-113.87186,
    5.1056438,-97.8762 };
  
  temp = testCreatePath();
  vgAppendPathData(temp, sizeof(segs), segs, data);

  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgScale(1,-1);
  vgTransformPath(p, temp);
  vgDestroyPath(temp);
}

int main(int argc, char **argv)
{
  testInit(argc, argv, 400,400, "ShivaVG: Path Interpolation Test");
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  
  iApple = testCreatePath();
  createApple(iApple);
  iPear = testCreatePath();
  createPear(iPear);
  iMorph = testCreatePath();
  
  fill = vgCreatePaint();
  vgSetParameterfv(fill, VG_PAINT_COLOR, 4, white);
  vgSetPaint(fill, VG_FILL_PATH);
  
  testOverlayColor(1,1,1,1);
  testRun();
  
  return EXIT_SUCCESS;
}
