#ifndef No1_Server_Config_h
#define No1_Server_Config_h

class No1Config : public No1LogBaseClass
{
public:
	No1Config();
	~No1Config();
	
	bool parse();

	std::int32_t			server_port;
	std::int32_t			log_level;
	bool					log_tofile;
	bool					log_toconsole;
};

#endif
