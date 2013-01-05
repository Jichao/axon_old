//string class for axon engine

#ifndef _AX_STRING_H_
#define _AX_STRING_H_

#include "stdheader.h"

namespace axon {

//reference to Facebook folly library FBString
//small string use direct char[] ( < 22 byte in 64bit machine)
//for larger string use heap memory.
//balance the speed and memory usage according to application's statistis

class string_t 
{
private:
	struct LargerStr {
		char* data_;
		uint32_t len_;
		uint32_t capacity_;
		char dummy[8];    //balance speed and memory usage according to actual statistics
						  //modified the value to achieve most optimization
	};

public:
	string_t();
	string_t(const string_t &rhs);
	string_t(const char* cstr);
	~string_t();

	string_t& operator=(const string_t &rhs);
	void assign(const char* s);

	const char* c_str() const;
	bool equal(const string_t &rhs) const;
	bool equal(const char* cstr) const;
	bool operator==(const string_t &rhs) const;
	bool operator==(const char* cstr) const;
	void strcat(const char* s);
	void strcat(const string_t &rhs);
	void clear();

	//reserve heap memory
	void expand(const uint32_t n);

	enum StringType {
		SMALL_STR = 0,  
		LARGER_STR = 0x80, 
	};
	enum {
		LAST_CHAR = sizeof(LargerStr) -1,
		TYPE_CHAR = LAST_CHAR, 
		MAX_SMALL_LEN = sizeof(LargerStr) - 2,
		TYPE_MASK = 0x80,
		SMALL_LEN_MASK = ~TYPE_MASK,
	};

	StringType type() const {
		//type info stored in the highest bit
		return static_cast<StringType>(small_[TYPE_CHAR] & 0x80);
	}
	
	uint32_t len() const {
		if (type() == SMALL_STR) {
			return static_cast<uint32_t>(small_[TYPE_CHAR] & SMALL_LEN_MASK);
		}
		return larger_.len_;
	}

	uint32_t capacity() const {
		if (type() == SMALL_STR) {
			return MAX_SMALL_LEN;
		}
		return larger_.capacity_;
	}

private:
	union {
		char small_[sizeof(LargerStr)];
		LargerStr larger_;
	};
};

} //namespace


#endif
