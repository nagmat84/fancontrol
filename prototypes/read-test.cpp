#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <signal.h>

static char const* const TEMP_SENSOR_FILE_PATH = "/sys/class/drm/card0/device/hwmon/hwmon0/temp1_input";

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
	std::ifstream tempSensorFileStream;
	tempSensorFileStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);

	tempSensorFileStream.open(TEMP_SENSOR_FILE_PATH);
	unsigned int temperature;
	while(running) {
		temperature = std::numeric_limits<unsigned int>::max();
		tempSensorFileStream.seekg(0);
		tempSensorFileStream >> temperature;
		std::cout << "Reported temperature " << temperature << std::endl << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	tempSensorFileStream.close();

	return 0;
}
