#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <algorithm>
using namespace std;

///////////////////////////////////////////////////////////////////////////
//  ACTOR implementation
///////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld *world, int imageID, int startX, int startY, 
             Direction dir, double size, unsigned int depth)
: GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), alive(true) {
    setVisible(true);
}

Actor::~Actor() {
}

bool Actor::getAlive() {
    return alive;
}

void Actor::setAlive(bool isalive) {
    alive = isalive;
}

StudentWorld* Actor::getWorld() {
    return m_world;
}

///////////////////////////////////////////////////////////////////////////
//  TUNNELMAN implementation
///////////////////////////////////////////////////////////////////////////

TunnelMan::TunnelMan(StudentWorld *world)
                    : Actor (world, TID_PLAYER, 30, 60, right, 1.0, 0) {
    hitpoints = 10;
    unitswater = 5;
    sonarcharge = 1;
    goldnuggets = 0;
    barrelsoil = 0;
}

TunnelMan::~TunnelMan() {
    
}

void TunnelMan::doSomething()
{
    if (!getAlive()) return;
    
    if (getWorld()->removeEarth(getX(), getY())) {
        getWorld()->playSound(SOUND_DIG);
    }
    else {
        userInput();
    }
}

void TunnelMan::isAnnoyed(int ptslost) {
    hitpoints = hitpoints - ptslost;
    if (hitpoints < 1) {
        setAlive(false);
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void TunnelMan::userInput() {
    int ch;
    if (getWorld()->getKey(ch) == true) {
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                setAlive(false);
                break;
            case KEY_PRESS_SPACE:
                if (unitswater > 0) {
                    fireSquirt();
                    unitswater--;
                }
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() != left) {
                    setDirection(left);
                }
                else if (getX() >= 0 && !(getWorld()->boulderPresent(getX() - 1, getY()))){
                    moveTo(getX() - 1, getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right) {
                    setDirection(right);
                }
                else if (getX() <= VIEW_WIDTH - SPRITE_WIDTH  && !(getWorld()->boulderPresent(getX() + 1, getY()))) {
                    moveTo(getX() + 1, getY());
                }
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up) {
                    setDirection(up);
                }
                else if (getY() <= VIEW_HEIGHT - SPRITE_HEIGHT && !(getWorld()->boulderPresent(getX(), getY() + 1))) {
                    moveTo(getX(), getY() + 1);
                }
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down) {
                    setDirection(down);
                }
                else if (getY() >= 0 && !(getWorld()->boulderPresent(getX(), getY() - 1))){
                    moveTo(getX(), getY() - 1);
                }
                break;
            case 'Z':
            case 'z':
                if (sonarcharge > 0) {
                    getWorld()->illuminate(getX(), getY());
                    sonarcharge--;
                }
                break;
            case KEY_PRESS_TAB:
                if (goldnuggets > 0) {
                    getWorld()->dropNugget(getX(), getY());
                    goldnuggets--;
                }
                break;
                
        }
    }
}

