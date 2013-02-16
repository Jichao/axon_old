//unit test tools

#include "utest.h"
#include "ax_debug.h"

namespace axon {

UTest* UTest::head = NULL;
uint32_t UTest::count = 0;

UTest::UTest(UTestFunc* func, const char* file, const char* name)
	: func_(func), file_(file), name_(name), next_(NULL)
{
	next_ = head;
	head = this;
	++UTest::count;
	enable = 1;
}

int UTest::start_test()
{
	int c = 0;
	UTest* p = head;
	DBG_WATCH("UTest::start_test");
	while (p != NULL) {
		if (p->enable) {
			utest_log(p->name_,"begin_test");
			p->run();
			++c;
		}
		p = p->next_;
	}
	return c;
}

uint32_t UTest::test_count()
{
	return UTest::count;
}

static RawLogger g_utest_logger("utest.log");

void utest_log(const char* name, const char* fmt, ...)
{
	char max_fmt[2048];
	va_list args;
	snprintf(max_fmt, sizeof(max_fmt), "UTEST--%s %s", name, fmt);
	va_start(args, fmt);
	g_utest_logger.vlog(max_fmt, args);
	va_end(args);
}

void ut_assert(const char* name, const char* expr, const char* file, int line)
{
	utest_log(name, "Test ASSERT Failed. expr: %s file:%s line:%d", expr, file, line);	
	//abort();
}


} //namespace
