#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GridPosition.h"

class Object : public sf::Drawable {
public:
	Object(sf::Vector2f p) {
		position = p;
	}

	virtual void update(sf::Time t) = 0;

	void setPosition(sf::Vector2f pos) {
		position = pos;
	}

	sf::Vector2f getPosition() {
		return position;
	}

	float getSize() {
		return size;
	}

	GridPosition gPos;

	int type = 0;
	enum objType : int {
		OBJECT,
		PLAYER,
		ALIEN,
		BULLET,
		HIVE
	};


protected:

	float size = 30.f;

	sf::Vector2f position;
	sf::Vector2f velocity;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};

#endif