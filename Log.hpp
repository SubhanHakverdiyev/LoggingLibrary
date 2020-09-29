#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include <thread>
#include <mutex>
#include <utility>
#include <ctime>
#include <chrono>
//header only logger library

//unique ID for Log entries
int ID = 0;

//creating namespace for flexibility
namespace nokia
{ 

	
class Log {

public:
	


	enum class LogLevel {
		LevelError = 0, LevelWarning = 1, LevelInfo = 2, LevelDebug = 3
	};

	enum class LogType
	{
	  console,
	  file,
	  everywhere
	};

	//function to detect log level 
	inline std::string stringify(const LogLevel level)
	{
		std::string result = "NONE";
		const static std::map<LogLevel, std::string> LEVEL_STRINGS
		{
			{ LogLevel::LevelError, "ERROR"},
			{ LogLevel::LevelWarning, "WARNING"},
			{ LogLevel::LevelInfo, "INFO"},
			{ LogLevel::LevelDebug, "DEBUG"},
		};

		//iterator for finding level in LEVEL_STRINGS map
		auto it = LEVEL_STRINGS.find(level);
		if (it != LEVEL_STRINGS.end())
		{
			result = it->second;
		}
		return result;
	}





private:

	std::string m_filename;
	std::fstream m_file;

	LogType m_output = LogType::everywhere;
	LogLevel m_level = LogLevel::LevelDebug;

	std::map<int, std::string> errors;
	
	//setting maximum file size for log rotation 
	int maxFileSize = 1024;//bytes
	const std::string filePattern = "Log.txt";
	std::string newFileName;
	int patternCounter = 0;

	//variable is checking file opened or not
	bool m_isOpened = false;
	

private:
	
	//Singleton design pattern
	// Private constructor so that no objects can be created.
	Log()
	{

	}

	~Log()
	{
		close();
	}


public:
	
	//setting time for log entries
	static std::string timeformat() {
		using namespace std::chrono;
		
		auto now = system_clock::now();
		auto tt = system_clock::to_time_t(now);

		auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		std::stringstream ss;
		ss << std::put_time(localtime(&tt), "%F %T");
		ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

		return ss.str();

		}
	
	//generating ID for each entry
	static int logid() {
		
	
		ID++;
		
		return ID;

	}
	
	//adding bracket to log entries
	static std::string addbrack(const std::string& value, const std::string& prefix = "[", const std::string & sufix = "]")
	{
		return prefix + value + sufix;
	}

public:
	
	//singleton design pattern- 
	static Log& get()
	{
		static Log self;
		return self;
	}
	
	
	//initializing the log entry and setting logtype and log level
	void init(const LogType type = LogType::everywhere, const LogLevel level = LogLevel::LevelDebug)
	{
		if (m_isOpened)
		{
			return;
		}

		m_filename = filePattern;
		m_output = type;
		m_level = level;
	
	}
	void flush()
	{
		if (!m_isOpened)
		{
			return;
		}
		{
			
		}

		m_file.flush();
	}

	//opening the log entry
	void open()
	{
		if (m_isOpened)
		{	
			return;
		}

		//checking file is full or not.(log rotation)
		if (!IsFull()) {
			m_file.open(m_filename, std::ios::out);
		
		}
		//if it is full then  another log file starts
		else
		{
			RenameFile();

			m_file.open(m_filename, std::ios::out);
		}
		m_isOpened = m_file.is_open();
	     
		if (!m_isOpened)
		{
			throw std::runtime_error("Coudn't open a file " + m_filename);
		}
	}

	//closing the file logging
	void close()
	{
		if (m_isOpened)
		{
			m_file.flush();
			m_file.close();
		}
		m_isOpened = false;
	}

	//logging function
	void log(const std::string& str, LogLevel level)
	{
		if (!m_isOpened)
		{
			return;
		}

		/*log level checking(if log level is DEBUG we will log all of the logs, 
		in case of WARNING, we log the WARNING and the ERROR logs*/
		if (level > m_level)
			return;

		int logidd = logid();
		
		//result string(long entry)
		std::string res = addbrack(timeformat()) + " " + addbrack(std::to_string(logidd)) +" " + 
			addbrack(stringify(level)) + " " + str;
		
		//saving errrors for getErrors() function
		if (level == LogLevel::LevelError) {
			
			errors.insert(std::pair<int,std::string>(logidd,res));
			
		 }
			
		//logging to file
		if (m_output == LogType::file) 
		{
			m_file << res << std::endl;
		}
		//logging to console
		else if (m_output == LogType::console)
		{
			if (level == LogLevel::LevelError || level == LogLevel::LevelWarning)
      
				/*using cerr instead of cout to show errors and warnings
				(because it is un-buffered and displays errors immediately)*/
				std::cerr << res << std::endl;
			else
				std::cout << res << std::endl;
		}

		//logging to both console and file
		else
		{
			std::cout << res << std::endl;
			m_file << res << std::endl;
		}

	}

	
	template<class T>
	void log(const T& value, LogLevel level)
	{
		std::stringstream ss;

		//serialization for making service externally configurable
#ifdef __SUPPORTS_NOKIA_SERIALIZER__
		
		if constexpr (sfinae_utils::is_serializable_struct<T>::value)
		{
			serializer::Serializer<serializer::StdOstream>::serialize(ss, value);
		}
		else
		{
			ss << value;
		}
#else
		ss << value;
#endif
		log(ss.str(), level);
	}

	//listing active errors
	void getErrors() {
		if (m_output != LogType::file)
			std::cout << std::endl;
		std :: cout << "-------ACTIVE ERRORS------" << std::endl<<std:: endl;
		for (auto itr = errors.begin(); itr!= errors.end(); ++itr) {
			std:: cout << itr->second << std::endl;
		}

	}

	//clearing the error  with given ID
	
	void clear(int id) {
	    
		
			if(!errors.erase(id)){
             
			//if there is no such id logging error
			Log logger;
			logger.init(Log::LogType::everywhere);
			logger.open();

			logger.log("ID ERROR", Log::LogLevel::LevelError);

			logger.close();
			}
			
		
	}

	
	//checking if the file is full or not
	bool IsFull() {
		
		long fileSize = 0;
		std::ifstream file(m_filename, std::ios::binary | std::ios::ate);
		if (file.is_open()) {
			fileSize = file.tellg();
			file.close();
		}

		if (fileSize < maxFileSize)
			return true;

		return false;
	}

	//creating file pattern like(log0,log1,log2)
	void RenameFile() {
		std::string temp = "Log";
		
		newFileName = temp + std::to_string(patternCounter)+".txt";
		
		
		int test=rename(temp.c_str(), newFileName.c_str());
		
		if (test == 0)
			patternCounter++;

	}
			
	};



	}//namespace nokia