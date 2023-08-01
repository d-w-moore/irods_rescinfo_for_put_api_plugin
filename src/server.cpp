#include "irods/plugins/api/private/get_rescinfo_for_put_common.hpp"
#include "irods/plugins/api/get_rescinfo_for_put_common.h" // For API plugin number.

#include <irods/apiHandler.hpp>
#include <irods/catalog_utilities.hpp> // Requires linking against libnanodbc.so
#include <irods/irods_logger.hpp>
//#include <irods/irods_re_serialization.hpp>
#include <irods/rodsErrorTable.h>

#include <fmt/format.h>

#include <cstring> // For strdup.

#include <irods/getHostForGet.h>
#include <irods/getHostForPut.h>
#include <irods/rodsLog.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/getRemoteZoneResc.h>
#include <irods/dataObjCreate.h>
#include <irods/objMetaOpr.hpp>
#include <irods/resource.hpp>
#include <irods/collection.hpp>
#include <irods/specColl.hpp>
#include <irods/miscServerFunct.hpp>
#include <irods/openCollection.h>
#include <irods/readCollection.h>
#include <irods/closeCollection.h>
#include <irods/dataObjOpr.hpp>
#include <irods/rsGetHostForGet.hpp>
#include <irods/irods_resource_backport.hpp>
#include <irods/irods_resource_redirect.hpp>

#include <cstring>

#include <nlohmann/json.hpp>

namespace
{
	using log_api = irods::experimental::log::api;

        auto rc_get_rescinfo_for_put(RcComm* _comm, const DataObjInp* _message, char** _response) -> int
        {
            if (!_message || !_response) {
                return SYS_INVALID_INPUT_PARAM;
            }

            return procApiRequest(_comm,
                                  IRODS_APN_GET_RESCINFO_FOR_PUT,
                                  _message, // NOLINT(cppcoreguidelines-pro-type-const-cast)
                                  nullptr,
                                  reinterpret_cast<void**>(_response), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast
                                  nullptr);
        } // rc_get_rescinfo_for_put

	//
	// Function Prototypes
	//

	auto call_get_rescinfo_for_put(irods::api_entry*, RsComm*, dataObjInp_t*, char**) -> int;

	auto rs_get_rescinfo_for_put(RsComm*, dataObjInp_t*, char**) -> int;

	//
	// Function Implementations
	//

	auto call_get_rescinfo_for_put(irods::api_entry* _api, RsComm* _comm, dataObjInp_t *dataObjInp, char** _resp) -> int
	{
		return _api->call_handler<dataObjInp_t*, char**>(_comm, dataObjInp, _resp);
	} // call_get_rescinfo_for_put


	auto rs_get_rescinfo_for_put(RsComm* rsComm, dataObjInp_t *dataObjInp, char** _resp) -> int
	{
            char _REMOTE_OPEN[]{"remoteOpen"};

            rodsServerHost_t *rodsServerHost;
            const int remoteFlag = getAndConnRemoteZone(rsComm, dataObjInp, &rodsServerHost, _REMOTE_OPEN);
            if (remoteFlag < 0) {
                return remoteFlag;
            }
            else if (REMOTE_HOST == remoteFlag) {
// // // //     //const int status = rcGetHostForPut(rodsServerHost->conn, dataObjInp, _resp);
                const int status = rc_get_rescinfo_for_put(rodsServerHost->conn, dataObjInp, _resp);
                if (status < 0) {
                    return status;
                }
            }
            else {
                // =-=-=-=-=-=-=-
                // working on the "home zone", determine if we need to redirect to a different
                // server in this zone for this operation.  if there is a RESC_HIER_STR_KW then
                // we know that the redirection decision has already been made
                std::string hier{};
                if ( getValByKey( &dataObjInp->condInput, RESC_HIER_STR_KW ) == NULL ) {
                    try {
                        auto result = irods::resolve_resource_hierarchy(irods::CREATE_OPERATION, rsComm, *dataObjInp);
                        hier = std::get<std::string>(result);
                    }
                    catch (const irods::exception& e ) {
                        irods::log(e);
                        return e.code();
                    }
                    addKeyVal( &dataObjInp->condInput, RESC_HIER_STR_KW, hier.c_str() );
                } // if keyword

                // =-=-=-=-=-=-=-
                // extract the host location from the resource hierarchy
                std::string location;
                irods::error ret = irods::get_loc_for_hier_string( hier, location );
                if ( !ret.ok() ) {
                    irods::log( PASSMSG( "rsGetHostForPut - failed in get_loc_for_hier_string", ret ) );
                    return -1;
                }

                // =-=-=-=-=-=-=-
                // set the out variable
                //*outHost = strdup( location.c_str() );
                //
                // =-=-=-=-=-=-=-
                // set the out variable
                nlohmann::json J;
                J["host"] = location;
                J["resc_hier"] = hier;
                *_resp = strdup( J.dump().c_str() );
            }
            return 0;
	} // rs_get_rescinfo_for_put
} //namespace

const operation_type op = rs_get_rescinfo_for_put;
auto fn_ptr = reinterpret_cast<funcPtr>(call_get_rescinfo_for_put);
