#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <fstream>

#include "text_interface.h"

Text_Interface::Text_Interface() 
	: init_error(false),
	program_running(true),
	button_pressed(false),
	black({0x00, 0x00, 0x00, 0x00}),
	white({0xff, 0xff, 0xff, 0xff}),
	window_width(640),
	window_height(480),
	char_width(12),
	char_height(20),
	grid_pos_x(0),
	grid_pos_y(0),
	filler_pos_y(0),
	grid_limit_x((window_width / char_width) - 1),
	grid_limit_y((window_height / char_height)),
	cursor_timer(0)
	
{}

void Text_Interface::run() {
	initialize_app();
	
	if (init_error) {
		return;
		
	}

	main_loop();
	cleanup_app();
	
	return;
	
}

void Text_Interface::initialize_app() {
	gui_window = SDL_CreateWindow("Chibi Text Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		window_width, window_height, SDL_WINDOW_SHOWN);
	screen_surface = SDL_GetWindowSurface(gui_window);
	
	text_rect.x = 0;
	text_rect.y = 0;
	cursor_rect.x = 0;
	cursor_rect.y = 0;
	cursor_rect.w = 2;
	cursor_rect.h = 20;
	
	data_entry = "text";
	load_file();
	
	TTF_Init();
	font = TTF_OpenFont("Fonts/Anonymous.ttf", 16);
	
	if (font == NULL) {
		fprintf(stderr, "Error: font could not be opened.\n");
		init_error = true;
		
	} else {
		TTF_SetFontStyle(font, TTF_STYLE_BOLD);
		
	}
	
}

void Text_Interface::main_loop() {
	while (program_running) {
		check_event();
		update_screen();
		SDL_Delay(20);
		
	}
	
}

void Text_Interface::cleanup_app() {
	TTF_Quit();
	save_file();
	
}

void Text_Interface::save_file() {
	std::ofstream file;
	std::string file_name = data_entry + ".txt";
	file.open(file_name);
	
	if (!file.is_open()) {
		return;
		
	}

	size_t line_pos = 0;
	
	while (line_pos < array_of_text.size()) {
		file << array_of_text[line_pos];
		line_pos++;
		
	}
	
	file.close();
	
}

void Text_Interface::load_file() {
	std::ifstream file;
	std::string file_name = data_entry + ".txt";
	file.open(file_name);
	
	if (!file.is_open()) {
		return;
	
	}
	
	array_of_text.clear();
	
	if (file.peek() == std::ifstream::traits_type::eof()) {
		array_of_text.insert(array_of_text.begin(), "");
		
	} else {
		std::string text_from_file;
		size_t start_grid_pos = 0;
		
		while (std::getline(file, text_from_file)) {
			array_of_text.insert(array_of_text.begin() + start_grid_pos, text_from_file);
			
			while (array_of_text[start_grid_pos].length() >= grid_limit_x) {
				std::string desired_substr;
				
				if (array_of_text[start_grid_pos].rfind(" ", grid_limit_x) <= grid_limit_x) {
					desired_substr = array_of_text[start_grid_pos].substr(array_of_text[start_grid_pos].rfind(" ", grid_limit_x) + 1);
					
					if (start_grid_pos >= (array_of_text.size() - 1) || array_of_text[start_grid_pos][array_of_text[start_grid_pos].length() - 1] == '\n') {
						array_of_text.insert(array_of_text.begin() + (start_grid_pos + 1), "");
						
					}
					
					array_of_text[start_grid_pos].erase(array_of_text[start_grid_pos].begin() + array_of_text[start_grid_pos].rfind(" ", grid_limit_x) + 1, array_of_text[start_grid_pos].end());
					array_of_text[start_grid_pos + 1].insert(0, desired_substr);
					
				} else {
					desired_substr = array_of_text[start_grid_pos].substr(grid_limit_x - 1);
					
					if (start_grid_pos >= (array_of_text.size() - 1) || array_of_text[start_grid_pos][array_of_text[start_grid_pos].length() - 1] == '\n') {
						array_of_text.insert(array_of_text.begin() + (start_grid_pos + 1), "");
						
					}
					
					array_of_text[start_grid_pos].erase(array_of_text[start_grid_pos].begin() + (grid_limit_x - 1), array_of_text[start_grid_pos].end());
					array_of_text[start_grid_pos + 1].insert(0, desired_substr);
					
				}
				
				start_grid_pos++;
				
			}
			
			array_of_text[start_grid_pos] += "\n";
			start_grid_pos++;
			
		}
		
	}
	
	file.close();
	
	
}

void Text_Interface::check_event() {
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			program_running = false;
			break;
			
		} else if (ev.type == SDL_MOUSEBUTTONDOWN) {
			mouse_button_down();
			cursor_timer = 0;
			
		} else if (ev.type == SDL_KEYDOWN) {
			if (ev.key.keysym.sym == SDLK_ESCAPE) {
				program_running = false;
				break;
				
			} else if (ev.key.keysym.sym == SDLK_BACKSPACE) {
				pressed_backspace();
				
			} else if (ev.key.keysym.sym == SDLK_RETURN) {
				pressed_enter();
				
			} else if (ev.key.keysym.sym == SDLK_UP) {
				pressed_up();
				
			} else if (ev.key.keysym.sym == SDLK_DOWN) {
				pressed_down();
				
			} else if (ev.key.keysym.sym == SDLK_LEFT) {
				pressed_left();
				
			} else if (ev.key.keysym.sym == SDLK_RIGHT) {
				pressed_right();
				
			}
			
			cursor_timer = 0;
		} else if (ev.type == SDL_TEXTINPUT) {
			if (!(SDL_GetModState() & KMOD_CTRL && (ev.text.text[0] == 'c' || ev.text.text[0] == 'C' || ev.text.text[0] == 'v' || ev.text.text[0] == 'V'))) {
				text_input();
				
			}
			
			cursor_timer = 0;
		}
		
		
	}
	
}

