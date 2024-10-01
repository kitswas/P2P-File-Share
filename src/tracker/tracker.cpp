/**
 * @file tracker.cpp
 * @brief The entrypoint to the tracker.
 * @author Swastik Pal
 * @date 2024-10-1
 *
 * @copyright Copyright (c) 2024
 */

#include <iostream>

void loop()
{
	// int socket_fd = create_socket();

	while (true)
	{
		if (std::cin.peek() != EOF) // check if there is input
		{
			std::string input;
			std::cin >> input;
			std::cin.ignore(1000, '\n'); // ignore the rest of the line
			if (input == "quit")
			{
				break;
			}
		}
	}
}

/**
 * @brief Execution starts here
 */
int main()
{
	std::cout << "Hello from tracker!" << std::endl;
	loop();
	return 0;
}
