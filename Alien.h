#ifndef ALIEN_H
#define ALIEN_H

#include "Object.h"
#include "GridPosition.h"

class Alien : public Object {
public:
	Alien();
	Alien(sf::Vector2f p);

	void setMove(sf::Vector2f d);

	void update(sf::Time t);

	void setVelocity(sf::Vector2f v);

	void start(sf::Vector2f p);

	bool isAlive = false;
	
protected:
	float moveSpeed = 50.f;
	float accSpeed = 100.f;
	sf::Vector2f acc;


	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif