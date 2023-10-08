#include "runtime_config.h"
#include "pwm_controllers.h"
#include "logger2.h"

#include <iostream>
#include <locale>
#include <clocale>
#include <signal.h>

static sigset_t signalSet;
static struct sigaction signalAction;

static void terminate( int ) {
	AmdGpuFanControl::PWMControllers& controllers( AmdGpuFanControl::PWMControllers::get() );
	controllers.stop();
}

static void configureLocale() {
	setlocale( LC_ALL, "C" );
	std::locale loc( "C" );
	std::cout.imbue( loc );
	std::cerr.imbue( loc );
	std::clog.imbue( loc );
	std::cin.imbue( loc );
}

/**
 * Configures the signal handler.
 *
 * @internal This method does not use the C function `signal` or the C++
 * equivalent `std::signal` on purpose, but the more "complicated" C function
 * `sigaction`.
 * The POSIX man page recommends using `sigaction` and advises against
 * `signal`.
 * The behaviour of `signal` is implementation-dependent, in particular with
 * respect to signal handling while a previous signal is still being handled.
 * We want to ensure that the program does not receive the same signal
 * a second time while the program still handles the first occurance.
 * Only `sigaction` allows to ensurre that in a cross-platform compatible
 * manner.
 */
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

static void parseCmdLineArgs( int argc, char* argv[] ) {
	for( int i = 1; i < argc; i++ ) {
		std::string arg(argv[i]);
		if ( arg.compare("-d") == 0 || arg.compare("--debug") == 0 ) {
			AmdGpuFanControl::LogStream& log( AmdGpuFanControl::LogStream::get() );
			log.setTreshold( AmdGpuFanControl::LogBuffer::Severity::DEBUG );
		}
	}
}

int main( int argc, char* argv[] ) {
	configureLocale();
	configureSignalHandling();

	AmdGpuFanControl::RuntimeConfig& config( AmdGpuFanControl::RuntimeConfig::get() );
	config.loadFromFile();

	parseCmdLineArgs( argc, argv );

	AmdGpuFanControl::PWMControllers& controllers( AmdGpuFanControl::PWMControllers::get() );
	return controllers.run();
}
