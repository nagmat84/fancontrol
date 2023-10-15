#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <signal.h>

static char const* const PWM_MODE_FILE_PATH = "/sys/class/drm/card0/device/hwmon/hwmon0/pwm1_enable";
static char const* const PWM_CONTROL_FILE_PATH = "/sys/class/drm/card0/device/hwmon/hwmon0/pwm1";
static unsigned short const PWM_VALUES[] = {70, 75, 65};

static bool running = false;
static sigset_t signalSet;
static struct sigaction signalAction;

static void terminate( int ) {
	running = false;
}

static void configureSignalHandling() {
	// Collect all signals upon which the program shall terminate and which
	// shall be mutually blocked when handling any of them
	sigemptyset( &signalSet );
	sigaddset( &signalSet, SIGHUP );
	sigaddset( &signalSet, SIGINT );
	sigaddset( &signalSet, SIGQUIT );
	sigaddset( &signalSet, SIGTERM );
	sigaddset( &signalSet, SIGTSTP );

	// Configures the handler function and masks all of the above signals while
	// the handler is called
	signalAction.sa_handler = terminate;
	signalAction.sa_mask = signalSet;
	signalAction.sa_flags = 0;

	// Install the action for each of the signals
	sigaction( SIGHUP, &signalAction, NULL );
	sigaction( SIGINT, &signalAction, NULL );
	sigaction( SIGQUIT, &signalAction, NULL );
	sigaction( SIGTERM, &signalAction, NULL );
	sigaction( SIGTSTP, &signalAction, NULL );
}

int main(int /*argc*/, char* /*argv*/[]) {
	configureSignalHandling();

	running = true;
	std::ofstream pwmModeFileStream;
	pwmModeFileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	std::ofstream pwmControlFileStream;
	pwmControlFileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);

	// Set to user-control
	pwmModeFileStream.open(PWM_MODE_FILE_PATH);
	pwmModeFileStream << 1 << std::endl;
	pwmModeFileStream.close();

	pwmControlFileStream.open(PWM_CONTROL_FILE_PATH);
	unsigned short idx = 0;
	while(running) {
		idx = (idx + 1) % 3;
		std::cout << "Setting PWM value to " << PWM_VALUES[idx] << std::endl << std::flush;
		//pwmControlFileStream.seekp(0);
		//pwmControlFileStream.open(PWM_CONTROL_FILE_PATH);
		pwmControlFileStream << PWM_VALUES[idx] << std::endl << std::flush;
		//pwmControlFileStream.close();
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	pwmControlFileStream.close();

	// Set to auto-control
	pwmModeFileStream.open(PWM_MODE_FILE_PATH);
	pwmModeFileStream << 2 << std::endl;
	pwmModeFileStream.close();

	return 0;
}
