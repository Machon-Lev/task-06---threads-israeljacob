#include<iostream>
#include<vector>
#include<mutex>
#include"Message.h"

std::vector<Message> messages;
std::mutex mtx_double;


 //Inserts messages that contain double values until it reaches 0.
void insert_double()
{
    std::thread::id corrent_thread;
    double corrent_num = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Message message = Message();

    // Loop until the current number becomes zero
    while (corrent_num != 0)
    {
        // Check if the current digit is the last digit
        if (corrent_num / (double)10 == 0)
        {
            std::lock_guard lck_names(mtx_double);
            messages.push_back(Message(corrent_thread, corrent_num, true)); // Create a new "finished" message and add it to the vector
        }
        else
        {
            std::lock_guard lck_names(mtx_double);
            messages.push_back(Message(corrent_thread, corrent_num, false)); // Create a new message with a sent value and add it to the vector
        }
        corrent_num /= (double)10;

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Wait for 30 milliseconds
    }
}


 //Prints all the messages stored in the vector.
void print_messages()
{
    int finished_ctr = 0;
    bool there_is_a_new_message = false;
    Message message;

    // Continue until at least one thread did not reach 0 yet
    while (finished_ctr < 2)
    {
        {
            std::lock_guard lck_names(mtx_double);

            // Check if the messages vector is not empty
            if (!messages.empty())
            {
                there_is_a_new_message = true;
                message = messages.back(); // Retrieve the last message from the vector
                messages.pop_back(); // Remove the message from the vector
            }
        }

        // Check if there is a new message available
        if (there_is_a_new_message)
        {
            // Check if the message represents a "finished" status
            if (message.flag)
            {
                std::cout << message.thread << " finished" << std::endl; // Print the thread ID followed by "finished"
                ++finished_ctr; // Increment the counter of "finished" messages
            }
            else
                std::cout << message.thread << " sent: " << message.num << std::endl; // Print the thread ID and the sent value
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Wait for 20 milliseconds
    }
}

int main()
{
    // Create threads for printing messages and inserting double values
    std::thread tOut(print_messages);
    std::thread tIn1(insert_double);
    std::thread tIn2(insert_double);

    // Wait for the threads to finish
    tIn1.join();
    tIn2.join();
    tOut.join();
    return 0;
}
