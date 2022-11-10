#include "Kick.h"
#include "player_service.hpp"
#include "player.hpp"
#include "common.hpp"

void Kick::breakup_kick(big::player_ptr target)
{
	rage::snMsgRemoveGamersFromSessionCmd cmd{};	//容器

	cmd.m_session_id = (*big::g_pointers->m_network)->m_game_session_ptr->m_rline_session.m_session_id;	///自己的会话ID
	cmd.m_num_peers = 1;	//踢出数量
	cmd.m_peer_ids[0] = target->get_session_peer()->m_peer_data.m_peer_id_2;	//被踢出者PeerID

	//在本地移除被踢出者
	big::g_pointers->m_handle_remove_gamer_cmd((*big::g_pointers->m_network)->m_game_session_ptr, target->get_session_player(), &cmd);

	for (auto& [name, plyr] : big::g_player_service->players())
	{
		if (plyr->id() != target.get()->id())
		{
			//告诉其他Peer,移除指定玩家
			big::g_pointers->m_send_remove_gamer_cmd(

				//当前的连接管理器句柄
				(*big::g_pointers->m_network)->m_game_session_ptr->m_net_connection_mgr,

				//目标Peer的信息
				big::g_pointers->m_get_connection_peer(
					(*big::g_pointers->m_network)->m_game_session_ptr->m_net_connection_mgr,
					(int)plyr->get_session_player()->m_player_data.m_peer_id_2),

				//自己的PeerID
				(*big::g_pointers->m_network)->m_game_session_ptr->m_connection_identifier,

				//cmd信息储存了被踢出目标ID+被踢出者数量
				&cmd,

				//默认发送模式
				0x1000000);
		}	
	}
}