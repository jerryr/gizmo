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

void SetupAnimations()
{
    // fade all pixels providing a tail that is longer the faster
    // the pixel moves.
    animations.StartAnimation(0, 5, FadeAnimUpdate);

    // take several seconds to move eye fron one side to the other
    animations.StartAnimation(1, 2000, MoveAnimUpdate);
}

void init_serial_lights(int count) {
  PixelCount = count;
  strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(count);
  strip->Begin();
  strip->Show();
  SetupAnimations();
}

void deinit_serial_lights() {
  strip->ClearTo(RgbColor(0,0,0));
  strip->Show();
  delay(100);
  delete strip;
  strip = NULL;
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


