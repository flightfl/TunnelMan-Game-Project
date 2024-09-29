#include "StudentWorld.h"
#include <string>
#include <sstream> // for std::ostringstream
#include <iostream> // defines the overloads of the << operator
#include <iomanip> // for setw()
#include <queue>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir), tunnelMan(nullptr), isTickOne(true), ticksSinceLastAddP(0), n_aliveP(0), n_barrelsLeft(0)
{
	
}

StudentWorld::~StudentWorld() {
	// Delete any objects here
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			delete earth[x][y];
		}
	}
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		delete* it;
	}
	m_actors.clear();
	delete tunnelMan;
}

int StudentWorld::init()
{
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			if (x < 30 || x>33 || y < 4)
				earth[x][y] = new Earth(this, x, y);
		}
	}
	//reset variables
	n_barrelsLeft = 0;
	isTickOne = true;
	ticksSinceLastAddP = 0;
	n_aliveP = 0;
	
	tunnelMan = new TunnelMan(this);  // create tunnelman
	// create boulders, gold and barrel of oil
	int B = min((int)getLevel() / 2 + 2, 9);
	int G = max((int)getLevel() / 2, 2);
	int L = min(2 + (int)getLevel(), 21);
	addItems(B, 'B');
	addItems(L, 'L');
	addItems(G, 'G');

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	setDisplayText();

	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isAlive()) {
			(*it)->doSomething();
		}
		if (!tunnelMan->isAlive()) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (n_barrelsLeft == 0) {
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	tunnelMan->doSomething();

	addS_W();
	addP();

	// clean dead actors
	for (it = m_actors.begin(); it != m_actors.end();) {
		if (!(*it)->isAlive()) {
			(*it)->setVisible(false);
			delete* it;
			it = m_actors.erase(it);
		}
		else it++;
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			delete earth[x][y];
		}
	}
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		delete* it;
	}
	m_actors.clear();
	delete tunnelMan;

}

bool StudentWorld::digEarth(int x, int y) {
	bool clear = false;
	for (int i = x; i < x + 4; i++)
		for (int j = y; j < y + 4; j++)
		{
			if (earth[i][j] != nullptr) {
				delete earth[i][j];
				earth[i][j] = nullptr;
				clear = true;
			}
		}
	return clear;
}

TunnelMan* StudentWorld::getTunnelMan()
{
	return tunnelMan;
}

void StudentWorld::setDisplayText()
{
	int level = getLevel();
	int lives = getLives();
	int health = tunnelMan->getHealth();
	int squirts = tunnelMan->getWater();
	int gold = tunnelMan->getGold();
	//int barrelsLeft = getBarrels();
	int sonar = tunnelMan->getSonar();
	int score = getScore();

	string s = formatDisplayText(level, lives, health, squirts, gold, n_barrelsLeft, sonar, score);
	setGameStatText(s);

}
string StudentWorld::formatDisplayText(int level, int lives, int health, int squirts, int gold, int oilLeft, int sonar, int score)
{
	ostringstream oss;

	// Score
	oss << "Scr: ";
	oss.fill('0');
	oss << setw(6) << score;

	// Level
	oss << "  Lvl: ";
	oss << setw(2) << level;

	// Lives
	oss << "  Lives: ";
	oss << setw(1) << lives;

	// Health
	oss << "  Hlth: ";
	oss << setw(3) << health * 10;
	oss << '%';

	// Water 
	oss << "  Wtr: ";
	oss << setw(2) << squirts;

	// Gold
	oss << "  Gld: ";
	oss << setw(2) << gold;

	// Oil Left
	oss << "  Oil Left: ";
	oss << setw(2) << oilLeft;

	// Sonar
	oss << "  Sonar: ";
	oss << setw(2) << sonar;

	return oss.str();
}

void StudentWorld::addA(Actor* actor) {
	m_actors.push_back(actor);
}

