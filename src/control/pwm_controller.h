#ifndef _PWM_CONTROLLER_H_
#define _PWM_CONTROLLER_H_

#include "../config/controller_config.h"
#include "../hal/pwm_actuator.h"
#include "../hal/temp_sensor.h"

namespace FanControl {
class PWMController {
	private:
		static Temperature const INITIAL_TEMPERATURE;
		static PwmValue const INITIAL_PWM_VALUE;

	public:
		PWMController(
			ControllerConfig const& c,
			TemperatureSensor::Ptr const& s,
			PWMActuator::Ptr const& a
		) :
			config( c ),
			lastTemperature( INITIAL_TEMPERATURE ),
			lastPwmValue( INITIAL_PWM_VALUE ),
			hasJustStartedSpinning( false ),
			sensor( s ),
			actuator( a ) {};
		void update();

	private:
		bool needsUpdate( Temperature temperature ) const;
		PwmValue calcPwmValue( Temperature temperature ) const;

	private:
		ControllerConfig const& config;
		Temperature lastTemperature;
		PwmValue lastPwmValue;
		bool hasJustStartedSpinning;
		TemperatureSensor::Ptr sensor;
		PWMActuator::Ptr actuator;
};
}

#endif
