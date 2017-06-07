#pragma once

#ifdef PRECOMPILED_INFO
#	pragma message("Compiling PCH...")
#endif

#ifndef _DEBUG
#	define NDEBUG
#	define _SECURE_SCL 0
#	define _HAS_ITERATOR_DEBUGGING 0
#endif
/*#else
#	define D3D_DEBUG_INFO
#	ifndef COMMON_ONLY
#		include <vld.h>
#	endif
#endif*/

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cstdarg>
#include <cassert>

#include "FastFunc.h"

using std::string;
using std::vector;
using std::list;
using std::min;
using std::max;

#undef NULL
