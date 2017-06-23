#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "LightBarAnimation.h"

using namespace rapidjson;
const std::string PATTERN = "pattern";

//PJ - Ideally this method should parse config file and return all the animations
//listed in config file. :w

bool CLightBarAnimation::getAnimation (std::string anim_name)
{
	//std::string anim_path = anim_name+".ptrn";
	std::string anim_path = anim_name+".anim";
	std::cout << "animation path = " << anim_path << std::endl;
#if 1 
	//open animation file
	//FILE* fd_anim = fopen(anim_path+"/"+anim_name+".ptrn", "r");
	FILE* fd_anim = fopen(anim_path.c_str(), "r");
	//return emptry string if file not found
        if (fd_anim == NULL)
	{
		std::cout << "unable to open file " << std::endl;
		return "";
	}
	fseek(fd_anim,0L,SEEK_END);
	long total_size = ftell (fd_anim);
	std::cout << "total file size = " << total_size << std::endl;
	fseek(fd_anim,0L,SEEK_SET);
	char* anim_data = new char [total_size+1];
	memset(anim_data,0,total_size+1);
	int bytes_read = fread(anim_data,total_size,1,fd_anim);
	if (bytes_read == 0)
	{
		std::cout << "unable to read any data " << std::endl;
		fclose (fd_anim);
		return "";
	}
	//closing file
	fclose (fd_anim);
	//std::cout << "data - " << anim_data << std::endl;
	std::string anim_json_data = static_cast <std::string>(anim_data);

	//need to find a better way to conver such that string can be access as char
	//char buf[10000];
	//memset(buf,0,sizeof(buf));
 	//memcpy(buf,anim_json_data.c_str(),anim_json_data.length()+1);	
	Document doc_anim;

	if(doc_anim.ParseInsitu(anim_data).HasParseError())
	{
		std::cout << "unable to parse json data " << std::endl;
		return false;
	}

	assert(doc_anim.IsObject());
	int i = 1;
#if 0
	for (;;)
	{
		std::string key_json = PATTERN + std::to_string(i);
		//std::cout << "looking for key - " << key_json << std::endl;
		Value::MemberIterator doc_it_anim = doc_anim.FindMember(key_json.c_str());
		if(doc_it_anim == doc_anim.MemberEnd()) break;
		//std::cout << "key-" <<key_json << "value - " << doc_it_anim->value["pattern"].GetString() << std::endl;
		//write /GetString() data to serial port from here
		int activetime = doc_it_anim->value["active_time"].GetInt();
		pattern_vec tmp_vec;
		
		tmp_vec.name 		= doc_it_anim->value["name"].GetString();
		tmp_vec.pattern 	= doc_it_anim->value["pattern"].GetString();
		tmp_vec.active_time 	= activetime;
		
		lb_patterns.push_back(tmp_vec);
		i++;
	}
#endif	

        const Value& a = doc_anim["patterns"];


         // rapidjson uses SizeType instead of size_t.
        for (rapidjson::SizeType i = 0; i < a.Size(); i++)
        {
            const Value& c = a[i];
            printf("%s \n",c["name"].GetString());
            printf("%s \n",c["pattern"].GetString());
            printf("%d \n",std::stoi(c["active_time"].GetString()));
		pattern_vec tmp_vec;
		
		tmp_vec.name 		= c["name"].GetString();
		tmp_vec.pattern 	= c["pattern"].GetString();
		tmp_vec.active_time 	= std::stoi(c["active_time"].GetString());
		
		lb_patterns.push_back(tmp_vec);
        }      




	delete [] anim_data;
	return true;
#else
	//PJ - <TBD> below code should be able to replace above code
	//std::ifstream ifs(anim_path);
	std::ifstream ifs("test.json");
	IStreamWrapper isw(ifs);
	Document d;
	d.ParseStream(isw);
	assert(d.IsObject());
	assert(d.HasMember("pattern1"));
#endif
}

