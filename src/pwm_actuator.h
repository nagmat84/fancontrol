#ifndef _PWM_ACTUATOR_H_
#define _PWM_ACTUATOR_H_

#include "types.h"
#include <fstream>
#include <string>
#include <memory>

namespace AmdGpuFanControl {

class PWMActuatorFactory;

class PWMActuator {
	friend class PWMActuatorFactory;

	private:
		static char const* const MODE_FILE_SUFFIX;

	public:
		enum PwmMode : unsigned short {
			USER_CONTROL = 1,
			AUTO_CONTROL = 2
		};
		typedef std::shared_ptr<PWMActuator> Ptr;

	protected:
		PWMActuator( std::string const& devFilePath );
		PWMActuator( PWMActuator const& ) = delete;

	public:
		PWMActuator( PWMActuator&& other );
		virtual ~PWMActuator();
		void setValue( PwmValue const pwmValue );

	private:
		void setMode( PwmMode const pwmMode ) const;

	private:
		std::string filePath;
		std::ofstream fileStream;
};
}

#endif
