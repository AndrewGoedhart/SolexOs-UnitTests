#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <waveforms/SineGenerator.hpp>
#include <testFramework/UnitAssert.hpp>

using FP = numeric::FixedPoint<22>;

constexpr FP    START  = FP(0.0);
constexpr FP    INCREMENT = FP(128/1024.0/1024.0);
constexpr FP    PERIOD = INCREMENT*128;
constexpr double     TOLERANCE = 0.0025;


TEST(SineWaveGenerator, Start0){
  constexpr FP expected = FP(0.0);
  Drivers::SineGenerator<FP> generator= Drivers::SineGenerator<FP>(PERIOD);
  auto result = generator.calculateSine(START);
  EXPECT_EQ(result, expected);
}

TEST(SineWaveGenerator, max90){
  constexpr FP expected = FP(1.0);
  constexpr FP p = FP(0.016);
  Drivers::SineGenerator<FP> generator= Drivers::SineGenerator<FP>(p);
  auto result = generator.calculateSine(p/4);
  checkTolerance(0.001, result, expected );
}


TEST(SineWaveGenerator, CheckWorstCaseError){
  constexpr double PI = 3.141592654;
  Drivers::SineGenerator<FP> generator= Drivers::SineGenerator<FP>(PERIOD);
  for( int i=0; i < 32; i++){
    FP t = INCREMENT * i;
    FP result = generator.calculateSine(t);
    double angle = 2*PI*i/128;
    double expected = sin(angle);
    checkTolerance(TOLERANCE, result.asDouble(), expected );
  }
}

TEST(SineWaveGenerator, Checkwrap){
  constexpr double PI = 3.141592654;
  Drivers::SineGenerator<FP> generator= Drivers::SineGenerator<FP>(FP(0.016));
  for( int i=0; i < 200; i++){
    FP t = FP(0.000160) * i;
    FP result = generator.calculateSine(t);
    double angle = 2*PI*i/100;
    double expected = sin(angle);
    checkTolerance(TOLERANCE, result.asDouble(), expected );
  }
}
