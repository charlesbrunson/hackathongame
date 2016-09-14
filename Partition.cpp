
#include "Partition.h"


void Partition::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	target.draw(d, states);

	if (DEBUG_FLOWFIELD) {
		if (distFromPlayer > 0) {
			sf::RectangleShape aV;
			aV.setFillColor(sf::Color::Blue);
			aV.setSize(sf::Vector2f(25.f, 2.f));
			aV.setPosition(sf::Vector2f(pos.x * AREASIZE, pos.y * AREASIZE) + sf::Vector2f(AREASIZE / 2, AREASIZE / 2));
			aV.setOrigin(1.f, 1.f);
			aV.setRotation(atan(alienPathVec.y / alienPathVec.x) * (180.f / PI));

			if (alienPathVec.x < 0) {
				aV.setRotation(aV.getRotation() + 180);
			}

			sf::CircleShape aD;
			aD.setFillColor(sf::Color::Blue);
			aD.setRadius(3.f);
			aD.setPosition(area.left + AREASIZE / 2, area.top + AREASIZE / 2);
			aD.setOrigin(3.f, 3.f);

			target.draw(aV, states);
			target.draw(aD, states);
		}
	}
}