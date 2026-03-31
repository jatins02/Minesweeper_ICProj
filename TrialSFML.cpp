#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctime>
#include <string>  // to utilise the std::to_sting function

const unsigned int WIDTH = 560;
const unsigned int HEIGHT = 690;


class DifficultyBtn : public sf::RectangleShape {
private:
	const int BTN_WIDTH = 440;
	const int BTN_HEIGHT = 120;

public:
	sf::Text content;

	int get_BTN_WIDTH() {
		return BTN_WIDTH;
	}

	int get_BTN_HEIGHT() {
		return BTN_HEIGHT;
	}


	DifficultyBtn(std::string text, sf::Font &font, float yPos)			// get an error saying, no default constructor for the sf::Text class, this is because, you have to pass in the font to the text class as soon as the constructor is called.
		: content(font, text, 22){										// you can pass in the text string as well as the size of characters while initialising sf::Text

		this->setSize(sf::Vector2f(BTN_WIDTH, BTN_HEIGHT));
		this->setOrigin(this->getGeometricCenter());
		this->setFillColor(sf::Color(0xFFF0F5));
		this->setPosition({ WIDTH / 2.0f, yPos });

		content.setFillColor(sf::Color::Black);

		sf::FloatRect textbounds = content.getLocalBounds();
		content.setOrigin(textbounds.size / 2.0f);
		content.setPosition(this->getPosition());
	}

};


typedef enum { easy, mid, hard, none } difficulties;


// FUNCTION PROTOTYPES

void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard, difficulties &difficulty_setting);

void draw_grid(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
				int box_size[], int grid_size[], int num_bombs[], int bomb_indices[], int button_clicked, int* revealed, int box_values[]);

int* get_bomb_indices(int num_of_bombs, int grid_size);
int check_if_present(int* arr, int num, int length);
int get_box_value(int bomb_indices[], int box_number, int rows, int num_bomb, int grid_size);
int get_value(int bomb_indices[], int neighbours[], int num_bombs, int num_of_neighbours);
int* get_neighbours(int box_number, int grid_size);

void handle_grid_click(int button_clicked, int *revealed, int* box_values, int grid_size);
void revealtile(sf::Font& font, sf::RectangleShape& tile, std::string& text, sf::RenderWindow &window);
void handle_bomb_click(sf::RenderWindow &window, sf::Font &font, int &running);
int check_game_won(int *revealed, int *box_values, int grid_size);
void handle_game_won(sf::RenderWindow& window, sf::Font &font);


