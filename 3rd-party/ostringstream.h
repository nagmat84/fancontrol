#ifndef _OSTRINGTREAM_H_
#define _OSTRINGTREAM_H_

#include "ostream.h"
#include "stringbuf.h"

class ostringstream : public ostream {
	private:
		stringbuf _M_stringbuf;

	public:
		/**
		 *  @brief  Default constructor starts with an empty string buffer.
		 *
		 *  Initializes @c sb using @c mode|out, and passes @c &sb to the base
		 *  class initializer.  Does not allocate any buffer.
		 *
		 *  That's a lie.  We initialize the base class with NULL, because the
		 *  string class does its own memory management.
		 */
		ostringstream() : ostream(), _M_stringbuf(ios_base::out) {
			this->init(&_M_stringbuf);
		}

		/**
		 *  @brief  Starts with an empty string buffer.
		 *  @param  __mode  Whether the buffer can read, or write, or both.
		 *
		 *  @c ios_base::out is automatically included in @a mode.
		 *
		 *  Initializes @c sb using @c mode|out, and passes @c &sb to the base
		 *  class initializer.  Does not allocate any buffer.
		 *
		 *  That's a lie.  We initialize the base class with NULL, because the
		 *  string class does its own memory management.
		 */
		explicit
		ostringstream(ios_base::openmode __mode) :
			ostream(), _M_stringbuf(__mode | ios_base::out) {
			this->init(&_M_stringbuf);
		}

		/**
		 *  @brief  Starts with an existing string buffer.
		 *  @param  __str  A string to copy as a starting buffer.
		 *  @param  __mode  Whether the buffer can read, or write, or both.
		 *
		 *  @c ios_base::out is automatically included in @a mode.
		 *
		 *  Initializes @c sb using @a str and @c mode|out, and passes @c &sb
		 *  to the base class initializer.
		 *
		 *  That's a lie.  We initialize the base class with NULL, because the
		 *  string class does its own memory management.
		 */
		explicit
		ostringstream(const std::string& __str, ios_base::openmode __mode = ios_base::out) :
			ostream(), _M_stringbuf(__str, __mode | ios_base::out) {
			this->init(&_M_stringbuf);
		}

		/**
		 *  @brief  The destructor does nothing.
		 *
		 *  The buffer is deallocated by the stringbuf object, not the
		 *  formatting stream.
		 */
		~ostringstream() { }

		ostringstream(const ostringstream&) = delete;

		ostringstream(ostringstream&& __rhs) :
			ostream(std::move(__rhs)),
			_M_stringbuf(std::move(__rhs._M_stringbuf)) {
			ostream::set_rdbuf(&_M_stringbuf);
		}

		// Members:
		/**
		 *  @brief  Accessing the underlying buffer.
		 *  @return  The current basic_stringbuf buffer.
		 *
		 *  This hides both signatures of std::basic_ios::rdbuf().
		 */
		std::stringbuf* rdbuf() const {
				return const_cast<std::stringbuf*>(&_M_stringbuf);
		}

		/**
		 *  @brief  Copying out the string buffer.
		 *  @return  @c rdbuf()->str()
		 */
		std::string str() const {
			return _M_stringbuf.str();
		}

		/**
		 *  @brief  Setting a new buffer.
		 *  @param  __s  The string to use as a new sequence.
		 *
		 *  Calls @c rdbuf()->str(s).
		 */
		void str(const std::string& __s) {
			_M_stringbuf.str(__s);
		}
};

#endif
