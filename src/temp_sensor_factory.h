#ifndef _TEMP_SENSOR_FACTORY_H_
#define _TEMP_SENSOR_FACTORY_H_

#include <unordered_map>
#include <memory>
#include <string>
#include "temp_sensor.h"

namespace AmdGpuFanControl {

class TemperatureSensorFactory {
	typedef std::weak_ptr<TemperatureSensor> WeakSensorPtr;
	typedef std::unordered_map<std::string, WeakSensorPtr> SensorRepo;

	protected:
		TemperatureSensorFactory() : repo() {};
		TemperatureSensorFactory(TemperatureSensorFactory const&) = delete;
		TemperatureSensorFactory(TemperatureSensorFactory&&) = delete;

	public:
		static TemperatureSensorFactory& get();

		TemperatureSensor::Ptr getSensor( std::string const& devFilePath );

	private:
		SensorRepo repo;
};

}

#endif
