#ifndef _STREAMBUF_H_
#define _STREAMBUF_H_

#include "ios.h"

class streambuf {
	public:
		typedef typename char_traits<char>::int_type int_type;
		typedef typename char_traits<char>::pos_type pos_type;
		typedef typename char_traits<char>::off_type off_type;

	protected:
		/*
		 *  This is based on _IO_FILE, just reordered to be more consistent,
		 *  and is intended to be the most minimal abstraction for an
		 *  internal buffer.
		 *  -  get == input == read
		 *  -  put == output == write
		 */
		char* _M_in_beg;     ///< Start of get area.
		char* _M_in_cur;     ///< Current read area.
		char* _M_in_end;     ///< End of get area.
		char* _M_out_beg;    ///< Start of put area.
		char* _M_out_cur;    ///< Current put area.
		char* _M_out_end;    ///< End of put area.

		/// Current locale setting.
		locale _M_buf_locale;

	public:
		virtual ~streambuf() {}

		/**
		 *  @brief  Entry point for imbue().
		 *  @param  __loc  The new locale.
		 *  @return  The previous locale.
		 *
		 *  Calls the derived imbue(__loc).
		 */
		locale pubimbue(const locale& __loc) {
			locale __tmp(this->getloc());
			this->imbue(__loc);
			_M_buf_locale = __loc;
			return __tmp;
		}

		/**
		 *  @brief  Locale access.
		 *  @return  The current locale in effect.
		 *
		 *  If pubimbue(loc) has been called, then the most recent @c loc
		 *  is returned.  Otherwise the global locale in effect at the time
		 *  of construction is returned.
		 */
		locale getloc() const { return _M_buf_locale; }

		/**
		 *  @brief  Entry points for derived buffer functions.
		 *
		 *  The public versions of @c pubfoo dispatch to the protected
		 *  derived @c foo member functions, passing the arguments (if any)
		 *  and returning the result unchanged.
		 */
		streambuf* pubsetbuf(char* __s, streamsize __n) { return this->setbuf(__s, __n); }

		/**
		 *  @brief  Alters the stream position.
		 *  @param  __off  Offset.
		 *  @param  __way  Value for ios_base::seekdir.
		 *  @param  __mode Value for ios_base::openmode.
		 *
		 *  Calls virtual seekoff function.
		 */
		pos_type pubseekoff(
			off_type __off,
			ios_base::seekdir __way,
			ios_base::openmode __mode = ios_base::in | ios_base::out
		) { return this->seekoff(__off, __way, __mode); }

		/**
		 *  @brief  Alters the stream position.
		 *  @param  __sp  Position
		 *  @param  __mode Value for ios_base::openmode.
		 *
		 *  Calls virtual seekpos function.
		 */
		pos_type pubseekpos(
			pos_type __sp,
			ios_base::openmode __mode = ios_base::in | ios_base::out
		) { return this->seekpos(__sp, __mode); }

		/**
		 *  @brief  Calls virtual sync function.
		 */
		int pubsync() { return this->sync(); }

		/**
		 *  @brief  Looking ahead into the stream.
		 *  @return  The number of characters available.
		 *
		 *  If a read position is available, returns the number of characters
		 *  available for reading before the buffer must be refilled.
		 *  Otherwise returns the derived @c showmanyc().
		 */
		streamsize in_avail() {
			const streamsize __ret = this->egptr() - this->gptr();
			return __ret ? __ret : this->showmanyc();
		}

		/**
		 *  @brief  Getting the next character.
		 *  @return  The next character, or eof.
		 *
		 *  Calls @c sbumpc(), and if that function returns
		 *  @c traits::eof(), so does this function.  Otherwise, @c sgetc().
		 */
		int_type snextc() {
			int_type __ret = char_traits<char>::eof();
			if (
				__builtin_expect(!char_traits<char>::eq_int_type(this->sbumpc(), __ret), true)
			)
				__ret = this->sgetc();
			return __ret;
		}

		/**
		 *  @brief  Getting the next character.
		 *  @return  The next character, or eof.
		 *
		 *  If the input read position is available, returns that character
		 *  and increments the read pointer, otherwise calls and returns
		 *  @c uflow().
		 */
		int_type sbumpc() {
			int_type __ret;
			if (__builtin_expect(this->gptr() < this->egptr(), true)) {
				__ret = char_traits<char>::to_int_type(*this->gptr());
				this->gbump(1);
			} else
				__ret = this->uflow();
			return __ret;
		}

		/**
		 *  @brief  Getting the next character.
		 *  @return  The next character, or eof.
		 *
		 *  If the input read position is available, returns that character,
		 *  otherwise calls and returns @c underflow().  Does not move the
		 *  read position after fetching the character.
		 */
		int_type sgetc() {
			int_type __ret;
			if (__builtin_expect(this->gptr() < this->egptr(), true))
				__ret = char_traits<char>::to_int_type(*this->gptr());
			else
				__ret = this->underflow();
			return __ret;
		}

