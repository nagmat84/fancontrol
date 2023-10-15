#ifndef _TYPES_H_
#define _TYPES_H_

#include <chrono>

namespace FanControl {

typedef unsigned int Temperature;
typedef unsigned int PwmValue;
typedef std::chrono::milliseconds Duration;

struct ControlPoint {
	Temperature temp;
	PwmValue pwmValue;
};

}

#endif
