//|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//| ADSODA Types.h
//|
//| This defines the basic types used by ADSODA.
//|________________________________________________

#ifndef HTYPES
#define HTYPES

//#define VERY_SMALL_NUM	1E-15
#define VERY_SMALL_NUM	1E-5

#include <iostream>


void HandleAssertion(char a);
//void HandleAssertion(bool a);
#define ASSERT(a) { \
  if (!(a)) { \
    std::cerr << "Assertion Failed, " << __FILE__ << ":" << __LINE__ << "\n"; \
    abort();					\
    exit(-1);					\
  } \
}

enum
	{
	BEHIND,
	INFRONT,
	NEITHER
	};

#endif