		/**
		 *  @brief  Entry point for xsgetn.
		 *  @param  __s  A buffer area.
		 *  @param  __n  A count.
		 *
		 *  Returns xsgetn(__s,__n).  The effect is to fill @a __s[0] through
		 *  @a __s[__n-1] with characters from the input sequence, if possible.
		 */
		streamsize sgetn(char* __s, streamsize __n) { return this->xsgetn(__s, __n); }

		/**
		 *  @brief  Pushing characters back into the input stream.
		 *  @param  __c  The character to push back.
		 *  @return  The previous character, if possible.
		 *
		 *  Similar to sungetc(), but @a __c is pushed onto the stream
		 *  instead of <em>the previous character.</em> If successful,
		 *  the next character fetched from the input stream will be @a
		 *  __c.
		 */
		int_type sputbackc(char __c) {
			int_type __ret;
			const bool __testpos = this->eback() < this->gptr();
			if (
				__builtin_expect(!__testpos ||
				!char_traits<char>::eq(__c, this->gptr()[-1]), false)
			)
				__ret = this->pbackfail(char_traits<char>::to_int_type(__c));
			else {
					this->gbump(-1);
					__ret = char_traits<char>::to_int_type(*this->gptr());
			}
			return __ret;
		}

		/**
		 *  @brief  Moving backwards in the input stream.
		 *  @return  The previous character, if possible.
		 *
		 *  If a putback position is available, this function decrements
		 *  the input pointer and returns that character.  Otherwise,
		 *  calls and returns pbackfail().  The effect is to @a unget
		 *  the last character @a gotten.
		 */
		int_type sungetc() {
			int_type __ret;
			if (__builtin_expect(this->eback() < this->gptr(), true)) {
				this->gbump(-1);
				__ret = char_traits<char>::to_int_type(*this->gptr());
			} else
				__ret = this->pbackfail();
			return __ret;
		}

		/**
		 *  @brief  Entry point for all single-character output functions.
		 *  @param  __c  A character to output.
		 *  @return  @a __c, if possible.
		 *
		 *  One of two public output functions.
		 *
		 *  If a write position is available for the output sequence (i.e.,
		 *  the buffer is not full), stores @a __c in that position, increments
		 *  the position, and returns @c traits::to_int_type(__c).  If a write
		 *  position is not available, returns @c overflow(__c).
		 */
		int_type sputc(char __c) {
			int_type __ret;
			if (__builtin_expect(this->pptr() < this->epptr(), true)) {
				*this->pptr() = __c;
				this->pbump(1);
				__ret = char_traits<char>::to_int_type(__c);
			}
			else
				__ret = this->overflow(char_traits<char>::to_int_type(__c));
				return __ret;
		}

		/**
		 *  @brief  Entry point for all single-character output functions.
		 *  @param  __s  A buffer read area.
		 *  @param  __n  A count.
		 *
		 *  One of two public output functions.
		 *
		 *
		 *  Returns xsputn(__s,__n).  The effect is to write @a __s[0] through
		 *  @a __s[__n-1] to the output sequence, if possible.
		 */
		streamsize sputn(const char* __s, streamsize __n) { return this->xsputn(__s, __n); }

	protected:
		/**
		 *  @brief  Base constructor.
		 *
		 *  Only called from derived constructors, and sets up all the
		 *  buffer data to zero, including the pointers described in the
		 *  streambuf class description.  Note that, as a result,
		 *  - the class starts with no read nor write positions available,
		 *  - this is not an error
		 */
		streambuf() :
			_M_in_beg(0),
			_M_in_cur(0),
			_M_in_end(0),
			_M_out_beg(0),
			_M_out_cur(0),
			_M_out_end(0),
			_M_buf_locale(locale()) {
		}

		/**
		 *  @brief  Access to the get area.
		 *
		 *  These functions are only available to other protected functions,
		 *  including derived classes.
		 *
		 *  - eback() returns the beginning pointer for the input sequence
		 *  - gptr() returns the next pointer for the input sequence
		 *  - egptr() returns the end pointer for the input sequence
		 */
		char* eback() const { return _M_in_beg; }

		char* gptr()  const { return _M_in_cur;  }

		char* egptr() const { return _M_in_end; }

		/**
		 *  @brief  Moving the read position.
		 *  @param  __n  The delta by which to move.
		 *
		 *  This just advances the read position without returning any data.
		 */
		void gbump(int __n) { _M_in_cur += __n; }

