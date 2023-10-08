#include "temp_sensor_factory.h"

namespace AmdGpuFanControl {

TemperatureSensorFactory& TemperatureSensorFactory::get() {
	static TemperatureSensorFactory singleton;
	return singleton;
}

TemperatureSensor::Ptr TemperatureSensorFactory::getSensor(
	std::string const& devFilePath
) {
	WeakSensorPtr& weakSensorPtr( repo.emplace( std::pair( devFilePath, WeakSensorPtr() ) ).first->second );
	TemperatureSensor::Ptr sensorPtr;
	if( weakSensorPtr.expired() ) {
		sensorPtr.reset( new TemperatureSensor(devFilePath) );
		weakSensorPtr = sensorPtr;
	} else {
		sensorPtr = weakSensorPtr.lock();
	}
	return sensorPtr;
}

}
