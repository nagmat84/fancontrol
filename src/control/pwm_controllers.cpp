#include "pwm_controller.h"
#include "pwm_controllers.h"
#include "../hal/temp_sensor.h"
#include "../hal/temp_sensor_factory.h"
#include "../hal/pwm_actuator.h"
#include "../hal/pwm_actuator_factory.h"
#include "../logging/logger2.h"

#include <chrono>
#include <thread>

namespace FanControl {

PWMControllers::PWMControllers() :
	config( Config::get() ),
	runState( RunState::STOPPED ),
	temperatureSensors(),
	pwmActuators(),
	pwmControllers() {
	TemperatureSensorFactory& temperatureSensorFactory( TemperatureSensorFactory::get() );
	PWMActuatorFactory& pwmActuatorFactory( PWMActuatorFactory::get() );

	temperatureSensors.push_back( temperatureSensorFactory.getSensor(
		 config.getTempSensorFilePath()
	) );
	pwmActuators.push_back( pwmActuatorFactory.getActuator(
		config.getPwmActuatorFilePath()
	) );
	pwmControllers.push_back(
		PWMController( temperatureSensors.back(), pwmActuators.back() )
	);
}

PWMControllers& PWMControllers::get() {
	static PWMControllers singleton;
	return singleton;
}

int PWMControllers::run() {
	if( runState == RunState::RUNNING ) return 0;
	runState = RunState::RUNNING;
	return loop();
}

int PWMControllers::loop() {
	LogStream& log(LogStream::get());
	log << LogBuffer::Severity::INFO;

	log << "Entering control loop" << std::flush;
	while( runState == RunState::RUNNING ) {
		for ( auto& controller : pwmControllers ) controller.update();
		std::this_thread::sleep_for( config.getControlInterval() );
	}
	log << "Exiting control loop" << std::flush;
	return 0;
}
}
