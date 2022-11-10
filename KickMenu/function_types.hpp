#pragma once
#include "datanodes/player/CPlayerGameStateDataNode.hpp"
#include "network/snSession.hpp"
#include "natives.hpp"

namespace big::functions
{
	using get_native_handler = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable* registration_table, rage::scrNativeHash hash);
	using fix_vectors = void(*)(rage::scrNativeCallContext* call_ctx);

	using get_connection_peer = uintptr_t(*)(rage::netConnectionManager* manager, int peer_id);
	using send_remove_gamer_cmd = void(*)(rage::netConnectionManager* net_connection_mgr, uintptr_t player, int connection_id, rage::snMsgRemoveGamersFromSessionCmd* cmd, int flags);
	using handle_remove_gamer_cmd = void* (*)(rage::snSession* session, rage::snPlayer* origin, rage::snMsgRemoveGamersFromSessionCmd* cmd);
}
