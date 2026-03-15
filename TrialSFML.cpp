#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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


void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard);


int main() {

	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ WIDTH, HEIGHT }), "minesweeper");
	window.setFramerateLimit(69);

	sf::Font font;
	if (!font.openFromFile("PressStart2P-Regular.ttf")) {
		std::cerr << "ERROR IN LOADING THE FONT: PressStart2P-Regular.ttf..." << std::endl;
	}

	sf::Text heading(font, "MINESWEEPER", 44);
	heading.setPosition({ WIDTH / 2.0f - (44*11/2), 100});

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

					setting_btn_clicked(mouse_x, mouse_y, easyBtn, midBtn, hardBtn);

				}
			}
		}

		// render object

		window.clear(sf::Color::Magenta);

		// draw objects
		window.draw(easyBtn);
		window.draw(midBtn);
		window.draw(hardBtn);

		window.draw(easyBtn.content);
		window.draw(midBtn.content);
		window.draw(hardBtn.content);
		
		window.draw(heading);


		window.display();

	}

	return 0;
}


void setting_btn_clicked(int x, int y, DifficultyBtn &easy, DifficultyBtn &mid, DifficultyBtn &hard) {
	//int btn_width = easy.get_BTN_WIDTH();
	//int btn_height = easy.get_BTN_HEIGHT();


	//// bounds list contains, [top, bottom, left, right]

	//sf::FloatRect easy_bounds = easy.getGlobalBounds();
	//int easy_x = easy_bounds.position.x;
	//int easy_y = easy_bounds.position.y;
	//int easy_bounds_list[4] = { easy_y - btn_height / 2.0f,
	//						    easy_y + btn_height / 2.0f,
	//						    easy_x - btn_width / 2.0f,
	//						    easy_x + btn_width / 2.0f };

	//sf::FloatRect mid_bounds = mid.getGlobalBounds();
	//int mid_x = mid_bounds.position.x;
	//int mid_y = mid_bounds.position.y;
	//int mid_bounds_list[4] = { mid_y - btn_height / 2.0f,
	//						   mid_y + btn_height / 2.0f,
	//						   mid_x - btn_width / 2.0f,
	//						   mid_x + btn_width / 2.0f };

	//sf::FloatRect hard_bounds = hard.getGlobalBounds();
	//int hard_x = hard_bounds.position.x;
	//int hard_y = hard_bounds.position.y;
	//int hard_bounds_list[4] = { hard_y - btn_height / 2.0f,
	//						    hard_y + btn_height / 2.0f,
	//						    hard_x - btn_width / 2.0f,
	//						    hard_x + btn_width / 2.0f };


	//if (y >= easy_bounds_list[0] &&
	//	y <= easy_bounds_list[1] &&
	//	x >= easy_bounds_list[2] &&
	//	x <= easy_bounds_list[3]) {
	//	std::cout << "difficulty = easy";
	//}

	//else if (y >= mid_bounds_list[0] &&
	//	y <= mid_bounds_list[1] &&
	//	x >= mid_bounds_list[2] &&
	//	x <= mid_bounds_list[3]) {
	//	std::cout << "difficulty = mid";
	//}

	//else if (y >= hard_bounds_list[0] &&
	//	y <= hard_bounds_list[1] &&
	//	x >= hard_bounds_list[2] &&
	//	x <= hard_bounds_list[3]) {
	//	std::cout << "difficulty = hard";
	//}

	sf::Vector2f mousePos((float)x, (float)y);

	if (easy.getGlobalBounds().contains(mousePos)) {
		std::cout << "easy was clicked" << std::endl;
	}
	else if (mid.getGlobalBounds().contains(mousePos)) {
		std::cout << "mid was clicked" << std::endl;
	}
	else if (hard.getGlobalBounds().contains(mousePos)) {
		std::cout << "hard was clicked" << std::endl;
	}
}


