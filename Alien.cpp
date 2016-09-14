#include "Alien.h"

Alien::Alien() : Object(sf::Vector2f(0.f, 0.f)) {
	size = 20.f;
	type = ALIEN;
}

Alien::Alien(sf::Vector2f p) : Object(p) {
	size = 20.f;
	type = ALIEN;
}

void Alien::setMove(sf::Vector2f d) {
	acc = d * accSpeed;
}

void Alien::update(sf::Time t) {
	velocity += acc * t.asSeconds();

	//cap speed
	float d = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
	if (d > moveSpeed){
		velocity = (velocity / d) * moveSpeed;
	}

	position += velocity * t.asSeconds();
}

void Alien::setVelocity(sf::Vector2f v) {
	velocity = v;
}

void Alien::start(sf::Vector2f p) {
	position = p;
	isAlive = true;
}
void Alien::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::CircleShape d;
	d.setFillColor(sf::Color::Red);
	d.setRadius(size / 2.f);
	d.setPosition(position);

	d.setOutlineThickness(-1.f);
	d.setOutlineColor(sf::Color::Black);

	d.setOrigin(sf::Vector2f(size / 2, size / 2));

	target.draw(d);
}
