#ifndef _STRINGBUF_H_
#define _STRINGBUF_H_

#include <string>
#include "streambuf.h"

using std::string;
using std::streamsize;

class stringbuf : public streambuf {
	struct __xfer_bufptrs;
	using allocator_traits = std::allocator_traits<std::allocator<char>>;
	using _Noexcept_swap = std::__or_<
		typename allocator_traits::propagate_on_container_swap,
		typename allocator_traits::is_always_equal
	>;

	public:
		typedef typename char_traits<char>::int_type int_type;
		typedef typename char_traits<char>::pos_type pos_type;
		typedef typename char_traits<char>::off_type off_type;
		typedef typename string::size_type __size_type;

	protected:
		/// Place to stash in || out || in | out settings for current stringbuf.
		ios_base::openmode _M_mode;

		// Data Members:
		string _M_string;

	public:
		/**
 		 *  @brief  Starts with an empty string buffer.
 		 *
 		 *  The default constructor initializes the parent class using its
 		 *  own default ctor.
		 */
		stringbuf() : streambuf(), _M_mode(ios_base::in | ios_base::out), _M_string() {}

		/**
 		 *  @brief  Starts with an empty string buffer.
 		 *  @param  __mode  Whether the buffer can read, or write, or both.
 		 *
 		 *  The default constructor initializes the parent class using its
 		 *  own default ctor.
		 */
		explicit stringbuf(ios_base::openmode __mode) : streambuf(), _M_mode(__mode), _M_string() {}

		/**
 		 *  @brief  Starts with an existing string buffer.
 		 *  @param  __str  A string to copy as a starting buffer.
 		 *  @param  __mode  Whether the buffer can read, or write, or both.
 		 *
 		 *  This constructor initializes the parent class using its
 		 *  own default ctor.
		 */
		explicit stringbuf(
			const string& __str,
			ios_base::openmode __mode = ios_base::in | ios_base::out
		) :
			streambuf(),
			_M_mode(),
			_M_string(__str.data(), __str.size(), __str.get_allocator()) {
			_M_stringbuf_init(__mode);
		}

		stringbuf(const stringbuf&) = delete;

		stringbuf(stringbuf&& __rhs) :
			stringbuf(std::move(__rhs), __xfer_bufptrs(__rhs, this)) {
			__rhs._M_sync(const_cast<char*>(__rhs._M_string.data()), 0, 0);
		}

		stringbuf& operator=(const stringbuf&) = delete;

		stringbuf& operator=(stringbuf&& __rhs) {
			__xfer_bufptrs __st{__rhs, this};
			const streambuf& __base = __rhs;
			streambuf::operator=(__base);
			this->pubimbue(__rhs.getloc());
			_M_mode = __rhs._M_mode;
			_M_string = std::move(__rhs._M_string);
			__rhs._M_sync(const_cast<char*>(__rhs._M_string.data()), 0, 0);
			return *this;
		}

		void swap(stringbuf& __rhs) noexcept(_Noexcept_swap::value) {
			__xfer_bufptrs __l_st{*this, std::__addressof(__rhs)};
			__xfer_bufptrs __r_st{__rhs, this};
			streambuf& __base = __rhs;
			streambuf::swap(__base);
			__rhs.pubimbue(this->pubimbue(__rhs.getloc()));
			std::swap(_M_mode, __rhs._M_mode);
			std::swap(_M_string, __rhs._M_string); // XXX not exception safe
		}

		/**
 		 *  @brief  Copying out the string buffer.
 		 *  @return  A copy of one of the underlying sequences.
 		 *
 		 *  <em>If the buffer is only created in input mode, the underlying
 		 *  character sequence is equal to the input sequence; otherwise, it
 		 *  is equal to the output sequence.</em> [27.7.1.2]/1
		 */
		string str() const {
			string __ret(_M_string.get_allocator());
			if (char* __hi = _M_high_mark())
				__ret.assign(this->pbase(), __hi);
			else
				__ret = _M_string;
			return __ret;
		}

		string str() && {
			if (char* __hi = _M_high_mark()) {
				// Set length to end of character sequence and add null terminator.
				_M_string._M_set_length(_M_high_mark() - this->pbase());
			}
			auto __str = std::move(_M_string);
			_M_string.clear();
			_M_sync(_M_string.data(), 0, 0);
			return __str;
		}

		/**
 		 *  @brief  Setting a new buffer.
 		 *  @param  __s  The string to use as a new sequence.
 		 *
 		 *  Deallocates any previous stored sequence, then copies @a s to
 		 *  use as a new one.
		 */
		void str(const string& __s) {
			// Cannot use _M_string = __s, since v3 strings are COW
			// (not always true now but assign() always works).
			_M_string.assign(__s.data(), __s.size());
			_M_stringbuf_init(_M_mode);
		}

		void str(string&& __s) {
			_M_string = std::move(__s);
			_M_stringbuf_init(_M_mode);
		}

	protected:
		// Common initialization code goes here.
		void _M_stringbuf_init(ios_base::openmode __mode) {
			_M_mode = __mode;
			__size_type __len = 0;
			if (_M_mode & (ios_base::ate | ios_base::app))
				__len = _M_string.size();
			_M_sync(const_cast<char*>(_M_string.data()), 0, __len);
		}

		virtual streamsize showmanyc() {
			streamsize __ret = -1;
			if (_M_mode & ios_base::in) {
				_M_update_egptr();
			__ret = this->egptr() - this->gptr();
			}
			return __ret;
		}

