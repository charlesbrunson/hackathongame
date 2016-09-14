#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Object.h"

class Player : public Object {
public:
	Player () : Object(sf::Vector2f(0.f, 0.f)) {
		type = PLAYER;
	}

	void update(sf::Time t);

	struct toShoot {
		sf::Vector2f pos;
		float ang;
	};
	std::vector<toShoot> bulletsShot;

	int health = 1;

protected:

	void shoot();
	
	const sf::Time shootCooldown = sf::milliseconds(75);
	sf::Time shootTimer;

	const float shootVariation = 10.f;

	float angle = 0.f;
	float moveSpeed = 200.f;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};

#endif