#include <gtest/gtest.h>
#include <cmath>
#include <testFramework/UnitAssert.hpp>
#include <tests/drivers/stepper/StepperTestModel.hpp>
#include <drivers/timers/PwmTypes.hpp>
#include <iostream>

using Drivers::DutyCycle;



TEST(ModelsTest, ShortPeriods) {
  constexpr double expected[] = {
      0.48130,
      0.48401,
      0.48673,
      0.48945,
      0.49218,
      0.49491,
      0.49764,
      0.50037,
      0.50310,
      0.50584,
      0.50858};

  constexpr double COIL_INDUCTANCE = 0.028;
  constexpr double COIL_RESISTANCE = 17.8;
  constexpr double VOLTS = 24;
  constexpr double BACK_EMF = 0;
  constexpr double period = 0.000016;

  Drivers::StepperPlantModel<double> plant(VOLTS, COIL_RESISTANCE, COIL_INDUCTANCE, BACK_EMF, period,
        Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1000);

  for (uint32_t i = 0; i < 11; i++) {
    Drivers::DutyCycle duty(static_cast<uint16_t>(i*100), Drivers::PWM_CHANNEL::CHANNEL1);
    auto computed = plant.computeCurrentAtEndOfCycle(0.0, 0.5, duty);
    checkTolerance(0.00001, computed, expected[i]);
  }
}

TEST(ModelsTest, LongPeriods) {
  constexpr double expected[] = {
      -0.57551,
      -0.45992,
      -0.33195,
      -0.19029,
      -0.033459,
      0.14016,
      0.33238,
      0.54517,
      0.78074,
      1.04154};

  constexpr double COIL_INDUCTANCE = 0.028;
  constexpr double COIL_RESISTANCE = 17.8;
  constexpr double VOLTS = 24.0;
  constexpr double BACK_EMF = 0.0;
  constexpr double period = 0.0016;

  Drivers::StepperPlantModel<double> plant(VOLTS, COIL_RESISTANCE, COIL_INDUCTANCE, BACK_EMF, period,
      Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1000);

  for (uint32_t i = 1; i < 11; i++) {
    Drivers::DutyCycle duty(static_cast<uint16_t>(i*100), Drivers::PWM_CHANNEL::CHANNEL1);
    double computed = plant.computeCurrentAtEndOfCycle(0.0, 0.5, duty);
    checkTolerance(0.00001, computed, expected[i - 1]);
  }
}
TEST(ModelsTest, Shape) {
  constexpr double expected[] = {
      0.500000,
      0.542064,
      0.582043,
      0.620039,
      0.656151,
      0.690472,
      0.723091,
      0.754093,
      0.783558,
      0.811562,
      0.838177,
      0.863473,
      0.887514,
      0.776649,
      0.671281,
      0.571139,
      0.475961,
      0.385504,
      0.299531,
      0.217822};

  constexpr double COIL_INDUCTANCE = 0.028;
  constexpr double COIL_RESISTANCE = 17.8;
  constexpr double VOLTS = 24.0;
  constexpr double BACK_EMF = 0;
  constexpr double period = 0.0016;

  Drivers::StepperPlantModel<double> plant(VOLTS,COIL_RESISTANCE, COIL_INDUCTANCE, BACK_EMF, period,
      Drivers::PWM_CHANNEL::CHANNEL1, Drivers::PWM_CHANNEL::CHANNEL2, 1000);

  for (uint32_t i = 0; i < 20; i++) {
    double computed = plant.computeInstantaeousCurrent(0.0, 0.5, 0.6, (i)*period/20);
    checkTolerance(0.00001, computed, expected[i]);
  }
}

