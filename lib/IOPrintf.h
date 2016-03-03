// -*- c++ -*-

#ifndef IOPRINTF_H
#define IOPRINTF_H 1

#include <string>

std::string ioprintf(const char* format, ...)
    __attribute__ ((format (printf, 1, 2)));

#endif /* IOPRINTF_H */
