#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include "Actor.h"
#include <vector>

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// student added functions
	virtual ~StudentWorld();
	TunnelMan* getTunnelMan();
	void setDisplayText();
	std::string formatDisplayText(int level, int lives, int health, int squirts, int gold, int oilLeft, int sonar, int score);

	void SonicScan(int x, int y, int radius); // use sonar to reveal nearby gold and barrels
	bool digEarth(int x, int y); // remove earth infront of the tunnel man
	void addA(Actor* actor); // add game object
	void addS_W(); // add sonar or waterpool
	void addP(); // add regular protester or hard core protester
	void decreB(); // decrement the amount of barrels
	void decreP(); // decrement the amount of protesters
	bool nearby(int x1, int y1, int x2, int y2, int radius); // return true if two actor's distance is less than radius based on their coordiantes 
	bool hasActor(int x, int y, int radius);  //return if there is an actor in radius
	void addItems(int num, char actor);

	bool aboveEarth(int x, int y);
	bool hasEarth(int x, int y);
	bool hasBoulder(int x, int y, int radius = 3);

	bool canMove(int x, int y, GraphObject::Direction direction);
	bool nearTunnelMan(Actor* actor, int radius);

	Protester* getNearP(Actor* actor, int radius);
	std::vector<Protester*> getAllNearP(Actor* actor, int radius);

	void exitP(Protester* pr);
	GraphObject::Direction followTM(Protester* pr, int M);

private:
	// student added code
	Earth* earth[64][64];
	TunnelMan* tunnelMan;

	std::vector<Actor*> m_actors;

	bool isTickOne; // first tick
	int ticksSinceLastAddP; // the number of ticks since last time adding a protester
	int n_aliveP; // number of alive protesters
	int n_barrelsLeft;
	int board[64][64];
	struct Grid {
		int x;
		int y;
		Grid(int a, int b) : x(a), y(b) {}
	};
};

#endif // STUDENTWORLD_H_
