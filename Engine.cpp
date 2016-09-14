#include "Engine.h"

#include <SFML/System.hpp>
#include <iostream>

#include "GameConstants.h"

void Engine::run() {

	font.loadFromFile("arial.ttf");

	endText.setFont(font);
	endText.setCharacterSize(72);
	endText.setPosition((WINDOWWIDTH/2) - 200, (WINDOWHEIGHT/2) - 100);

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	window.create(sf::VideoMode(WINDOWWIDTH, WINDOWHEIGHT, desktop.bitsPerPixel), "SFML Game", sf::Style::Default);


	sf::Time elapsed;
	sf::Clock clock;

	while (window.isOpen() && !stopGame) {
		handleEvents();

		focus = window.hasFocus();

		if (focus) {
			elapsed += clock.restart();

			if (elapsed >= fps) {
				
				//update
				mousePos = sf::Mouse::getPosition(window);

				if (game.win || game.lose)
					showEnd = true;

				update(elapsed);
				elapsed = sf::Time::Zero;
				render();

				sf::sleep(std::max(fps - elapsed - clock.getElapsedTime(), sf::Time::Zero));
			}


		}
		else {
			elapsed = sf::Time::Zero;
			clock.restart();
		}

	}
}

void Engine::update(sf::Time t) {
	if (!showEnd)
		game.update(t);
}

void Engine::render() {
	window.clear(sf::Color::Black);

	if (!showEnd) {
		window.draw(game);
	}
	else {
		if (game.win) {
			endText.setString("YOU WIN");
			endText.setColor(sf::Color::Green);
		}
		else if (game.lose) {
			endText.setString("YOU LOSE");
			endText.setColor(sf::Color::Red);
		}
		window.draw(endText);
	}

	window.display();
}

//capture user's input
void Engine::handleEvents() {


	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == event.LostFocus) {
			focus = false;
		}
		else if (event.type == event.KeyPressed && event.key.code == sf::Keyboard::Escape) {
			stopGame = true;
			window.close();
		}
		else if (event.type == sf::Event::Closed) {
			stopGame = true;
			window.close();
		}
	}
}