#pragma once 
#include <map>
#include "LEDAnimation_Manifest.h" 

class AnimationIntentIdMapping { 
public:
	AnimationIntentIdMapping () 
	{
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("alexa_listening",ALEXA_LISTENING_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("alexa_alarm",ALEXA_ALARM_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("alexa_speaking",ALEXA_SPEAKING_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("CRITICAL_ERROR",CRITICAL_ERROR_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("alexa_thinking",ALEXA_THINKING_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("FILL_CENTER_OUT",FILL_CENTER_OUT_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("ALL_FADEOUT",ALL_FADEOUT_ANIMATION_ID));
		m_animation_intent_id.insert(std::pair<std::string, unsigned int> ("SYSTEM_BOOTUP_FACTORY_RESET",SYSTEM_BOOTUP_FACTORY_RESET_ANIMATION_ID));
	}
	~AnimationIntentIdMapping (){} 

	inline unsigned int GetIdFromIntent (std::string intent) 
	{ 
		return m_animation_intent_id[intent]; 
	} 

	inline std::string GetIntentFromId (unsigned int id) 
	{ 
		for (auto it = m_animation_intent_id.begin(); it != m_animation_intent_id.end();++it) 
		{ 
			if (it->second == id) 
				return it->first; 
		} 
		return ""; 
	} 
	inline int GetIntentIdMapSize () 
	{ 
		return m_animation_intent_id.size(); 
	} 

private: 
	std::map <std::string, unsigned int> m_animation_intent_id; 

};