		/**
		 *  @brief  Setting the three read area pointers.
		 *  @param  __gbeg  A pointer.
		 *  @param  __gnext  A pointer.
		 *  @param  __gend  A pointer.
		 *  @post  @a __gbeg == @c eback(), @a __gnext == @c gptr(), and
		 *         @a __gend == @c egptr()
		 */
		void setg(char* __gbeg, char* __gnext, char* __gend) {
			_M_in_beg = __gbeg;
			_M_in_cur = __gnext;
			_M_in_end = __gend;
		}

		/**
		 *  @brief  Access to the put area.
		 *
		 *  These functions are only available to other protected functions,
		 *  including derived classes.
		 *
		 *  - pbase() returns the beginning pointer for the output sequence
		 *  - pptr() returns the next pointer for the output sequence
		 *  - epptr() returns the end pointer for the output sequence
		 */
		char* pbase() const { return _M_out_beg; }
		char* pptr() const { return _M_out_cur; }
		char* epptr() const { return _M_out_end; }

		/**
		 *  @brief  Moving the write position.
		 *  @param  __n  The delta by which to move.
		 *
		 *  This just advances the write position without returning any data.
		 */
		void pbump(int __n) { _M_out_cur += __n; }

		/**
		 *  @brief  Setting the three write area pointers.
		 *  @param  __pbeg  A pointer.
		 *  @param  __pend  A pointer.
		 *  @post  @a __pbeg == @c pbase(), @a __pbeg == @c pptr(), and
		 *         @a __pend == @c epptr()
		 */
		void setp(char* __pbeg, char* __pend) {
			_M_out_beg = _M_out_cur = __pbeg;
			_M_out_end = __pend;
		}

		/**
		 *  @brief  Changes translations.
		 *  @param  __loc  A new locale.
		 *
		 *  Translations done during I/O which depend on the current
		 *  locale are changed by this call.  The standard adds,
		 *  <em>Between invocations of this function a class derived
		 *  from streambuf can safely cache results of calls to locale
		 *  functions and to members of facets so obtained.</em>
		 *
		 *  @note  Base class version does nothing.
		 */
		virtual void imbue(const locale& __loc _IsUnused) { }

		/**
		 *  @brief  Manipulates the buffer.
		 *
		 *  Each derived class provides its own appropriate behavior.  See
		 *  the next-to-last paragraph of
		 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html#io.streambuf.buffering
		 *  for more on this function.
		 *
		 *  @note  Base class version does nothing, returns @c this.
		 */
		virtual streambuf* setbuf(char*, streamsize) { return this; }

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		virtual pos_type seekoff(
			off_type, ios_base::seekdir,
			ios_base::openmode /*__mode*/ = ios_base::in | ios_base::out
		) {
			return pos_type(off_type(-1));
		}

		/**
		 *  @brief  Alters the stream positions.
		 *
		 *  Each derived class provides its own appropriate behavior.
		 *  @note  Base class version does nothing, returns a @c pos_type
		 *         that represents an invalid stream position.
		 */
		virtual pos_type seekpos(
			pos_type,
			ios_base::openmode /*__mode*/ = ios_base::in | ios_base::out
		) {
			return pos_type(off_type(-1));
		}

		/**
		 *  @brief  Synchronizes the buffer arrays with the controlled sequences.
		 *  @return  -1 on failure.
		 *
		 *  Each derived class provides its own appropriate behavior,
		 *  including the definition of @a failure.
		 *  @note  Base class version does nothing, returns zero.
		 */
		virtual int sync() { return 0; }

		/**
		 *  @brief  Investigating the data available.
		 *  @return  An estimate of the number of characters available in the
		 *           input sequence, or -1.
		 *
		 *  <em>If it returns a positive value, then successive calls to
		 *  @c underflow() will not return @c traits::eof() until at
		 *  least that number of characters have been supplied.  If @c
		 *  showmanyc() returns -1, then calls to @c underflow() or @c
		 *  uflow() will fail.</em> [27.5.2.4.3]/1
		 *
		 *  @note  Base class version does nothing, returns zero.
		 *  @note  The standard adds that <em>the intention is not only that the
		 *         calls [to underflow or uflow] will not return @c eof() but
		 *         that they will return immediately.</em>
		 *  @note  The standard adds that <em>the morphemes of @c showmanyc are
		 *         @b es-how-many-see, not @b show-manic.</em>
		 */
		virtual streamsize showmanyc() { return 0; }

		/**
		 *  @brief  Multiple character extraction.
		 *  @param  __s  A buffer area.
		 *  @param  __n  Maximum number of characters to assign.
		 *  @return  The number of characters assigned.
		 *
		 *  Fills @a __s[0] through @a __s[__n-1] with characters from the input
		 *  sequence, as if by @c sbumpc().  Stops when either @a __n characters
		 *  have been copied, or when @c traits::eof() would be copied.
		 *
		 *  It is expected that derived classes provide a more efficient
		 *  implementation by overriding this definition.
		 */
		virtual streamsize xsgetn(char* __s, streamsize __n);

