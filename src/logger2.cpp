#include "logger2.h"

namespace AmdGpuFanControl {

LogBuffer::size_type const LogBuffer::LOG_BUFFER_SIZE = 1536;
int const LogBuffer::SUCCESS = 0;
int const LogBuffer::FAIL = -1;
LogBuffer::Severity const LogBuffer::DEFAULT_LOG_LEVEL( Severity::WARNING );

/**
 * C'tor.
 *
 * Allocates an internal buffer of size `LOG_BUFFER_SIZE`, calls `init()`
 * and opens the connection to syslog.
 */
LogBuffer::LogBuffer() :
	std::streambuf(),
	allocator(),
	buffer(allocator.allocate(LOG_BUFFER_SIZE)),
	treshhold(DEFAULT_LOG_LEVEL),
	severity(DEFAULT_LOG_LEVEL) {
	init();
	openlog( NULL, LOG_ODELAY, LOG_DAEMON );
}

/**
 * D'tor.
 *
 * Deallocates the internal buffer and closes the connection to syslog.
 */
LogBuffer::~LogBuffer() {
	allocator.deallocate(buffer, LOG_BUFFER_SIZE);
	closelog();
}

/**
 * Sets the current put or get or both pointers to a new position.
 *
 * If both pointers are set simultenously, the offset must be an absolute
 * offset either from the beginning or end of the buffer, i.e. `way` must
 * equal `std::ios_base:beg` or `std::ios_base:end`.
 * It is not possible to set both pointers simultenously using a relative
 * position, as the result would be ambigous.
 *
 * It is not possible to forward the put pointer beyond its current position,
 * i.e. it is only possible to truncate the current content of the buffer.
 *
 * If the get pointer is set, the resulting get pointer must be positioned
 * within the beginning of the buffer and the current position of the put
 * pointer.
 * It is not possible to set the get pointer beyond what has already been
 * written.
 *
 * If the put pointer is set and the current position of the get pointer
 * would happen to point after the new position of the the put pointer, the
 * get pointer is reset to the new end.
 */
LogBuffer::pos_type LogBuffer::seekoff(
	off_type offset, seekdir way, openmode mode
) {
	// Relative re-positiong with respect to current position is not allowed
	// for both pointers (put and get) at once as they might have different
	// current positions and hence the result would be ambigious.
	if (
		(way == std::ios_base::cur ) &&
		(mode & (std::ios_base::out | std::ios_base::in)) == (std::ios_base::out | std::ios_base::in)
	) {
		throw std::ios_base::failure("Invalid argument: Cannot relatively move put and get pointer at once");
	}

	// Reposition the put pointer first, only truncing is supported
	if ((mode & std::ios_base::out) == std::ios_base::out) {

		char const* newPPtr;
		switch( way ) {
			case std::ios_base::beg:
				newPPtr = pbase() + offset;
				break;
			case std::ios_base::cur:
				newPPtr = pptr() + offset;
				break;
			case std::ios_base::end:
				newPPtr = epptr() + offset;
				break;
		}

		if( newPPtr < pbase() )
			throw std::ios_base::failure("Out-of-bound: Cannot set put pointer before start of log buffer");
		if( newPPtr > epptr() )
			throw std::ios_base::failure("Out-of-bound: Cannot set put pointer after end of log buffer");
		if( newPPtr > pptr() )
			throw std::ios_base::failure("Out-of-bound: Cannot forward put pointer");

		pos_type newPPos(newPPtr - pbase());
		setp(pbase(), epptr());
		pbump(newPPos);

		// We must at least resize the get buffer to the new put pointer position
		// We must also set the get pointer, if both put and get pointer shall be
		// set or if the current position of the get pointer points beyond the
		// new position of the put pointer.
		pos_type newGPos;
		if (
			(mode & std::ios_base::in) == std::ios_base::in ||
			gptr() - eback() > newPPos
		)
			newGPos = newPPos;
		else
			newGPos = gptr() - eback();
		setg(pbase(), pbase(), pptr());
		gbump(newGPos);

		return newPPos;
	}

	// This is the case when only the get pointer shall be set

	char const* newGPtr;
	switch( way ) {
		case std::ios_base::beg:
			newGPtr = eback() + offset;
			break;
		case std::ios_base::cur:
			newGPtr = gptr() + offset;
			break;
		case std::ios_base::end:
			newGPtr = egptr() + offset;
			break;
	}

	if( newGPtr < eback() )
		throw std::ios_base::failure("Out-of-bound: Cannot set get pointer before start of log buffer");
	if( newGPtr > pptr() )
		throw std::ios_base::failure("Out-of-bound: Cannot set get pointer after put pointer of log buffer");

	pos_type newGPos(newGPtr - eback());
	setg(pbase(), pbase(), pptr());
	gbump(newGPos);
	return newGPos;
}

int LogBuffer::sync() {
	if (!isEmpty() && severity <= treshhold) {
		// Append NUL at current end; note this relies on `init()` to set the end
		// put pointer `epptr` one less than the allocated size such that there
		// is always one more character available even if the buffer is full.
		*pptr() = '\0';
		syslog( severity, "%s", pbase() );
	}
	init();
	return SUCCESS;
}

LogBuffer::int_type LogBuffer::underflow() {
	if( 0 == showmanyc()) {
		return std::char_traits<char>::eof();
	} else {
		setg(pbase(), gptr(), pptr());
		return *gptr();
	}
}

void LogBuffer::init() {
	// The end put pointer `epptr` must be one less than the total buffer size,
	// as we need to reserve one character to terminate the final message with
	// `NUL`.
	setp(buffer, buffer + LOG_BUFFER_SIZE - 1);
	// As nothing has been written to the buffer yet, the read buffer has size
	// zero, i.e. the end points to the start position
	setg(buffer, buffer, buffer);
}

LogStream& LogStream::get() {
	static LogStream singleton;
	return singleton;
}

}
