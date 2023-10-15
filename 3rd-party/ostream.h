#ifndef _OSTREAM_H_
#define _OSTREAM_H_

#include "ios.h"
#include "streambuf.h"

/**
 *  @brief  Template class ostream.
 *  @ingroup io
 *
 *
 *  This is the base class for all output streams.  It provides text
 *  formatting of all builtin types, and communicates with any class
 *  derived from streambuf to do the actual output.
*/
class ostream : virtual public ios {
	public:
		typedef typename char_traits<char>::pos_type pos_type;
		typedef typename char_traits<char>::off_type off_type;

		// Non-standard Types:
		typedef num_put<char, ostreambuf_iterator<char, char_traits<char>> > __num_put_type;

		/**
		 *  @brief  Base constructor.
		 *
		 *  This ctor is almost never called by the user directly, rather from
		 *  derived classes' initialization lists, which pass a pointer to
		 *  their own stream buffer.
		 */
		explicit ostream(streambuf* __sb) {
			this->init(__sb);
		}

		/**
		 *  @brief  Base destructor.
		 *
		 *  This does very little apart from providing a virtual base dtor.
		 */
		virtual ~ostream() { }

		/// Safe prefix/suffix operations.
		class sentry;
		friend class sentry;

		/**
		 *  @brief  Interface for manipulators.
		 *
		 *  Manipulators such as @c std::endl and @c std::hex use these
		 *  functions in constructs like "std::cout << std::endl".  For more
		 *  information, see the iomanip header.
		 */
		ostream& operator<<(ostream& (*__pf)(ostream&)) {
			return __pf(*this);
		}

		ostream& operator<<(ios& (*__pf)(ios&)) {
			__pf(*this);
			return *this;
		}

		ostream& operator<<(ios_base& (*__pf) (ios_base&)) {
			__pf(*this);
			return *this;
		}

		/**
		 *  @name Inserters
		 *
		 *  All the @c operator<< functions (aka <em>formatted output
		 *  functions</em>) have some common behavior.  Each starts by
		 *  constructing a temporary object of type std::ostream::sentry.
		 *  This can have several effects, concluding with the setting of a
		 *  status flag; see the sentry documentation for more.
		 *
		 *  If the sentry status is good, the function tries to generate
		 *  whatever data is appropriate for the type of the argument.
		 *
		 *  If an exception is thrown during insertion, ios_base::badbit
		 *  will be turned on in the stream's error state without causing an
		 *  ios_base::failure to be thrown.  The original exception will then
		 *  be rethrown.
		 */

		/**
		 *  @brief Integer arithmetic inserters
		 *  @param  __n A variable of builtin integral type.
		 *  @return  @c *this if successful
		 *
		 *  These functions use the stream's current locale (specifically, the
		 *  @c num_get facet) to perform numeric formatting.
		 */
		ostream& operator<<(long __n) { return _M_insert(__n); }
		ostream& operator<<(unsigned long __n) { return _M_insert(__n); }
		ostream& operator<<(bool __n) { return _M_insert(__n); }
		ostream& operator<<(short __n);
		ostream& operator<<(unsigned short __n) { return _M_insert(static_cast<unsigned long>(__n)); }
		ostream& operator<<(int __n);
		ostream& operator<<(unsigned int __n) { return _M_insert(static_cast<unsigned long>(__n)); }
		ostream& operator<<(long long __n) { return _M_insert(__n); }
		ostream& operator<<(unsigned long long __n) { return _M_insert(__n); }

		/**
		 *  @brief  Floating point arithmetic inserters
		 *  @param  __f A variable of builtin floating point type.
		 *  @return  @c *this if successful
		 *
		 *  These functions use the stream's current locale (specifically, the
		 *  @c num_get facet) to perform numeric formatting.
		 */
		ostream& operator<<(double __f) { return _M_insert(__f); }
		ostream& operator<<(float __f) { return _M_insert(static_cast<double>(__f)); }
		ostream& operator<<(long double __f) { return _M_insert(__f); }

		/**
		 *  @brief  Pointer arithmetic inserters
		 *  @param  __p A variable of pointer type.
		 *  @return  @c *this if successful
		 *
		 *  These functions use the stream's current locale (specifically, the
		 *  @c num_get facet) to perform numeric formatting.
		 */
		ostream& operator<<(const void* __p) { return _M_insert(__p); }

		/**
		 *  @brief  Extracting from another streambuf.
		 *  @param  __sb  A pointer to a streambuf
		 *
		 *  This function behaves like one of the basic arithmetic extractors,
		 *  in that it also constructs a sentry object and has the same error
		 *  handling behavior.
		 *
		 *  If @p __sb is NULL, the stream will set failbit in its error state.
		 *
		 *  Characters are extracted from @p __sb and inserted into @c *this
		 *  until one of the following occurs:
		 *
		 *  - the input stream reaches end-of-file,
		 *  - insertion into the output sequence fails (in this case, the
		 *    character that would have been inserted is not extracted), or
		 *  - an exception occurs while getting a character from @p __sb, which
		 *    sets failbit in the error state
		 *
		 *  If the function inserts no characters, failbit is set.
		 */
		ostream& operator<<(streambuf* __sb);

		/**
		 *  @name Unformatted Output Functions
		 *
		 *  All the unformatted output functions have some common behavior.
		 *  Each starts by constructing a temporary object of type
		 *  std::ostream::sentry.  This has several effects, concluding
		 *  with the setting of a status flag; see the sentry documentation
		 *  for more.
		 *
		 *  If the sentry status is good, the function tries to generate
		 *  whatever data is appropriate for the type of the argument.
		 *
		 *  If an exception is thrown during insertion, ios_base::badbit
		 *  will be turned on in the stream's error state.  If badbit is on in
		 *  the stream's exceptions mask, the exception will be rethrown
		 *  without completing its actions.
		 */