void StudentWorld::addS_W()
{
	int x, y;
	int G = (int)getLevel() * 25 + 300;
	if (int(rand() % G) + 1 == 1) { // 1 in G chance
		if (int(rand() % 5) + 1 == 1) { // 1 in 5 chance
			addA(new Sonar(this, 0, 60));
		}
		else { // 4 in 5 chance
			do {
				x = rand() % 60 + 1;
				y = rand() % 60 + 1;
			} while (hasEarth(x, y));
			addA(new WaterPool(this, x, y));
		}
	}
}

void StudentWorld::addP()
{
	int T = max(25, 200 - (int)getLevel());
	int P = fmin(15, 2 + getLevel() * 1.5);	 // min can't compare two different data types, so use fmin() here
	int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30);
	// A new Protester (Regular or Hardcore) may only be added to the oil field after at
	// least T ticks have passed since the last Protester of any type was added
	// The first Protester must be added to the oil field during the very first tick
	if (isTickOne || (ticksSinceLastAddP > T && n_aliveP < P)) { 
		if (rand() % 100 + 1 < probabilityOfHardcore)
			addA(new HardcoreProtester(this));
		else addA(new RegularProtester(this));
		ticksSinceLastAddP = 0;
		n_aliveP++;
		isTickOne = false;
	}
	ticksSinceLastAddP++;
}

void StudentWorld::decreB()
{
	n_barrelsLeft--;
}

void StudentWorld::decreP() {
	n_aliveP--;
}

bool StudentWorld::nearby(int x1, int y1, int x2, int y2, int radius)
{
	if ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= radius * radius)
		return true;
	else return false;
}

bool StudentWorld::hasActor(int x, int y, int radius) {
	vector<Actor*>::iterator it;
	it = m_actors.begin();
	while (it != m_actors.end()) {
		if (nearby(x, y, (*it)->getX(), (*it)->getY(), radius))
			return true;
		it++;
	}
	return false;
}

void StudentWorld::SonicScan(int x, int y, int radius)
{
	int a, b;
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD)
		{
			a = (*it)->getX(); b = (*it)->getY();
			if ((x - a) * (x - a) + (y - b) * (y - b) <= radius * radius)
				(*it)->setVisible(true);
		}
	}
}

void StudentWorld::addItems(int num, char actor) {
	int x, y;
	for (int i = 0; i < num; i++) {
		do {
			x = rand() % 60 + 1;
			if (actor == 'B') y = rand() % 36 + 1 + 20;
			else	y = rand() % 56 + 1;
		} while (hasActor(x, y, 6) || (x > 26 && x < 34 && y>0));
		switch (actor) {
		case 'B':
			addA(new Boulder(this, x, y));
			break;
		case 'G':
			addA(new Gold(this, x, y, false, false));
			break;
		case 'L':
			addA(new Barrel(this, x, y));
			n_barrelsLeft++;
			break;
		}
	}
}

bool StudentWorld::aboveEarth(int x, int y)
{
	for (int i = x; i < x + 4; i++) {
		if (earth[i][y] != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::hasEarth(int x, int y)
{
	for (int i = x; i < x + 4; i++) {

		for (int j = y; j < y + 4; j++) {

			if (earth[i][j] != nullptr)
				return true;
		}
	}
	return false;
}

bool StudentWorld::hasBoulder(int x, int y, int radius)
{
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->getID() == TID_BOULDER && nearby(x, y, (*it)->getX(), (*it)->getY(), radius))
			return true;
	}
	return false;
}

bool StudentWorld::canMove(int x, int y, GraphObject::Direction direction)
{
	switch (direction) {
	case GraphObject::left:
		return (x != 0 && !hasEarth(x - 1, y) && !hasBoulder(x, y));
	case GraphObject::right:
		return (x != 60 && !hasEarth(x + 1, y) && !hasBoulder(x + 1, y));
	case GraphObject::up:
		return (y != 60 && !hasEarth(x, y + 1) && !hasBoulder(x, y + 1));
	case GraphObject::down:
		return (y != 0 && !hasEarth(x, y - 1) && !hasBoulder(x, y - 1));
	case GraphObject::none:
		return false;
	}
	return false;
}

bool StudentWorld::nearTunnelMan(Actor* actor, int radius)
{
	return nearby(actor->getX(), actor->getY(), tunnelMan->getX(), tunnelMan->getY(), radius);
}

//  function to return a collection of all Protester objects found
std::vector<Protester*> StudentWorld::getAllNearP(Actor* actor, int radius)
{
	std::vector<Protester*> getAllNearP;

	for (Actor* a : m_actors) {
		if (a->getID() == TID_PROTESTER || a->getID() == TID_HARD_CORE_PROTESTER) {
			if (nearby(actor->getX(), actor->getY(), a->getX(), a->getY(), radius)) {
				Protester* protester = dynamic_cast<Protester*>(a);
				if (protester != nullptr) {
					getAllNearP.push_back(protester);
				}
			}
		}
	}

	return getAllNearP;
}

//  function to return only one Protester objects found
Protester* StudentWorld::getNearP(Actor* actor, int radius)
{
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
			if (nearby(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius))
				return dynamic_cast<Protester*> (*it);
	}
	return nullptr;
}

