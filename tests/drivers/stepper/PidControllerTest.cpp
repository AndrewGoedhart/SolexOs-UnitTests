#include <cmath>
#include <gtest/gtest.h>
#include <numeric/FixedPoint.hpp>
#include <iostream>
#include <drivers/stepper/PidController.hpp>
#include <drivers/stepper/StepperPredictiveModel.hpp>
#include <waveforms/SineGenerator.hpp>
#include <tests/drivers/stepper/StepperTestModel.hpp>
#include <testFramework/UnitAssert.hpp>

using Drivers::PWM_CHANNEL;
using Drivers::DutyCycle;
using FP = numeric::FixedPoint<22>;

struct TestParameters {
    typedef double NumType;
    /**
     * Constants for the PID tuning parameters  - Proprotional (Kp), Integral(Ki) and Derivative(Kd)
     */
    static constexpr double Kd = 0.1;
    static constexpr double Kp = -0.35;
    static constexpr double Ki = 0.9;

    // weighting for adding to running integral.
    static constexpr double Wi = 0.9;

    /**
     * The range for the output (o) max>= (o) >= min
     */
    static constexpr double MAX_OUTPUT_VALUE = 0.1;
    static constexpr double MIN_OUTPUT_VALUE = -0.1;

    /**
     * Range for integral
     */
    static constexpr double MAX_INTEGRAL = 0.025;

    /**
     * double used to reset history for the controller
     */
    static constexpr double RESET_VALUE_ERROR = 0.0;
    static constexpr double RESET_VALUE_INTEGRAL = 0.0;
    static constexpr double RESET_VALUE_SETPOINT = 0.0;
};

constexpr Drivers::PwmChannel POSITIVE_CHANNEL = PWM_CHANNEL::CHANNEL1;
constexpr Drivers::PwmChannel NEGATIVE_CHANNEL = PWM_CHANNEL::CHANNEL2;

template<typename T, typename S>
void testSineTracking(Drivers::PidController<T, S> pidController, Drivers::StepperPlantModel<double> plant, double stepSize) {
  constexpr double SPEED = 0.0;
  const double PERIOD = stepSize * 100;

  Drivers::SineGenerator<FP> sine(PERIOD);
  Drivers::StepperPredictiveModel<T> pwm(24, 17.8, 0.028, 0.0, stepSize, POSITIVE_CHANNEL, NEGATIVE_CHANNEL,
      1024);

  T In[200];
  In[0] = 0.0;
  T error = 0.0;
  for (int i = 1; i < 200; i++) {
    if (i == 100) {
      error = 0.0;
    }

    T currentI = In[i - 1];
    auto t = (i % 100) * stepSize;
    auto tlast = t - stepSize;
    auto target = sine.calculateSine(t).asDouble();
    auto prevTarget = sine.calculateSine(tlast).asDouble();
    auto errorDelta = prevTarget - currentI;
    error = error + errorDelta * errorDelta;
    auto deltaI = pidController.computeOutput(currentI, prevTarget);
    DutyCycle duty = pwm.computeDutyCycle(SPEED, deltaI + target, currentI);
    for (int j = 0; j < 10; j++) {
      currentI = plant.computeCurrentAtEndOfCycle(SPEED, currentI, duty);
    }
    In[i] = currentI;
  }
  checkTolerance(0.001, sqrt(error / 100), 0.001);
}

TEST(PidControllerTest, SmallStep) {
  constexpr double TARGET = 0.510;
  constexpr double SPEED = 0.0;
  constexpr double ESTIMATED_SPEED = 4.0;

  Drivers::PidController<double, TestParameters> pidControler;
  Drivers::StepperPredictiveModel<double> pwm(24, 17.8, 28, 0.0, 0.016, POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024 );
  Drivers::StepperPlantModel<double> plant(24, 20.8, 36, 24, 0.016, POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024 );

  double In[40];
  In[0] = 0.5;
  for (int i = 1; i < 40; i++) {
    double currentI = In[i - 1];
    double deltaI = pidControler.computeOutput(currentI, TARGET);
    DutyCycle duty = pwm.computeDutyCycle(ESTIMATED_SPEED, TARGET + deltaI, currentI);
    In[i] = plant.computeCurrentAtEndOfCycle(SPEED, currentI, duty);
  }
  checkTolerance(0.001, In[19], TARGET);


}

