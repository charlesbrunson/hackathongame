#include <iostream>

#include "Game.h"

#include "GameConstants.h"

#include <list>

#include <thread>


const float Game::sqrt2 = sqrtf(2);
const float Game::sin45 = sin(PI/4.f);

Game::Game() {
	GridPosition p;

	sf::Vector2i gridSize(WINDOWWIDTH / AREASIZE, WINDOWHEIGHT / AREASIZE);

	//initialize partition grid
	for (int x = 0; x < gridSize.x; x++) {
		for (int y = 0; y < gridSize.y; y++) {
			p.x = x;
			p.y = y;

			gameArea.insert(std::pair<GridPosition, Partition>(p, Partition()));
			gameArea[p].area = sf::FloatRect(x * AREASIZE, y * AREASIZE, AREASIZE, AREASIZE);
			gameArea[p].pos = p;

			//randomize solidity on non-edge partitions
			if (x > 0 && x < gridSize.x - 1 && y > 0 && y < gridSize.y - 1)
				gameArea[p].setSolid((rand() % 4) == 0);

			gameArea[p].init();
		}
	}

	//link gridarea
	for (int j = 0; j < gridSize.x; j++) {
		for (int k = 0; k < gridSize.y; k++) {
			p.x = j;
			p.y = k;
			
			//cardinal
			//North
			if (p.y - 1 >= 0) {
				gameArea[p].setNeighbor(Partition::N, &gameArea[GridPosition(p.x, p.y - 1)]);

				//diagonals
				if (p.x - 1 >= 0) {
					gameArea[p].setNeighbor(Partition::NW, &gameArea[GridPosition(p.x - 1, p.y - 1)]);
				}
				if (p.x + 1 < gridSize.x) {
					gameArea[p].setNeighbor(Partition::NE, &gameArea[GridPosition(p.x + 1, p.y - 1)]);
				}
			}
			//South
			if (p.y + 1 < gridSize.y) {
				gameArea[p].setNeighbor(Partition::S, &gameArea[GridPosition(p.x, p.y + 1)]);

				//diagonals
				if (p.x - 1 >= 0) {
					gameArea[p].setNeighbor(Partition::SW, &gameArea[GridPosition(p.x - 1, p.y + 1)]);
				}
				if (p.x + 1 < gridSize.x) {
					gameArea[p].setNeighbor(Partition::SE, &gameArea[GridPosition(p.x + 1, p.y + 1)]);
				}
			}
			//West
			if (p.x - 1 >= 0) {
				gameArea[p].setNeighbor(Partition::W, &gameArea[GridPosition(p.x - 1, p.y)]);
			}
			//East
			if (p.x + 1 < gridSize.x) {
				gameArea[p].setNeighbor(Partition::E, &gameArea[GridPosition(p.x + 1, p.y)]);
			}
		}
	}

	//place player randomly
	GridPosition plrPos;
	do {
		plrPos.x = rand() % gridSize.x;
		plrPos.y = rand() % gridSize.y;
	} while (gameArea[plrPos].getSolid());
	plr.setPosition(sf::Vector2f(plrPos * AREASIZE) + sf::Vector2f(AREASIZE / 2, AREASIZE / 2));

	//create flow field
	generateFlowField(gameArea, plrGridPos);

	//initialize alien stack
	for (int i = 0; i < MAX_ALIENS; i++) {
		aliens.push_back(Alien());
	}
	//initialize bullet stack
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets.push_back(Bullet());
	}

	//make some hives
	for (int i = 0; i < 8; i++) {

		//randomize placement
		GridPosition aPos;
		do {
			aPos.x = rand() % gridSize.x;
			aPos.y = rand() % gridSize.y;
		} while (gameArea[aPos].getSolid() || aPos == plrGridPos || gameArea[aPos].distFromPlayer < 10);

		hives.push_back(Hive(sf::Vector2f(aPos * AREASIZE) + sf::Vector2f(AREASIZE / 2, AREASIZE / 2)));
		hives.back().gPos = aPos;
	}
}


