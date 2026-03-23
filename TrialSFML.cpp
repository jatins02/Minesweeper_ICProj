#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctime>
#include <vector>

const unsigned int WIDTH = 560;
const unsigned int HEIGHT = 690;

typedef enum { easy, mid, hard, none } difficulties;

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
		: content(font, text, 22){					// you can pass in the text string as well as the size of characters while initialising sf::Text

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




void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard, difficulties &difficulty_setting);

void draw_grid(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
				int box_size[], int grid_size[], int num_bombs[], int bomb_indices[], int button_clicked, int* revealed, int box_values[]);

int* get_bomb_indices(int num_of_bombs, int grid_size);
int check_if_present(int* arr, int num, int length);
int get_box_number(int bomb_indices[], int box_number, int rows, int num_bomb);
int get_value(int bomb_indices[], int neighbours[], int num_bombs, int num_of_neighbours);
int* get_neighbours(int box_number, int grid_size);

void handle_grid_click(int button_clicked, int *revealed, int* box_values, int grid_size);

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

	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ WIDTH, HEIGHT }), "minesweeper");
	window.setFramerateLimit(69);

	sf::Font font;
	if (!font.openFromFile("PressStart2P-Regular.ttf")) {
		std::cerr << "ERROR IN LOADING THE FONT: PressStart2P-Regular.ttf..." << std::endl;
	}

	sf::Text heading(font, "MINESWEEPER", 44);
	heading.setPosition({ WIDTH / 2.0f - (44*11/2), 100});


	sf::Text newthings(font, "guess who's back", 44);
	newthings.setPosition({ WIDTH / 2.0f - (44 * 15 / 2), 121 });

	difficulties difficulty_setting = none;

	// INSTANTIATE THE BUTTONS

	DifficultyBtn hardBtn("HARD", font, 600.0f);
	DifficultyBtn midBtn("MID", font, 450.0f);
	DifficultyBtn easyBtn("EASY", font, 300.0f);
	

	while (window.isOpen()) {
		while (std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}

			else if (auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {
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

						if (difficulty_setting != none) {
							int total_tiles = grid_size[difficulty_setting] * grid_size[difficulty_setting];
							bomb_indices = get_bomb_indices(num_bombs[difficulty_setting], grid_size[difficulty_setting]);
							
							//int total_tiles = grid_size[difficulty_setting] * grid_size[difficulty_setting];
							revealed = new int[total_tiles]();
							// getting box values in the old way that i have been doing...


							box_values = (int*)malloc((grid_size[difficulty_setting] * grid_size[difficulty_setting]) * sizeof(int));

							for (int i = 1; i <= grid_size[difficulty_setting]; i++) {
								for (int j = 1; j <= grid_size[difficulty_setting]; j++) {
									int box_number = (i - 1) * grid_size[difficulty_setting] + j;
									if (box_values != nullptr) {
										box_values[box_number - 1] = get_box_number(bomb_indices, box_number, grid_size[difficulty_setting], num_bombs[difficulty_setting]);
									}
									else {
										std::cout << "Error in inserting values to boxes..." << std::endl;
									}
								}
							}
						}
						
						
					}
					else {

						int p = padding[0];
						int ox = offset_x[difficulty_setting];
						int oy = offset_y[difficulty_setting];
						int bs = box_size[difficulty_setting];

						int col = ((mouse_x - ox) / (bs + p)) + 1;
						int row = ((mouse_y - oy) / (bs + p)) + 1;

						if (col >= 1 && col <= grid_size[difficulty_setting] && row >= 1 && row <= grid_size[difficulty_setting]) {
							button_clicked = (row - 1) * grid_size[difficulty_setting] + col;
							//revealed[button_clicked - 1] = 1;
							//std::cout << "box number, " << button_clicked << " was clicked" << std::endl;
						}

						handle_grid_click(button_clicked, revealed, box_values, grid_size[difficulty_setting]);

						mouse_x = -1;
						mouse_y = -1;




						// so revealed stores the boxes that havebeen opened in its memory:-
						// 
						//for (int i = 0; i < (grid_size[difficulty_setting] * grid_size[difficulty_setting]); i++) {
						//	std::cout << revealed[i] << " ";
						//}

					}
				}
			}
		}
	

		// render object

		window.clear(sf::Color::Magenta);

		// draw objects

		if (difficulty_setting == none) {
			window.draw(easyBtn);
			window.draw(midBtn);
			window.draw(hardBtn);

			window.draw(easyBtn.content);
			window.draw(midBtn.content);
			window.draw(hardBtn.content);

			window.draw(heading);
		}

		else {
			draw_grid(window, font, difficulty_setting, box_size, grid_size, num_bombs, bomb_indices, button_clicked, revealed, box_values);
		}
		
		window.display();

	}

	return 0;
}

