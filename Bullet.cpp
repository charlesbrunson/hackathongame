#include "Bullet.h"

Bullet::Bullet() : Object(sf::Vector2f(0,0)) {
	size = 15.f;
	type = BULLET;
}

void Bullet::update(sf::Time t) {
	position += velocity * t.asSeconds();
}

void Bullet::start(sf::Vector2f p, sf::Vector2f dir) {
	position = p;
	isAlive = true;
	hitCounter = 0;
	velocity = dir * bulletSpeed;
}

void Bullet::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::CircleShape d;
	d.setFillColor(sf::Color::Yellow);
	d.setRadius(size / 2.f);
	d.setPosition(position);

	d.setOutlineThickness(-1.f);
	d.setOutlineColor(sf::Color::Black);

	d.setOrigin(sf::Vector2f(size / 2, size / 2));

	target.draw(d);
}