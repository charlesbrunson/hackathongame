
#include "Hive.h"

Hive::Hive() : Object(sf::Vector2f(0.f, 0.f)) {
	size = 50.f;
}
Hive::Hive(sf::Vector2f p) : Object(p) {
	size = 50.f;
}

void Hive::update(sf::Time t) {
	spawnTimer += t;
	if (spawnTimer >= spawnCooldown) {
		spawnTimer -= spawnCooldown;
		aliensToSpawn++;
	}
}

void Hive::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::CircleShape d;
	d.setFillColor(sf::Color(128, 0, 128, 255));
	d.setRadius(size / 2.f);
	d.setPosition(position);

	d.setOutlineThickness(-1.f);
	d.setOutlineColor(sf::Color::Black);

	d.setOrigin(sf::Vector2f(size / 2, size / 2));

	target.draw(d);
}
