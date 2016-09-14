#ifndef HIVE_H
#define HIVE_H

#include "Object.h"

#include "GridPosition.h"

class Hive : public Object {
public:
	Hive();
	Hive(sf::Vector2f p);

	int health = 10;
	
	void update(sf::Time t);
	
	int aliensToSpawn = 0;

private:

	sf::Time spawnCooldown = sf::milliseconds(250);
	sf::Time spawnTimer;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};

#endif