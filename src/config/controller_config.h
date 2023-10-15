#ifndef _CONTROLLER_CONFIG_H_
#define _CONTROLLER_CONFIG_H_

#include "../types.h"

namespace FanControl {

class ConfigOption;

class ControllerConfig {
	friend class Config;

	public:
		// Settings which define a controller ans should be iterated with a
		// suffix ".<number>" for each controller
		static char const* const  TEMPERATURE_SENSOR_INDEX_ATTRIBUTE;
		static char const* const  PWM_ACTUATOR_INDEX_ATTRIBUTE;
		static char const* const  UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE;
		static Temperature const  UPWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE;
		static char const* const  DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE;
		static Temperature const  DOWNWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE;
		static char const* const  BASE_CONTROL_TEMPERATURE_ATTRIBUTE;
		static char const* const  BASE_CONTROL_PWM_ATTRIBUTE;
		static ControlPoint const BASE_CONTROL_POINT_DEFAULT_VALUE;
		static char const* const  MIN_CONTROL_TEMPERATURE_ATTRIBUTE;
		static char const* const  MIN_CONTROL_PWM_ATTRIBUTE;
		static ControlPoint const MIN_CONTROL_POINT_DEFAULT_VALUE;
		static char const* const  MAX_CONTROL_TEMPERATURE_ATTRIBUTE;
		static char const* const  MAX_CONTROL_PWM_ATTRIBUTE;
		static ControlPoint const MAX_CONTROL_POINT_DEFAULT_VALUE;

	public:
		static bool handles(ConfigOption const& configOption);

	public:
		ControllerConfig() :
			temperatureSensorIdx(-1),
			pwmActuatorIdx(-1),
			upwardTemperatureHysteresis(
				UPWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE
			),
			downwardTemperatureHysteresis(
				DOWNWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE
			),
			baseControlPoint(BASE_CONTROL_POINT_DEFAULT_VALUE),
			minControlPoint(MIN_CONTROL_POINT_DEFAULT_VALUE),
			maxControlPoint(MAX_CONTROL_POINT_DEFAULT_VALUE) {};

		ControllerConfig(ControllerConfig const& other) :
			temperatureSensorIdx(other.temperatureSensorIdx),
			pwmActuatorIdx(other.pwmActuatorIdx),
			upwardTemperatureHysteresis(other.upwardTemperatureHysteresis),
			downwardTemperatureHysteresis(other.downwardTemperatureHysteresis),
			baseControlPoint(other.baseControlPoint),
			minControlPoint(other.minControlPoint),
			maxControlPoint(other.maxControlPoint) {};

		size_t getTemperatureSensorIdx() const {
			return temperatureSensorIdx;
		};
		size_t getPwmActuatorIdx() const {
			return pwmActuatorIdx;
		};
		Temperature getUpwardTemperatureHysteresis() const {
			return upwardTemperatureHysteresis;
		};
		Temperature getDownwardTemperatureHysteresis() const {
			return downwardTemperatureHysteresis;
		};
		ControlPoint const& getBaseControlPoint() const {
			return baseControlPoint;
		};
		ControlPoint const& getLowControlPoint() const {
			return minControlPoint;
		};
		ControlPoint const& getHighControlPoint() const {
			return maxControlPoint;
		};

	protected:
		void handle(ConfigOption const& configOption);

	private:
		size_t temperatureSensorIdx;
		size_t pwmActuatorIdx;
		Temperature upwardTemperatureHysteresis;
		Temperature downwardTemperatureHysteresis;
		ControlPoint baseControlPoint;
		ControlPoint minControlPoint;
		ControlPoint maxControlPoint;
};
}

#endif
