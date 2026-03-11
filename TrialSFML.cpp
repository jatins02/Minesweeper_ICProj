#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


int main() {

	const unsigned int WIDTH = 690;
	const unsigned int HEIGHT = 440;

	sf::ConvexShape convex;
	convex.setPointCount(6);		// to make irregular shapes, now you can set the position of individual points of the shape.

	convex.setPoint(0, { 13.0f, 17.0f });		// setpoint takes in the arguments, the index (number) of the point and the coordinates to where it will be placed.
	convex.setPoint(1, { 3.5f, 1.6f });
	convex.setPoint(2, { 0.25f, -12.0f });
	convex.setPoint(0, { -12.0f, -7.3f });
	convex.setPoint(0, { -12.5f, -1.6f });
	convex.setPoint(0, { -5.0f, 7.5f });
	convex.setOrigin(convex.getGeometricCenter());
	convex.setFillColor(sf::Color(0x3F00FFFF));
	convex.setPosition({ WIDTH / 2.0f, HEIGHT / 2.0f });



	sf::RenderWindow window = sf::RenderWindow(
		sf::VideoMode({	WIDTH, HEIGHT }), "hehehe, this is the title");
	window.setFramerateLimit(69);

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event-> is<sf::Event::Closed>()) {
				window.close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
			}
		}


		//render objects


		window.clear();


		//draw objects
		window.draw(convex);


		window.display();
	}

	return 0;
}