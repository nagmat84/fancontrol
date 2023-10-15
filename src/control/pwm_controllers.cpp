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
	temperatureSensors( config.getTemperatureSensorPathSeq().size() ),
	pwmActuators( config.getPwmActuatorPathSeq().size() ),
	pwmControllers() {
	TemperatureSensorFactory& temperatureSensorFactory( TemperatureSensorFactory::get() );
	Config::StringSeq const& tempPaths(config.getTemperatureSensorPathSeq());
	for( size_t i = 0; i != tempPaths.size(); i++ ) {
		if( tempPaths[i].empty() ) continue;
		temperatureSensors[i] = temperatureSensorFactory.getSensor(tempPaths[i]);
	}

	PWMActuatorFactory& pwmActuatorFactory( PWMActuatorFactory::get() );
	Config::StringSeq const& pwmPaths(config.getPwmActuatorPathSeq());
	for( size_t i = 0; i != pwmPaths.size(); i++ ) {
		if( pwmPaths[i].empty() ) continue;
		pwmActuators[i] = pwmActuatorFactory.getActuator(pwmPaths[i]);
	}

	Config::ControllerConfigSeq ctrlConfs( config.getControllerConfigSeq() );
	pwmControllers.reserve( ctrlConfs.size() );
	for( auto const& conf : ctrlConfs ) {
		pwmControllers.push_back( PWMController(
			conf,
			temperatureSensors[ conf.getTemperatureSensorIdx() ],
			pwmActuators[ conf.getPwmActuatorIdx() ]
		) );
	}
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
