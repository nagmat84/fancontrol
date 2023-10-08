#ifndef _PWM_CONTROLLERS_H_
#define _PWM_CONTROLLERS_H_

#include "runtime_config.h"
#include "pwm_controller.h"
#include <vector>

namespace AmdGpuFanControl {
class PWMControllers {
	public:
		enum RunState {
			STOPPED,
			RUNNING
		};

		typedef std::vector<TemperatureSensor::Ptr> TemperatureSensorCollection;
		typedef std::vector<PWMActuator::Ptr> PWMActuatorCollection;
		typedef std::vector<PWMController> PWMControllerCollection;

	private:
		PWMControllers();
		PWMControllers( PWMControllers const& ) = delete;
		PWMControllers( PWMControllers&& ) = delete;
		PWMControllers& operator=( PWMControllers const& ) = delete;

	public:
		static PWMControllers& get();
		int run();
		inline void stop() { runState = RunState::STOPPED; };

	protected:
		int loop();

	private:
		RuntimeConfig const& config;
		RunState runState;
		TemperatureSensorCollection temperatureSensors;
		PWMActuatorCollection pwmActuators;
		PWMControllerCollection pwmControllers;
};
}

#endif
