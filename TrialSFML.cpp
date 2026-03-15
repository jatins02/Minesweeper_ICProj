#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

const unsigned int WIDTH = 560;
const unsigned int HEIGHT = 690;

typedef enum { none, easy, mid, hard } difficulties;

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

void main_game(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting);

int main() {

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
						std::cout << "current difficulty setting set to " << difficulty_setting << std::endl;
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
			// draw the game grid window
			//window.draw(newthings);

			main_game(window, font, difficulty_setting);
		}
		
		window.display();

	}

	return 0;
}


void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard, difficulties &difficulty_setting) {

	sf::Vector2f mousePos((float)x, (float)y);

	if (easy.getGlobalBounds().contains(mousePos)) {
		//std::cout << "easy was clicked" << std::endl;
		difficulty_setting = difficulties::easy;
		// start game in easy mode
	}
	else if (mid.getGlobalBounds().contains(mousePos)) {
		//std::cout << "mid was clicked" << std::endl;
		difficulty_setting = difficulties::mid;
		// start game in medium mode
	}
	else if (hard.getGlobalBounds().contains(mousePos)) {
		//std::cout << "hard was clicked" << std::endl;
		difficulty_setting = difficulties::hard;
		// start game in hard mode
	}
}


void main_game(sf::RenderWindow& window, sf::Font& font, difficulties difficulty_setting) {
	int box_size[3] = { 44, 28, 22 };
	int grid_size[3] = { 8, 10, 12 };
	int num_bombs[3] = { 6, 11, 15 };
	int offset[3] = { 2, 4, 5 };

	for (int i = 1; i <= grid_size[difficulty_setting - 1]; i++) {
		for (int j = 1; j <= grid_size[difficulty_setting - 1]; j++) {
			sf::RectangleShape rect(sf::Vector2f(box_size[difficulty_setting-1], box_size[difficulty_setting-1]));
			rect.setPosition({ (float)box_size[difficulty_setting-1] * (i - 1) + offset[difficulty_setting-1] * i, 
							   (float)box_size[difficulty_setting-1] * (j - 1) + offset[difficulty_setting-1] * j });
			window.draw(rect);
		}
	}
}

