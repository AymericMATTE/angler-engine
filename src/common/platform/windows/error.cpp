#include "pch.h"
#include "error.h"

namespace angler::platform::win {
	const bool Result::succeeded() const {
		return SUCCEEDED(HRESULT(type));
	}
}