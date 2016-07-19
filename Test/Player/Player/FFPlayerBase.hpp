#pragma once

#include <functional>

using namespace std;

typedef function<void()> NotifyEvent;
typedef function<void(void *data)> DataEvent;