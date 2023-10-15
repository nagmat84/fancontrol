#include "config.h"
#include "config_option.h"
#include "../logging/logger2.h"

#include <sstream>
#include <string>
#include <fstream>


namespace FanControl {

// General global settings which should only appear once
char const* const Config::SYSTEM_CONFIG_FILE_PATH = "/etc/amdgpu-fanctrl.conf";
char const* const Config::USER_CONFIG_FILE_PATH = "/~/.local/amdgpu-fanctrl.conf";
char const* const Config::LOG_TRESHOLD_ATTRIBUTE = "LOG_TRESHOLD";
char const* const Config::CONTROL_INTERVAL_ATTRIBUTE = "CONTROL_INTERVAL";
Duration const    Config::CONTROL_INTERVAL_DEFAULT_VALUE( Duration( 1000 ) );

// Settings which define sensor/actuators and should be iterated with a
// suffix ".<number>" for each sensor/actuator
char const* const Config::
	TEMPERATURE_SENSOR_PATH_ATTRIBUTE = "TEMPERATURE_SENSOR_PATH";
char const* const Config::
	PWM_ACTUATOR_PATH_ATTRIBUTE = "PWM_ACTUATOR_PATH";

Config::Config() {
	loadDefaults();
}

Config& Config::get() {
	static Config singleton;
	return singleton;
}

void Config::loadDefaults() {
	controlInterval = CONTROL_INTERVAL_DEFAULT_VALUE;
	temperatureSensorPaths.clear();
	pwmActuatorPaths.clear();
	controllerConfigs.clear();
}

void Config::loadFromFile() {
	std::ifstream configFileStream;
	configFileStream.open( USER_CONFIG_FILE_PATH );
	if ( !configFileStream.is_open() )
		configFileStream.open( SYSTEM_CONFIG_FILE_PATH );
	if ( !configFileStream.is_open() )
		return;
	configFileStream.exceptions( std::ifstream::failbit | std::ifstream::badbit );

	for( std::string line; std::getline(configFileStream, line); ) {
		ConfigOption configOption(line);
		if ( !configOption.isValid() ) continue;

		if( configOption.getAttribute().compare( LOG_TRESHOLD_ATTRIBUTE ) == 0 )
			loadLogTreshold( configOption.getValue() );

		if( configOption.getAttribute().compare( CONTROL_INTERVAL_ATTRIBUTE ) == 0 )
			controlInterval = Duration( configOption.getValueAsUL() );

		if( configOption.getAttribute().compare( TEMPERATURE_SENSOR_PATH_ATTRIBUTE ) == 0 ) {
			if( configOption.getIndex() >= temperatureSensorPaths.size() )
				temperatureSensorPaths.resize( configOption.getIndex()+1 );
			temperatureSensorPaths[configOption.getIndex()] = configOption.getValue();
		}

		if( configOption.getAttribute().compare( PWM_ACTUATOR_PATH_ATTRIBUTE ) == 0 ) {
			if( configOption.getIndex() >= pwmActuatorPaths.size() )
				pwmActuatorPaths.resize( configOption.getIndex()+1 );
			pwmActuatorPaths[configOption.getIndex()] = configOption.getValue();
		}

		if( ControllerConfig::handles( configOption ) ) {
			if( configOption.getIndex() >= controllerConfigs.size() )
				controllerConfigs.resize( configOption.getIndex()+1 );
			controllerConfigs[configOption.getIndex()].handle(configOption);
		}
	}

	logConfiguration();
}

void Config::loadLogTreshold( std::string const& value ) {
	LogStream& log( LogStream::get() );
	if( value.compare("EMERGENCY") == 0 || value.compare("0") == 0 )
		log.setTreshold( LogBuffer::Severity::EMERGENCY );
	else if( value.compare("ALERT") == 0 || value.compare("1") == 0 )
		log.setTreshold( LogBuffer::Severity::ALERT );
	else if( value.compare("CRITICAL") == 0 || value.compare("2") == 0 )
		log.setTreshold( LogBuffer::Severity::CRITICAL );
	else if( value.compare("ERROR") == 0 || value.compare("3") == 0 )
		log.setTreshold( LogBuffer::Severity::ERROR );
	else if( value.compare("WARNING") == 0 || value.compare("4") == 0 )
		log.setTreshold( LogBuffer::Severity::WARNING );
	else if( value.compare("NOTICE") == 0 || value.compare("5") == 0 )
		log.setTreshold( LogBuffer::Severity::NOTICE );
	else if( value.compare("INFO") == 0 || value.compare("6") == 0 )
		log.setTreshold( LogBuffer::Severity::INFO );
	else if( value.compare("DEBUG") == 0 || value.compare("7") == 0 )
		log.setTreshold( LogBuffer::Severity::DEBUG );
	else
		log << LogBuffer::Severity::WARNING << "Invalid log level" << std::flush;
}

void Config::logConfiguration() const {
	LogStream& log( LogStream::get() );
	log << LogBuffer::Severity::INFO;
	log << CONTROL_INTERVAL_ATTRIBUTE
	    << " = "
	    << controlInterval.count() << std::flush;
	for(size_t i = 0; i != temperatureSensorPaths.size(); i++) {
		log << TEMPERATURE_SENSOR_PATH_ATTRIBUTE << "." << i
		    << " = "
		    << temperatureSensorPaths[i] << std::flush;
	}
	for(size_t i = 0; i != pwmActuatorPaths.size(); i++) {
		log << PWM_ACTUATOR_PATH_ATTRIBUTE << "." << i
		    << " = "
		    << pwmActuatorPaths[i] << std::flush;
	}
	for(size_t i = 0; i != controllerConfigs.size(); i++) {
		ControllerConfig const& ctrCnf(controllerConfigs[i]);
		log << ControllerConfig::TEMPERATURE_SENSOR_INDEX_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.temperatureSensorIdx << std::flush;
		log << ControllerConfig::PWM_ACTUATOR_INDEX_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.pwmActuatorIdx << std::flush;
		log << ControllerConfig::UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.upwardTemperatureHysteresis << std::flush;
		log << ControllerConfig::DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.downwardTemperatureHysteresis << std::flush;
		log << ControllerConfig::BASE_CONTROL_TEMPERATURE_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.baseControlPoint.temp << std::flush;
		log << ControllerConfig::BASE_CONTROL_PWM_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.baseControlPoint.pwmValue << std::flush;
		log << ControllerConfig::MIN_CONTROL_TEMPERATURE_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.minControlPoint.temp << std::flush;
		log << ControllerConfig::MIN_CONTROL_PWM_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.minControlPoint.pwmValue << std::flush;
		log << ControllerConfig::MAX_CONTROL_TEMPERATURE_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.maxControlPoint.temp << std::flush;
		log << ControllerConfig::MAX_CONTROL_PWM_ATTRIBUTE << "." << i
		    << " = "
		    << ctrCnf.maxControlPoint.pwmValue << std::flush;
	}
}

}
