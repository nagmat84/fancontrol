#ifndef _PWM_ACTUATOR_FACTORY_H_
#define _PWM_ACTUATOR_FACTORY_H_

#include <unordered_map>
#include <memory>
#include <string>
#include "pwm_actuator.h"

namespace AmdGpuFanControl {

class PWMActuatorFactory {
	typedef std::weak_ptr<PWMActuator> WeakActuatorPtr;
	typedef std::unordered_map<std::string, WeakActuatorPtr> ActuatorRepo;

	protected:
		PWMActuatorFactory() : repo() {};
		PWMActuatorFactory(PWMActuatorFactory const&) = delete;
		PWMActuatorFactory(PWMActuatorFactory&&) = delete;

	public:
		static PWMActuatorFactory& get();

		PWMActuator::Ptr getActuator( std::string const& devFilePath );

	private:
		ActuatorRepo repo;
};

}

#endif