		/**
		 *  @brief  Fetches more data from the controlled sequence.
		 *  @return  The first character from the <em>pending sequence</em>.
		 *
		 *  Informally, this function is called when the input buffer is
		 *  exhausted (or does not exist, as buffering need not actually be
		 *  done).  If a buffer exists, it is @a refilled.  In either case, the
		 *  next available character is returned, or @c traits::eof() to
		 *  indicate a null pending sequence.
		 *
		 *  For a formal definition of the pending sequence, see a good text
		 *  such as Langer & Kreft, or [27.5.2.4.3]/7-14.
		 *
		 *  A functioning input streambuf can be created by overriding only
		 *  this function (no buffer area will be used).  For an example, see
		 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		virtual int_type underflow() { return char_traits<char>::eof(); }

		/**
		 *  @brief  Fetches more data from the controlled sequence.
		 *  @return  The first character from the <em>pending sequence</em>.
		 *
		 *  Informally, this function does the same thing as @c underflow(),
		 *  and in fact is required to call that function.  It also returns
		 *  the new character, like @c underflow() does.  However, this
		 *  function also moves the read position forward by one.
		 */
		virtual int_type uflow() {
			int_type __ret = char_traits<char>::eof();
			const bool __testeof = char_traits<char>::eq_int_type(this->underflow(), __ret);
			if (!__testeof) {
				__ret = char_traits<char>::to_int_type(*this->gptr());
				this->gbump(1);
			}
			return __ret;
		}

		/**
		 *  @brief  Tries to back up the input sequence.
		 *  @param  __c  The character to be inserted back into the sequence.
		 *  @return  eof() on failure, <em>some other value</em> on success
		 *  @post  The constraints of @c gptr(), @c eback(), and @c pptr()
		 *         are the same as for @c underflow().
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		virtual int_type pbackfail(int_type __c _IsUnused  = char_traits<char>::eof()) {
			return char_traits<char>::eof();
		}

		/**
		 *  @brief  Multiple character insertion.
		 *  @param  __s  A buffer area.
		 *  @param  __n  Maximum number of characters to write.
		 *  @return  The number of characters written.
		 *
		 *  Writes @a __s[0] through @a __s[__n-1] to the output sequence, as if
		 *  by @c sputc().  Stops when either @a n characters have been
		 *  copied, or when @c sputc() would return @c traits::eof().
		 *
		 *  It is expected that derived classes provide a more efficient
		 *  implementation by overriding this definition.
		 */
		virtual streamsize xsputn(const char* __s, streamsize __n);

		/**
		 *  @brief  Consumes data from the buffer; writes to the
		 *          controlled sequence.
		 *  @param  __c  An additional character to consume.
		 *  @return  eof() to indicate failure, something else (usually
		 *           @a __c, or not_eof())
		 *
		 *  Informally, this function is called when the output buffer
		 *  is full (or does not exist, as buffering need not actually
		 *  be done).  If a buffer exists, it is @a consumed, with
		 *  <em>some effect</em> on the controlled sequence.
		 *  (Typically, the buffer is written out to the sequence
		 *  verbatim.)  In either case, the character @a c is also
		 *  written out, if @a __c is not @c eof().
		 *
		 *  For a formal definition of this function, see a good text
		 *  such as Langer & Kreft, or [27.5.2.4.5]/3-7.
		 *
		 *  A functioning output streambuf can be created by overriding only
		 *  this function (no buffer area will be used).
		 *
		 *  @note  Base class version does nothing, returns eof().
		 */
		virtual int_type overflow(int_type __c _IsUnused  = char_traits<char>::eof()) {
			return char_traits<char>::eof();
		}

		// Also used by specializations for char and wchar_t in src.
		void __safe_gbump(streamsize __n) { _M_in_cur += __n; }

		void __safe_pbump(streamsize __n) { _M_out_cur += __n; }

	protected:
		streambuf(const streambuf&);

		streambuf& operator=(const streambuf&);

		void swap(streambuf& __sb) {
			std::swap(_M_in_beg, __sb._M_in_beg);
			std::swap(_M_in_cur, __sb._M_in_cur);
			std::swap(_M_in_end, __sb._M_in_end);
			std::swap(_M_out_beg, __sb._M_out_beg);
			std::swap(_M_out_cur, __sb._M_out_cur);
			std::swap(_M_out_end, __sb._M_out_end);
			std::swap(_M_buf_locale, __sb._M_buf_locale);
		}
};

#endif // _STREAMBUF_H_