void StudentWorld::exitP(Protester* pr)
{
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			board[i][j] = 0;
		}
	}
	int a = pr->getX();
	int b = pr->getY();
	queue<Grid> q;
	q.push(Grid(60, 60));
	board[60][60] = 1;
	while (!q.empty()) {
		Grid c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;
		if (canMove(x, y, GraphObject::left) && board[x - 1][y] == 0) {
			q.push(Grid(x - 1, y));
			board[x - 1][y] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::right) && board[x + 1][y] == 0) {
			q.push(Grid(x + 1, y));
			board[x + 1][y] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::up) && board[x][y + 1] == 0) {
			q.push(Grid(x, y + 1));
			board[x][y + 1] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::down) && board[x][y - 1] == 0) {
			q.push(Grid(x, y - 1));
			board[x][y - 1] = board[x][y] + 1;
		}
	}
	if (canMove(a, b, GraphObject::left) && board[a - 1][b] < board[a][b])
		pr->moveInDir(GraphObject::left);
	if (canMove(a, b, GraphObject::right) && board[a + 1][b] < board[a][b])
		pr->moveInDir(GraphObject::right);
	if (canMove(a, b, GraphObject::up) && board[a][b + 1] < board[a][b])
		pr->moveInDir(GraphObject::up);
	if (canMove(a, b, GraphObject::down) && board[a][b - 1] < board[a][b])
		pr->moveInDir(GraphObject::down);
	return;
	
}


GraphObject::Direction StudentWorld::followTM(Protester* pr, int M) {
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			board[i][j] = 0;
		}
	}
	int a = pr->getX();
	int b = pr->getY();
	queue<Grid> q;
	q.push(Grid(getTunnelMan()->getX(), getTunnelMan()->getY()));
	board[getTunnelMan()->getX()][getTunnelMan()->getY()] = 1;
	while (!q.empty()) {
		Grid c = q.front();
		q.pop();
		int x = c.x;
		int y = c.y;
		if (canMove(x, y, GraphObject::left) && board[x - 1][y] == 0) {
			q.push(Grid(x - 1, y));
			board[x - 1][y] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::right) && board[x + 1][y] == 0) {
			q.push(Grid(x + 1, y));
			board[x + 1][y] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::up) && board[x][y + 1] == 0) {
			q.push(Grid(x, y + 1));
			board[x][y + 1] = board[x][y] + 1;
		}
		if (canMove(x, y, GraphObject::down) && board[x][y - 1] == 0) {
			q.push(Grid(x, y - 1));
			board[x][y - 1] = board[x][y] + 1;
		}
	}
	if (board[a][b] <= M + 1) {
		if (canMove(a, b, GraphObject::left) && board[a - 1][b] < board[a][b])
			return GraphObject::left;
		if (canMove(a, b, GraphObject::right) && board[a + 1][b] < board[a][b])
			return GraphObject::right;
		if (canMove(a, b, GraphObject::up) && board[a][b + 1] < board[a][b])
			return GraphObject::up;
		if (canMove(a, b, GraphObject::down) && board[a][b - 1] < board[a][b])
			return GraphObject::down;
	}
	return GraphObject::none;
}