		virtual int_type underflow();
		virtual int_type pbackfail(int_type __c = char_traits<char>::eof());
		virtual int_type overflow(int_type __c = char_traits<char>::eof());

		/**
 		 *  @brief  Manipulates the buffer.
 		 *  @param  __s  Pointer to a buffer area.
 		 *  @param  __n  Size of @a __s.
 		 *  @return  @c this
 		 *
 		 *  If no buffer has already been created, and both @a __s and @a __n are
 		 *  non-zero, then @c __s is used as a buffer; see
 		 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html#io.streambuf.buffering
 		 *  for more.
		 */
		virtual streambuf* setbuf(char* __s, streamsize __n) {
			if (__s && __n >= 0) {
				// This is implementation-defined behavior, and assumes
				// that an external char array of length __n exists
				// and has been pre-allocated. If this is not the case,
				// things will quickly blow up.

				// Step 1: Destroy the current internal array.
				_M_string.clear();

				// Step 2: Use the external array.
				_M_sync(__s, __n, 0);
			}
			return this;
		}

		virtual pos_type seekoff(off_type __off, ios_base::seekdir __way, ios_base::openmode __mode = ios_base::in | ios_base::out);

		virtual pos_type seekpos(pos_type __sp, ios_base::openmode __mode = ios_base::in | ios_base::out);

		// Internal function for correctly updating the internal buffer
		// for a particular _M_string, due to initialization or re-sizing
		// of an existing _M_string.
		void _M_sync(char* __base, __size_type __i, __size_type __o);

		// Internal function for correctly updating egptr() to the actual
		// string end.
		void _M_update_egptr() {
			if (char* __pptr = this->pptr()) {
				char* __egptr = this->egptr();
				if (!__egptr || __pptr > __egptr) {
					if (_M_mode & ios_base::in)
						this->setg(this->eback(), this->gptr(), __pptr);
					else
						this->setg(__pptr, __pptr, __pptr);
				}
			}
		}

		// Works around the issue with pbump, part of the protected
		// interface of basic_streambuf, taking just an int.
		void _M_pbump(char* __pbeg, char* __pend, off_type __off);

	private:
		// Return a pointer to the end of the underlying character sequence.
		// This might not be the same character as _M_string.end() because
		// stringbuf::overflow might have written to unused capacity
		// in _M_string without updating its length.
		__attribute__((__always_inline__)) char* _M_high_mark() const _GLIBCXX_NOEXCEPT {
			if (char* __pptr = this->pptr()) {
				char* __egptr = this->egptr();
				if (!__egptr || __pptr > __egptr)
					return __pptr;  // Underlying sequence is [pbase, pptr).
				else
					return __egptr; // Underlying sequence is [pbase, egptr).
			}
			return 0; // Underlying character sequence is just _M_string.
		}


		// This type captures the state of the gptr / pptr pointers as offsets
		// so they can be restored in another object after moving the string.
		struct __xfer_bufptrs {
			__xfer_bufptrs(const stringbuf& __from, stringbuf* __to) :
				_M_to{__to}, _M_goff{-1, -1, -1}, _M_poff{-1, -1, -1} {
				const char* const __str = __from._M_string.data();
				const char* __end = nullptr;
				if (__from.eback()) {
					_M_goff[0] = __from.eback() - __str;
					_M_goff[1] = __from.gptr() - __str;
					_M_goff[2] = __from.egptr() - __str;
					__end = __from.egptr();
				}
				if (__from.pbase()) {
					_M_poff[0] = __from.pbase() - __str;
					_M_poff[1] = __from.pptr() - __from.pbase();
					_M_poff[2] = __from.epptr() - __str;
					if (!__end || __from.pptr() > __end)
						__end = __from.pptr();
				}

				// Set _M_string length to the greater of the get and put areas.
				if (__end) {
					// The const_cast avoids changing this constructor's signature,
					// because it is exported from the dynamic library.
					auto& __mut_from = const_cast<stringbuf&>(__from);
					__mut_from._M_string._M_length(__end - __str);
				}
			}

			~__xfer_bufptrs() {
				char* __str = const_cast<char*>(_M_to->_M_string.data());
				if (_M_goff[0] != -1)
					_M_to->setg(__str+_M_goff[0], __str+_M_goff[1], __str+_M_goff[2]);
				if (_M_poff[0] != -1)
					_M_to->_M_pbump(__str+_M_poff[0], __str+_M_poff[2], _M_poff[1]);
			}

			stringbuf* _M_to;
			off_type _M_goff[3];
			off_type _M_poff[3];
		};


		// The move constructor initializes an __xfer_bufptrs temporary then
		// delegates to this constructor to performs moves during its lifetime.
		stringbuf(stringbuf&& __rhs, __xfer_bufptrs&&) :
			streambuf(static_cast<const streambuf&>(__rhs)),
		_M_mode(__rhs._M_mode), _M_string(std::move(__rhs._M_string)) {}

		// The move constructor initializes an __xfer_bufptrs temporary then
		// delegates to this constructor to performs moves during its lifetime.
		stringbuf(stringbuf&& __rhs, const std::allocator<char>& __a, __xfer_bufptrs&&) :
			streambuf(static_cast<const streambuf&>(__rhs)),
			_M_mode(__rhs._M_mode),
			_M_string(std::move(__rhs._M_string), __a) {
		}
};

#endif
