#include "pwm_actuator_factory.h"

namespace AmdGpuFanControl {

PWMActuatorFactory& PWMActuatorFactory::get() {
	static PWMActuatorFactory singleton;
	return singleton;
}

PWMActuator::Ptr PWMActuatorFactory::getActuator(
	std::string const& devFilePath
) {
	WeakActuatorPtr& weakActuatorPtr( repo.emplace( std::pair( devFilePath, WeakActuatorPtr() ) ).first->second );
	PWMActuator::Ptr actuatorPtr;
	if( weakActuatorPtr.expired() ) {
		actuatorPtr.reset( new PWMActuator(devFilePath) );
		weakActuatorPtr = actuatorPtr;
	} else {
		actuatorPtr = weakActuatorPtr.lock();
	}
	return actuatorPtr;
}

}
