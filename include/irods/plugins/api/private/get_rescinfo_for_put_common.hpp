#ifndef IRODS_PROJECT_TEMPLATE_PRIVATE_COMMON_HPP
#define IRODS_PROJECT_TEMPLATE_PRIVATE_COMMON_HPP

// This file is for the implementation of the plugin. Symbols declared and/or defined in
// this header MUST NOT be used outside of the plugin.

#include <irods/rodsDef.h> // For funcPtr.

#include <functional>

// Forward declarations.
struct RsComm;
struct DataObjInp;

// The function signature of the API plugin.
using operation_type = std::function<int(RsComm*, DataObjInp*, char**)>;

// Defined differently based on whether the client module or server module
// is being compiled. DO NOT CHANGE THESE DECLARATIONS!
extern const operation_type op;
extern funcPtr fn_ptr;

#endif // IRODS_PROJECT_TEMPLATE_PRIVATE_COMMON_HPP
