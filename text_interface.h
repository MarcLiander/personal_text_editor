#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

class Text_Interface {
	public:
		Text_Interface();
		void run();
		
	private:
		bool init_error;
		bool program_running;
		bool button_pressed;
	
		SDL_Window *gui_window;
		SDL_Surface *screen_surface;
		SDL_Surface *main_text_surface;
		SDL_Event ev;
		SDL_Rect text_rect;
		SDL_Rect cursor_rect;
		SDL_Color black;
		SDL_Color white;
		
		std::vector<std::string> array_of_text;
		std::string data_entry;
		TTF_Font *font;
		
		size_t window_width;
		size_t window_height;
		
		size_t char_width;
		size_t char_height;
		
		size_t grid_pos_x;
		size_t grid_pos_y;
		size_t filler_pos_y;
		size_t grid_limit_x;
		size_t grid_limit_y;
		
		unsigned int cursor_timer;
		
		void initialize_app();
		void main_loop();
		void cleanup_app();
		
		void save_file();
		void load_file();
		
		void check_event();
		void update_screen();
		
		void mouse_button_down();
		
		void pressed_backspace();
		void pressed_enter();
		void pressed_up();
		void pressed_down();
		void pressed_left();
		void pressed_right();
		
		void text_input();
		
		void adjust_lengths_of_text();
		void increase_vertical_pos();
		void correct_position();
		
};