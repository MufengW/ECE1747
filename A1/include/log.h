#pragma once
#include <iostream>
#include <cstdio>
#include <stdexcept>

#ifndef NDEBUG
#define M_assert(condition, format, ...) \
    do { \
        if (!(condition)) { \
            char errorMessage[512]; \
            snprintf(errorMessage, sizeof(errorMessage), \
                     "Assertion failed in %s at line %d: (%s) " format, \
                     __FILE__, __LINE__, #condition, ##__VA_ARGS__); \
            std::cerr << errorMessage << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)

    #define M_log(format, ...) \
        do { \
            char logMessage[512]; \
            snprintf(logMessage, sizeof(logMessage), format, ##__VA_ARGS__); \
            std::cerr << "LOG: " << logMessage << std::endl; \
        } while (false)
#else
    /* Empty definition for release mode */
    #define M_assert(condition, format, ...) (void)0
    #define M_log(format, ...) (void)0
#endif
