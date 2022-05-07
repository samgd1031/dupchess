#include "io_handler.h"

/// <summary>
/// constructor
/// </summary>
IO_handler::IO_handler(){}

/// <summary>
/// loop that listens for incoming commmands and sends appropriate outputs through stdin/out
/// </summary>
void IO_handler::process_io() {
	std::string cmd_string;
	while (std::getline(std::cin, cmd_string)) {
		// exit if command string is "exit"
		if (_stricmp(cmd_string.c_str(), "exit") == 0) {
			std::cout << "Exiting..." << std::endl;
			break;
		}
		// talk smack
		else if (cmd_string.compare(0, 5, "smack") == 0) {
			//std::cout << engine.trash_talk() << std::endl;
			std::cout << "fuck you" << std::endl;
		}
	}
}

/// <summary>
/// start processing io
/// </summary>
void IO_handler::start() {
	std::thread io_thread(&IO_handler::process_io, this);
	io_thread.join();
}