// MAIN AND GAME FUNCTIONS
int main() {

	int box_size[3] = {65, 52, 42};
	int grid_size[3] = {8, 10, 12};
	int num_bombs[3] = {6, 11, 15};
	int padding[3] = { 3, 3, 3 };
	int offset_x[3] = { 7, 4, 9 };
	int offset_y[3] = { 126, 120, 136 };
	int* bomb_indices = nullptr;
	int mouse_x = 0;
	int mouse_y = 0;
	int button_clicked = 0;
	int* box_values = nullptr;
	int* revealed = nullptr;

	int running = 1;
	int gameWon = 0;

	// SFML LIBRARY
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ WIDTH, HEIGHT }), "minesweeper");
	window.setFramerateLimit(69);

	sf::Font font;
	if (!font.openFromFile("PressStart2P-Regular.ttf")) {
		std::cerr << "ERROR IN LOADING THE FONT: PressStart2P-Regular.ttf..." << std::endl;
	}

	sf::Text heading(font, "MINESWEEPER", 44);
	heading.setPosition({ WIDTH / 2.0f - (44*11/2), 100});

	difficulties difficulty_setting = none;


	// INSTANTIATE THE BUTTONS
	DifficultyBtn hardBtn("HARD", font, 600.0f);
	DifficultyBtn midBtn("MID", font, 450.0f);
	DifficultyBtn easyBtn("EASY", font, 300.0f);
	

	// MAIN GAME WINDOW
	while (window.isOpen()) {
		while (std::optional event = window.pollEvent()) {				// cross is clicked
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}

			else if (auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {			// escape pressed
				if (keypressed->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
			}

			else if (const auto* mouseBtnPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseBtnPressed->button == sf::Mouse::Button::Left) {
					mouse_x = mouseBtnPressed->position.x;
					mouse_y = mouseBtnPressed->position.y;

					if (difficulty_setting == none) {
						setting_btn_clicked(mouse_x, mouse_y, easyBtn, midBtn, hardBtn, difficulty_setting);

						if (difficulty_setting != none) {		// difficulty not set yet
							int total_tiles = grid_size[difficulty_setting] * grid_size[difficulty_setting];
							bomb_indices = get_bomb_indices(num_bombs[difficulty_setting], grid_size[difficulty_setting]);
							
							revealed = new int[total_tiles]();

							box_values = (int*)malloc((grid_size[difficulty_setting] * grid_size[difficulty_setting]) * sizeof(int));

							for (int i = 1; i <= grid_size[difficulty_setting]; i++) {
								for (int j = 1; j <= grid_size[difficulty_setting]; j++) {
									int box_number = (i - 1) * grid_size[difficulty_setting] + j;
									if (box_values != nullptr) {
										box_values[box_number - 1] = get_box_value(bomb_indices, box_number, grid_size[difficulty_setting], num_bombs[difficulty_setting], grid_size[difficulty_setting]);
									}
									else {
										std::cout << "Error in inserting values to boxes..." << std::endl;
									}
								}
							}
						}
					}

					else {		// difficulty set, now render the grid

						int p = padding[0];
						int ox = offset_x[difficulty_setting];
						int oy = offset_y[difficulty_setting];
						int bs = box_size[difficulty_setting];

						int col = ((mouse_x - ox) / (bs + p)) + 1;
						int row = ((mouse_y - oy) / (bs + p)) + 1;


						// if the click registered is inside the grid, get the box number
						if (col >= 1 && col <= grid_size[difficulty_setting] && row >= 1 && row <= grid_size[difficulty_setting]) {
							button_clicked = (row - 1) * grid_size[difficulty_setting] + col;
						}

						// check if bomb at that box number
						if (*(box_values + button_clicked - 1) == -1) {
							handle_bomb_click(window, font, running);
						}

						else if (running) {
							handle_grid_click(button_clicked, revealed, box_values, grid_size[difficulty_setting]);
							gameWon = check_game_won(revealed, box_values, (grid_size[difficulty_setting] * grid_size[difficulty_setting]));
						}
						
						// to reset the mouse clicked coordinates
						mouse_x = -1;
						mouse_y = -1;
					}
					gameWon = check_game_won(revealed, box_values, (grid_size[difficulty_setting] * grid_size[difficulty_setting]));
				}
			}
		}

		
		// render object
		window.clear(sf::Color::Magenta);
		
		// draw objects
		if (running && !gameWon) {

			if (difficulty_setting == none) {			// when difficulty not set, draw difficulty setting buttons
				window.draw(easyBtn);
				window.draw(midBtn);
				window.draw(hardBtn);

				window.draw(easyBtn.content);
				window.draw(midBtn.content);
				window.draw(hardBtn.content);

				window.draw(heading);
			}

			else {				// when difficulty is set, draw the game grid				
				draw_grid(window, font, difficulty_setting, box_size, grid_size, num_bombs, bomb_indices, button_clicked, revealed, box_values);
			}
		}

		else if (gameWon){
			draw_grid(window, font, difficulty_setting, box_size, grid_size, num_bombs, bomb_indices, button_clicked, revealed, box_values);
			handle_game_won(window, font);
		}

		else {
			draw_grid(window, font, difficulty_setting, box_size, grid_size, num_bombs, bomb_indices, button_clicked, revealed, box_values);
			handle_bomb_click(window, font, running);
		}
			
		window.display();
	}

	return 0;
}


int check_game_won(int* revealed, int* box_values, int grid_size) {
	for (int i = 0; i < grid_size; i++) {
		if (box_values[i] != -1 && revealed[i] == 0) {
			std::cout << "tile " << i + 1 << " is not yet revealed" << std::endl;
			return 0;
		}
	}

	return 1;
}

void handle_game_won(sf::RenderWindow& window, sf::Font& font) {
	sf::RectangleShape win_label({ WIDTH, 150.f });
	win_label.setOrigin(win_label.getGeometricCenter());
	win_label.setPosition({ WIDTH / 2.0f, HEIGHT / 2.0f });
	win_label.setFillColor(sf::Color::Blue);

	sf::Text mssg(font, "YOU WIN...", 44);
	mssg.setFillColor(sf::Color::Green);

	sf::FloatRect textbounds = mssg.getLocalBounds();
	mssg.setOrigin(textbounds.size / 2.0f);

	sf::Vector2f posn = win_label.getPosition();
	mssg.setPosition({ posn.x, posn.y });

	window.draw(win_label);
	window.draw(mssg);

}

