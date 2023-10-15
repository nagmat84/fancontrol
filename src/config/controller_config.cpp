#include "controller_config.h"
#include "config_option.h"

namespace FanControl {

// Settings which define a controller ans should be iterated with a
// suffix ".<number>" for each controller
char const* const  ControllerConfig::
	TEMPERATURE_SENSOR_INDEX_ATTRIBUTE = "TEMPERATURE_SENSOR_INDEX";
char const* const  ControllerConfig::
	PWM_ACTUATOR_INDEX_ATTRIBUTE = "PWM_ACTUATOR_INDEX";
char const* const  ControllerConfig::
	UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE = "UPWARD_TEMPERATURE_HYSTERESIS";
Temperature const  ControllerConfig::
	UPWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE( 500 );
char const* const  ControllerConfig::
	DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE = "DOWNWARD_TEMPERATURE_HYSTERESIS";
Temperature const  ControllerConfig::
	DOWNWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE( 3000 );
char const* const  ControllerConfig::
	BASE_CONTROL_TEMPERATURE_ATTRIBUTE = "BASE_CONTROL_TEMPERATURE";
char const* const  ControllerConfig::
	BASE_CONTROL_PWM_ATTRIBUTE = "BASE_CONTROL_PWM";
ControlPoint const ControllerConfig::
	BASE_CONTROL_POINT_DEFAULT_VALUE( { 40000, 70} );
char const* const  ControllerConfig::
	MIN_CONTROL_TEMPERATURE_ATTRIBUTE = "LOW_CONTROL_TEMPERATURE";
char const* const  ControllerConfig::
	MIN_CONTROL_PWM_ATTRIBUTE = "LOW_CONTROL_PWM";
ControlPoint const ControllerConfig::
	MIN_CONTROL_POINT_DEFAULT_VALUE( { 45000, 57} );
char const* const  ControllerConfig::
	MAX_CONTROL_TEMPERATURE_ATTRIBUTE = "HIGH_CONTROL_TEMPERATURE";
char const* const  ControllerConfig::
	MAX_CONTROL_PWM_ATTRIBUTE = "HIGH_CONTROL_PWM";
ControlPoint const ControllerConfig::
	MAX_CONTROL_POINT_DEFAULT_VALUE( { 95000, 255} );

bool ControllerConfig::handles(ConfigOption const& configOption) {
	return
		configOption.getAttribute().compare(TEMPERATURE_SENSOR_INDEX_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(PWM_ACTUATOR_INDEX_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(BASE_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(BASE_CONTROL_PWM_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(MIN_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(MIN_CONTROL_PWM_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(MAX_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 ||
		configOption.getAttribute().compare(MAX_CONTROL_PWM_ATTRIBUTE) == 0;
}

void ControllerConfig::handle(ConfigOption const& configOption) {
	std::string const& attr(configOption.getAttribute());
	if( attr.compare(TEMPERATURE_SENSOR_INDEX_ATTRIBUTE) == 0 )
		temperatureSensorIdx = configOption.getValueAsUL();
	if( attr.compare(PWM_ACTUATOR_INDEX_ATTRIBUTE) == 0 )
		pwmActuatorIdx = configOption.getValueAsUL();
	if( attr.compare(UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE) == 0 )
		upwardTemperatureHysteresis = configOption.getValueAsUL();
	if( attr.compare(DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE) == 0 )
		downwardTemperatureHysteresis = configOption.getValueAsUL();
	if( attr.compare(BASE_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 )
		baseControlPoint.temp = configOption.getValueAsUL();
	if( attr.compare(BASE_CONTROL_PWM_ATTRIBUTE) == 0 )
		baseControlPoint.pwmValue = configOption.getValueAsUL();
	if( attr.compare(MIN_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 )
		minControlPoint.temp = configOption.getValueAsUL();
	if( attr.compare(MIN_CONTROL_PWM_ATTRIBUTE) == 0 )
		minControlPoint.pwmValue = configOption.getValueAsUL();
	if( attr.compare(MAX_CONTROL_TEMPERATURE_ATTRIBUTE) == 0 )
		maxControlPoint.temp = configOption.getValueAsUL();
	if( attr.compare(MAX_CONTROL_PWM_ATTRIBUTE) == 0 )
		maxControlPoint.pwmValue = configOption.getValueAsUL();
}

}
