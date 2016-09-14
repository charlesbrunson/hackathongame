#include "Player.h"

#include <iostream>

#include "GameConstants.h"

void Player::update(sf::Time t) {

	//update position
	sf::Vector2f moveInput;

	moveInput.x -= (int)sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	moveInput.x += (int)sf::Keyboard::isKeyPressed(sf::Keyboard::D);

	moveInput.y -= (int)sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	moveInput.y += (int)sf::Keyboard::isKeyPressed(sf::Keyboard::S);

	if (pow(moveInput.x, 2) + pow(moveInput.y, 2) == 2) {
		moveInput *= 0.8509f;
	}

	velocity = moveInput * moveSpeed;

	position += (velocity * t.asSeconds());
	
	//update angle
	sf::Vector2f diff = sf::Vector2f(mousePos) - position;
	angle = atan2f(diff.y, diff.x);

	//update shooting
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (shootTimer <= sf::Time::Zero) {
			shoot();
			shootTimer = shootCooldown;
		}
		else {
			shootTimer -= t;
		}
	}
	else {
		shootTimer = std::max(sf::Time::Zero, shootTimer - shootCooldown);
	}

}

void Player::shoot() {

	toShoot t;
	t.pos = position;
	t.ang = angle;

	//apply bullet spray
	float r = ((float)(rand() % 1000)) / 1000.f;
	t.ang -= (shootVariation * PI / 180.f);
	t.ang += (r * 2 * shootVariation) * (PI / 180.f);

	bulletsShot.push_back(t);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::CircleShape d;
	d.setFillColor(sf::Color::Green);
	d.setRadius(size / 2.f);
	d.setPosition(position);

	d.setOutlineThickness(-1.f);
	d.setOutlineColor(sf::Color::Black);

	d.setOrigin(sf::Vector2f(size/2, size/2));

	target.draw(d);
}