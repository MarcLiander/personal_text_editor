#define SDL_MAIN_HANDLED
#include <iostream>
#include "text_interface.h"

int main() {
	Text_Interface app;
	
	try {
		app.run();
	}
	catch (const std::exception &er) {
		std::cerr << er.what() << "\n";
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}