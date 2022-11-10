#pragma once
#include <iostream>
#include "network/snSession.hpp"
#include "pointers.hpp"
#include "player_service.hpp"


using Player = std::int32_t;
namespace Kick
{
	void breakup_kick(big::player_ptr target);
}