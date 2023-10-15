#include "pwm_controller.h"
#include "../logging/logger2.h"

#include <sstream>

namespace FanControl {

unsigned int const PWMController::INITIAL_TEMPERATURE( std::numeric_limits<Temperature>::min() );
unsigned int const PWMController::INITIAL_PWM_VALUE( std::numeric_limits<PwmValue>::max() );

void PWMController::update() {
	LogStream& log( LogStream::get() );
	log << LogBuffer::Severity::DEBUG;

	Temperature const temp = sensor->getValue();
	log << "Previous temperature: " << lastTemperature << " °mC; current temperature: " << temp << " °mC" << std::flush;

	if( !needsUpdate(temp) ) {
		log << "No setting update for this control cycle needed" << std::flush;
		return;
	}

	PwmValue pwmValue = calcPwmValue(temp);
	log << "Previous PWM value: " << lastPwmValue << "; calculated PWM value: " << pwmValue << std::flush;

	// If the actuator transits from "off" to "on", the next PWM value must be
	// at least `getBaseControlPoint().pwmValue` to ensure that the fan savely
	// starts spinning for DC controlled fans.
	if (lastPwmValue == 0 && pwmValue != 0) {
		pwmValue = std::max(pwmValue, config.getBaseControlPoint().pwmValue);
		hasJustStartedSpinning = true;
		log << "Fan starts spinning; new PWM value: " << pwmValue << std::flush;
	} else {
		hasJustStartedSpinning = false;
	}

	actuator->setValue(pwmValue);
	lastTemperature = temp;
	lastPwmValue = pwmValue;
}

bool PWMController::needsUpdate(unsigned int temperature) const {
	return
		( temperature > lastTemperature + config.getUpwardTemperatureHysteresis() ) ||
		( temperature + config.getDownwardTemperatureHysteresis() < lastTemperature ) ||
		hasJustStartedSpinning ||
		lastTemperature == INITIAL_TEMPERATURE ||
		lastPwmValue == INITIAL_PWM_VALUE;
}

PwmValue PWMController::calcPwmValue( Temperature temperature ) const {
	ControlPoint const& bCP( config.getBaseControlPoint() );
	ControlPoint const& lCP( config.getLowControlPoint() );
	ControlPoint const& hCP( config.getHighControlPoint() );

	if( temperature < bCP.temp )
		return 0;
	if( temperature < lCP.temp )
		return lCP.pwmValue;
	if( temperature < hCP.temp ) {
		float const slope = (float)(hCP.pwmValue - lCP.pwmValue) / (float)(hCP.temp - lCP.temp);
		return slope * (float)(temperature - lCP.temp) + lCP.pwmValue;
	}
	return hCP.pwmValue;
}

}
