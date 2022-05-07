#pragma once


#include <thread>
#include <iostream>
#include <string>


class IO_handler {
public:
	IO_handler(); // constructor
	void start(); // start processing io
private:
	
	std::thread io_thread;
	void process_io(); // loop to listen for incoming commands and send appropriate output
};