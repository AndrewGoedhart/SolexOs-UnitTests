#include <cmath>
#include <gtest/gtest.h>
#include <numeric/FixedPoint.hpp>
#include <iostream>
#include <drivers/stepper/PidController.hpp>
#include <drivers/stepper/StepperPredictiveModel.hpp>
#include <waveforms/SineGenerator.hpp>
#include <tests/drivers/stepper/StepperTestModel.hpp>
#include <testFramework/UnitAssert.hpp>

template<typename T>
class ErrorTracker {
  private:
    double total;
    double maxError;
    int count;

  public:
    ErrorTracker() :
            total(0.0),
            maxError(0),
            count(0) {
    }
    void reset() {
      total = 0.0;
      count = 0;
      maxError = 0;
    }
    void addError(T target, T actual) {
      double delta = fabs(target.asDouble() - actual.asDouble());
      double deltaSqr = delta * delta;
      if (delta > maxError) {
        maxError = delta;
      }
      total = total + deltaSqr;
      count++;
    }

    void checkError(double tolerance, T expectedError) {
      double error = sqrt(total / count);
      checkTolerance(tolerance, error, expectedError.asDouble());
    }
    void checkMaxError(double tolerance, T expectedError) {
      checkTolerance(tolerance, maxError, expectedError.asDouble());
    }
};

namespace Fixed {
  struct TestParameters {
      // adjust types for the ones we are using
      using NumType = numeric::FixedPoint<16, int32_t>;

      /**
       * Constants for the PID tuning parameters  - Proprotional (Kp), Integral(Ki) and Derivative(Kd)
       */
      static constexpr NumType Kd = NumType(0.1);
      static constexpr NumType Kp = NumType(-0.35);
      static constexpr NumType Ki = NumType(0.9);

      /**
       * The range for the output (o) max>= (o) >= min
       */
      static constexpr NumType MAX_OUTPUT_VALUE = NumType(0.1);
      static constexpr NumType MIN_OUTPUT_VALUE = NumType(-0.1);

      /**
       * Range for integral
       */
      static constexpr NumType MAX_INTEGRAL = NumType(0.025);

      /**
       * Value used to reset history for the controller
       */
      static constexpr NumType RESET_VALUE_ERROR = NumType(0.0);
      static constexpr NumType RESET_VALUE_INTEGRAL = NumType(0.0);
      static constexpr NumType RESET_VALUE_SETPOINT = NumType(0.0);
  };
}

using FP = Fixed::TestParameters::NumType;
using Drivers::DutyCycle;

constexpr FP SPEED = FP(0.0);
constexpr FP ESTIMATED_SPEED = FP(4.0);

constexpr FP modelV = FP(24.0);
constexpr FP modelK = FP(0.0);
constexpr FP modelR = FP(17.2);
constexpr FP modelL = FP(28.0);
constexpr FP modelP = FP(0.160);

constexpr Drivers::PwmChannel POSITIVE_CHANNEL = Drivers::PWM_CHANNEL::CHANNEL1;
constexpr Drivers::PwmChannel NEGATIVE_CHANNEL = Drivers::PWM_CHANNEL::CHANNEL2;


template<typename T, typename S>
void testSineTracking(Drivers::PidController<T, S> pidController, Drivers::StepperPlantModel<double> plant) {
  constexpr FP SPEED = FP(0.0);
  constexpr FP PERIOD = FP(16.0);
  constexpr FP TIMESTEP = FP(0.160);

  Drivers::SineGenerator<FP> sine(100);
  Drivers::StepperPredictiveModel<T> pwm(24, 17.8, 28, 0.0, 0.16, POSITIVE_CHANNEL, NEGATIVE_CHANNEL,
      1024);

  T In[200];
  In[0] = 0.0;
  T error = 0.0;
  for (int i = 1; i < 200; i++) {
    if (i == 100) {
      error = 0.0;
    }

    T currentI = In[i - 1];
    auto target = sine.calculateSine(i);
    auto prevTarget = sine.calculateSine(i-1);
    auto errorDelta = prevTarget - currentI;
    error = error + errorDelta * errorDelta;
    auto deltaI = pidController.computeOutput(currentI, prevTarget);
    DutyCycle duty = pwm.computeDutyCycle(SPEED, deltaI + target, currentI);
    for (int j = 0; j < 10; j++) {
      currentI = plant.computeCurrentAtEndOfCycle(SPEED.asDouble(), currentI.asDouble(), duty);
    }
    In[i] = currentI;
  }
  checkTolerance(0.001, sqrt(error.asDouble() / 100), 0.001);
}



