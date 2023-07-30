#include "irods/plugins/api/rc_get_rescinfo_for_put.h"

#include "irods/plugins/api/get_rescinfo_for_put_common.h" // For API plugin number.

#include <irods/procApiRequest.h>
#include <irods/rodsErrorTable.h>

auto rc_get_rescinfo_for_put(RcComm* _comm, const char* _message, char** _response) -> int
{
	if (!_message || !_response) {
		return SYS_INVALID_INPUT_PARAM;
	}

	return procApiRequest(_comm,
	                      APN_PROJECT_TEMPLATE,
	                      const_cast<char*>(_message), // NOLINT(cppcoreguidelines-pro-type-const-cast)
	                      nullptr,
	                      reinterpret_cast<void**>(_response), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
	                      nullptr);
} // rc_get_rescinfo_for_put
