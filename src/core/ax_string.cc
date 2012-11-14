//
//string class for axon engine
//Created: Daly 2012-11-18
//

#include "ax_debug.h"
#include "utest.h"
#include "ax_string.h"

namespace axon {

string_t::string_t()
{
	memset(&larger_, 0, sizeof(larger_));
	DBG_ASSERT(type() == SMALL_STR && len() == 0);
}

string_t::string_t(const string_t &rhs)
{
	if (&rhs == this) return;

	if (rhs.type() == SMALL_STR) {
		//just copy the whole thing
		memcpy(small_, rhs.small_, sizeof(small_));
	} else {
		uint32_t alloc_size = rhs.larger_.len_ + 1;
		//deep copy the string, alloc new space
		larger_.data_ = (char*)malloc(alloc_size);
		memcpy(larger_.data_, rhs.larger_.data_, alloc_size);

		larger_.len_ = rhs.larger_.len_;
		larger_.capacity_ = larger_.len_;
		//copy the type byte
		small_[LAST_CHAR] = rhs.small_[LAST_CHAR];
	}
}

//created from C string
string_t::string_t(const char* cstr)
{
	uint32_t len = strlen(cstr);
	memset(&larger_, 0, sizeof(larger_));
	if (len <= MAX_SMALL_LEN && len < LAST_CHAR) {
		memcpy(small_, cstr, len);		
		small_[TYPE_CHAR] |= len & SMALL_LEN_MASK;
	} else {
		uint32_t alloc_size = len + 1;
		larger_.len_ = len;
		larger_.capacity_ = len;	
		larger_.data_ = (char*)malloc(alloc_size);
		//include terminated '\0'
		memcpy(larger_.data_, cstr, alloc_size);
		//set last byte
		small_[TYPE_CHAR] = LARGER_STR;
	}
}

string_t::~string_t()
{
	if (type() == LARGER_STR) {
		free(larger_.data_);	
	}
}

const char* string_t::c_str() const
{
	if (type() == SMALL_STR) {
		return small_;
	} 
	return larger_.data_;
}

bool string_t::equal(const string_t &rhs) const
{
	return equal(rhs.c_str());
}

bool string_t::equal(const char* cstr) const
{
	uint32_t sl = strlen(cstr);
	if (sl != len()) return false;
	if (type() == SMALL_STR) {
		return !::strcmp(small_, cstr);
	}

	return !::strcmp(larger_.data_, cstr);
}

bool string_t::operator==(const string_t &rhs) const
{
	return equal(rhs);
}

bool string_t::operator==(const char* cstr) const
{
	return equal(cstr);
}

//expand capacity to n (usually used for string buffer)
void string_t::expand(uint32_t n)
{
	uint32_t alloc_size = n + 1;
	uint32_t slen;
	if (n < MAX_SMALL_LEN) return;  //won't shrink
	
	slen = len();
	if (type() == SMALL_STR) {
		char tmp[MAX_SMALL_LEN + 1];
		if (slen > 0) strcpy(tmp, small_);
		larger_.data_ = (char*)malloc(alloc_size);
		memset(larger_.data_, 0, alloc_size);
		strcpy(larger_.data_, tmp);
	} else if (n > larger_.capacity_) {
		char *newp = (char*)realloc(larger_.data_, alloc_size);
		RT_ASSERT(newp != NULL);
		larger_.data_ = newp;
		return;
	} else {
		//won't shrink
		return;
	}	
	larger_.capacity_ = n;
	larger_.len_ = slen;

}

//push back
void string_t::strcat(const char* s)
{
	uint32_t len_s = strlen(s);
	uint32_t new_len = len_s + len();
	if (new_len > capacity()) {
		expand(new_len);
	}
	if (new_len > MAX_SMALL_LEN) {
		::strcat(larger_.data_, s);	
	} else {
		::strcat(small_, s);
	}
}

//unit test=======================================
UTEST(string_t)
{
	string_t p;
	string_t q("hello");
	string_t s(q); 
	string_t se("123456789012345678901234");
	string_t ls("larger string test. I'm really really long enough");
	UT_LOGV("p:%s \n q:%s \n s:%s", p.c_str(), q.c_str(), s.c_str());
	UT_LOGV("ls:%s", ls.c_str());
	UT_ASSERT(q == s && q == "hello" & q.equal(s));

}

}  //namespace