void handle_grid_click(int button_clicked, int* revealed, int* box_values, int grid_size) {

	if (button_clicked > (grid_size * grid_size) || button_clicked < 1) {
		// tile outside of the boundary is clicked
		return;
	}
	if (revealed[button_clicked - 1] == 1) {
		// tile that is already revealed is clicked
		return;
	}
	if (box_values[button_clicked - 1] == -1 ) {
		// tile clicked is a bomb
		return;			
	}

	if (box_values[button_clicked - 1] >= 1 && box_values[button_clicked - 1] <= 8) {
		// tile is a valid number, reveal it
		revealed[button_clicked - 1] = 1;
	}

	if (box_values[button_clicked - 1] == 0){
		revealed[button_clicked - 1] = 1;

		int neighbour_counter = 0;
		int* neighbours = get_neighbours(button_clicked, grid_size);

		while (neighbours[neighbour_counter] != -1) {

			handle_grid_click(neighbours[neighbour_counter], revealed, box_values, grid_size);
			neighbour_counter++;
		}
	}
}

int* get_neighbours(int box_number, int grid_size) {
	// this function returning a local array, this is dangerous, create an array, just before the function call then get inside this function
	// for now it kind of works...

	if (box_number == 1) { // top left box
		int neighbours[4] = { 0 };
		neighbours[0] = box_number + 1;
		neighbours[1] = box_number + grid_size;
		neighbours[2] = box_number + grid_size + 1;
		neighbours[3] = -1;

		return neighbours;
	}
	else if (box_number == grid_size) {  // top right
		int neighbours[4] = { 0 };
		neighbours[0] = box_number - 1;
		neighbours[1] = box_number + grid_size;
		neighbours[2] = box_number + grid_size - 1;
		neighbours[3] = -1;

		return neighbours;
	}
	else if (box_number == grid_size * (grid_size - 1) + 1) {		// bottom left
		int neighbours[4] = { 0 };
		neighbours[0] = box_number - grid_size;
		neighbours[1] = box_number - grid_size + 1;
		neighbours[2] = box_number + 1;
		neighbours[3] = -1;

		return neighbours;
	}
	else if (box_number == grid_size * grid_size) {		// bottom right
		int neighbours[4] = { 0 };
		neighbours[0] = box_number - 1;
		neighbours[1] = box_number - grid_size - 1;
		neighbours[2] = box_number - grid_size;
		neighbours[3] = -1;

		return neighbours;
	}
	else if (box_number % grid_size == 1) {		// first column, except the top and bottom
		int neighbours[6] = { 0 };
		neighbours[0] = box_number - grid_size;
		neighbours[1] = box_number - grid_size + 1;
		neighbours[2] = box_number + 1;
		neighbours[3] = box_number + grid_size;
		neighbours[4] = box_number + grid_size + 1;
		neighbours[5] = -1;

		return neighbours;
	}
	else if (box_number % grid_size == 0) {		// last column, except the top and bottom
		int neighbours[6] = { 0 };
		neighbours[0] = box_number - grid_size;
		neighbours[1] = box_number - grid_size - 1;
		neighbours[2] = box_number - 1;
		neighbours[3] = box_number + grid_size;
		neighbours[4] = box_number + grid_size - 1;
		neighbours[5] = -1;

		return neighbours;
	}
	else if (box_number > 1 && box_number < grid_size) {		// first row, except first and last
		int neighbours[6] = { 0 };
		neighbours[0] = box_number - 1;
		neighbours[1] = box_number + 1;
		neighbours[2] = box_number + grid_size - 1;
		neighbours[3] = box_number + grid_size;
		neighbours[4] = box_number + grid_size + 1;
		neighbours[5] = -1;

		return neighbours;
	}
	else if (box_number > (grid_size * (grid_size - 1) + 1) && box_number < (grid_size * grid_size)) {		// last row, except first and last
		int neighbours[6] = { 0 };
		neighbours[0] = box_number - grid_size;
		neighbours[1] = box_number - grid_size + 1;
		neighbours[2] = box_number + 1;
		neighbours[3] = box_number + grid_size;
		neighbours[4] = box_number + grid_size + 1;
		neighbours[5] = -1;

		return neighbours;
	}
	else {
		int neighbours[9] = { 0 };
		neighbours[0] = box_number - grid_size - 1;
		neighbours[1] = box_number - grid_size;
		neighbours[2] = box_number - grid_size + 1;
		neighbours[3] = box_number - 1;
		neighbours[4] = box_number + 1;
		neighbours[5] = box_number + grid_size - 1;
		neighbours[6] = box_number + grid_size;
		neighbours[7] = box_number + grid_size + 1;
		neighbours[8] = -1;

		return neighbours;
	}
}