/*
void handle_grid_click(int button_clicked, int* revealed, int* box_values, int grid_size) {
	// individual button clicks reach here, now you have the revealed arr


	//for (int i = 0; i < grid_size; i++) {
	//	for (int j = 0; j < grid_size; j++) {
	//		std::cout << revealed[grid_size * i + j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	std::cout << "box number: " << button_clicked << " box value: " << box_values[button_clicked - 1] << std::endl;

	if (revealed[button_clicked - 1] == 1) {
		std::cout << "box already clicked" << std::endl;
		return;
	}


	else {
		if (box_values[button_clicked - 1] == -1) {
			revealed[button_clicked - 1] = 1;
			std::cout << "you clicked on a bomb, GAME OVER..." << std::endl;
			return;
		}

		else if (box_values[button_clicked - 1] != 0) {
			revealed[button_clicked - 1] = 1;
			return;
		}

		else if (box_values[button_clicked - 1] == 0) {
			revealed[button_clicked - 1] = 1;

			// when zero is clicked, get its neighbours, then for each of the neighbours, check if they are a number, if they are, simply reveal them
			// if they are another zero, then call recursion, a zero HAS TO HAVE numbers in its surroundings, it CANNOT have a bomb in its surroudings.
			int neighbour_counter = 0;
			int* neighbours = get_neighbours(button_clicked, grid_size);
			int curr_box_num = neighbours[neighbour_counter];

			while (curr_box_num != -1) {

				handle_grid_click(curr_box_num, revealed, box_values, grid_size);

				//revealed[curr_box_num - 1] = 1;
				//if (box_values[curr_box_num - 1] == 0) {
				//	handle_grid_click(curr_box_num, revealed, box_values, grid_size);
				//}
				neighbour_counter++;

				//return;
			}
			//return;
		}

		else {
			std::cout << "handle_grid_click encountered an unaccounted for case..." << std::endl;
		}
	}

	

}
*/


void handle_grid_click(int button_clicked, int* revealed, int* box_values, int grid_size) {

	std::cout << "box number: " << button_clicked << " box value: " << box_values[button_clicked - 1] << std::endl;


	if (button_clicked > (grid_size * grid_size) || button_clicked < 1) {
		// tile outside of the boundary is clicked
		return;
	}

	if (box_values[button_clicked - 1] == -1 || revealed[button_clicked - 1] == 1) {
		return;			// for now account for both the cases, when tile opened or is a bomb in the same case
	}

	if (box_values[button_clicked - 1] >= 1 && box_values[button_clicked - 1] <= 8) {
		// tile is a valid number, reveal it
		revealed[button_clicked - 1] = 1;
	}

	if (box_values[button_clicked - 1] == 0){
		revealed[button_clicked - 1] = 1;

		int neighbour_counter = 0;
		int* neighbours = get_neighbours(button_clicked, grid_size);
		//int curr_box_num = neighbours[neighbour_counter];

		while (neighbours[neighbour_counter] != -1) {

			handle_grid_click(neighbours[neighbour_counter], revealed, box_values, grid_size);

			//revealed[curr_box_num - 1] = 1;
			//if (box_values[curr_box_num - 1] == 0) {
			//	handle_grid_click(curr_box_num, revealed, box_values, grid_size);
			//}
			neighbour_counter++;

			//return;
		}
		//return;
	}
	
}

int* get_neighbours(int box_number, int grid_size) {

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
		// start game in easy mode
	}
	else if (mid.getGlobalBounds().contains(mousePos)) {
		difficulty_setting = difficulties::mid;
		// start game in medium mode
	}
	else if (hard.getGlobalBounds().contains(mousePos)) {
		difficulty_setting = difficulties::hard;
		// start game in hard mode
	}
}


