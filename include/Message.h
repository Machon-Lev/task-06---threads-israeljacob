#pragma once
#include<thread>

struct  Message{
	std::thread::id thread = std::this_thread::get_id();
	double num;
	bool flag;
};