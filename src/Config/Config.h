#ifndef CHATROOM_CONFIG_CONFIG_H
#define CHATROOM_CONFIG_CONFIG_H
#include <cstdint>
class No1Config
{
private:
	No1Config();
public:
	~No1Config();
	
	static No1Config* get_instance();

	bool parse();

	int get_server_port()const{return m_port;}
	int get_log_level()const{return m_level;}
	bool get_log_toconsole()const {return m_toconsole;}
	bool get_log_tofile()const{return m_tofile;}
	int  get_handle_thrd_num()const{return m_handle_thrd_num;}
	int	 get_process_num()const{return m_process_num;}

	int						m_port;
	int						m_level;
	bool					m_tofile;
	bool					m_toconsole;
	int						m_process_num;
	int						m_handle_thrd_num;
	static No1Config*		m_instance;
};

#endif
