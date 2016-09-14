#ifndef GRIDPOSITION_H
#define GRIDPOSITION_H

#include <SFML/System.hpp>

class GridPosition : public sf::Vector2i {
public:
	GridPosition() : sf::Vector2i() {

	}
	GridPosition(int a, int b) : sf::Vector2i(a, b) {

	}

	GridPosition(GridPosition *a) {
		x = a->x;
		y = a->y;
	}

	friend inline bool operator< (const GridPosition& l, const GridPosition& r);
};
inline bool operator< (const GridPosition& l, const GridPosition& r) {
	if (l.x > r.x) {
		return true;
	}
	else if (l.x < r.x) {
		return false;
	}
	else {
		if (l.y > r.y) {
			return true;
		}
		else if (l.y < r.y) {
			return false;
		}
	}
	return false;
}

#endif