void Game::update(sf::Time t) {
	
	//update player
	{
		plr.update(t);

		while (!plr.bulletsShot.empty()) {
			addBullet(plr.bulletsShot.back().pos, plr.bulletsShot.back().ang);
			plr.bulletsShot.pop_back();
		}

		//keep player from leaving area
		sf::Vector2f nP = plr.getPosition();
		float r = plr.getSize() / 2;
		nP.x = std::max(r, std::min(nP.x, WINDOWWIDTH - r));
		nP.y = std::max(r, std::min(nP.y, WINDOWHEIGHT - r));

		plr.setPosition(nP);

		//do player collision
		plr.setPosition(areaCollision(plr.getPosition(), plr.getSize(), plrGridPos));

		//update player grid pos
		GridPosition temp;
		temp.x = std::floor(plr.getPosition().x / AREASIZE);
		temp.y = std::floor(plr.getPosition().y / AREASIZE);
		if (temp != plrGridPos) {
			plrGridPos = temp;
			generateFlowField(gameArea, plrGridPos);
		}
		else {
			plrGridPos = temp;
		}
	}

	//update hives
	for (auto h = hives.begin(); h != hives.end(); h++) {
		h->update(t);
		while (h->aliensToSpawn > 0) {

			if (activeAliens < MAX_ALIENS - 1) {
				addAlien(h->gPos);
			}

			h->aliensToSpawn = 0;
		}
	}

	//update aliens
	for (int i = 0; i < activeAliens; i++) {
		//update
		aliens.at(i).update(t);

		//update gridarea
		GridPosition temp;
		temp.x = std::floor(aliens.at(i).getPosition().x / AREASIZE);
		temp.y = std::floor(aliens.at(i).getPosition().y / AREASIZE);
		if (temp != aliens.at(i).gPos) {
			moveObject(&aliens.at(i), aliens.at(i).gPos, temp);
		}

		//determine alien move dir
		if (abs(aliens.at(i).gPos.x - plrGridPos.x) <= 1 && abs(aliens.at(i).gPos.y - plrGridPos.y) <= 1) {
			//close enough to go directly towards player
			sf::Vector2f diff = plr.getPosition() - aliens.at(i).getPosition();
			float ang = atan2f(diff.y, diff.x);
			aliens.at(i).setMove(sf::Vector2f(cos(ang), sin(ang)));

			//check if alien has touched player
			float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);
			if (!PLAYER_GODMODE && dist < (plr.getSize() / 2) + (aliens.at(i).getSize() / 2)) {
				killAlien(&aliens.at(i));
				plr.health--;

				if (plr.health <= 0) {
					lose = true;
				}

			}
		}
		else {
			aliens.at(i).setMove(gameArea[aliens.at(i).gPos].getMoveVec());
		}
	}

	//alien to alien collision
	alienCollision();

	//do alien collision to level
	for (int i = 0; i < activeAliens; i++) {

		//keep alien from leaving area
		sf::Vector2f nP = aliens.at(i).getPosition();
		float r = aliens.at(i).getSize() / 2;
		nP.x = std::max(r, std::min(nP.x, WINDOWWIDTH - r));
		nP.y = std::max(r, std::min(nP.y, WINDOWHEIGHT - r));
		aliens.at(i).setPosition(nP);
		
		//do level collision
		aliens.at(i).setPosition(areaCollision(aliens.at(i).getPosition(), aliens.at(i).getSize(), aliens.at(i).gPos));
	}

	//update bullets
	for (int i = 0; i < activeBullets; i++) {
		Bullet *b = &bullets.at(i);
		b->update(t);
		
		//keep bullet from leaving area
		sf::Vector2f nP = b->getPosition();
		float r = b->getSize() / 2;
		nP.x = std::max(r, std::min(nP.x, WINDOWWIDTH - r));
		nP.y = std::max(r, std::min(nP.y, WINDOWHEIGHT - r));

		sf::Vector2f postCol = areaCollision(b->getPosition(), b->getSize(), b->gPos);

		//update gridarea
		GridPosition temp;
		temp.x = std::floor(b->getPosition().x / AREASIZE);
		temp.y = std::floor(b->getPosition().y / AREASIZE);
		if (temp != b->gPos) {
			moveObject(&(*b), b->gPos, temp);
		}

		bool remove = false;

		//test collision with hives
		for (auto h = hives.begin(); h != hives.end(); h++) {

			sf::Vector2f diff = b->getPosition() - h->getPosition();
			float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);

			float prefDist = (b->getSize() / 2) + (h->getSize() / 2);
			if (dist < prefDist) {
				h->health--;
				if (h->health <= 0) {

					hives.erase(h);

				}

				remove = true;

				break;

			}
		}

		//bullet hit level
		if (nP != b->getPosition() || postCol != b->getPosition()) {
			remove = true;
		}

		if (remove) {
			removeBullet(&*b);
			b--;
		}
	}

	if (activeAliens == 0 && hives.empty()) {
		win = true;
	}
};


