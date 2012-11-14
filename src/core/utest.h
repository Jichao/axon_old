//
//Purpose: Unit test macros and class
//Created: Daly 2012-11-18


#ifndef _AX_UTEST_H_
#define _AX_UTEST_H_

#include "stdheader.h"

namespace axon {

//define a unit test function
//Usage:
// 	UTEST(module_name)
// 	{
//     ...your test code...
// 	}

#define UTEST(name)   \
	static void utest_##name(const char* _utname);   \
	UTest reg_test_##name(utest_##name, __FILE__, #name);    \
	static void utest_##name(const char* _utname)

#define UT_LOG(s) utest_log(_utname, s)
#define UT_LOGV(s, ...) utest_log(_utname, s, __VA_ARGS__)
#define UT_ASSERT(expr) (void)((expr) || (ut_assert(_utname, #expr, __FILE__, __LINE__), 0))

void utest_log(const char* name, const char* fmt, ...);
void ut_assert(const char* name, const char* expr, const char* file, int line); 

class UTest 
{
public:
	typedef void (UTestFunc) (const char* _utname);
	UTest(UTestFunc* func, const char* file, const char* name);
	void run() { func_(name_); }
		
	static int test_count();
	const char* file() { return file_; }
	const char* name() { return name_; }
	UTest* get_head() { return head; }
	static int start_test();

	static uint32_t count;
private:
	static UTest* head;    //global UnitTest link list head

	UTestFunc* func_;
	const char* file_;
	const char* name_;
	UTest* next_;   //test link list
	int enable;     //is this test enable
};

} //namepsace

#endif



