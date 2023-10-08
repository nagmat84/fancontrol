#include "runtime_config.h"

#include <sstream>
#include <string>
#include <fstream>
#include <regex>
#include "logger2.h"

namespace AmdGpuFanControl {

// General global settings which should only appear once
char const* const RuntimeConfig::SYSTEM_CONFIG_FILE_PATH = "/etc/amdgpu-fanctrl.conf";
char const* const RuntimeConfig::USER_CONFIG_FILE_PATH = "/~/.local/amdgpu-fanctrl.conf";
char const* const RuntimeConfig::LOG_TRESHOLD_ATTRIBUTE = "LOG_TRESHOLD";
char const* const RuntimeConfig::CONTROL_INTERVAL_ATTRIBUTE = "CONTROL_INTERVAL";
Duration const    RuntimeConfig::CONTROL_INTERVAL_DEFAULT_VALUE( Duration( 1000 ) );

// Settings which define sensor/actuators and should be iterated with a
// suffix ".<number>" for each sensor/actuator
char const* const RuntimeConfig::
	TEMPERATURE_SENSOR_PATH_ATTRIBUTE = "TEMPERATURE_SENSOR_PATH";
char const* const RuntimeConfig::
	PWM_ACTUATOR_PATH_ATTRIBUTE = "PWM_ACTUATOR_PATH";

// Settings which define a controller ans should be iterated with a
// suffix ".<number>" for each controller
char const* const  RuntimeConfig::ControllerConfig::
	TEMPERATURE_SENSOR_INDEX_ATTRIBUTE = "TEMPERATURE_SENSOR_INDEX";
char const* const  RuntimeConfig::ControllerConfig::
	PWM_ACTUATOR_INDEX_ATTRIBUTE = "PWM_ACTUATOR_INDEX";
char const* const  RuntimeConfig::ControllerConfig::
	UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE = "UPWARD_TEMPERATURE_HYSTERESIS";
Temperature const  RuntimeConfig::ControllerConfig::
	UPWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE( 500 );
char const* const  RuntimeConfig::ControllerConfig::
	DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE = "DOWNWARD_TEMPERATURE_HYSTERESIS";
Temperature const  RuntimeConfig::ControllerConfig::
	DOWNWARD_TEMPERATURE_HYSTERESIS_DEFAULT_VALUE( 3000 );
char const* const  RuntimeConfig::ControllerConfig::
	BASE_CONTROL_TEMPERATURE_ATTRIBUTE = "BASE_CONTROL_TEMPERATURE";
char const* const  RuntimeConfig::ControllerConfig::
	BASE_CONTROL_PWM_ATTRIBUTE = "BASE_CONTROL_PWM";
ControlPoint const RuntimeConfig::ControllerConfig::
	BASE_CONTROL_POINT_DEFAULT_VALUE( { 40000, 70} );
char const* const  RuntimeConfig::ControllerConfig::
	MIN_CONTROL_TEMPERATURE_ATTRIBUTE = "LOW_CONTROL_TEMPERATURE";
char const* const  RuntimeConfig::ControllerConfig::
	MIN_CONTROL_PWM_ATTRIBUTE = "LOW_CONTROL_PWM";
ControlPoint const RuntimeConfig::ControllerConfig::
	MIN_CONTROL_POINT_DEFAULT_VALUE( { 45000, 57} );
char const* const  RuntimeConfig::ControllerConfig::
	MAX_CONTROL_TEMPERATURE_ATTRIBUTE = "HIGH_CONTROL_TEMPERATURE";
char const* const  RuntimeConfig::ControllerConfig::
	MAX_CONTROL_PWM_ATTRIBUTE = "HIGH_CONTROL_PWM";
ControlPoint const RuntimeConfig::ControllerConfig::
	MAX_CONTROL_POINT_DEFAULT_VALUE( { 95000, 255} );

RuntimeConfig::ConfigLine::ConfigLine(std::string const& line) :
	attribute(),
	index(-1),
	value(),
	valid(false),
	failed(false)
{
	static std::regex const commentOrEmpty("^\\s*($|#])");
	// This regex has three caputing groups: attribute, index, value
	// The value-group is special, because we want to support values with
	// spaces in the middle, but spaces at the beginning and end shall be removed,
	// i.e. a valid line could look like this
	//
	// Name = John Dear
	//
	// In this case the value is "John Dear" with a space between first and
	// last name, but any preceeding blank (i.e. between the equal sign and the
	// first letter) as well as any spurious blank at the end shall be ignored.
	// The regex `\\s*((?:\\s*\\S+)+)\\s*$`works, because regex are greedy.
	// The first `\\s*\\ catches all preceeding blanks greedily.
	// The actual value is captured by `((?:\\s*\\S+)+)` with a list of
	// inner sub-expressions that allow optional, seperating blanks at the
	// beginning but must at least contain a non-space at the end.
	static std::regex const attrIdxValuePair("^\\s*([_A-Za-z]+)(?:\\.(\\d+))?\\s*=\\s*((?:\\s*\\S+)+)\\s*$");
	std::smatch pieces;

	if( std::regex_match(line, commentOrEmpty) )
		return;
	if( std::regex_match(line, pieces, attrIdxValuePair) ) {
		if( pieces.size() == 2 ) {
			attribute = pieces[0];
			value = pieces[1];
			valid = true;
		} else if( pieces.size() == 3 ) {
			attribute = pieces[0];
			index = std::stoi(pieces[1]);
			value = pieces[2];
			valid = true;
		} else {
			failed = true;
		}
	}
	failed = true;
}


RuntimeConfig::RuntimeConfig() {
	loadDefaults();
}

RuntimeConfig& RuntimeConfig::get() {
	static RuntimeConfig singleton;
	return singleton;
}

void RuntimeConfig::loadDefaults() {
	controlInterval = CONTROL_INTERVAL_DEFAULT_VALUE;
	temperatureSensorPaths.clear();
	pwmActuatorPaths.clear();
	controllerConfigs.clear();
}

void RuntimeConfig::loadFromFile() {
	std::ifstream configFileStream;
	configFileStream.open( USER_CONFIG_FILE_PATH );
	if ( !configFileStream.is_open() )
		configFileStream.open( SYSTEM_CONFIG_FILE_PATH );
	if ( !configFileStream.is_open() )
		return;
	configFileStream.exceptions( std::ifstream::failbit | std::ifstream::badbit );

	for( std::string line; std::getline(configFileStream, line); ) {
		ConfigLine configLine(line);
		if ( !configLine.isValid() ) continue;

		if( configLine.getAttribute().compare( LOG_TRESHOLD_ATTRIBUTE ) == 0 ) {
			loadLogTreshold( configLine.getValue() );
		}
		if( configLine.getAttribute().compare( CONTROL_INTERVAL_ATTRIBUTE ) == 0 ) {
			controlInterval = Duration( configLine.getValueAsUL() );
		}


		if(
				configAttribute.find( TEMPERATURE_SENSOR_PATH_ATTRIBUTE ) !=
				std::string::npos
		) {
				std::istringstream attributeStream( configAttribute );
				std::string configIdx;
				std::getline( attributeStream, configIdx, '.');
				std::getline( attributeStream, configIdx );
		}



		if( configAttribute.find( ControllerConfig::UPWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> upwardTemperatureHysteresis;
		}
		if( configAttribute.find( ControllerConfig::DOWNWARD_TEMPERATURE_HYSTERESIS_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> downwardTemperatureHysteresis;
		}
		if( configAttribute.find( ControllerConfig::BASE_CONTROL_TEMPERATURE_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> baseControlPoint.temp;
		}
		if( configAttribute.find( ControllerConfig::BASE_CONTROL_PWM_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> baseControlPoint.pwmValue;
		}
		if( configAttribute.find( ControllerConfig::MIN_CONTROL_TEMPERATURE_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> lowControlPoint.temp;
		}
		if( configAttribute.find( ControllerConfig::MIN_CONTROL_PWM_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> lowControlPoint.pwmValue;
		}
		if( configAttribute.find( ControllerConfig::MAX_CONTROL_TEMPERATURE_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> highControlPoint.temp;
		}
		if( configAttribute.find( ControllerConfig::MAX_CONTROL_PWM_ATTRIBUTE ) != std::string::npos ) {
				valueStream >> highControlPoint.pwmValue;
		}
	}

	std::string configAttribute;
	std::string configValue;
	while ( !configFileStream.eof() ) {
		std::getline( configFileStream, configAttribute, '=' );
		std::getline( configFileStream, configValue);
		std::istringstream valueStream( configValue );
		valueStream.exceptions( std::ifstream::failbit | std::ifstream::badbit );

	}

	logConfiguration();
}

void RuntimeConfig::loadLogTreshold( std::string const& value ) {
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

void RuntimeConfig::logConfiguration() const {
	LogStream& log( LogStream::get() );
	log << LogBuffer::Severity::INFO;
	log << CONTROL_INTERVAL_ATTRIBUTE
	    << " = "
	    << controlInterval.count() << std::flush;
	for(TemperatureSensorIdx i = 0; i != temperatureSensorPaths.size(); i++) {
		log << TEMPERATURE_SENSOR_PATH_ATTRIBUTE << "." << i
		    << " = "
		    << temperatureSensorPaths[i] << std::flush;
	}
	for(PwmActuatorIdx i = 0; i != pwmActuatorPaths.size(); i++) {
		log << PWM_ACTUATOR_PATH_ATTRIBUTE << "." << i
		    << " = "
		    << pwmActuatorPaths[i] << std::flush;
	}
	for(ControllerConfigIdx i = 0; i != controllerConfigs.size(); i++) {
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