		/**
		 *  @brief  Simple insertion.
		 *  @param  __c  The character to insert.
		 *  @return  *this
		 *
		 *  Tries to insert @p __c.
		 *
		 *  @note  This function is not overloaded on signed char and
		 *         unsigned char.
		 */
		ostream& put(char __c);

		/**
		 *  @brief  Character string insertion.
		 *  @param  __s  The array to insert.
		 *  @param  __n  Maximum number of characters to insert.
		 *  @return  *this
		 *
		 *  Characters are copied from @p __s and inserted into the stream until
		 *  one of the following happens:
		 *
		 *  - @p __n characters are inserted
		 *  - inserting into the output sequence fails (in this case, badbit
		 *    will be set in the stream's error state)
		 *
		 *  @note  This function is not overloaded on signed char and
		 *         unsigned char.
		 */
		ostream& write(const char* __s, streamsize __n);

		/**
		 *  @brief  Synchronizing the stream buffer.
		 *  @return  *this
		 *
		 *  If @c rdbuf() is a null pointer, changes nothing.
		 *
		 *  Otherwise, calls @c rdbuf()->pubsync(), and if that returns -1,
		 *  sets badbit.
		 */
		ostream& flush();

		/**
		 *  @brief  Getting the current write position.
		 *  @return  A file position object.
		 *
		 *  If @c fail() is not false, returns @c pos_type(-1) to indicate
		 *  failure.  Otherwise returns @c rdbuf()->pubseekoff(0,cur,out).
		 */
		pos_type tellp();

		/**
		 *  @brief  Changing the current write position.
		 *  @param  __pos  A file position object.
		 *  @return  *this
		 *
		 *  If @c fail() is not true, calls @c rdbuf()->pubseekpos(pos).  If
		 *  that function fails, sets failbit.
		 */
		ostream& seekp(pos_type);

		/**
		 *  @brief  Changing the current write position.
		 *  @param  __off  A file offset object.
		 *  @param  __dir  The direction in which to seek.
		 *  @return  *this
		 *
		 *  If @c fail() is not true, calls @c rdbuf()->pubseekoff(off,dir).
		 *  If that function fails, sets failbit.
		 */
		ostream& seekp(off_type, ios_base::seekdir);

	protected:
		ostream() { this->init(0); }

		// Non-standard constructor that does not call init()
		ostream(basic_iostream<char, char_traits<char>>&) { }

		ostream(const ostream&) = delete;

		ostream(ostream&& __rhs) : ios() {
			ios::move(__rhs);
		}

		ostream& operator=(const ostream&) = delete;

		ostream& operator=(ostream&& __rhs) {
			swap(__rhs);
			return *this;
		}

		void swap(ostream& __rhs) {
			ios::swap(__rhs);
		}

		template<typename _ValueT> ostream& _M_insert(_ValueT __v);

	private:
		void _M_write(const char* __s, streamsize __n) {
			std::__ostream_insert(*this, __s, __n);
		}
};

/**
 *  @brief  Performs setup work for output streams.
 *
 *  Objects of this class are created before all of the standard
 *  inserters are run.  It is responsible for <em>exception-safe prefix and
 *  suffix operations</em>.
 */
class ostream::sentry {
	bool _M_ok;
	ostream& _M_os;

	public:
		/**
		 *  @brief  The constructor performs preparatory work.
		 *  @param  __os  The output stream to guard.
		 *
		 *  If the stream state is good (@a __os.good() is true), then if the
		 *  stream is tied to another output stream, @c is.tie()->flush()
		 *  is called to synchronize the output sequences.
		 *
		 *  If the stream state is still good, then the sentry state becomes
		 *  true (@a okay).
		 */
		explicit sentry(ostream& __os);

		/**
		 *  @brief  Possibly flushes the stream.
		 *
		 *  If @c ios_base::unitbuf is set in @c os.flags(), and
		 *  @c std::uncaught_exception() is true, the sentry destructor calls
		 *  @c flush() on the output stream.
		 */
		~sentry() {
			if (bool(_M_os.flags() & ios_base::unitbuf) && !uncaught_exception()) {
				// Can't call flush directly or else will get into recursive lock.
				if (_M_os.rdbuf() && _M_os.rdbuf()->pubsync() == -1)
					_M_os.setstate(ios_base::badbit);
			}
		}

		/**
		 *  @brief  Quick status checking.
		 *  @return  The sentry state.
		 *
		 *  For ease of use, sentries may be converted to booleans.  The
		 *  return value is that of the sentry state (true == okay).
		 */
		explicit operator bool() const { return _M_ok; }
};

// Standard ostream manipulators

/**
 *  @brief  Write a newline and flush the stream.
 *
 *  This manipulator is often mistakenly used when a simple newline is
 *  desired, leading to poor buffering performance.  See
 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html#io.streambuf.buffering
 *  for more on this subject.
 */
inline ostream& endl(ostream& __os) {
	return flush(__os.put(__os.widen('\n')));
}

/**
 *  @brief  Write a null character into the output sequence.
 *
 *  <em>Null character</em> is @c CharT() by definition.  For CharT
 *  of @c char, this correctly writes the ASCII @c NUL character
 *  string terminator.
*/
inline ostream& ends(ostream& __os) {
	return __os.put(char());
}

/**
 *  @brief  Flushes the output stream.
 *
 *  This manipulator simply calls the stream's @c flush() member function.
*/
inline ostream& flush(ostream& __os) {
	return __os.flush();
}

#endif // _OSTREAM_H_
