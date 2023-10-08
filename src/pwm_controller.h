#ifndef _PWM_CONTROLLER_H_
#define _PWM_CONTROLLER_H_

#include "runtime_config.h"
#include "pwm_actuator.h"
#include "temp_sensor.h"

namespace AmdGpuFanControl {
class PWMController {
	private:
		static Temperature const INITIAL_TEMPERATURE;
		static PwmValue const INITIAL_PWM_VALUE;

	public:
		PWMController( TemperatureSensor::Ptr const& s, PWMActuator::Ptr const& a );
		void update();

	private:
		bool needsUpdate( Temperature temperature ) const;
		PwmValue calcPwmValue( Temperature temperature ) const;

	private:
		RuntimeConfig const& config;
		Temperature lastTemperature;
		PwmValue lastPwmValue;
		bool hasJustStartedSpinning;
		TemperatureSensor::Ptr sensor;
		PWMActuator::Ptr actuator;
};
}

#endif
