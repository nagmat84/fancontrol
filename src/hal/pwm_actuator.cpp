#include "pwm_actuator.h"

namespace FanControl {

char const* const PWMActuator::MODE_FILE_SUFFIX = "_enable";

PWMActuator::PWMActuator(
	std::string const& devFilePath
) :
	filePath( devFilePath ),
	fileStream() {
	fileStream.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	fileStream.open( devFilePath );
	setMode( PwmMode::USER_CONTROL );
}

PWMActuator::PWMActuator( PWMActuator&& other ) :
	filePath( std::move( other.filePath ) ),
	fileStream( std::move( other.fileStream ) ) {
	// Close the file streams of the object which has been moved from, to avoid
	// that the PWM mode is accidentally set to `AUTO_CONTROL` when the object
	// goes out-of-scope.
	// See comment on PWMActuator::setMode.
	other.fileStream.exceptions( std::ofstream::goodbit );
	other.fileStream.close();
}

PWMActuator::~PWMActuator() {
	setMode( PwmMode::AUTO_CONTROL );
	fileStream.exceptions( std::ofstream::goodbit );
	fileStream.close();
}

/**
 * Sets the operating mode of the PWM actuator.
 *
 * @internal This method has only an effect, if `filestream` is open.
 * This prevents that the PWM mode is unintentionally set to `AUTO` when an
 * object instance upon which the move-constructor has been called goes
 * out-of-scope.
 */
void PWMActuator::setMode( PwmMode const pwmMode ) const {
	if ( !fileStream.is_open() ) return;

	std::ofstream fs;
	fs.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	fs.open( filePath + MODE_FILE_SUFFIX );
	fs << pwmMode << std::endl;
	fs.close();
}

void PWMActuator::setValue( PwmValue const pwmValue ) {
	fileStream << pwmValue << std::endl << std::flush;
}

}