void TunnelMan::fireSquirt() {
    if (getDirection() == left) {
        if (getWorld()->earthPresent(getX() - 4, getY()) || getWorld()->boulderPresent(getX() - 4, getY())) {
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
        else {
            getWorld()->spawnSquirt(getX() - 4, getY(), getDirection());
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
    }
    if (getDirection() == right) {
        if (getWorld()->earthPresent(getX() + 4, getY()) || getWorld()->boulderPresent(getX() + 4, getY())) {
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
        else {
            getWorld()->spawnSquirt(getX() + 4, getY(), getDirection());
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
    }
    if (getDirection() == up) {
        if (getWorld()->earthPresent(getX(), getY() + 4) || getWorld()->boulderPresent(getX(), getY() + 4)) {
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
        else {
            getWorld()->spawnSquirt(getX(), getY() + 4, getDirection());
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
    }
    if (getDirection() == down) {
        if (getWorld()->earthPresent(getX(), getY() - 4) || getWorld()->boulderPresent(getX(), getY() - 4)) {
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
        else {
            getWorld()->spawnSquirt(getX(), getY() - 4, getDirection());
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
        }
    }
}

int TunnelMan::getNuggets() {
    return goldnuggets;
}

void TunnelMan::incNuggets() {
    goldnuggets++;
}

int TunnelMan::getSonar() {
    return sonarcharge;
}

void TunnelMan::incSonar() {
    sonarcharge++;
}

int TunnelMan::getWater() {
    return unitswater;
}

void TunnelMan::incWater() {
    unitswater = unitswater + 5;
}

int TunnelMan::getBarrels() {
    return barrelsoil;
}

void TunnelMan::incBarrels() {
    barrelsoil++;
}

int TunnelMan::getHitPoints() {
    return hitpoints;
}
///////////////////////////////////////////////////////////////////////////
//  EARTH implementation
///////////////////////////////////////////////////////////////////////////

Earth::Earth(StudentWorld *world, int x, int y)
            : Actor (world, TID_EARTH, x, y, right, 0.25, 3) {
}

Earth::~Earth() {
}

void Earth::doSomething() {
    //do nothing!
}

void Earth::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  BOULDER implementation
///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(StudentWorld *world, int x, int y)
                : Actor(world, TID_BOULDER, x, y, down, 1.0, 1) {
    stable = true;
}

Boulder::~Boulder() {
    
}

void Boulder::doSomething() {
    if (!getAlive()) return;
    
    if (stable) {
        if ((getWorld()->boulderSupported(getX(), getY()))) {
            return;
        }
        else {
            stable = false;
            ticks = 0;
        }
    }
    if (stable == false && fall == false && ticks == 30) {
        fall = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    ticks++;
    if (fall) {
        if ((getWorld()->earthPresent(getX(), getY() - 1))) {
            setAlive(false);
        }
        if (getY() >= 0 && !(getWorld()->boulderPresent(getX(), getY() - 4) && !(getWorld()->earthPresent(getX(), getY() - 1)))) {
            moveTo(getX(), getY() - 1);
        }
        else {
            setAlive(false);
        }
        if (getWorld()->tunnelmanPresent(getX(), getY(), 3)) {
            getWorld()->getTunnelMan()->isAnnoyed(100);
        }
        Protester* p = getWorld()->protesterPresent(getX(), getY(), 3);
        if (p != nullptr) {
            p->isAnnoyed(100);
        }
    }
}

void Boulder::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  SQUIRT implementation
///////////////////////////////////////////////////////////////////////////

Squirt::Squirt(StudentWorld *world, int x, int y, Direction dir)
                : Actor(world, TID_WATER_SPURT, x, y, dir, 1.0, 1) {
    traveldist = 4;
}

Squirt::~Squirt() {
    
}

void Squirt::doSomething() {
    if (traveldist >= 0) {
        if (getDirection() == left) {
            if (!(getWorld()->earthPresent(getX() - 1, getY()) && !(getWorld()->boulderPresent(getX() - 1, getY())))) {
                Protester *p = getWorld()->protesterPresent(getX(), getY(), 3);
                if (p != nullptr) {
                    p->isAnnoyed(2);
                    setAlive(false);
                }
                else {
                    moveTo(getX() - 1, getY());
                    traveldist--;
                }
            }
            else {
                setAlive(false);
            }
        }
        if (getDirection() == right) {
            if (!(getWorld()->earthPresent(getX() + 1, getY()) && !(getWorld()->boulderPresent(getX() + 1, getY())))) {
                Protester *p = getWorld()->protesterPresent(getX(), getY(), 3);
                if (p != nullptr) {
                    p->isAnnoyed(2);
                    setAlive(false);
                }
                else {
                    moveTo(getX() + 1, getY());
                    traveldist--;
                }
            }
            else {
                setAlive(false);
            }
        }
        if (getDirection() == up) {
            if (!(getWorld()->earthPresent(getX(), getY() + 1) && !(getWorld()->boulderPresent(getX(), getY() + 1)))) {
                Protester *p = getWorld()->protesterPresent(getX(), getY(), 3);
                if (p != nullptr) {
                    p->isAnnoyed(2);
                    setAlive(false);
                }
                else {
                    moveTo(getX(), getY() + 1);
                    traveldist--;
                }
            }
            else {
                setAlive(false);
            }
        }
        if (getDirection() == down) {
            if (!(getWorld()->earthPresent(getX(), getY() - 1) && !(getWorld()->boulderPresent(getX(), getY() - 1)))) {
                Protester *p = getWorld()->protesterPresent(getX(), getY(), 3);
                if (p != nullptr) {
                    p->isAnnoyed(2);
                    setAlive(false);
                }
                else {
                    moveTo(getX(), getY() - 1);
                    traveldist--;
                }
            }
            else {
                setAlive(false);
            }
        }
    }
    else {
        setAlive(false);
        return;
    }
}

void Squirt::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  GOODIES implementation
///////////////////////////////////////////////////////////////////////////

Goodies::Goodies(StudentWorld *world, int imageID, int x, int y)
                : Actor(world, imageID, x, y, right, 1.0, 2) {
}

Goodies::~Goodies() {
    
}

void Goodies::doSomething() {
    //do nothing!
}

void Goodies::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  BARRELS OF OIL implementation
///////////////////////////////////////////////////////////////////////////

BarrelOfOil::BarrelOfOil(StudentWorld *world, int x, int y)
                        : Goodies(world, TID_BARREL, x, y){
    setVisible(false);
}

BarrelOfOil::~BarrelOfOil() {
    
}

void BarrelOfOil::doSomething() {
    if (!getAlive()) return;
    else if (isVisible() == false && getWorld()->tunnelmanPresent(getX(), getY(), 4)) {
        setVisible(true);
        return;
    }
    else if (isVisible() == true && getWorld()->tunnelmanPresent(getX(), getY(), 3)) {
        setAlive(false);
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->getTunnelMan()->incBarrels();
        return;
    }
    if (getWorld()->getTunnelMan()->getBarrels() == getWorld()->numInLevel("o")) {
    }
}

void BarrelOfOil::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  GOLD NUGGET implementation
///////////////////////////////////////////////////////////////////////////

GoldNugget::GoldNugget(StudentWorld *world, int x, int y, bool visible, bool perm)
                      : Goodies(world, TID_GOLD, x, y){
    if (visible == false) {
        setVisible(false);
    }
    permanent = perm;
    ticks = 0;
}

GoldNugget::~GoldNugget() {
    
}

void GoldNugget::doSomething() {
    if (!getAlive()) return;
    
    if (isVisible() == false && getWorld()->tunnelmanPresent(getX(), getY(), 4)) {
        setVisible(true);
        return;
    }
    
    if (getWorld()->tunnelmanPresent(getX(), getY(), 3) && permanent == true) {
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        getWorld()->getTunnelMan()->incNuggets();
    }
    if (permanent == false) {
        Protester* p = getWorld()->protesterPresent(getX(), getY(), 3);
        if (p != nullptr) {
            setAlive(false);
            p->pickupNugget();
        }
        
        
    }
    if (permanent == false) {
        ticks++;
    }
    if (ticks == 100) {
        setVisible(false);
        setAlive(false);
    }
}

void GoldNugget::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  SONAR KIT implementation
///////////////////////////////////////////////////////////////////////////

SonarKit::SonarKit(StudentWorld *world, int x, int y)
                      : Goodies(world, TID_SONAR, x, y) {
    Tmax = max(100,static_cast<int>(300 - 10* (getWorld()->getLevel())));
    ticks = 0;
}

SonarKit::~SonarKit() {
    
}

void SonarKit::doSomething() {
    if (!getAlive()) return;
    
    ticks++;
    if (ticks == Tmax) {
        setVisible(false);
        setAlive(false);
    }
    if (getWorld()->tunnelmanPresent(getX(), getY(), 3)) {
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incSonar();
        getWorld()->increaseScore(75);
        return;
    }
    
}

void SonarKit::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  WATER POOL implementation
///////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(StudentWorld *world, int x, int y)
                      : Goodies(world, TID_WATER_POOL, x, y){
    Tmax = max(100,static_cast<int>(300 - 10* (getWorld()->getLevel())));
    ticks = 0;
}

WaterPool::~WaterPool() {
    
}

void WaterPool::doSomething() {
    if (!getAlive()) return;
    
    ticks++;
    if (ticks == Tmax) {
        setVisible(false);
        setAlive(false);
    }
    if (getWorld()->tunnelmanPresent(getX(), getY(), 3)) {
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incWater();
        getWorld()->increaseScore(100);
        return;
    }
}

void WaterPool::isAnnoyed(int ptslost) {
    //do nothing!
}

///////////////////////////////////////////////////////////////////////////
//  PROTESTOR implementation
///////////////////////////////////////////////////////////////////////////

Protester::Protester(StudentWorld *world, int imageID)
                    : Actor(world, imageID, 60, 60, left, 1.0, 0) {
}

Protester::~Protester() {
    
}

void Protester::doSomething() {
    //do nothing!
}

void Protester::isAnnoyed(int ptslost) {
    //do nothing!
}

int Protester::numSquaresToMove() {
    return 8 + (rand() % (53));
}

int Protester::ticksToWait() {
    return max(0,static_cast<int>(3 - getWorld()->getLevel()/4));
}

void Protester::faceAndStep() {
    if (getWorld()->getTunnelMan()->getX() > getX()) {
        setDirection(right);
        moveTo(getX() + 1, getY());
    }
    if (getWorld()->getTunnelMan()->getX() < getX()) {
        setDirection(left);
        moveTo(getX() - 1, getY());
    }
    if (getWorld()->getTunnelMan()->getY() > getY()) {
        setDirection(up);
        moveTo(getX(), getY() + 1);
    }
    if (getWorld()->getTunnelMan()->getY() < getY()) {
        setDirection(down);
        moveTo(getX(), getY() - 1);
    }
}

void Protester::stepInDir() {
    if (getDirection() == left ) {
        moveTo(getX() - 1, getY());
    }
    if (getDirection() == right) {
        moveTo(getX() + 1, getY());
    }
    if (getDirection() == up ) {
        moveTo(getX(), getY() + 1);
    }
    if (getDirection() == down ) {
        moveTo(getX(), getY() - 1);
    }
}

bool Protester::cantMove() {
    if (getDirection() == down) {
        if ((getWorld()->boulderPresent(getX(), getY() - 1)) || (getWorld()->earthPresent(getX(), getY() - 1))) {
            return true;
        }
        return false;
    }
    if (getDirection() == up) {
        if ((getWorld()->boulderPresent(getX(), getY() + 1)) || (getWorld()->earthPresent(getX(), getY() + 1))) {
            return true;
        }
        return false;
    }
    if (getDirection() == left) {
        if ((getWorld()->boulderPresent(getX() - 1, getY())) || (getWorld()->earthPresent(getX() - 1, getY()))) {
            return true;
        }
        return false;
    }
    if (getDirection() == right) {
        if ((getWorld()->boulderPresent(getX() + 1, getY())) || (getWorld()->earthPresent(getX() + 1, getY()))) {
            return true;
        }
        return false;
    }
    return false;
}

Actor::Direction Protester::getrandomdir() {
    int pick = rand() % 4;
    
    if (pick == 0) {
        return left;
    }
    if (pick == 1) {
        return right;
    }
    if (pick == 2) {
        return up;
    }
    else return down;
}

bool Protester::perpValid(Actor::Direction &dir) {
    if (getDirection() == left || getDirection() == right) {
        if (!(getWorld()->boulderPresent(getX(), getY() - 1)) && !(getWorld()->earthPresent(getX(), getY() - 1))
            && !(getWorld()->boulderPresent(getX(), getY() + 1)) && !(getWorld()->earthPresent(getX(), getY() + 1))) {
            int pick = rand() % 2;
            if (pick == 0) {
                dir = up;
            }
            else {
                dir = down;
            }
            return true;
        }
        if (!(getWorld()->boulderPresent(getX(), getY() - 1)) && !(getWorld()->earthPresent(getX(), getY() - 1))) {
            dir = down;
            return true;
        }
        if (!(getWorld()->boulderPresent(getX(), getY() + 1)) && !(getWorld()->earthPresent(getX(), getY() + 1))) {
            dir = up;
            return true;
        }
        return false;
    }
    if (getDirection() == up || getDirection() == down) {
        if (!(getWorld()->boulderPresent(getX() + 1, getY())) && !(getWorld()->earthPresent(getX() + 1, getY()))
            && !(getWorld()->boulderPresent(getX() - 1, getY())) && !(getWorld()->earthPresent(getX() - 1, getY()))) {
            int pick = rand() % 2;
            if (pick == 0) {
                dir = left;
            }
            else {
                dir = right;
            }
            return true;
        }
        if (!(getWorld()->boulderPresent(getX() + 1, getY())) && !(getWorld()->earthPresent(getX() + 1, getY()))) {
            dir = right;
            return true;
        }
        if (!(getWorld()->boulderPresent(getX() - 1, getY())) && !(getWorld()->earthPresent(getX() - 1, getY()))) {
            dir = left;
            return true;
        }
        return false;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//  REGULAR PROTESTOR implementation
////////////////////////////////////////////////////////////////////////////////

RegularProtester::RegularProtester(StudentWorld *world)
                                   : Protester(world, TID_PROTESTER)  {
    hitpoints = 5;
    leavefield = false;
    noperp = 200;
    shutup = 15;
    tickwait = ticksToWait();
    numtomove = numSquaresToMove();
}

RegularProtester::~RegularProtester() {
}

void RegularProtester::doSomething() {
    if (!getAlive()) return;
    
    if (tickwait > 0) {
        tickwait--;
        return;
    }
    
    tickwait = ticksToWait();
    noperp--;
    shutup--;
    
    if (leavefield == true) {
        if (getX() == 60 && getY() == 60) {
            setAlive(false);
            return;
        }
        else {
            return;
        }
    }
    
    if (getWorld()->tunnelmanPresent(getX(), getY(), 4) && getWorld()->facingTunnelMan(getX(), getY(), getDirection())) {
        if (shutup < 0) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getTunnelMan()->isAnnoyed(2);
            shutup = 15;
            return;
        }
        return;
    }
    
    if (getWorld()->lineOfSight(getX(), getY()) && !(getWorld()->tunnelmanPresent(getX(), getY(), 4))) {
        faceAndStep();
        numtomove = 0;
        return;
    }
    
    numtomove--;
    
    Actor::Direction d;
    if (numtomove <=0) {
        Actor::Direction dir;
        bool cantMove, earthPresent, boulderPresent;
        int newX, newY;
        do {
            newX = getX();
            newY = getY();
            
            dir = getrandomdir();
                
            if (dir == left) {
                newX -= 1;
            }
            else if (dir == right) {
                newX += 1;
            }
            else if (dir == up) {
                newY += 1;
            }
            else if (dir == down) {
                newY -= 1;
            }
            earthPresent = getWorld()->earthPresent(newX, newY);
            boulderPresent = getWorld()->boulderPresent(newX, newY);
            cantMove = earthPresent || boulderPresent;
        } while(cantMove);
        setDirection(dir);
        numtomove = numSquaresToMove();
    }
    
    if (perpValid(d) && noperp <= 0) {
        setDirection(d);
        numtomove = numSquaresToMove();
        noperp = 200;
    }
    
    if (cantMove()) {
        numtomove = 0;
    }
    else {
        stepInDir();
    }
    
    
}

void RegularProtester::isAnnoyed(int ptslost) {
    if (leavefield == true) {
        return;
    }
    
    hitpoints = hitpoints - ptslost;
    if (hitpoints < 1) {
        leavefield = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        setVisible(false);
        setAlive(false);
        tickwait = 0;
        if (ptslost == 100) {
            getWorld()->increaseScore(500);
        }
        else {
            getWorld()->increaseScore(100);
        }
    }
    else {
        tickwait = max(20,static_cast<int>(100 - getWorld()->getLevel()*10));
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        
    }
}

void RegularProtester::pickupNugget() {
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(25);
    leavefield = true;
}

/////////////////////////////////////////////////////////////////////////////////
//  HARDCORE PROTESTOR implementation
//////////////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(StudentWorld *world)
                                    : Protester(world, TID_HARD_CORE_PROTESTER) {
    hitpoints = 20;
    leavefield = false;
    tickwait = ticksToWait();;
    shutup = 15;
    numtomove = numSquaresToMove();
    noperp = 200;
}

HardcoreProtester::~HardcoreProtester() {
    
}

void HardcoreProtester::doSomething() {
    if (!getAlive()) return;
    
    if (tickwait > 0) {
        tickwait--;
        return;
    }
    
    tickwait = ticksToWait();
    noperp--;
    shutup--;
    
    if (leavefield == true) {
        if (getX() == 60 && getY() == 60) {
            setAlive(false);
            return;
        }
        else {
            return;
        }
    }
    
    if (getWorld()->tunnelmanPresent(getX(), getY(), 4) && getWorld()->facingTunnelMan(getX(), getY(), getDirection())) {
        if (shutup < 0) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getTunnelMan()->isAnnoyed(2);
            shutup = 15;
            return;
        }
        return;
    }
    
    if (getWorld()->lineOfSight(getX(), getY()) && !(getWorld()->tunnelmanPresent(getX(), getY(), 4))) {
        faceAndStep();
        numtomove = 0;
        return;
    }
    
    numtomove--;
    Actor::Direction d;
    
    if (numtomove <=0) {
        Actor::Direction dir;
        bool cantMove, earthPresent, boulderPresent;
        int newX, newY;
        do {
            newX = getX();
            newY = getY();
            
            dir = getrandomdir();
                
            if (dir == left) {
                newX -= 1;
            }
            else if (dir == right) {
                newX += 1;
            }
            else if (dir == up) {
                newY += 1;
            }
            else if (dir == down) {
                newY -= 1;
            }
            earthPresent = getWorld()->earthPresent(newX, newY);
            boulderPresent = getWorld()->boulderPresent(newX, newY);
            cantMove = earthPresent || boulderPresent;
        } while(cantMove);
        setDirection(dir);
        numtomove = numSquaresToMove();
    }
    
    if (perpValid(d) && noperp <= 0) {
        setDirection(d);
        numtomove = numSquaresToMove();
        noperp = 200;
    }
    
    if (cantMove()) {
        numtomove = 0;
    }
    else {
        stepInDir();
    }
    
}

void HardcoreProtester::isAnnoyed(int ptslost) {
    if (leavefield == true) {
        return;
    }
    
    hitpoints = hitpoints - ptslost;
    if (hitpoints < 1) {
        leavefield = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        setVisible(false);
        setAlive(false);
        tickwait = 0;
        if (ptslost == 100) {
            getWorld()->increaseScore(500);
        }
        else {
            getWorld()->increaseScore(250);
        }
    }
    else {
        tickwait = max(20,static_cast<int>(100 - getWorld()->getLevel()*10));
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        
    }
}

void HardcoreProtester::pickupNugget() {
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(50);
    tickwait = max(50, static_cast<int>(100 - getWorld()->getLevel()*10));
    
}