void Text_Interface::update_screen() {
	SDL_FillRect(screen_surface, NULL, 0x000000);
	
	cursor_rect.x = (grid_pos_x * char_width);
	cursor_rect.y = ((grid_pos_y * char_height) - 3);
	
	if (cursor_timer < 100) {
		if (cursor_timer < 50)
			SDL_FillRect(screen_surface, &cursor_rect, 0xFFFFFF);
		cursor_timer++;
		
	} else {
		cursor_timer = 0;
		
	}
	
	if (array_of_text.size() > grid_limit_y) {
		for (size_t i = 0; i < grid_limit_y; i++) {
			text_rect.y = 20 * i;
			main_text_surface = TTF_RenderText_Blended(font, array_of_text[i + filler_pos_y].c_str(), white);			
			SDL_BlitSurface(main_text_surface, NULL, screen_surface, &text_rect);	
			
		}
		
	} else {
		for (size_t i = 0; i < array_of_text.size(); i++) {
			text_rect.y = 20 * i;
			main_text_surface = TTF_RenderText_Blended(font, array_of_text[i].c_str(), white);
			SDL_BlitSurface(main_text_surface, NULL, screen_surface, &text_rect);
			
		}
		
	}
	
	SDL_UpdateWindowSurface(gui_window);
	
}

void Text_Interface::mouse_button_down() {
	unsigned int click_x = ev.button.x;
	unsigned int click_y = ev.button.y;
	
	grid_pos_y = (click_y / char_height);
	
	if (grid_pos_y > (array_of_text.size() - 1)) {
		grid_pos_y = (array_of_text.size() - 1);
		
		if (grid_pos_y > (grid_limit_y - 1)) {
			grid_pos_y = (grid_limit_y - 1);
			
		}
		
	}
	
	grid_pos_x = (click_x / char_width);
	
	if (grid_pos_x > array_of_text[grid_pos_y + filler_pos_y].length()) {
		grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
		
		if (grid_pos_x > 0 && array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n') {
			grid_pos_x += -1;
			
		}
		
	}
	
}

