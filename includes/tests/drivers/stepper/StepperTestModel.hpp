#ifndef INCLUDE_DRIVERS_STEPPER_PLANTMODEL_HPP_
#define INCLUDE_DRIVERS_STEPPER_PLANTMODEL_HPP_

#include <numeric/FixedPoint.hpp>
#include <drivers/timers/PwmTypes.hpp>
#include <cmath>
#include <type_traits>

namespace Drivers {
  template<typename T>
  typename std::enable_if<std::is_class<T>::value, T>::type log(T val) {
    return T(std::log(val.asDouble()));
  }

  template<typename T>
  typename std::enable_if<std::is_class<T>::value, T>::type exp(T val) {
    return T(std::exp(val.asDouble()));
  }

  template<typename T>
  typename std::enable_if<!std::is_class<T>::value, T>::type log(T val) {
    return T(std::log(val));
  }

  template<typename T>
  typename std::enable_if<!std::is_class<T>::value, T>::type exp(T val) {
    return T(std::exp(val));
  }

  /**
   * Models the relationship between the requested rate of rise and on time of the duty cycle
   */

  template<typename Number>
  class StepperPlantModel {
    private:
      Number _coilR;
      Number _coilL;
      Number _v;
      Number _emfK;
      Number _period;

      int32_t _ticks;
      PwmChannel _positiveChannel;
      PwmChannel _negativeChannel;

      static constexpr Number one = Number(1.0);

    public:
      /**
       * Setup the stepper model with the given parameters for attack:
       * L *di/dt  = V-k*S - i*R
       *
       * or di/dt = (V-k*s - i*R)/ L
       *
       * for decay the formula is slightly different:
       *
       *  di/dt = (V + k*s + i*R)/L
       *
       * Therefore the effective change in current is:
       *
       * di = D(V-k*s-I*R)/L - (1-D)(V+k*s+I*R)/L
       *
       * or di = (2*D*V - (V+e+I*R))/L
       *
       * Therefore
       *
       * @param coilResistance the resistance of the coil
       * @param coilInductance the inductance of the coil
       * @param k the ratio of speed to emf.
       * @param period the period in seconds
       * @param ticks the number of PWM counts in a period
       * @param positive the PWM channel that causes positive current in the coil
       * @param negative the PWM channel that causes negative current in the coil
       *
       * The
       */
      StepperPlantModel(Number V, Number coilResistance, Number coilInductance, Number k, Number period,
          PwmChannel positive, PwmChannel negative, int32_t ticks) :
              _coilR(coilResistance),
              _coilL(coilInductance),
              _v(V),
              _emfK(k),
              _period(period),
              _ticks(ticks),
              _positiveChannel(positive),
              _negativeChannel(negative) {
      }

      Number delayFromStartCurrent(const Number &startI, Number speed) {
        constexpr Number one(1.0);
        return -_coilL / _coilR * log(one - (startI * _coilR) / (_v - _emfK * speed));
      }

      Number computeCurrent(const Number t, Number speed) {
        return (_v - _emfK * speed) / _coilR * (1 - exp(-_coilR / _coilL * t));
      }

      /**
       * Compute the final current at the end of a complete cycle for a given duty ratio,
       * initial current and
       */
      __attribute__((optimize("-Og")))
       __attribute__((noinline))
       inline Number computeCurrentAtEndOfCycle(Number speed,
          Number initialCurrent, DutyCycle duty) {
        if (duty.channel == _negativeChannel) {
          initialCurrent = -initialCurrent;
        }
        auto ton = _period * duty.duty / _ticks;
        auto toff = _period * (_ticks - duty.duty) / _ticks;
        auto t0 = delayFromStartCurrent(initialCurrent, speed);
        auto Ipeak = computeCurrent(t0 + ton, speed);
        auto t1 = delayFromStartCurrent(-Ipeak, speed);
        auto Ifinal = -computeCurrent(t1 + toff, speed);
        if (duty.channel == _negativeChannel) {
          Ifinal = -Ifinal;
        }
        return Ifinal;
      }

      /**
       * Compute the final current at the end of a complete cycle for a given duty ratio,
       * initial current and speed
       */
      __attribute__((optimize("-Og")))
       inline Number computeInstantaeousCurrent(Number speed, Number initialCurrent,
          Number duty, Number t) {
        auto ton = duty * _period;
        auto t0 = delayFromStartCurrent(initialCurrent, speed);
        if (t < ton) {
          return computeCurrent(t0 + t, speed);
        }
        auto Ipeak = computeCurrent(t0 + ton, speed);
        t0 = delayFromStartCurrent(-Ipeak, speed);
        return -computeCurrent(t0 + t - ton, speed);
      }

  };

}

#endif

