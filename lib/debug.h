// -*- c++ -*-

#ifndef DEBUG_H
#define DEBUG_H 1

#ifndef NDEBUG
#include <iostream>
#define DBGP(x) std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << x << std::endl
#define DBG1(x) #x "='" << x << "' "
#else  /* !NDEBUG */
#define DBG1(x)
#define DBGP(x) 
#endif /* !NDEBUG */
#define DBGL DBGP("")
#define DBGM(x) DBGP(": " x )
#define DBGV(x) DBGM(DBG1(x))

#endif /* DEBUG_H */
