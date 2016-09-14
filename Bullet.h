#ifndef BULLET_H
#define BULLET_H

#include "Object.h"

class Bullet : public Object {
public:
	Bullet();

	void update(sf::Time t);

	int hitCounter = 0;

	//max aliens this bullet can kill before disappearing
	static const int hitMax = 3;

	void start(sf::Vector2f p, sf::Vector2f dir);

	bool isAlive = false;

private:
	float bulletSpeed = 800.f;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif