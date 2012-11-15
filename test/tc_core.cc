
#include <core/ax_core.h>
#include <stdio.h>

using namespace axon;

void test_hashmap(const char* _utname);

int main()
{
	UTest* head = UTest::get_head();
	printf("start unit test\n");
	UTest::start_test();
	test_hashmap("ax_hashmap");
	printf("finish test case: %d\n", UTest::test_count() + 1);
	return 0;
}


void test_hashmap(const char* _utname)
{
	string_t s1("hello");
	string_t s2("world");
	string_t *ps;
	HashMapInt mpi(100);
	HashMapStr mps(100);
	UT_ASSERT(mpi.size() == 0 && mpi.nslot() == 100);
	mpi.insert(10, &s1);
	mpi.insert(20, &s2);
	UT_ASSERT(mpi.size() == 2 && mpi.nslot() == 100);
	//iteration
	HashEntryInt *phead = mpi.head();
	while(phead != NULL) {
		printf("test_hash. key=%d val=%s\n", phead->get_key(), ((string_t*)phead->data)->c_str());
		phead = phead->get_link_next();
	}

	ps = (string_t*)mpi.get_data(10);	
	UT_ASSERT(ps->equal("hello"));
	mpi.remove_get(10);
	printf("test_hash, mpi_size=%d\n", mpi.size());
	UT_ASSERT(mpi.size() == 1);
	mpi.remove_get(100000);
	mpi.insert(10, &s1);
	
	UT_ASSERT(mps.size() == 0 && mps.nslot() == 100);
	mps.insert(s2, (void*)20);
	HashEntryStr *pe = mps.find("world");	
	UT_ASSERT((long)(pe->data) == 20);
	mps.remove(pe);
	UT_ASSERT(mps.size() == 0);
	pe = mps.find("123455");
	UT_ASSERT(pe == NULL);
}
