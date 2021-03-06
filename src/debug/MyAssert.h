/*
 * Assert.h
 *
 *  Created on: Apr 8, 2014
 *      Author: hudo
 */

#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#include <iostream>
#include <cstdlib>

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

#endif