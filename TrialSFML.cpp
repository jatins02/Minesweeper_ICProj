#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctime>

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

void main_game(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
				int box_size[], int grid_size[], int num_bombs[]);

int* get_bomb_indices(int num_of_bombs, int grid_size);
int check_if_present(int* arr, int num, int length);


int main() {

	int box_size[3] = {65, 52, 42};
	int grid_size[3] = {8, 10, 12};
	int num_bombs[3] = {6, 11, 15};


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
				if (keypressed->scancode == sf::Keyboard::Scancode::Escape){
					window.close();
				}
			}

			else if (const auto* mouseBtnPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouseBtnPressed->button == sf::Mouse::Button::Left) {
					int mouse_x = mouseBtnPressed->position.x;
					int mouse_y = mouseBtnPressed->position.y;

					if (difficulty_setting == none) {
						setting_btn_clicked(mouse_x, mouse_y, easyBtn, midBtn, hardBtn, difficulty_setting);

						int* bomb_indices = (int*)malloc(num_bombs[difficulty_setting] * sizeof(int));
						bomb_indices = get_bomb_indices(num_bombs[difficulty_setting], grid_size[difficulty_setting]);
						
						//for (int i = 0; i < num_bombs[difficulty_setting]; i++) {
						//	std::cout << *(bomb_indices + i) << " ";
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

			main_game(window, font, difficulty_setting, box_size, grid_size, num_bombs);
		}
		
		window.display();

	}

	return 0;
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


void main_game(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting,
				int box_size[], int grid_size[], int num_bombs[]) {				// pass by value will also work for the three matrices
	int padding[3] = { 3, 3, 3 };
	int offset_x[3] = {7, 4, 9};
	int offset_y[3] = {126, 120, 136};

	for (int i = 1; i <= grid_size[difficulty_setting]; i++) {
		for (int j = 1; j <= grid_size[difficulty_setting]; j++) {

			sf::RectangleShape rect(sf::Vector2f(box_size[difficulty_setting], box_size[difficulty_setting]));

			rect.setPosition({ (float)box_size[difficulty_setting] * (i - 1) + padding[difficulty_setting] * i + offset_x[difficulty_setting],
							   (float)box_size[difficulty_setting] * (j - 1) + padding[difficulty_setting] * j + offset_y[difficulty_setting]});
			
			window.draw(rect);
		}
	}
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

























