#pragma once

#ifdef PRECOMPILED_INFO
#	pragma message("Compiling PCH...")
#endif

#ifndef _DEBUG
#	define NDEBUG
#	define _SECURE_SCL 0
#	define _HAS_ITERATOR_DEBUGGING 0
#else
#	include <vld.h>
#endif

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <cstdarg>
#include <cassert>
#include <thread>
#include <DirectXMath.h>

#include "FastFunc.h"

using std::string;
using std::vector;
using std::list;
using std::min;
using std::max;

#undef NULL