void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard, difficulties &difficulty_setting) {

	sf::Vector2f mousePos((float)x, (float)y);

	if (easy.getGlobalBounds().contains(mousePos)) {
		difficulty_setting = difficulties::easy;
	}

	else if (mid.getGlobalBounds().contains(mousePos)) {
		difficulty_setting = difficulties::mid;
	}

	else if (hard.getGlobalBounds().contains(mousePos)) {
		difficulty_setting = difficulties::hard;
	}
}


void draw_grid(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
	int box_size[], int grid_size[], int num_bombs[], int bomb_indices[], int button_clicked, int *revealed, int box_values[]) {				// pass by value will also work for the three matrices
	
	int padding[3] = { 3, 3, 3 };
	int offset_x[3] = { 7, 4, 9 };
	int offset_y[3] = { 126, 120, 136 };
	sf::Text content(font, "", 2);

	for (int i = 1; i <= grid_size[difficulty_setting]; i++) {
		for (int j = 1; j <= grid_size[difficulty_setting]; j++) {

			int box_number = (i - 1) * grid_size[difficulty_setting] + j;

			sf::RectangleShape rect(sf::Vector2f(box_size[difficulty_setting], box_size[difficulty_setting]));

			rect.setPosition({ (float)box_size[difficulty_setting] * (j - 1) + padding[difficulty_setting] * j + offset_x[difficulty_setting],
							   (float)box_size[difficulty_setting] * (i - 1) + padding[difficulty_setting] * i + offset_y[difficulty_setting] });

			if (revealed[box_number - 1]) {
				// if the tile is revealed, draw the rect before the number
				std::string text = std::to_string(box_values[box_number - 1]);
				revealtile(font, rect, text, window);
			}
			else {
				window.draw(rect);
			}
		}
	}
}


void handle_bomb_click(sf::RenderWindow &window, sf::Font &font, int &running) {
	running = 0;
	
	sf::RectangleShape end_label({ WIDTH, 150.f});
	end_label.setOrigin(end_label.getGeometricCenter());
	end_label.setPosition({ WIDTH / 2.0f, HEIGHT / 2.0f });
	end_label.setFillColor(sf::Color::Blue);

	sf::Text mssg(font, "GAME OVER...", 44);

	mssg.setFillColor(sf::Color::Green);

	sf::FloatRect textbounds = mssg.getLocalBounds();
	mssg.setOrigin(textbounds.size / 2.0f);
	sf::Vector2f posn = end_label.getPosition();

	mssg.setPosition({ posn.x , posn.y });

	window.draw(end_label);		// not working rn
	window.draw(mssg);
}


// important logic:
// in the draw_grid loop, if the tile is revealed, we get into the revealtile function, wherein, we first draw the tile, then draw the number on top of it, so that it is visible.
// else if the tile is not revealed, we simply just draw the tile, without making any changes onto it.

void revealtile(sf::Font &font, sf::RectangleShape &tile, std::string &text, sf::RenderWindow &window) {

	tile.setFillColor(sf::Color::Black);
	window.draw(tile);

	if (text == "0") {
		return;
	}

	sf::Text content(font, text, 22);

	content.setFillColor(sf::Color::White);

	sf::FloatRect textbounds = content.getLocalBounds();
	content.setOrigin(textbounds.size / 2.0f);
	sf::Vector2f posn = tile.getPosition();

	content.setPosition({ posn.x + 22.0f, posn.y + 22.0f });

	window.draw(content);
}


// MISCELLANEOUS FUNCTION, (HELPERS)