void draw_grid(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
	int box_size[], int grid_size[], int num_bombs[], int bomb_indices[], int button_clicked, int *revealed, int box_values[]) {				// pass by value will also work for the three matrices
	
	
	int padding[3] = { 3, 3, 3 };
	int offset_x[3] = { 7, 4, 9 };
	int offset_y[3] = { 126, 120, 136 };

	//int* box_values = (int*)malloc((grid_size[difficulty_setting] * grid_size[difficulty_setting]) * sizeof(int));

	for (int i = 1; i <= grid_size[difficulty_setting]; i++) {
		for (int j = 1; j <= grid_size[difficulty_setting]; j++) {

			int box_number = (i - 1) * grid_size[difficulty_setting] + j;
			//if (box_values != nullptr) {
			//	box_values[box_number - 1] = get_box_number(bomb_indices, box_number, grid_size[difficulty_setting], num_bombs[difficulty_setting]);
			//}
			//else {
			//	std::cout << "Error in inserting values to boxes..." << std::endl;
			//}
			sf::RectangleShape rect(sf::Vector2f(box_size[difficulty_setting], box_size[difficulty_setting]));

			rect.setPosition({ (float)box_size[difficulty_setting] * (j - 1) + padding[difficulty_setting] * j + offset_x[difficulty_setting],
							   (float)box_size[difficulty_setting] * (i - 1) + padding[difficulty_setting] * i + offset_y[difficulty_setting] });


			// if revealed at this tile number is 1, then color it something else...

			if (revealed[box_number - 1]) {
				rect.setFillColor(sf::Color::Black);
				//std::cout << "box number: " << box_number << " box value: " << box_values[box_number - 1] << std::endl;
			}
			
			window.draw(rect);

			// we could send in the mouse_x and mouse_y to this function itself and handle it within this function, within this loop.
		}
	}
}

int get_value(int bomb_indices[], int neighbours[], int num_bomb, int num_of_neighbours) {
	int value = 0;
	for (int i = 0; i < num_of_neighbours; i++) {
		if (check_if_present(bomb_indices, neighbours[i], num_bomb)) {
			value++;
		}
	}
	return value;
}

int get_box_number(int bomb_indices[], int box_number, int rows, int num_bomb) {
	int value = 0;

	if (check_if_present(bomb_indices, box_number, num_bomb)) {
		value = -1;
	}

	else {

		if (box_number % rows == 0) {
			// box is at the last of the row
			if (box_number == rows) {
				// top right box
				int neighbours[3] = { box_number - 1, box_number + rows, box_number + rows - 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 3);
			}
			else if (box_number == rows * rows) {
				// bottom right box
				int neighbours[3] = { box_number - 1, box_number - rows, box_number - rows - 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 3);
			}
			else {
				// remaining boxes in last column
				int neighbours[5] = { box_number - 1, box_number - rows, box_number - rows - 1, box_number + rows, box_number + rows - 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 5);
			}
		}

		else if (box_number % rows == 1) {
			// box is at the start of the row
			if (box_number == 1) {
				// top left box
				int neighbours[3] = { box_number + 1, box_number + rows, box_number + rows + 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 3);
			}
			else if (box_number == rows * (rows - 1) + 1) {
				// bottom left box
				int neighbours[3] = { box_number + 1, box_number - rows, box_number - rows + 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 3);
			}
			else {
				// remaining boxes in first column
				int neighbours[5] = { box_number + 1, box_number - rows, box_number - rows + 1, box_number + rows, box_number + rows + 1 };
				value = get_value(bomb_indices, neighbours, num_bomb, 5);
			}
		}

		else if (box_number >= 2 && box_number <= rows - 1) {
			int neighbours[5] = { box_number - 1, box_number + 1, box_number + rows - 1, box_number + rows, box_number + rows + 1 };
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else if (box_number >= 2 + (rows * (rows - 1)) && box_number <= rows * rows - 1) {
			int neighbours[5] = { box_number - 1, box_number + 1, box_number - rows - 1, box_number - rows, box_number - rows + 1 };
			value = get_value(bomb_indices, neighbours, num_bomb, 5);
		}
		else {
			// box is in some of the middle columns, account for the top and bottom rows, else good
			int neighbours[8] = { box_number - rows - 1, box_number - rows, box_number - rows - 1,
								  box_number - 1,                           box_number + 1,
								  box_number + rows - 1, box_number + rows, box_number + rows + 1 };
			value = get_value(bomb_indices, neighbours, num_bomb, 8);
		}
	}

	return value;
}


int* get_bomb_indices(int num_of_bombs, int grid_size) {
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

























