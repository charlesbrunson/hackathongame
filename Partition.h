#ifndef PARTITION
#define PARTITION

#include <assert.h>
#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GridPosition.h"

#include "GameConstants.h"

#include "Alien.h"
#include "Bullet.h"

class Partition : public sf::Drawable {
public:
	Partition() {
		for (int i = 0; i < 8; i++) {
			neighbors[i] = nullptr;
		}
		isSolid = false;
	}

	void init() {		
		if (isSolid)
			d.setFillColor(sf::Color::Black);
		else {
			if (((pos.x + pos.y) % 2) == 0)
				d.setFillColor(sf::Color(255, 255, 255, 255));
			else
				d.setFillColor(sf::Color(200, 200, 200, 255));
		}
		
		d.setSize(sf::Vector2f(area.width, area.height));
		d.setPosition(area.left, area.top);
	}

	enum Dir : int {
		N,
		NE,
		E,
		SE,
		S,
		SW,
		W,
		NW
	};
	
	sf::FloatRect area;
	GridPosition pos;

	void setMoveVec(sf::Vector2f v) {
		alienPathVec = v;


		if (DEBUG_FLOWFIELD) {
			aV.setFillColor(sf::Color::Blue);
			aV.setSize(sf::Vector2f(25.f, 2.f));
			aV.setPosition(sf::Vector2f(pos.x * AREASIZE, pos.y * AREASIZE) + sf::Vector2f(AREASIZE / 2, AREASIZE / 2));
			aV.setOrigin(1.f, 1.f);
			aV.setRotation(atan(alienPathVec.y / alienPathVec.x) * (180.f / PI));

			if (alienPathVec.x < 0) {
				aV.setRotation(aV.getRotation() + 180);
			}
		}
	}

	sf::Vector2f getMoveVec() {
		return alienPathVec;
	}

	float distFromPlayer = -1.f;

	bool getSolid() {
		return isSolid;
	}
	void setSolid(bool s) {
		isSolid = s;
	}

	Partition* getNeighbor(int d) {
		assert(d >= 0 && d <= 7);
		
		return neighbors[d];
	}
	void setNeighbor(int d, Partition* p) {
		assert(d >= 0 && d <= 7);

		neighbors[d] = p;
	}

	void setColor(sf::Color co) {
		if (DEBUG_FLOWFIELD) {
			d.setFillColor(co);
		}
	}

	bool complete = false;

	std::vector<Alien*>* getAliens() {
		return &aliens;
	}
	std::vector<Bullet*>* getBullets() {
		return &bullets;
	}

protected:

	sf::Vector2f alienPathVec;

	bool isSolid = false;

	Partition* neighbors[8];

	std::vector<Alien*> aliens;
	std::vector<Bullet*> bullets;
	
	sf::RectangleShape d;
	sf::RectangleShape aV;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;


};

#endif
