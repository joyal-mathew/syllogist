#pragma once

#include <iostream>

#define ASSERT(condition)                                                      \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << __FILE__ << ':' << __LINE__                           \
                      << ":\tAssertion Failed: " #condition << std::endl;      \
            throw std::runtime_error("ASSERTION ERROR");                       \
        }                                                                      \
    } while (false)

#ifdef NO_CHECKS
#define DEBUG_ASSERT(condition)
#else
#define DEBUG_ASSERT(condition) ASSERT(condition)
#endif
