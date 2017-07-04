#include <iostream>
#include <vector>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"

//////////////////////
//TBD
//1. Define config file format for intent and animation mapping
//2. utility class for writing to serial/i2c port
/////////////////////

class CLightBarAnimation {
public:
	//PJ - <TBD> open a config file that defines mapping of intent and pattern,
	CLightBarAnimation ():serial_fd(-1){}
	~CLightBarAnimation (){}


	//this functin goes through all ./Animation folder and gets list of animations
	static std::string getLightBarConfig (std::string);

	//this functin goes through all ./Animation folder and gets list of animations
	bool getAnimation (std::string anim_name);


	//this functin goes through all ./Animation folder and gets list of animations
	static std::string getAllAnimations ();

	//this functin goes through all ./Animation folder and gets list of animations
	bool playAnimation (std::string anim_name);


	//this functin goes through all ./Animation folder and gets list of animations
	bool stopAnimation ();

	//this function goes through a particular animation folder and gets all patterns
	bool playPattern (std::string anim_name,std::string pattern_name);

	//this function saves animations by saving all patterns under animation folder
	static bool saveAnimation (char* anim_data);

	//this function saves animations by saving all patterns under animation folder
	static bool saveExistingAnimation (char* anim_data);

	//this function assigns serial port file descriptor
	bool setCommunicationfd(int s_fd);
	
	//this function deletes animation file from server
	static bool deleteAnimation (const char* anim_name);

	struct pattern_vec{
		std::string name;
		std::string pattern;
		int	    active_time;
	};
	std::vector <pattern_vec> lb_patterns;
private:
	//PJ - <TBD> define websocket inf to talk to java client
	//CWSCommunication m_wsComm;
	int serial_fd;
};
