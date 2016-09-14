#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Game.h"

class Engine {
public:
	void run();

private:
	sf::Time fps = sf::seconds(1.f/60.f);

	sf::RenderWindow window;

	Game game;

	sf::Font font;
	sf::Text endText;

	bool focus = false;
	bool stopGame = false;

	bool showEnd = false;

	void handleEvents();
	void update(sf::Time t);
	void render();

};

#endif