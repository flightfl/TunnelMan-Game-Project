#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/// Actor class implementations ///
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_alive(true) {
	setVisible(true);
}

Actor::~Actor() {
	setVisible(false);
}

bool Actor::isAlive() {
	return m_alive;
}

void Actor::toDie() {
	m_alive = false;
}

StudentWorld* Actor::getWorld() {
	return m_world;
}


/// Earth class implementations ///
Earth::Earth(StudentWorld* world, int x, int y)
	: Actor(world, TID_EARTH, x, y, right, 0.25, 3) {
	setVisible(true);
}

Earth::~Earth() {
	// no need a destructor implementations
}

void Earth::doSomething() {}  // it doesn't need to do anything.


/// Boulder class implementations ///
Boulder::Boulder(StudentWorld* world, int startX, int startY)
	: Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1), stable(true), fall(false), n_ticks(0) {
	world->digEarth(startX, startY);
}

void Boulder::annoyCharacter()
{
	if (getWorld()->nearTunnelMan(this, 3)) {
		getWorld()->getTunnelMan()->isAnnoyed(100);
	}
	// Get a list of all Protesters within the radius
	std::vector<Protester*> protesters = getWorld()->getAllNearP(this, 3);

	// Annoy all Protesters in the list
	for (Protester* p : protesters) {
		if (p != nullptr) {
			p->isAnnoyed(100);
		}
	}
	/*
	Protester* p = getWorld()->getAllNearP(this, 3);
	if (p != nullptr) {
		p->isAnnoyed(100);
	}
	*/
}

void Boulder::doSomething() {
	if (!isAlive()) return;
	if (stable) {
		if (getWorld()->aboveEarth(getX(), getY() - 1))
			return;
		else stable = false;
	}
	if (n_ticks == 30) {
		fall = true;
		getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	n_ticks++;
	if (fall) { // no earth underneath the boulder
		if (getWorld()->aboveEarth(getX(), getY() - 1) || getWorld()->hasBoulder(getX(), getY() - 4, 0) || getY() == 0)
			toDie();
		else moveTo(getX(), getY() - 1);
		annoyCharacter();
	}
}


/// Squirt class implementations ///
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir)
	:Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1), n_travel(0) {}

bool Squirt::annoyProtesters()
{
	// Get a list of all Protesters within the radius
	std::vector<Protester*> protesters = getWorld()->getAllNearP(this, 3);

	// Annoy all Protesters in the list
	for (Protester* p : protesters) {
		if (p != nullptr) {
			p->isAnnoyed(2);
			return true;
		}
	}
	return false;
	/*
	Protester* p = getWorld()->getNearP(this, 3);
	if (p != nullptr) {
		p->isAnnoyed(2);
		return true;
	}
	return false;
	*/
}

void Squirt::doSomething() {
	if (!isAlive()) return;
	if (annoyProtesters() || n_travel == 4) {
		toDie();
		return;
	}
	switch (getDirection()) {
	case left:
		if (getWorld()->hasEarth(getX() - 1, getY()) || getWorld()->hasBoulder(getX() - 1, getY())) {
			toDie(); return;
		}
		else   moveTo(getX() - 1, getY());
		break;
	case right:
		if (getWorld()->hasEarth(getX() + 1, getY()) || getWorld()->hasBoulder(getX() + 1, getY())) {
			toDie(); return;
		}
		else   moveTo(getX() + 1, getY());
		break;
	case up:
		if (getWorld()->hasEarth(getX(), getY() + 1) || getWorld()->hasBoulder(getX(), getY() + 1)) {
			toDie(); return;
		}
		else moveTo(getX(), getY() + 1);
		break;
	case down:
		if (getWorld()->hasEarth(getX(), getY() - 1) || getWorld()->hasBoulder(getX(), getY() - 1)) {
			toDie(); return;
		}
		else moveTo(getX(), getY() - 1);
		break;
	case none: return;
	}
	n_travel++;
}


