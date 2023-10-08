#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_

#include <fstream>
#include <memory>
#include "types.h"

namespace AmdGpuFanControl {

class TemperatureSensorFactory;

class TemperatureSensor {
	friend class TemperatureSensorFactory;

	public:
		typedef std::shared_ptr<TemperatureSensor> Ptr;

	protected:
		TemperatureSensor( std::string const& devFilePath );
		TemperatureSensor( TemperatureSensor const& ) = delete;

	public:
		TemperatureSensor( TemperatureSensor&& other ) : fileStream( std::move( other.fileStream ) ) {};
		Temperature getValue();

	private:
		std::ifstream fileStream;
};
}

#endif
