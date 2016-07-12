#ifndef RYULIB_BASE_HPP
#define RYULIB_BASE_HPP

#include <functional>

using namespace std;

typedef function<void()> NotifyEvent;
typedef function<void(void*)> PointerEvent;
typedef function<void(int)> IntegerEvent;
typedef function<void(void *, int)> DataEvent;

#endif  // RYULIB_BASE_HPP
