#include "temp_sensor.h"

namespace FanControl {

TemperatureSensor::TemperatureSensor( std::string const& devFilePath ) :
	fileStream() {
	fileStream.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	fileStream.open( devFilePath );
}

Temperature TemperatureSensor::getValue() {
	Temperature t;
	fileStream.seekg(0);
	fileStream >> t;
	return t;
}

}