/// Character class implementations ///
Character::Character(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp)
	:Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hP(hp) {
	// all Characters are in size 1, layer 0
}

int Character::getHealth() {
	return m_hP;
}

void Character::decHealth(int points) {
	m_hP -= points;
}


// TunnelMan class implementations
TunnelMan::TunnelMan(StudentWorld* world)
	: Character(world, TID_PLAYER, 30, 60, right, 10), n_water(5), n_sonar(1), n_gold(0) {
	setVisible(true);
}

TunnelMan::~TunnelMan() {
	// implementation
}

void TunnelMan::loot(int id) {
	if (id == TID_WATER_POOL) n_water += 5;
	if (id == TID_SONAR) n_sonar += 2;
	if (id == TID_GOLD) n_gold++;
}

int TunnelMan::getWater() {
	return n_water;
}
int TunnelMan::getSonar() {
	return n_sonar;
}
int TunnelMan::getGold() {
	return n_gold;
}

void TunnelMan::squirtWtr() {
	switch (getDirection()) {
	case left:
		if (!getWorld()->hasEarth(getX() - 4, getY()) && !getWorld()->hasBoulder(getX() - 4, getY()))
			getWorld()->addA(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
		break;
	case right:
		if (!getWorld()->hasEarth(getX() + 4, getY()) && !getWorld()->hasBoulder(getX() + 4, getY()))
			getWorld()->addA(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
		break;
	case down:
		if (!getWorld()->hasEarth(getX(), getY() - 4) && !getWorld()->hasBoulder(getX(), getY() - 4))
			getWorld()->addA(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
		break;
	case up:
		if (!getWorld()->hasEarth(getX(), getY() + 4) && !getWorld()->hasBoulder(getX(), getY() + 4))
			getWorld()->addA(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
		break;
	case none:return;
	}
	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

void TunnelMan::moveToward() {
	if (getDirection() == up && getY() < VIEW_HEIGHT - 4)
		moveTo(getX(), getY() + 1);
	else if (getDirection() == down && getY() > 0)
		moveTo(getX(), getY() - 1);
	else if (getDirection() == right && getX() < VIEW_WIDTH - 4)
		moveTo(getX() + 1, getY());
	else if (getDirection() == left && getX() > 0)
		moveTo(getX() - 1, getY());
}

void TunnelMan::doSomething() {
	if (!isAlive()) return;

	int x = getX();
	int y = getY();
	StudentWorld* myWorld = getWorld();

	if (myWorld->digEarth(getX(), getY()))
		myWorld->playSound(SOUND_DIG);

	// Get user input
	// Move and dig based on input
	int input;
	if (getWorld()->getKey(input) == true) {
		// user hit a key this tick!
		Direction curDir = getDirection();
		switch (input)
		{
		case KEY_PRESS_ESCAPE:
			toDie();
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() == right) {
				if (!myWorld->hasBoulder(x + 1, y))
					moveToward();
			}
			else
				setDirection(right);
			break;

		case KEY_PRESS_LEFT:
			if (getDirection() == left) {
				if (!myWorld->hasBoulder(x - 1, y))
					moveToward();
			}
			else
				setDirection(left);
			break;

		case KEY_PRESS_UP:
			if (getDirection() == up) {
				if (!myWorld->hasBoulder(x, y + 1))
					moveToward();
			}
			else
				setDirection(up);
			break;

		case KEY_PRESS_DOWN:
			if (getDirection() == down) {
				if (!myWorld->hasBoulder(x, y - 1))
					moveToward();
			}
			else
				setDirection(down);
			break;
		case KEY_PRESS_SPACE:
			if (n_water > 0) {
				n_water--;
				squirtWtr();
			}
			break;
		case 'z':
		case 'Z':			//use sonar kit
			if (n_sonar > 0)
			{
				n_sonar--;
				myWorld->SonicScan(getX(), getY(), 12);
				myWorld->playSound(SOUND_SONAR);
			}
			break;
		case KEY_PRESS_TAB:		//bribe with gold
			if (n_gold > 0) {
				myWorld->addA(new Gold(getWorld(), getX(), getY(), true, true));
				n_gold--;
			}
			break;
		}
	}

}

void TunnelMan::isAnnoyed(int hP) {
	decHealth(hP);
	if (getHealth() <= 0) {
		toDie();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}


/// Protester class implemenations ///
Protester::Protester(StudentWorld* world, int imageID, int hP)
	: Character(world, imageID, 60, 60, left, hP), m_leave(false), m_tickSinceLastTurn(200), m_tickNoYell(15)
{
	move8To60();
	ticks_to_stare = std::max(0, 3 - (int)getWorld()->getLevel() / 4);
}

void Protester::move8To60()
{
	n_moves = rand() % 53 + 8; //  8 <= numSquaresToMoveInCurrentDirection <= 60
}

void Protester::doSomething()
{
	// check if the protester is alive
	if (!isAlive()) return;
	// if the protester is in stunned state, reduce its stunned time
	if (ticks_to_stare > 0) {
		ticks_to_stare--;
		return;
	}
	else {
		ticks_to_stare = std::max(0, 3 - (int)getWorld()->getLevel() / 4);
		m_tickSinceLastTurn++;
		m_tickNoYell++;
	}

	// check if the proteseter is in LeaveTheOilField state
	if (m_leave) {
		if (getX() == 60 && getY() == 60) { // exit point
			toDie();
			setVisible(false);
			getWorld()->decreP();
			return;
		}
		getWorld()->exitP(this);// move one square closer to exit
		return;
	}

	// if the tunnel man is right in front of the protester, shout the tunnel man
	if (getWorld()->nearTunnelMan(this, 4) && facedTM()) {
		if (m_tickNoYell > 15) {
			getWorld()->getTunnelMan()->isAnnoyed(2);
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			m_tickNoYell = 0;
			return;
		}
		return;
	}

	// only for hardCore protester
	if (getID() == TID_HARD_CORE_PROTESTER) {
		int M = 16 + int(getWorld()->getLevel());
		Direction s = getWorld()->followTM(this, M);
		if (s != none) {
			moveInDir(s);
			return;
		}
	}

	// line of sight to tunnel man and go to find tunnel man
	Direction d = faceToTM();
	if (d != none && noBlockPtoTM(d) && (!getWorld()->nearTunnelMan(this, 4))) {
		setDirection(d);
		moveInDir(d);
		n_moves = 0;
		return;
	}

	n_moves--;
	if (n_moves <= 0) {
		Direction k = none;
		k = randomDir();
		while (true) {
			k = randomDir();
			if (getWorld()->canMove(getX(), getY(), k)) break;
		}
		setDirection(k);
		move8To60();
	}
	else if (atCorner() && m_tickSinceLastTurn > 200) {
		pickTurnDir();
		m_tickSinceLastTurn = 0;
		move8To60();
	}

	moveInDir(getDirection());
	
	if (!getWorld()->canMove(getX(), getY(), getDirection())) {
		n_moves = 0;
	}
}

void Protester::moveInDir(Direction direction)
{
	switch (direction) {
	case left:
		if (getDirection() == left) {
			if (getX() == 0) 
				setDirection(right);

			moveTo(getX() - 1, getY());
		}
		else setDirection(left);
		break;

	case right:
		if (getDirection() == right) {
			if (getX() == 60) 
				setDirection(left);

			moveTo(getX() + 1, getY());
		}
		else setDirection(right);
		break;

	case up:
		if (getDirection() == up) {
			if (getY() == 60)
				setDirection(down);

			moveTo(getX(), getY() + 1);
		}
		else setDirection(up);
		break;

	case down:
		if (getDirection() == down) {
			if (getY() == 0)
				setDirection(up);

			moveTo(getX(), getY() - 1);
		}
		else setDirection(down);
		break;

	case none: return;

	}
}

GraphObject::Direction Protester::faceToTM()
{
	int tunnelMan_X = getWorld()->getTunnelMan()->getX();
	int tunnelMan_Y = getWorld()->getTunnelMan()->getY();

	if (getY() == tunnelMan_Y && getX() == tunnelMan_X) {
		return getDirection();
	}
	if (getX() == tunnelMan_X) {
		if (getY() < tunnelMan_Y)
			return up;
		if (getY() > tunnelMan_Y)
			return down;
	}
	if (getY() == tunnelMan_Y) {
		if (getX() > tunnelMan_X)
			return left;
		if (getY() < tunnelMan_Y)
			return right;
	}

	// didnt see the tunnel man in straight line
	return none;
}

bool Protester::noBlockPtoTM(Direction direction)
{
	int tunnelMan_X = getWorld()->getTunnelMan()->getX();
	int tunnelMan_Y = getWorld()->getTunnelMan()->getY();

	switch (direction) {
	case left:
		for (int i = getX(); i >= tunnelMan_X; i--) {
			if (getWorld()->hasEarth(i, getY()) || getWorld()->hasBoulder(i, getY()))
				return false;
		}
		return true;
		break;
	case right:
		for (int i = getX(); i <= tunnelMan_X; i++) {
			if (getWorld()->hasEarth(i, getY()) || getWorld()->hasBoulder(i, getY()))
				return false;
		}
		return true;
		break;
	case up:
		for (int j = getY(); j <= tunnelMan_Y; j++) {
			if (getWorld()->hasEarth(getX(), j) || getWorld()->hasBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case down:
		for (int j = getY(); j >= tunnelMan_Y; j--) {
			if (getWorld()->hasEarth(getX(), j) || getWorld()->hasBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case none:
		return false;
	}
}

GraphObject::Direction Protester::randomDir()
{
	int r;
	r = rand() % 4;

	switch (r) {
	case 0: return left;
	case 1: return  right;
	case 2: return up;
	case 3: return down;
	}
	// not returnning left, right, up, or down
	return none;
}

bool Protester::atCorner()
{
	if (getDirection() == up || getDirection() == down) {
		return (getWorld()->canMove(getX(), getY(), left) || getWorld()->canMove(getX(), getY(), right));
	}
	else {
		return (getWorld()->canMove(getX(), getY(), up) || getWorld()->canMove(getX(), getY(), down));
	}
}

void Protester::pickTurnDir()
{
	if (getDirection() == up || getDirection() == down) {

		if (!getWorld()->canMove(getX(), getY(), left)) setDirection(right);
		else if (!getWorld()->canMove(getX(), getY(), right)) setDirection(left);
		else {
			switch (rand() % 2) {
			case 0: setDirection(left);
			case 1: setDirection(right);
			}
		}

	}
	else {

		if (!getWorld()->canMove(getX(), getY(), up)) setDirection(down);
		else if (!getWorld()->canMove(getX(), getY(), down)) setDirection(up);
		else {
			switch (rand() % 2) {
			case 0: setDirection(up);
			case 1: setDirection(down);
			}
		}

	}

}

void Protester::isAnnoyed(int hP)
{
	if (m_leave) return;

	decHealth(hP);
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);

	if (getHealth() <= 0) {
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_leave = true;
		ticks_to_stare = 0;
		// If the Hardcore Protester was annoyed due to being bonked by a Boulder, then it will increase the player’s score by 500 points.
		if (hP == 100 && getID() == TID_HARD_CORE_PROTESTER) getWorld()->increaseScore(500);
		// if the Hardcore Protester was annoyed into submission due to being squirted repeatedly, then it will increase the player’s score by 250 points.
		else if (getID() == TID_HARD_CORE_PROTESTER) getWorld()->increaseScore(250);
		// If the Regular Protester was annoyed due to being bonked by a Boulder, then it will increase the player’s score by 500 points.
		else if (hP == 100 && getID() == TID_PROTESTER) getWorld()->increaseScore(500);
		// if the Regular Protester was annoyed due to being squirted repeatedly, then it will increase the player’s score by 100 points.
		else if (getID() == TID_PROTESTER) getWorld()->increaseScore(100);
	}
	else getStunned();
}

void Protester::getStunned()
{
	ticks_to_stare = std::max(50, 100 - (int)getWorld()->getLevel() * 10);
}

void Protester::getBribed()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	if (getID() == TID_PROTESTER) {
		getWorld()->increaseScore(25);
		m_leave = true;
	}
	else { // else it is hard core protester
		getWorld()->increaseScore(50);
		ticks_to_stare = std::max(50, 100 - int(getWorld()->getLevel()) * 10);
	}
}

bool Protester::facedTM()
{
	switch (getDirection()) {
		case left:
			return getWorld()->getTunnelMan()->getX() <= getX();
		case right:
			return getWorld()->getTunnelMan()->getX() >= getX();
		case up:
			return getWorld()->getTunnelMan()->getY() >= getY();
		case down:
			return getWorld()->getTunnelMan()->getY() <= getY();
		case none:
			return false;
	}
	return false;
}

/// RegularProtester class implementations ///
RegularProtester::RegularProtester(StudentWorld* world)
	: Protester(world, TID_PROTESTER, 5) {}

/// HardcoreProtester class implementations ///
HardcoreProtester::HardcoreProtester(StudentWorld* world)
	: Protester(world, TID_HARD_CORE_PROTESTER, 20) {}


/// PickedUpStuffs class implementations ///
PickedUpStuffs::PickedUpStuffs(StudentWorld* world, int imageID, int startX, int startY)
	:Actor(world, imageID, startX, startY, right, 1.0, 2), n_tick(0) {}

void PickedUpStuffs::disappearIn(int ticks) {
	if (n_tick == ticks) toDie();
	else n_tick++;
}

/// Barrel class implementations ///
Barrel::Barrel(StudentWorld* world, int startX, int startY)
	:PickedUpStuffs(world, TID_BARREL, startX, startY)
{
	setVisible(false);
}

void Barrel::doSomething() {
	if (!isAlive()) return;
	if (!isVisible() && getWorld()->nearTunnelMan(this, 4)) {
		setVisible(true);
		return;
	}
	if (getWorld()->nearTunnelMan(this, 3)) {
		toDie();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decreB();
		return;
	}
}

/// Gold class implementations ///
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped)
	:PickedUpStuffs(world, TID_GOLD, startX, startY), isDropped(dropped)
{
	setVisible(isVisible);
}

void Gold::doSomething() {
	if (!isAlive()) return;
	if (!isVisible() && getWorld()->nearTunnelMan(this, 4)) {
		setVisible(true);
		return;
	}
	if (!isDropped && getWorld()->nearTunnelMan(this, 3)) {
		toDie();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getTunnelMan()->loot(getID());
		getWorld()->increaseScore(10);
		return;
	}
	if (isDropped) {
		Protester* p = (getWorld()->getNearP(this, 3));
		if (p != nullptr) {
			toDie();
			p->getBribed();	
		};
		disappearIn(100);
	}
}

//Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY)
	:PickedUpStuffs(world, imageID, startX, startY) {}

void Goodie::doSomething() {
	if (!isAlive()) return;
	if (getWorld()->nearTunnelMan(this, 3)) {
		toDie();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getTunnelMan()->loot(getID());
		getWorld()->increaseScore(75);
		return;
	}
	disappearIn(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

/// Sonar class ///
Sonar::Sonar(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_SONAR, startX, startY) {}

/// WaterPool class implementations ///
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_WATER_POOL, startX, startY) {}

