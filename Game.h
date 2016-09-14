#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GridPosition.h"
#include "Partition.h"

#include "Object.h"
#include "Player.h"
#include "Alien.h"
#include "Hive.h"
#include "Bullet.h"

#define MAX_ALIENS 1024
#define MAX_BULLETS 128

class Game : public sf::Drawable {
public:
	Game();

	void update(sf::Time t);

	bool win = false;
	bool lose = false;

private:
	
	void addAlien(GridPosition p);
	void killAlien(Alien *a);

	void addBullet(sf::Vector2f pos, float ang);
	void removeBullet(Bullet* a);

	void moveObject(Object *obj, GridPosition p, GridPosition np);

	void alienCollision();

	std::map<GridPosition, Partition> gameArea;

	std::vector<Alien> aliens;
	std::vector<Hive> hives;
	std::vector<Bullet> bullets;
	int activeAliens = 0;
	int activeBullets = 0;

	sf::Vector2f areaCollision(sf::Vector2f p, float size, GridPosition g);

	Player plr;
	GridPosition plrGridPos;

	static void generateFlowField(std::map<GridPosition, Partition> &gameArea, GridPosition startPos);
	static bool isPathable(std::map<GridPosition, Partition> &gameArea, GridPosition p, int d);
	
	//couple of useful constants
	static const float sqrt2;
	static const float sin45;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};