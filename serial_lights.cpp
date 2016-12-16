#include "gizmo.h"
#include "serial_lights.h"
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
NeoPixelAnimator animations(2);
uint16_t lastPixel = 0;
int8_t moveDir = 1;
uint16_t PixelCount;
AnimEaseFunction moveEase = NeoEase::SinusoidalInOut;
const RgbColor CylonEyeColor(HtmlColor(0x7f0000));
void FadeAll(uint8_t darkenBy) {
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip->PixelCount(); indexPixel++)
    {
        color = strip->GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip->SetPixelColor(indexPixel, color);
  }
}

void FadeAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        FadeAll(10);
        animations.RestartAnimation(param.index);
    }
}

void MoveAnimUpdate(const AnimationParam& param)
{
    // apply the movement animation curve
    float progress = moveEase(param.progress);

    // use the curved progress to calculate the pixel to effect
    uint16_t nextPixel;
    if (moveDir > 0)
    {
        nextPixel = progress * PixelCount;
    }
    else
    {
        nextPixel = (1.0f - progress) * PixelCount;
    }

    // if progress moves fast enough, we may move more than
    // one pixel, so we update all between the calculated and
    // the last
    if (lastPixel != nextPixel)
    {
        for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir)
        {
            strip->SetPixelColor(i, CylonEyeColor);
        }
    }
    strip->SetPixelColor(nextPixel, CylonEyeColor);

    lastPixel = nextPixel;

    if (param.state == AnimationState_Completed)
    {
        // reverse direction of movement
        moveDir *= -1;

        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);
    }
}


// More Christmassy animation
RgbColor red = RgbColor(0x7f,0,0);
RgbColor green = RgbColor(0,0x7f,0);
RgbColor white = RgbColor(0x7f,0x7f,0x7f);
uint16_t lastpos;
void XMasAnimUpdate(const AnimationParam& param) {
  float progress = moveEase(param.progress);
  uint16_t start = progress * PixelCount;
  if(start < 5) {
    strip->ClearTo(green, lastpos, start);
  }
  else {
    uint16_t end = lastpos<(start-9)? lastpos: start-9;
    uint16_t delta = (start-end)/3;
    strip->ClearTo(red, end, end + delta);
    strip->ClearTo(white, end + delta + 1, end + (2*delta));
    strip->ClearTo(green, end + (2*delta) + 1, end + (3*delta));
    lastpos = end + (2*delta) + 1;
  }
//  strip->ClearTo(red, start, start+3);
//  strip->ClearTo(white, start+3, start+6);
//  strip->ClearTo(green, start+6, start+9);
//  strip->ClearTo(red, lastpos, start);
  if(param.state == AnimationState_Completed) {
    animations.RestartAnimation(param.index);
    lastpos = 0;
  }
}

void SetupAnimations()
{
    // fade all pixels providing a tail that is longer the faster
    // the pixel moves.
    animations.StartAnimation(0, 5, FadeAnimUpdate);

    // take several seconds to move eye fron one side to the other
    //animations.StartAnimation(1, 2000, MoveAnimUpdate);
    animations.StartAnimation(1, 2000, XMasAnimUpdate);
}


void init_serial_lights(int count) {
  if(strip) {
    deinit_serial_lights();
  }
  PixelCount = count;
  strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(count);
  strip->Begin();
  strip->Show();
  SetupAnimations();
}

void deinit_serial_lights() {
  if(strip) {
    strip->ClearTo(RgbColor(0,0,0));
    strip->Show();
    delay(100);
    delete strip;
    strip = NULL;
  }
}

void animate_serial_lights() {
  if(strip) {
    animations.UpdateAnimations();
    strip->Show();
  }
}

void control_serial_lights(String message, void *cbarg) {
  if(message == "ON") {
    init_serial_lights(144);
  }
  else if(message == "OFF") {
    deinit_serial_lights();
  }
  
}