int get_value(int bomb_indices[], int neighbours[], int num_bomb, int num_of_neighbours) {
	int value = 0;
	for (int i = 0; i < num_of_neighbours; i++) {
		if (check_if_present(bomb_indices, neighbours[i], num_bomb)) {
			value++;
		}
	}
	return value;
}


int get_box_value(int bomb_indices[], int box_number, int rows, int num_bomb, int grid_size) {
	int value = 0;

	if (check_if_present(bomb_indices, box_number, num_bomb)) {
		value = -1;
	}

	else {

		if (box_number == 1) { // top left box
			int neighbours[3] = { 0 };
			neighbours[0] = box_number + 1;
			neighbours[1] = box_number + grid_size;
			neighbours[2] = box_number + grid_size + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 3);
		}
		else if (box_number == grid_size) {  // top right
			int neighbours[3] = { 0 };
			neighbours[0] = box_number - 1;
			neighbours[1] = box_number + grid_size;
			neighbours[2] = box_number + grid_size - 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 3);
		}
		else if (box_number == grid_size * (grid_size - 1) + 1) {		// bottom left
			int neighbours[3] = { 0 };
			neighbours[0] = box_number - grid_size;
			neighbours[1] = box_number - grid_size + 1;
			neighbours[2] = box_number + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 3);
		}
		else if (box_number == grid_size * grid_size) {		// bottom right
			int neighbours[3] = { 0 };
			neighbours[0] = box_number - 1;
			neighbours[1] = box_number - grid_size - 1;
			neighbours[2] = box_number - grid_size;
			value = get_value(bomb_indices, neighbours, num_bomb, 3);
		}
		else if (box_number % grid_size == 1) {		// first column, except the top and bottom
			int neighbours[5] = { 0 };
			neighbours[0] = box_number - grid_size;
			neighbours[1] = box_number - grid_size + 1;
			neighbours[2] = box_number + 1;
			neighbours[3] = box_number + grid_size;
			neighbours[4] = box_number + grid_size + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else if (box_number % grid_size == 0) {		// last column, except the top and bottom
			int neighbours[5] = { 0 };
			neighbours[0] = box_number - grid_size;
			neighbours[1] = box_number - grid_size - 1;
			neighbours[2] = box_number - 1;
			neighbours[3] = box_number + grid_size;
			neighbours[4] = box_number + grid_size - 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else if (box_number > 1 && box_number < grid_size) {		// first row, except first and last
			int neighbours[5] = { 0 };
			neighbours[0] = box_number - 1;
			neighbours[1] = box_number + 1;
			neighbours[2] = box_number + grid_size - 1;
			neighbours[3] = box_number + grid_size;
			neighbours[4] = box_number + grid_size + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else if (box_number > (grid_size * (grid_size - 1) + 1) && box_number < (grid_size * grid_size)) {		// last row, except first and last
			int neighbours[5] = { 0 };
			neighbours[0] = box_number - grid_size;
			neighbours[1] = box_number - grid_size + 1;
			neighbours[2] = box_number + 1;
			neighbours[3] = box_number + grid_size;
			neighbours[4] = box_number + grid_size + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else {
			int neighbours[8] = { 0 };
			neighbours[0] = box_number - grid_size - 1;
			neighbours[1] = box_number - grid_size;
			neighbours[2] = box_number - grid_size + 1;
			neighbours[3] = box_number - 1;
			neighbours[4] = box_number + 1;
			neighbours[5] = box_number + grid_size - 1;
			neighbours[6] = box_number + grid_size;
			neighbours[7] = box_number + grid_size + 1;
			value = get_value(bomb_indices, neighbours, num_bomb, 8);
		}
	}

	return value;
}


int* get_bomb_indices(int num_of_bombs, int grid_size) {
	// here too, pointer to local array returned, local array will be erased from memory once function is done

	std::srand(std::time(nullptr));

	int* arr = (int*)malloc(num_of_bombs * sizeof(int));

	for (int i = 0; i < num_of_bombs; i++) {

		int random = std::rand() % (grid_size * grid_size);
		int is_present = check_if_present(arr, random, i + 1);

		while (is_present) {
			random = std::rand() % (grid_size * grid_size);
			is_present = check_if_present(arr, random, i + 1);
		}

		arr[i] = random;
	}
	return arr;
}

int check_if_present(int* arr, int num, int length) {
	for (int i = 0; i < length; i++) {
		if (arr[i] == num) {
			return 1;
		}
	}
	return 0;
}






