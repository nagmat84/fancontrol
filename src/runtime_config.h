#ifndef _RUNTIME_CONFIG_H_
#define _RUNTIME_CONFIG_H_

#include <string>
#include <vector>
#include "types.h"

namespace AmdGpuFanControl {

class RuntimeConfig {
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
		typedef std::vector<std::string> TemperatureSensorPathSeq;
		typedef TemperatureSensorPathSeq::size_type TemperatureSensorIdx;
		typedef std::vector<std::string> PwmActuatorPathSeq;
		typedef PwmActuatorPathSeq::size_type PwmActuatorIdx;

		class ControllerConfig {
			friend class RuntimeConfig;

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
				TemperatureSensorPathIdx getTemperatureSensorIdx() const {
					return temperatureSensorIdx;
				};
				PwmActuatorPathIdx getPwmActuatorIdx() const {
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
				void setTemperatureSensorIdx(TemperatureSensorIdx idx) {
					temperatureSensorIdx = idx;
				};
				void setPwmActuatorIdx(PwmActuatorIdx idx) {
					pwmActuatorIdx = idx;
				};
				void setUpwardTemperatureHysteresis(Temperature t) {
					upwardTemperatureHysteresis = t;
				};
				void setDownwardTemperatureHysteresis(Temperature t) {
					downwardTemperatureHysteresis = t;
				};
				void setBaseControlPoint(ControlPoint const& cp) {
					baseControlPoint = cp;
				};
				void setLowControlPoint(ControlPoint const& cp) {
					minControlPoint = cp;
				};
				void setHighControlPoint(ControlPoint const& cp) {
					maxControlPoint = cp;
				};

			private:
				TemperatureSensorIdx temperatureSensorIdx;
				PwmActuatorIdx pwmActuatorIdx;
				Temperature upwardTemperatureHysteresis;
				Temperature downwardTemperatureHysteresis;
				ControlPoint baseControlPoint;
				ControlPoint minControlPoint;
				ControlPoint maxControlPoint;
		};

		typedef std::vector<ControllerConfig> ControllerConfigSeq;
		typedef ControllerConfigSeq::size_type ControllerConfigIdx;

		class ConfigLine {
			public:
				ConfigLine(std::string const& line);
				ConfigLine(ConfigLine const& other) :
					attribute(other.attribute),
					index(other.index),
					value(other.value),
					valid(false),
					failed(false) {};
				ConfigLine(ConfigLine&& other) :
					attribute(std::move(other.attribute)),
					index(other.index),
					value(std::move(other.value)),
					valid(other.valid),
					failed(other.failed) {
					other.valid = false;
				};
				std::string const& getAttribute() const { return attribute; };
				size_t getIndex() const { return index; };
				std::string const& getValue() const { return value; };
				unsigned long getValueAsUL() const { return std::stoul(value); }
				/**
				 * Indicates whether the associated line has successfully been parsed
				 * as a proper configuration line with an (attribute,value)-pair.
				 *
				 * Note, this is not the inverse of `hasFailed`.
				 * It is possible that `isValid` and ` hasFailed` are both `false`
				 * at the same time, i.e. if the line is empty or a comment.
				 */
				bool isValid() const { return valid; };
				/**
				 * Indicates whether the associated line is syntactically wrong and
				 * could not be parsed.
				 *
				 * Note, this not the inverse of `isValid`.
				 * It is possible that `isValid` and ` hasFailed` are both `false`
				 * at the same time, i.e. if the line is empty or a comment.
				 */
				bool hasFailed() const { return failed; };

			private:
				std::string attribute;
				size_t index;
				std::string value;
				bool valid;
				bool failed;
		};
	private:
		RuntimeConfig();
		RuntimeConfig( RuntimeConfig const& ) = delete;
		RuntimeConfig& operator=( RuntimeConfig const& ) = delete;

	public:
		static RuntimeConfig& get();
		void loadDefaults();
		void loadFromFile();
		void logConfiguration() const;
		Duration getControlInterval() const { return controlInterval; };
		PwmActuatorPathSeq const& getTemperatureSensorPathSeq() const {
			return temperatureSensorPaths;
		};
		TemperatureSensorPathSeq const& getPwmActuatorPathSeq() const {
			return pwmActuatorPaths;
		};
		ControllerConfigSeq const& getControllerConfigSeq() const {
			return controllerConfigs;
		};

	private:
		void loadLogTreshold( std::string const& value );

	private:
		Duration controlInterval;
		TemperatureSensorPathSeq temperatureSensorPaths;
		PwmActuatorPathSeq pwmActuatorPaths;
		ControllerConfigSeq controllerConfigs;
};

}

#endif
