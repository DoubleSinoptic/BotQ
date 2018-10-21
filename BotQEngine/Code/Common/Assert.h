#pragma once

#include "Log.h"

#define Assert(x) {if(!(bool(x) == true)){ Error("Assertion failed: in: %s:(%d) expression: %s", __FILE__, __LINE__, # x); }}
