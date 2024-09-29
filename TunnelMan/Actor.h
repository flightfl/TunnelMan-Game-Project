#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// class forward since StudentWorld.h already included Actor.h
class StudentWorld;

/// Actor class /// 
// A class that can serve as the base class for all of your game’s objects
class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Actor();
	bool isAlive(); // to see if an actor is alive
	void toDie(); // to make an actor to die
	StudentWorld* getWorld(); // return the a studentWorld pointer

	// pure virtual function doSomething()
	// almost all actors/game objects can do something
	virtual void doSomething() = 0;

private:
	StudentWorld* m_world;
	bool m_alive;
};


/// Earth class ///
class Earth : public Actor {
public:
	Earth(StudentWorld* world, int x, int y);
	virtual ~Earth();
	virtual void doSomething();
};


/// Boulder class ///
class Boulder : public Actor {
public:
	Boulder(StudentWorld* world, int startX, int startY);
	// Boulder can annoy all characters
	void annoyCharacter();

	// override actor class's doSomething
	virtual void doSomething();
private:
	bool stable;
	bool fall;
	int n_ticks;
};

/// Squirt class ///
class Squirt : public Actor {
public:
	Squirt(StudentWorld* world, int startX, int startY, Direction dir);
	// Squirt can annoy regular and hard core protesters
	bool annoyProtesters();

	// override actor class's doSomething
	virtual void doSomething();
private:
	int n_travel;
};

/// Character class ///
// The base class for the tunnel man, protester, and coreprotester
// Commonalities: they all can be annoyed, they all have health
class Character : public Actor {
public:
	Character(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp);
	// all Characters have health
	int getHealth();
	void decHealth(int points);

	// Pure virtual function isAnnoyed()
	// all Characters can be annoyed
	virtual void isAnnoyed(int hp) = 0;
private:
	int m_hP;
};


/// TunnelMan class ///
class TunnelMan : public Character {
public:
	TunnelMan(StudentWorld* world);
	virtual ~TunnelMan();
	void loot(int id); // add water, sonar, or gold

	// Getters
	int getWater();
	int getSonar();
	int getGold();

	// squirt water in the facing direction
	void squirtWtr();

	// move toward if the tunnel man is in bounds, 
	// so it won't have the walking animation if the boundary is right in front of the tunnel man
	void moveToward();

	// override actor class's doSomething
	virtual void doSomething();
	// override Character class's isAnnoyed
	virtual void isAnnoyed(int hP);
private:
	// inventory of the tunnel man
	int n_water;
	int n_sonar;
	int n_gold;
};

/// Protester class ///
class Protester : public Character {
public:
	Protester(StudentWorld* world, int imageID, int hP);
	
	// move in given direction
	void moveInDir(Direction direction);
	// move a random number of steps in between 8 and 60 inclusively
	void move8To60();
	
	// be stunned when hit by squirt water
	void getStunned();
	// be bribed when picked up a gold
	void getBribed();

	// return true if the protester is facing at the tunnel man
	bool facedTM();

	// return a direction that can face the tunnel man
	Direction faceToTM();

	// return true when the protester is in the same line with the tunnel man and there is no block in between
	bool noBlockPtoTM(Direction direction);
	
	// generate a random Direction
	Direction randomDir();

	// return true if at an intersection
	bool atCorner();

	// pick a direction to face 
	void pickTurnDir();

	// override actor class's doSomething
	virtual void doSomething();
	// override Character class's isAnnoyed
	virtual void isAnnoyed(int hP);

private:
	bool m_leave; // leave state
	int n_moves;	// number of moves to move
	int m_tickSinceLastTurn;
	int m_tickNoYell;
	int ticks_to_stare;
};

/// RegularProtester class ///
class RegularProtester : public Protester {
public:
	RegularProtester(StudentWorld* world);
};

/// HardcoreProtester class ///
class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* world);
};

/// PickedUpStuffs class ///
// The base class for the water pool, gold, sonar, and barrel
// Commonalities: they all can be picked up
class PickedUpStuffs :public Actor {
public:
	PickedUpStuffs(StudentWorld* world, int imageID, int startX, int startY);
	
	// All pickupable item will be disappered if not picking up
	void disappearIn(int ticks);

	// PickedUpStuffs class also make the dosomething function pure virtual
	virtual void doSomething() = 0;
private:
	int n_tick;
};


/// Barrel class ///
class Barrel : public PickedUpStuffs {
public:
	Barrel(StudentWorld* world, int startX, int startY);

	// override the PickedUpStuffs class's doSomething
	virtual void doSomething();
};


/// Gold class ///
class Gold :public PickedUpStuffs {
public:
	Gold(StudentWorld* world, int startX, int startY, bool visible, bool dropped);

	// override the PickedUpStuffs class's doSomething
	virtual void doSomething();
private:
	bool isDropped; // gold has the dropped-on-the-ground state
};

/// Goodie class ///
class Goodie : public PickedUpStuffs {
public:
	Goodie(StudentWorld* world, int imageID, int startX, int startY);

	// override the PickedUpStuffs class's doSomething
	virtual void doSomething();
};

/// Sonar class ///
class Sonar :public Goodie {
public:
	Sonar(StudentWorld* world, int startX, int startY);
};

/// WaterPool class ///
class WaterPool : public Goodie {
public:
	WaterPool(StudentWorld* world, int startX, int startY);
};

#endif // ACTOR_H_
