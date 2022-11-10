#include "Kick.h"
#include "player_service.hpp"
#include "player.hpp"
#include "common.hpp"

void Kick::breakup_kick(big::player_ptr target)
{
	rage::snMsgRemoveGamersFromSessionCmd cmd{};	//����

	cmd.m_session_id = (*big::g_pointers->m_network)->m_game_session_ptr->m_rline_session.m_session_id;	///�Լ��ĻỰID
	cmd.m_num_peers = 1;	//�߳�����
	cmd.m_peer_ids[0] = target->get_session_peer()->m_peer_data.m_peer_id_2;	//���߳���PeerID

	//�ڱ����Ƴ����߳���
	big::g_pointers->m_handle_remove_gamer_cmd((*big::g_pointers->m_network)->m_game_session_ptr, target->get_session_player(), &cmd);

	for (auto& [name, plyr] : big::g_player_service->players())
	{
		if (plyr->id() != target.get()->id())
		{
			//��������Peer,�Ƴ�ָ�����
			big::g_pointers->m_send_remove_gamer_cmd(

				//��ǰ�����ӹ��������
				(*big::g_pointers->m_network)->m_game_session_ptr->m_net_connection_mgr,

				//Ŀ��Peer����Ϣ
				big::g_pointers->m_get_connection_peer(
					(*big::g_pointers->m_network)->m_game_session_ptr->m_net_connection_mgr,
					(int)plyr->get_session_player()->m_player_data.m_peer_id_2),

				//�Լ���PeerID
				(*big::g_pointers->m_network)->m_game_session_ptr->m_connection_identifier,

				//cmd��Ϣ�����˱��߳�Ŀ��ID+���߳�������
				&cmd,

				//Ĭ�Ϸ���ģʽ
				0x1000000);
		}	
	}
}