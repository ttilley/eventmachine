#ifndef __EventMachine__Rubyisms__H_
#define __EventMachine__Rubyisms__H_ 1


#include <ruby.h>
#if defined(HAVE_RBTRAP)
#include <rubysig.h>
#endif
#if defined(HAVE_RUBY_VERSION_H)
#include <ruby/version.h>
#endif
#if defined(HAVE_RUBY_DEFINES_H)
#include <ruby/defines.h>
#endif
#if defined(HAVE_RUBY_INTERN_H)
#include <ruby/intern.h>
#endif
#if defined(HAVE_RUBY_IO_H)
#include <ruby/io.h>
#endif
#if defined(HAVE_RUBY_MISSING_H)
#include <ruby/missing.h>
#endif
#if defined(HAVE_RUBY_ST_H)
#include <ruby/st.h>
#endif
#if defined(HAVE_RUBY_THREAD_H)
#include <ruby/thread.h>
#endif
#if defined(HAVE_RUBY_RUBY_H)
#include <ruby/ruby.h>
#endif


// ruby/defines.h insists on breaking this
#undef __


#ifndef RSTRING_PTR
#define RSTRING_PTR(str) RSTRING(str)->ptr
#endif
#ifndef RSTRING_LEN
#define RSTRING_LEN(str) RSTRING(str)->len
#endif
#ifndef RSTRING_LENINT
#define RSTRING_LENINT(str) RSTRING_LEN(str)
#endif
#ifndef RFLOAT_VALUE
#define RFLOAT_VALUE(arg) RFLOAT(arg)->value
#endif

#ifndef SIZET2NUM
#if SIZEOF_SIZE_T > SIZEOF_LONG && defined(HAVE_LONG_LONG)
# define SIZET2NUM(v) ULL2NUM(v)
# define SSIZET2NUM(v) LL2NUM(v)
#elif SIZEOF_SIZE_T == SIZEOF_LONG
# define SIZET2NUM(v) ULONG2NUM(v)
# define SSIZET2NUM(v) LONG2NUM(v)
#else
# define SIZET2NUM(v) UINT2NUM(v)
# define SSIZET2NUM(v) INT2NUM(v)
#endif
#endif


typedef void* em_blocking_function_t(void*);
typedef void em_unblock_function_t(void*);

static inline void*
em_blocking_region (em_blocking_function_t *func, void *data1,
					em_unblock_function_t *ubf, void *data2)
{
#if defined(HAVE_RB_THREAD_CALL_WITHOUT_GVL) && defined(RUBY_THREAD_H)
	return rb_thread_call_without_gvl(func, data1, ubf, data2);
#elif defined(HAVE_RB_THREAD_BLOCKING_REGION)
	VALUE(*f)(void*) = (VALUE(*)(void*))func;
	return (void*)rb_thread_blocking_region(f, data1, ubf, data2);
#elif defined(HAVE_RBTRAP)
	VALUE rv = Qnil;
	TRAP_BEG;
	rv = (VALUE)func(data1);
	TRAP_END;
	return (void*)rv;
#elif defined(HAVE_RB_THREAD_CHECK_INTS)
	VALUE rv = Qnil;
	rb_enable_interrupt();
	rv = (VALUE)func(data1);
	rb_disable_interrupt();
	rb_thread_check_ints()
	return (void*)rv;
#else
	#error unable to find usable idiom
#endif
}

#ifndef RUBY_UBF_IO
#define RUBY_UBF_IO ((em_unblock_function_t *)-1)
#endif
#ifndef RUBY_UBF_PROCESS
#define RUBY_UBF_PROCESS ((em_unblock_function_t *)-1)
#endif

#endif // __EventMachine__Rubyisms__H_
