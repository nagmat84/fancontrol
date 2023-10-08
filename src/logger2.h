#ifndef _LOGGER_2_H_
#define _LOGGER_2_H_

#include <ostream>
#include <streambuf>
#include <syslog.h>

namespace AmdGpuFanControl {

class LogStream;

/**
 * Implements a character buffer of constant size to buffer log messages
 * to syslog.
 *
 * The class allocates a buffer of constant size which is used to "assembly"
 * a log message to be written to syslog.
 * Officially, the syslog protocol does not define an upper limit on the size
 * of a single log message, but there are implementations which truncate a
 * log message to 1536 (i.e. 1.5 kiBytes) characters.
 * This class allocates a buffer that large which should be sufficient for
 * typical log messages.
 *
 * The class realizes a write (i.e. "put" or "out") buffer.
 * Initially, (and every time after `sync` has been called)
 *  - the base put pointer (`pbase`) points to the beginning of the buffer,
 *  - the current put pointer (`pptr`) points the beginning of the buffer,
 *    and
 *  - the end put pointer (`epptr`) points to the maximum size (i.e. 1536).
 * Then, the buffer is filled while `pptr` grows towards `epptr`.
 * If the message attempts to grow larger than the allocated buffer,
 * the method `overflow` returns an error.
 * I.e. `overflow` does not try to re-allocate a larger buffer, the upper
 * limit of the message size is fixed.
 * When `sync` is called, `sync`
 *  - passes the message in the buffer between `pbase` and `pptr` to `syslog`,
 *    and
 *  - resets the current put pointer to the beginning of the buffer.
 *
 * The class allows to rewind `pptr` to a earlier position, but does not
 * allow to forward `pptr` beyond its current position.
 * This means it is possible to truncate the current message and re-write
 * its tail, but it is not possible to skip ahead even if the current message
 * has already been larger before.
 *
 * Note: Alternatively, this class had to maintain another pointer `max_pptr`
 * which tracks how far the current message has already been written and is
 * "valid", but this use case is not supported.
 *
 * The class realizes a read (i.e. "get" or "in") buffer and allows to read
 * back the current message.
 * The base get pointer (`eback`) always points to the beginning of the
 * buffer.
 * The end get pointer (`egptr`) points somwhere between the begining of the
 * buffer and the current put pointer `pptr`.
 * This means it is impossible to read beyond what has been previously
 * written.
 * The current get pointer (`gptr`) can move freely between `eback` and
 * `egptr`.
 *
 * If the current put pointer `pptr` is moved to an ealier position, i.e.
 * if the current message is truncated, and the current get pointer `gptr`
 * would happen to point after the new position of `pptr`, then the current
 * get pointer `gptr` is also moved and set to the new position of `pptr`,
 * i.e. the current get pointer will point to the new end.
 *
 * If one attempts to read beyond the end get pointer `egptr`, the method
 * `underflow` tries to move the end get pointer `egptr` forward up to the
 * position of the current put pointer `pptr` and return the next character.
 * If this is impossible (because `egptr` has already reached `pptr`), then
 * `underflow` returns `EOF`.
 */
class LogBuffer : public std::streambuf {
	friend class LogStream;

	public:
		enum Severity {
				EMERGENCY = LOG_EMERG,
				ALERT = LOG_ALERT,
				CRITICAL = LOG_CRIT,
				ERROR = LOG_ERR,
				WARNING = LOG_WARNING,
				NOTICE = LOG_NOTICE,
				INFO = LOG_INFO,
				DEBUG = LOG_DEBUG
		};

	public:
		typedef std::streambuf::traits_type traits_type;
		typedef std::allocator<char> allocator_type;
		typedef allocator_type::size_type size_type;
		typedef std::streambuf::int_type int_type;
		typedef std::streambuf::pos_type pos_type;
		typedef std::streambuf::off_type off_type;
		typedef std::ios_base::seekdir seekdir;
		typedef std::ios_base::openmode openmode;

	public:
		static size_type const LOG_BUFFER_SIZE;
		static int const SUCCESS;
		static int const FAIL;
		static Severity const DEFAULT_LOG_LEVEL;

	protected:
		LogBuffer();

	public:
		virtual ~LogBuffer();

	public:
		void setTreshhold(LogBuffer::Severity const t) {
			treshhold = t;
		}

		void setSeverity(LogBuffer::Severity const s) {
			severity = s;
		}

	protected:
		virtual pos_type seekoff(
			off_type offset,
			seekdir way,
			openmode mode = std::ios_base::in | std::ios_base::out
		);
		virtual pos_type  seekpos(
				pos_type offset,
				openmode mode = std::ios_base::in | std::ios_base::out
		) {
			return seekoff(offset, std::ios_base::beg, mode);
		};
		virtual int sync();
		virtual std::streamsize showmanyc() { return pptr() - egptr(); };
		virtual int_type underflow();
		size_type size() const { return pptr() - pbase(); };
		bool isEmpty() const { return 0 == size(); };
		void init();

	private:
		allocator_type allocator;
		char* buffer;
		Severity treshhold;
		Severity severity;
};


/**
 * Stream-based logger
 */
class LogStream : public std::ostream {
	protected:
		/**
		 * C'tor.
		 *
		 * Initializes the log buffer.
		 */
		LogStream() : std::ostream(), logBuffer() {
			init(&logBuffer);
		}

	public:
		static LogStream& get();

		/**
		 * D'tor.
		 *
		 * Flushes the underlying log buffer such that the last message gets
		 * out even in case of a crash.
		 */
		~LogStream() {
			flush();
		}

		LogStream(const LogStream&) = delete;

	public:
		void setTreshold(LogBuffer::Severity const t) {
			logBuffer.setTreshhold(t);
		}
		void setSeverity(LogBuffer::Severity const s) {
			logBuffer.setSeverity(s);
		}
		LogStream& operator<<(LogBuffer::Severity const severity) {
			setSeverity(severity);
			return *this;
		};

	private:
		LogBuffer logBuffer;

};


}

#endif