TEST(PidControllerTest, LargeStep) {
  constexpr double TARGET = 0.60;
  constexpr double SPEED = 0.0;
  constexpr double ESTIMATED_SPEED = 4.0;

  Drivers::PidController<double, TestParameters> pidControler;
  Drivers::StepperPredictiveModel<double> pwm(24, 17.8, 28, 0.0, 0.016,POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024 );
  Drivers::StepperPlantModel<double> plant(24, 20.8, 36, 24, 0.016, POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024 );

  double In[40];
  In[0] = 0.5;
  for (int i = 1; i < 40; i++) {
    double currentI = In[i - 1];
    double deltaI = pidControler.computeOutput(currentI, TARGET);
    DutyCycle duty = pwm.computeDutyCycle(ESTIMATED_SPEED, TARGET + deltaI, currentI);
    In[i] = plant.computeCurrentAtEndOfCycle(SPEED, currentI, duty);
  }
  checkTolerance(0.0001, In[39], TARGET);

}

TEST(PidControllerTest, OPenLoopSmallStep) {
  constexpr double TARGET = 0.510;
  constexpr double SPEED = 0.0;

  Drivers::PidController<double, TestParameters> pidControler;
  Drivers::StepperPredictiveModel<double> pwm(24, 17.8, 0.028, 0.0, 0.000016, POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);
  Drivers::StepperPlantModel<double> plant(24, 20.8, 0.036, 24, 0.000016, POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);

  double In[40];
  In[0] = 0.5;
  for (int i = 1; i < 40; i++) {
    double currentI = In[i - 1];
    DutyCycle duty = pwm.computeDutyCycle(SPEED, TARGET, currentI);
    In[i] = plant.computeCurrentAtEndOfCycle(SPEED, currentI, duty);

  }
  checkTolerance(0.001, In[39], TARGET);

}
TEST(PidControllerTest, SineOpen) {
  constexpr double SPEED = 0.0;
  constexpr auto PERIOD = FP(0.016);
  constexpr auto TIMESTEP = FP(0.00016);
  Drivers::SineGenerator<FP> sine(PERIOD);
  Drivers::StepperPredictiveModel<double> pwm(24, 17.8, 0.028, 0.0, 0.00016,POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);
  Drivers::StepperPlantModel<double> plant(24, 18.9, 0.0336, 24, 0.000016,POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);

  double In[100];
  In[0] = 0.0;
  for (int i = 1; i < 100; i++) {
    double currentI = In[i - 1];
    auto t = i * TIMESTEP;
    double target = double(sine.calculateSine(t).asDouble());
    DutyCycle duty = pwm.computeDutyCycle(SPEED, target, currentI);
    for (int j = 0; j < 10; j++) {
      currentI = plant.computeCurrentAtEndOfCycle(SPEED, currentI, duty);
    }
    In[i] = currentI;
  }

}

TEST(PidControllerTest, SineHigh) {
  Drivers::PidController<double, TestParameters> pidControler;
  Drivers::StepperPlantModel<double> plant(24, 18.9, 0.0336, 24, 0.000016,POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);
  testSineTracking(pidControler, plant, 0.000160);
}

TEST(PidControllerTest, SineLow) {
  Drivers::PidController<double, TestParameters> pidControler;
  Drivers::StepperPlantModel<double> plant(24, 15.5, 0.0224, 24, 0.000016,POSITIVE_CHANNEL, NEGATIVE_CHANNEL, 1024);
  testSineTracking(pidControler, plant, 0.000160);
}


constexpr double TestParameters::Kd;
constexpr double TestParameters::Kp;
constexpr double TestParameters::Ki;
constexpr double TestParameters::Wi;
constexpr double TestParameters::MAX_INTEGRAL;
constexpr double TestParameters::MAX_OUTPUT_VALUE;
constexpr double TestParameters::MIN_OUTPUT_VALUE;

