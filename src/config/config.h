#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>
#include "../types.h"
#include "controller_config.h"

namespace FanControl {

class Config {
	public:
		// General global settings which should only appear once
		static char const* const SYSTEM_CONFIG_FILE_PATH;
		static char const* const USER_CONFIG_FILE_PATH;
		static char const* const LOG_TRESHOLD_ATTRIBUTE;
		static char const* const CONTROL_INTERVAL_ATTRIBUTE;
		static Duration const    CONTROL_INTERVAL_DEFAULT_VALUE;
		// Settings which define sensor/actuators and should be iterated with a
		// suffix ".<number>" for each sensor/actuator
		static char const* const TEMPERATURE_SENSOR_PATH_ATTRIBUTE;
		static char const* const PWM_ACTUATOR_PATH_ATTRIBUTE;

	public:
		typedef std::vector<std::string> StringSeq;
		typedef std::vector<ControllerConfig> ControllerConfigSeq;

	private:
		Config();
		Config( Config const& ) = delete;
		Config& operator=( Config const& ) = delete;

	public:
		static Config& get();
		void loadDefaults();
		void loadFromFile();
		void logConfiguration() const;
		Duration getControlInterval() const { return controlInterval; };
		StringSeq const& getTemperatureSensorPathSeq() const {
			return temperatureSensorPaths;
		};
		StringSeq const& getPwmActuatorPathSeq() const {
			return pwmActuatorPaths;
		};
		ControllerConfigSeq const& getControllerConfigSeq() const {
			return controllerConfigs;
		};

	private:
		void loadLogTreshold( std::string const& value );

	private:
		Duration controlInterval;
		StringSeq temperatureSensorPaths;
		StringSeq pwmActuatorPaths;
		ControllerConfigSeq controllerConfigs;
};

}

#endif
