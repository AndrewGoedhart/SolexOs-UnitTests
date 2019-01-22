#include <cmath>
#include <gtest/gtest.h>
#include <waveforms/SineLookup.hpp>
#include <testFramework/UnitAssert.hpp>

using FP = numeric::FixedPoint<16>;

constexpr FP     START  = FP(0.0);
constexpr double TOLERANCE = 0.0001;

using sine = waveforms::SineLookup;

TEST(SineWaveLookup, CheckBoundaries){
  constexpr FP expected0 = FP(0.0);
  constexpr FP expected90 = FP(1.0);
  constexpr FP expected180 = FP(0.0);
  constexpr FP expected270 = FP(-1.0);
  checkTolerance(0.0001, sine::calculateSine(0), expected0);
  checkTolerance(0.0001, sine::calculateSine(64), expected90);
  checkTolerance(0.0001, sine::calculateSine(128), expected180);
  checkTolerance(0.0001, sine::calculateSine(192), expected270);
  checkTolerance(0.0001, sine::calculateSine(256), expected0);
}

TEST(SineWaveLookup, CheckError){
  constexpr double PI = 3.141592654;
  for( int i=0; i <= 256; i++){
    FP result = sine::calculateSine(i);
    double angle = 2*PI*i/256;
    double expected = sin(angle);
    checkTolerance(TOLERANCE, result.asDouble(), expected );
  }
}