TEST(PidControllerTestFixed, SmallStep) {

  constexpr FP TARGET = FP(0.510);

  Drivers::PidController<FP, Fixed::TestParameters> pidControler;
  Drivers::StepperPredictiveModel<FP> pwm(modelV, modelR, modelL, modelK, modelP, Drivers::PWM_CHANNEL::CHANNEL1,
      Drivers::PWM_CHANNEL::CHANNEL2, 1024);
  Drivers::StepperPlantModel<double> plant(modelV.asDouble(), modelR.asDouble(), modelL.asDouble(), modelK.asDouble(),
                                           modelP.asDouble(), Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1024);
  FP In[40];
  In[0] = FP(0.5);
  for (int i = 1; i < 40; i++) {
    auto currentI = In[i - 1];
    auto deltaI = pidControler.computeOutput(currentI, TARGET);
    auto duty = pwm.computeDutyCycle(ESTIMATED_SPEED, TARGET + deltaI, currentI);
    In[i] = plant.computeCurrentAtEndOfCycle(SPEED.asDouble(), currentI.asDouble(), duty);
  }
  checkTolerance(0.0001, In[19], TARGET);
}

TEST(PidControllerTestFixed, LargeStep) {
  constexpr FP TARGET = FP(0.60);

  Drivers::PidController<FP, Fixed::TestParameters> pidControler;
  Drivers::StepperPredictiveModel<FP> pwm(modelV, modelR, modelL, modelK, modelP, Drivers::PWM_CHANNEL::CHANNEL1,
      Drivers::PWM_CHANNEL::CHANNEL2, 1024);
  Drivers::StepperPlantModel<double> plant(modelV.asDouble(), modelR.asDouble(), modelL.asDouble(), modelK.asDouble(),
                                           modelP.asDouble()/10, Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1024);

  FP In[40];
  In[0] = FP(0.5);
  for (int i = 1; i < 40; i++) {
    auto currentI = In[i - 1];
    auto deltaI = pidControler.computeOutput(currentI, TARGET);
    auto duty = pwm.computeDutyCycle(ESTIMATED_SPEED, TARGET + deltaI, currentI);
    for (int j = 0; j < 10; j++) {
      currentI = plant.computeCurrentAtEndOfCycle(SPEED.asDouble(), currentI.asDouble(), duty);
    }
    In[i] = currentI;
  }
  checkTolerance(0.0001, In[39], TARGET);
}

TEST(PidControllerTestFixed, LargeStepOpen) {
  constexpr FP TARGET = FP(0.60);

  Drivers::StepperPredictiveModel<FP> pwm(modelV, modelR, modelL, modelK, modelP, Drivers::PWM_CHANNEL::CHANNEL1,
      Drivers::PWM_CHANNEL::CHANNEL2, 1024);
  Drivers::StepperPlantModel<double> plant(modelV.asDouble(), modelR.asDouble(), modelL.asDouble(), modelK.asDouble(),
                                           modelP.asDouble()/10, Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1024);

  FP In[40];
  In[0] = FP(0.5);
  for (int i = 1; i < 40; i++) {
    auto currentI = In[i - 1];
    auto duty = pwm.computeDutyCycle(ESTIMATED_SPEED, TARGET, currentI);
    for (int j = 0; j < 10; j++) {
      currentI = plant.computeCurrentAtEndOfCycle(SPEED.asDouble(), currentI.asDouble(), duty);
    }
    In[i] = currentI;
  }
  checkTolerance(0.01, In[39], TARGET);
}

TEST(PidControllerTestFixed, SineHigh) {

  constexpr double actualV = 24.0;
  constexpr double actualK = 0.0;
  constexpr double actualR = 18.9;
  constexpr double actualL = 34;
  constexpr double actualP = 0.016;

  Drivers::PidController<FP, Fixed::TestParameters> pidControler;
  Drivers::StepperPlantModel<double> plant(actualV, actualR, actualL, actualK, actualP, Drivers::PWM_CHANNEL::CHANNEL1,
      Drivers::PWM_CHANNEL::CHANNEL2, 1024);

  testSineTracking(pidControler, plant);
}





/**
 * Run the loop twice testing on the second pass. This gives the PID time to settle. It needs a single step to get within
 * tolerance.
 */
TEST(PidControllerTestFixed, SineLow) {
  constexpr double actualV = 24.0;
  constexpr double actualK = 0.0;
  constexpr double actualR = 14.6;
  constexpr double actualL = 22.4;
  constexpr double actualP = 0.016;

  Drivers::PidController<FP, Fixed::TestParameters> pidControler;
  Drivers::StepperPlantModel<double> plant(actualV, actualR, actualL, actualK, actualP, Drivers::PWM_CHANNEL::CHANNEL1,
      Drivers::PWM_CHANNEL::CHANNEL2, 1024);

  testSineTracking(pidControler, plant);
}

constexpr FP Fixed::TestParameters::Kd;
constexpr FP Fixed::TestParameters::Kp;
constexpr FP Fixed::TestParameters::Ki;
constexpr FP Fixed::TestParameters::MAX_INTEGRAL;
constexpr FP Fixed::TestParameters::MAX_OUTPUT_VALUE;
constexpr FP Fixed::TestParameters::MIN_OUTPUT_VALUE;

