#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include"Message.h"


std::vector<Message> messages;
std::mutex mtx_double;

void insert_double(std::vector<Message>& messages, const int millisecondsToWait)
{
	std::thread::id corrent_thread = std::this_thread::get_id();
	double corrent_num = std::hash<std::thread::id>{}(std::this_thread::get_id());
	Message message = Message();
	while (corrent_num != 0)
	{
		if (corrent_num / (double)10 == 0)
		{
			std::lock_guard lck_names(mtx_double);
			messages.push_back(Message(corrent_thread, corrent_num, true));
		}
		else
		{
			std::lock_guard lck_names(mtx_double);
			messages.push_back(Message(corrent_thread, corrent_num, false));
		}
		corrent_num /= (double)10;

		std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToWait));
	}
}

void print_messages(std::vector<Message>& messages, const int millisecondsToWait)
{
	int finished_ctr = 0;
	bool there_is_a_new_message = false;
	Message message;

	while (finished_ctr < 2) 
	{
		{
			std::lock_guard lck_names(mtx_double);
			if (!messages.empty())
			{
				there_is_a_new_message = true;
				message = messages.back();
				messages.pop_back();
			}
		}
		if (there_is_a_new_message)
		{
			if (message.flag)
			{ 
				std::cout << message.thread << " finished" << std::endl;
				++finished_ctr;
			}
			else
				std::cout << message.thread << " sent: " << message.num << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToWait));
	}
}


int main()
{
	std::thread tOut(print_messages, ref(messages), 20);
	std::thread tIn1(insert_double, ref(messages), 30);
	std::thread tIn2(insert_double, ref(messages), 30);
	
	tIn1.join();
	tIn2.join();
	tOut.join();
	return 0;
}