//this function will extract all the pattern data from the file and create a map/vector
//and schedule onshot timer. On timer expiry, pattern will be send over serial/i2c
//to LPM/PSoC and onshot timer will be scheduled per active time.
bool CLightBarAnimation :: playAnimation (std::string anim_name)
{
	//get animation data parse one pattern at a time, activate if for a specified time
	//and keep doing till all patterns are activated or current animation
	
	std::vector <pattern_vec>::iterator it_pattern;

	//is aborted for a new animation
	if (getAnimation (anim_name))
	{
		//PJ - <TBD> should be moved to a thread so that it can be executed in parallel
		for (it_pattern = lb_patterns.begin();it_pattern!= lb_patterns.end();++it_pattern)
		{
			pattern_vec ptrn = *it_pattern;
			//create a command 
			std::string cmd = "{\"event\":\"cmd\",\"type\":\"led\",\"action\":\"set strip\",\"strip\":\"" + ptrn.pattern + "\"}";
			std::cout << "cmd - " << cmd << std::endl;
			if (serial_fd > 0)
				write (serial_fd,cmd.c_str(),cmd.length());
			usleep(ptrn.active_time * 1000);
		}
	}
	//parse anim, json data to extract patterns

	//create a time instance to set up a timer to issue pattern after defined time
	//use usleep to sleep for period, replace with APTime when integrated
	
}

bool CLightBarAnimation :: stopAnimation ()
{
	//disable timer
	
	//send blank pattern to ensure light bar is not showing any pattern

}

bool CLightBarAnimation :: setCommunicationfd (int s_fd)
{
	serial_fd = s_fd;
}

bool CLightBarAnimation :: saveAnimation (char* anim_data)
{
	Document doc;

	std::cout << "in save animation - " << anim_data << std::endl;
	if(doc.Parse(anim_data).HasParseError())
	{
		std::cout << anim_data << std::endl;
		std::cout << "unable to save animation data" << std::endl;
		return false;
	}

	assert(doc.IsObject());

	if (doc.HasMember("animation"))
	{
		//std::string patterns = doc["patterns"].GetString();
		std::string patterns = static_cast <std::string> (anim_data);
		std::string anim_name = doc["animation"].GetString();

		if (std::ifstream(anim_name + ".anim"))
			return false;
		//opn file wiht name anim_name and save patterns in it
		std::ofstream anim_file;
		//anim_file.open ("test.anim");
		anim_file.open (anim_name+".anim");
		anim_file << patterns ;
		anim_file.close();

	}
 	return true;

}

std::string CLightBarAnimation :: getAllAnimations () 
{
	//open all files with .anim extension and read the file content, send it to client
	//client will parse javascript data to display on page
	//
	DIR *dir;
	struct dirent *ent;
	std::vector <std::string> anim_vec;
	std::vector <std::string> anim_array;

	if ((dir = opendir ("./")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			char tmp_fname[256] ;
			memset(tmp_fname,0,256);
			//printf ("%s\n", ent->d_name);
			strcpy(tmp_fname,ent->d_name);
			char* fname = strtok(tmp_fname,".");
			if (fname != NULL)
			{
				char *fext = strtok(NULL,".");
				if (NULL != fext)
				{
					if (strcmp(fext,"anim") == 0)
					{
						std::cout << "animation file - " << ent->d_name << std::endl;
						anim_vec.push_back(ent->d_name);
					}

				}
			}
  		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return -1;
	}
	
	std::vector <std::string> :: iterator it_fname;

	for (it_fname = anim_vec.begin(); it_fname != anim_vec.end();++it_fname)
	{
		std::ifstream ifs(*it_fname);
		std::string data;
		if (ifs.is_open())
		{
			getline(ifs,data);
			ifs.close();
			anim_array.push_back(data);
		}
	}

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();               // Between StartObject()/EndObject(), 
	writer.Key("event");                // output a key,
	writer.String("animation list");             // follow by a value.
	writer.Key("animations");
	writer.StartArray();                // Between StartArray()/EndArray(),
	for (unsigned i = 0; i < anim_array.size(); i++)
		writer.String(anim_array[i].c_str());                 // all values are elements of the array.
	writer.EndArray();
	writer.EndObject();
	
	std::string s(strbuf.GetString(),strbuf.GetSize());
	return s;


}

bool CLightBarAnimation :: deleteAnimation (const char* anim_name)
{
	if (anim_name != NULL)
		if(0 == std::remove (strcat(anim_name,".anim")))
			std::cout << "animation \"" << anim_name << "\" successfully!!" << std::endl;
	return true;
		



}