/*---------------------------------------------------------------------------------------------------------------*/
/* OBJECT MANAGEMENT --------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/

void Game::addAlien(GridPosition p) {
	if (activeAliens < MAX_ALIENS) {
		aliens[activeAliens].start(sf::Vector2f(p * AREASIZE) + sf::Vector2f(AREASIZE / 2, AREASIZE / 2));
		aliens[activeAliens].gPos = p;
		gameArea[p].getAliens()->push_back(&aliens[activeAliens]);
		aliens[activeAliens].setVelocity(sf::Vector2f(rand() % 100, rand() % 100));
		activeAliens++;
	}
}

void Game::killAlien(Alien *a) {
	bool check = false;
	for (auto alien = aliens.begin(); alien != aliens.end(); alien++) {
		if (&(*alien) == a) {
			check = true;
			alien->isAlive = false;

			//remove from gridspace
			for (auto a2 = gameArea[alien->gPos].getAliens()->begin(); a2 != gameArea[alien->gPos].getAliens()->end(); a2++) {
				if (a == *a2) {
					gameArea[alien->gPos].getAliens()->erase(a2);
					break;
				}
			}

			//move to end
			if (alien + 1 < aliens.end()) {
				std::rotate(alien, alien + 1, aliens.end());
			}
			
			activeAliens--;
		}
	}
	assert(check);
}

void Game::addBullet(sf::Vector2f pos, float ang) {
	if (activeBullets < MAX_BULLETS) {
		GridPosition p;
		p.x = std::floor(pos.x / AREASIZE);
		p.y = std::floor(pos.y / AREASIZE);

		bullets[activeBullets].start(pos, sf::Vector2f(cos(ang), sin(ang)));
		bullets[activeBullets].gPos = p;
		gameArea[p].getBullets()->push_back(&bullets[activeBullets]);
		activeBullets++;
	}
}

void Game::removeBullet(Bullet* a) {
	bool check = false;
	for (auto bullet = bullets.begin(); bullet != bullets.end(); bullet++) {
		if (&(*bullet) == a) {
			check = true;
			bullet->isAlive = false;

			//remove from gridspace
			for (auto a2 = gameArea[bullet->gPos].getBullets()->begin(); a2 != gameArea[bullet->gPos].getBullets()->end(); a2++) {
				if (a == *a2) {
					gameArea[bullet->gPos].getBullets()->erase(a2);
					break;
				}
			}

			//move to end
			if (bullet + 1 < bullets.end()) {
				std::rotate(bullet, bullet + 1, bullets.end());
			}

			activeBullets--;
		}
	}
	assert(check);
}

//move object from one partition to another
void Game::moveObject(Object *obj, GridPosition p, GridPosition np) {
	
	//add to new area
	if (obj->type == Object::ALIEN) {

		//remove from gridspace
		for (auto a = gameArea[p].getAliens()->begin(); a != gameArea[p].getAliens()->end(); a++) {
			//remove from current area
			if (*a == obj) {
				gameArea[p].getAliens()->erase(a);
				break;
			}
		}

		//add to new area
		gameArea[np].getAliens()->push_back((Alien*)obj);
	}
	else if (obj->type == Object::BULLET) {

		//remove from gridspace
		for (auto b = gameArea[p].getBullets()->begin(); b != gameArea[p].getBullets()->end(); b++) {
			//remove from current area
			if (*b == obj) {
				gameArea[p].getBullets()->erase(b);
				break;
			}
		}

		//add to new area
		gameArea[np].getBullets()->push_back((Bullet*)obj);
	}

	obj->gPos = np;
}

//collision between aliens and other aliens, bullets
void Game::alienCollision() {
	sf::FloatRect a;
	float r;
	sf::Vector2f pos;

	GridPosition g;

	std::vector<GridPosition> areas;
	sf::Vector2f diff;
	float dist;
	sf::Vector2f move;

	std::vector<Bullet*> rmBullet;
	std::vector<Alien*> rmAlien;

	int checks = 0;

	for (int i = 0; i < activeAliens; i++) {
		g = aliens.at(i).gPos;
		areas.push_back(g);

		r = aliens.at(i).getSize() / 2;
		a = gameArea[g].area;

		a.left += r;
		a.top += r;
		a.width -= r * 2;
		a.height -= r * 2;

		//North/South
		if (pos.y >= a.top + a.height) {
			areas.push_back(GridPosition(g.x, g.y + 1));
		}
		else if (pos.y < a.top) {
			areas.push_back(GridPosition(g.x, g.y - 1));
		}
		//East/West
		if (pos.x < a.left) {
			areas.push_back(GridPosition(g.x + 1, g.y));
		}
		else if (pos.y >= a.top + a.height) {
			areas.push_back(GridPosition(g.x - 1, g.y));
		}

		//diagonals
		if (pos.y >= a.top + a.height && pos.x < a.left) {
			areas.push_back(GridPosition(g.x + 1, g.y + 1));
		}
		else if (pos.y >= a.top + a.height && pos.y >= a.top + a.height) {
			areas.push_back(GridPosition(g.x - 1, g.y + 1));
		}
		else if (pos.y < a.top && pos.x < a.left) {
			areas.push_back(GridPosition(g.x + 1, g.y - 1));
		}
		else if (pos.y < a.top && pos.y >= a.top + a.height) {
			areas.push_back(GridPosition(g.x - 1, g.y - 1));
		}

		//compare to all objects in same partition and ones we're crossing into
		for (auto grid = areas.begin(); grid != areas.end(); grid++) {

			//alien to alien collision
			std::vector<Alien*>* alienarr = gameArea[*grid].getAliens();
			for (auto alien = alienarr->begin(); alien != alienarr->end(); alien++) {
				checks++;
				Alien *a1 = &aliens.at(i);
				Alien *a2 = *alien;

				if (a1 == a2)
					continue;

				diff = a1->getPosition() - a2->getPosition();
				dist = sqrtf(diff.x * diff.x + diff.y * diff.y);


				float prefDist = (a1->getSize() / 2) + (a2->getSize() / 2);
				if (dist < prefDist) {
					move = (diff * (prefDist / dist)) - diff;

					//solve collision partially (move / 2.f fully solves collision)
					//allows enemies to clump up a bit
					a1->setPosition(a1->getPosition() + (move / 8.f));
					a2->setPosition(a2->getPosition() - (move / 8.f));
				}
			}

			//alien to bullet collision
			std::vector<Bullet*>* b = gameArea[*grid].getBullets();
			bool c = true;
			for (int b = 0; b < activeBullets; b++) {
				checks++;
				Bullet *bul = &bullets.at(b);
				
				diff = aliens.at(i).getPosition() - bul->getPosition();
				dist = sqrtf(diff.x * diff.x + diff.y * diff.y);

				float prefDist = (aliens.at(i).getSize() / 2) + (bul->getSize() / 2);
				if (dist < prefDist) {
										
					bul->hitCounter++;

					if (bul->hitCounter >= Bullet::hitMax) {
						removeBullet(bul);
						b--;
					}

					//die
					killAlien(&aliens.at(i));
					i--;

					c = false;
					break;
					
				}
			}
			if (!c) break;
		}

		areas.clear();
	}
}

/*---------------------------------------------------------------------------------------------------------------*/
/* LEVEL COLLISION ----------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/

//does collision with the level
sf::Vector2f Game::areaCollision(sf::Vector2f p, float size, GridPosition g) {
	sf::Vector2f pos = p;
	float r = size / 2;

	sf::FloatRect area = gameArea[g].area;

	Partition *part;

	//SURFACES

	//North
	part = gameArea[g].getNeighbor(Partition::N);
	if (part != nullptr && part->getSolid()) {
		pos.y = std::max(area.top + r, pos.y);
	}
	//South
	part = gameArea[g].getNeighbor(Partition::S);
	if (part != nullptr && part->getSolid()) {
		pos.y = std::min(area.top + area.height - r, pos.y);
	}
	//West
	part = gameArea[g].getNeighbor(Partition::W);
	if (part != nullptr && part->getSolid()) {
		pos.x = std::max(area.left + r, pos.x);
	}
	//East
	part = gameArea[g].getNeighbor(Partition::E);
	if (part != nullptr && part->getSolid()) {
		pos.x = std::min(area.left + area.width - r, pos.x);
	}

	//CORNERS
	//lambda function to move position from a corner
	auto moveFromCorner = [](sf::Vector2f &pos, float rad, sf::Vector2f c) {
		sf::Vector2f corner = c;
		sf::Vector2f diff = pos - corner;
		float dist = sqrtf(diff.y * diff.y + diff.x * diff.x);
		if (dist < rad) {
			//get unit vector
			sf::Vector2f uVec = diff / dist;
			pos = corner + (uVec * rad);
		}

	};

	//Northeast
	part = gameArea[g].getNeighbor(Partition::NE);
	if (part != nullptr && part->getSolid()) {
		moveFromCorner(pos, r, sf::Vector2f(area.left + area.width, area.top));
	}
	//Southeast
	part = gameArea[g].getNeighbor(Partition::SE);
	if (part != nullptr && part->getSolid()) {
		moveFromCorner(pos, r, sf::Vector2f(area.left + area.width, area.top + area.height));
	}
	//Southwest
	part = gameArea[g].getNeighbor(Partition::SW);
	if (part != nullptr && part->getSolid()) {
		moveFromCorner(pos, r, sf::Vector2f(area.left, area.top + area.height));
	}
	//Northwest
	part = gameArea[g].getNeighbor(Partition::NW);
	if (part != nullptr && part->getSolid()) {
		moveFromCorner(pos, r, sf::Vector2f(area.left, area.top));
	}

	return pos;
}


/*---------------------------------------------------------------------------------------------------------------*/
/* ALIEN PATHING ------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/
void Game::generateFlowField(std::map<GridPosition, Partition> &gameArea, GridPosition startPos) {
	GridPosition g;

	int numSolved = 0;

	//reset all
	for (int x = 0; x < WINDOWWIDTH / AREASIZE; x++) {
		for (int y = 0; y < WINDOWHEIGHT / AREASIZE; y++) {

			g.x = x;
			g.y = y;

			gameArea[g].complete = false;
			gameArea[g].setMoveVec(sf::Vector2f());
			gameArea[g].distFromPlayer = -1;

			if (!gameArea[g].getSolid())
				gameArea[g].setColor(sf::Color(0, 0, 0, 255));

			if (g == startPos) {
				gameArea[g].distFromPlayer = 0;
				gameArea[g].setColor(sf::Color(255, 255, 255, 255));
			}
		}
	}


	//wavefront algorithm
	std::vector<GridPosition> visited;
	std::list<GridPosition> fringe;

	fringe.push_back(startPos);

	GridPosition p;
	Partition *part;
	while (!fringe.empty()) {
		p = fringe.front();

		//search neighbors
		for (int i = 0; i < 8; i++) {
			//node is valid
			if (isPathable(gameArea, p, i)) {


				//check if already visited
				part = gameArea[p].getNeighbor(i);
				bool add = true;

				for (auto j = visited.begin(); j != visited.end(); j++) {
					if (*j == part->pos) {
						add = false;
						break;
					}
				}
				if (add) {
					for (auto k = fringe.begin(); k != fringe.end(); k++) {
						if (*k == part->pos) {

							//found shorter path?
							part->distFromPlayer = std::min(part->distFromPlayer,
								gameArea[p].distFromPlayer + (i % 2 == 1 ? sqrt2 : 1.f));

							add = false;
							break;
						}
					}
				}

				//new node, add to list
				if (add) {
					part->distFromPlayer = gameArea[p].distFromPlayer + (i % 2 == 1 ? sqrt2 : 1.f);


					float c = std::max(0, 255 - (int)(part->distFromPlayer * 6));
					part->setColor(sf::Color(c, c, c, 255));

					fringe.push_back(part->pos);
				}
			}
		}

		visited.push_back(p);
		fringe.pop_front();
	}

	//generate flow field
	for (int x = 0; x < WINDOWWIDTH / AREASIZE; x++) {
		for (int y = 0; y < WINDOWHEIGHT / AREASIZE; y++) {

			g.x = x;
			g.y = y;

			int dir = -1;
			//arbitratily large number
			float dist = 999999.f;
			for (int i = 0; i < 8; i++) {
				if (isPathable(gameArea, g, i) && gameArea[g].getNeighbor(i)->distFromPlayer < dist) {
					dir = i;
					dist = gameArea[g].getNeighbor(i)->distFromPlayer;
				}
			}

			switch (dir) {
			case Partition::N:	gameArea[g].setMoveVec(sf::Vector2f(0.f, -1.f)); break;
			case Partition::NE: gameArea[g].setMoveVec(sf::Vector2f(1.f, -1.f)); break;
			case Partition::E:	gameArea[g].setMoveVec(sf::Vector2f(1.f, 0.f)); break;
			case Partition::SE: gameArea[g].setMoveVec(sf::Vector2f(1.f, 1.f)); break;
			case Partition::S:	gameArea[g].setMoveVec(sf::Vector2f(0.f, 1.f)); break;
			case Partition::SW: gameArea[g].setMoveVec(sf::Vector2f(-1.f, 1.f)); break;
			case Partition::W:	gameArea[g].setMoveVec(sf::Vector2f(-1.f, 0.f)); break;
			case Partition::NW: gameArea[g].setMoveVec(sf::Vector2f(-1.f, -1.f)); break;
			}

			//is diagonal
			if (dir % 2 == 1) {
				//turn into unit vector
				gameArea[g].setMoveVec(gameArea[g].getMoveVec() * sin45);
			}
		}
	}
}

bool Game::isPathable(std::map<GridPosition, Partition> &gameArea, GridPosition p, int d) {
	assert(d >= 0 && d < 8);

	bool r = gameArea[p].getNeighbor(d) != nullptr && !gameArea[p].getNeighbor(d)->getSolid();

	if (d % 2 == 1) {
		//check if diagonal is cut off by wall
		r = r &&
			!((gameArea[p].getNeighbor((d - 1) % 8) != nullptr && gameArea[p].getNeighbor((d - 1) % 8)->getSolid()) ||
			(gameArea[p].getNeighbor((d + 1) % 8) != nullptr && gameArea[p].getNeighbor((d + 1) % 8)->getSolid()));
	}

	return r;
}


/*---------------------------------------------------------------------------------------------------------------*/
/* RENDERING ----------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/

void Game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto i = gameArea.begin(); i != gameArea.end(); i++) {
		target.draw(i->second, states);
	}

	for (int i = 0; i < activeAliens; i++) {
		target.draw(aliens.at(i), states);
	}

	for (auto h = hives.begin(); h != hives.end(); h++) {
		target.draw(*h, states);
	}

	for (int i = 0; i < activeBullets; i++) {
		target.draw(bullets.at(i), states);
	}

	target.draw(plr, states);
}
