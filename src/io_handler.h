#pragma once


#include <thread>
#include <iostream>
#include <iomanip>
#include <string>

#include "dupEngine.h"


class IO_handler {
public:
	IO_handler(); // constructor
	void start(); // start processing io
private:
	DupEngine engine;
	std::thread io_thread;
	void process_io(); // loop to listen for incoming commands and send appropriate output

	// uci commands
	void respond_uci();
};