void Text_Interface::pressed_backspace() {
	if (grid_pos_x > 0) {
		array_of_text[grid_pos_y + filler_pos_y].erase(grid_pos_x - 1, 1);
		grid_pos_x += -1;
		bool remove_filler_space = false;
		
		if (array_of_text.size() > (grid_pos_y + filler_pos_y + 1)) {
			size_t pos_of_next_line = grid_pos_y + filler_pos_y + 1;
			
			while (array_of_text.size() > pos_of_next_line) {
				size_t check_next_line;
				
				if (array_of_text[pos_of_next_line].find(" ") != std::string::npos) {
					check_next_line = array_of_text[pos_of_next_line].find(" ");
					
				} else {
					check_next_line = array_of_text[pos_of_next_line].length();
					
				}
				
				size_t check_current_line = grid_limit_x - array_of_text[pos_of_next_line - 1].length();
				
				if (check_next_line <= grid_limit_x && check_next_line < check_current_line && array_of_text[pos_of_next_line - 1].find('\n') == std::string::npos) {
					std::string desired_substr = array_of_text[pos_of_next_line].substr(0, check_next_line + 1);
					array_of_text[pos_of_next_line - 1] += desired_substr;
					
					if (check_next_line == array_of_text[pos_of_next_line].length()) {
						array_of_text.erase(array_of_text.begin() + pos_of_next_line);
						
						if (array_of_text.size() == grid_limit_y && filler_pos_y > 0) {
							remove_filler_space = true;
							
						}
						
					} else {
						array_of_text[pos_of_next_line].erase(0, check_next_line + 1);
						
					}
					
				} else {
					break;
					
				}
				
				pos_of_next_line++;
				
			}
			
		}
		
		if (remove_filler_space) {
			filler_pos_y += -1;
			grid_pos_y++;
			
		}
		
	} else if (grid_pos_y != 0) {
		if (array_of_text[grid_pos_y + filler_pos_y].length() > 0) {
			std::string desired_substr = array_of_text[grid_pos_y + filler_pos_y];
			array_of_text.erase(array_of_text.begin() + grid_pos_y + filler_pos_y);
			grid_pos_y += -1;
			grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
			
			if (array_of_text[grid_pos_y + filler_pos_y].length() > 0) {
				array_of_text[grid_pos_y + filler_pos_y].pop_back();
				grid_pos_x += -1;
				
			}
			
			array_of_text[grid_pos_y + filler_pos_y] += desired_substr;
			bool insert_new_line = false;
			
			if (array_of_text[grid_pos_y + filler_pos_y].length() > grid_limit_x) {
				adjust_lengths_of_text();
				
				if (grid_pos_x > grid_limit_x) {
					insert_new_line = true;
					
				}
				
			}
			
			if (insert_new_line) {
				increase_vertical_pos();
				grid_pos_x = 1;
				insert_new_line = false;
				
			}
			
		} else {
			array_of_text.erase(array_of_text.begin() + grid_pos_y + filler_pos_y);
			grid_pos_y += -1;
			grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
			
			if (grid_pos_x > 0 && (array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n' || array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == ' ')) {
				array_of_text[grid_pos_y + filler_pos_y].pop_back();
				grid_pos_x += -1;
				
			}
			
		}
		
		if (filler_pos_y > 0) {
			filler_pos_y += -1;
			grid_pos_y++;
			
		}
		
	} else if (grid_pos_y == 0 && filler_pos_y > 0) {
		if (array_of_text[grid_pos_y + filler_pos_y].length() > 0) {
			std::string desired_substr = array_of_text[grid_pos_y + filler_pos_y];
			array_of_text.erase(array_of_text.begin() + grid_pos_y + filler_pos_y);
			filler_pos_y += -1;
			grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
			
			if (array_of_text[grid_pos_y + filler_pos_y].length() > 0) {
				array_of_text[grid_pos_y + filler_pos_y].pop_back();
				grid_pos_x += -1;
				
			}
			
			array_of_text[grid_pos_y + filler_pos_y] += desired_substr;
			bool insert_new_line = false;
			if (array_of_text[grid_pos_y + filler_pos_y].length() > grid_limit_x) {
				adjust_lengths_of_text();
				
				if (grid_pos_x > grid_limit_x) {
					insert_new_line = true;
					
				}
				
			}
			
			if (insert_new_line) {
				increase_vertical_pos();
				grid_pos_x = 1;
				insert_new_line = false;
				
			}
			
		} else {
			array_of_text.erase(array_of_text.begin() + grid_pos_y + filler_pos_y);
			grid_pos_y += -1;
			grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
			
			if (grid_pos_x > 0 && (array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n' || array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == ' ')) {
				array_of_text[grid_pos_y + filler_pos_y].pop_back();
				grid_pos_x += -1;
				
			}
			
		}
		
	}
	
}

void Text_Interface::pressed_enter() {
	if (grid_pos_x < array_of_text[grid_pos_y + filler_pos_y].length()) {
		std::string desired_substr = array_of_text[grid_pos_y + filler_pos_y].substr(grid_pos_x);
		array_of_text[grid_pos_y + filler_pos_y].erase(array_of_text[grid_pos_y + filler_pos_y].begin() + grid_pos_x, array_of_text[grid_pos_y + filler_pos_y].end());
		array_of_text.insert(array_of_text.begin() + (grid_pos_y + filler_pos_y + 1), desired_substr);
		grid_pos_x = 0;
		array_of_text[grid_pos_y + filler_pos_y] += '\n';
		
	} else {
		array_of_text.insert(array_of_text.begin() + (grid_pos_y + filler_pos_y + 1), "");
		grid_pos_x = 0;
		array_of_text[grid_pos_y + filler_pos_y] += '\n';
		
	}
	
	increase_vertical_pos();
	
}

void Text_Interface::pressed_up() {
	if (grid_pos_y > 0) {
		grid_pos_y += -1;
		correct_position();
		
	} else if (filler_pos_y > 0) {
		filler_pos_y += -1;
		correct_position();
		
	}
	
}

void Text_Interface::pressed_down() {
	if (grid_pos_y < (grid_limit_y - 1) && grid_pos_y < (array_of_text.size() - 1)) {
		grid_pos_y++;
		correct_position();
		
	} else if (grid_pos_y >= (grid_limit_y - 1) && filler_pos_y < array_of_text.size() - grid_limit_y) {
		filler_pos_y++;
		correct_position();
		
	}
	
}

void Text_Interface::pressed_left() {
	if (grid_pos_x > 0) {
		grid_pos_x += -1;
		
	} else if (grid_pos_y > 0) {
		grid_pos_y += -1;
		grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
		
		if (array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n' || grid_pos_x > grid_limit_x) {
			grid_pos_x += -1;
			
		}
		
	} else if (filler_pos_y > 0) {
		filler_pos_y += -1;
		grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
		
		if (array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n' || grid_pos_x > grid_limit_x) {
			grid_pos_x += -1;
			
		}
		
	}
	
}

void Text_Interface::pressed_right() {
	if (grid_pos_x < array_of_text[grid_pos_y + filler_pos_y].length() && array_of_text[grid_pos_y + filler_pos_y][grid_pos_x] != '\n' && (grid_pos_x + 1) <= grid_limit_x) {
		grid_pos_x++;
		
	} else if (grid_pos_y < (grid_limit_y - 1) && grid_pos_y < (array_of_text.size() - 1)) {
		grid_pos_y++;
		grid_pos_x = 0;
		
	} else if (grid_pos_y >= (grid_limit_y - 1) && filler_pos_y < (array_of_text.size() - grid_limit_y)) {
		filler_pos_y++;
		grid_pos_x = 0;
		
	}
	
}

void Text_Interface::text_input() {
	size_t pos_of_cur_line = grid_pos_y + filler_pos_y;
	bool insert_new_line = false;
	array_of_text[pos_of_cur_line].insert(grid_pos_x, ev.text.text);
	grid_pos_x++;
	
	if (array_of_text[pos_of_cur_line].length() >= grid_limit_x) {
		adjust_lengths_of_text();
		
		if (grid_pos_x >= grid_limit_x) {
			grid_pos_x = 1;
			insert_new_line = true;
			
		}
		
	}
	
	if (insert_new_line) {
		increase_vertical_pos();
		insert_new_line = false;
		
	}
	
}

void Text_Interface::adjust_lengths_of_text() {
	size_t pos_of_cur_line = grid_pos_y + filler_pos_y;
	
	while (array_of_text[pos_of_cur_line].length() > grid_limit_x) {
		std::string desired_substr;
		
		if (array_of_text[pos_of_cur_line].rfind(" ", grid_limit_x) <= grid_limit_x) {
			desired_substr = array_of_text[pos_of_cur_line].substr(array_of_text[pos_of_cur_line].rfind(" ", grid_limit_x) + 1);
			
			if (pos_of_cur_line >= (array_of_text.size() - 1) || array_of_text[pos_of_cur_line][array_of_text[pos_of_cur_line].length() - 1] == '\n') {
				array_of_text.insert(array_of_text.begin() + (pos_of_cur_line + 1), "");
				
			}
			
			if (grid_pos_x > array_of_text[pos_of_cur_line].rfind(" ", grid_limit_x) && pos_of_cur_line == (grid_pos_y + filler_pos_y)) {
				increase_vertical_pos();
				grid_pos_x += -(array_of_text[pos_of_cur_line].rfind(" ", grid_limit_x) + 1);
				
			}
			
			array_of_text[pos_of_cur_line].erase(array_of_text[pos_of_cur_line].begin() + array_of_text[pos_of_cur_line].rfind(" ", grid_limit_x) + 1, array_of_text[pos_of_cur_line].end());
			array_of_text[pos_of_cur_line + 1].insert(0, desired_substr);
			
		} else {
			desired_substr = array_of_text[pos_of_cur_line].substr(grid_limit_x - 1);
			
			if (pos_of_cur_line >= (array_of_text.size() - 1) || array_of_text[pos_of_cur_line][array_of_text[pos_of_cur_line].length() - 1] == '\n') {
				array_of_text.insert(array_of_text.begin() + (pos_of_cur_line + 1), "");
				
			}
			
			array_of_text[pos_of_cur_line].erase(array_of_text[pos_of_cur_line].begin() + (grid_limit_x - 1), array_of_text[pos_of_cur_line].end());
			array_of_text[pos_of_cur_line + 1].insert(0, desired_substr);
			
		}
		
		pos_of_cur_line++;
		
	}
	
}

void Text_Interface::increase_vertical_pos() {
	if (array_of_text.size() > grid_limit_y || grid_pos_y >= (grid_limit_y - 1)) {
		filler_pos_y++;
		
	} else {
		grid_pos_y++;
		
	}
	
}

void Text_Interface::correct_position() {
	if (grid_pos_x > array_of_text[grid_pos_y + filler_pos_y].length()) {
		grid_pos_x = array_of_text[grid_pos_y + filler_pos_y].length();
		
	}
	
	if (grid_pos_x > 0 && array_of_text[grid_pos_y + filler_pos_y][grid_pos_x - 1] == '\n') {
		grid_pos_x += -1;
		
	}
	
}