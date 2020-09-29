#include "Log.hpp"
using namespace nokia;

//main function to test the program
int main() {


	Log &logger = Log::get();

	logger.init(Log::LogType::everywhere);
	logger.open();
	
	logger.log("test log", Log::LogLevel::LevelDebug);
	logger.log("serious", Log::LogLevel::LevelError);
	logger.log("exe", Log::LogLevel::LevelError);
	logger.log("byte", Log::LogLevel::LevelError);

	logger.clear(2);
	
	logger.getErrors();
	logger.close();

	std::cin.get();
	
	
	return 0;
}