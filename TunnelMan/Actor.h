#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject {
    public:
        Actor(StudentWorld *world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
        virtual ~Actor();
        bool getAlive();
        void setAlive(bool isalive);
        virtual void doSomething() = 0;
        virtual void isAnnoyed(int ptslost) = 0;
        StudentWorld* getWorld();
    private:
        StudentWorld *m_world;
        bool alive;
};

class TunnelMan : public Actor {
    public:
        TunnelMan(StudentWorld *world);
        virtual ~TunnelMan();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
        void fireSquirt();
        void userInput();
        int getNuggets();
        void incNuggets();
        int getSonar();
        void incSonar();
        int getWater();
        void incWater();
        int getBarrels();
        void incBarrels();
        int getHitPoints();
        
    private:
        int hitpoints;
        int unitswater;
        int sonarcharge;
        int goldnuggets;
        int barrelsoil;
};

class Earth : public Actor {
    public:
        Earth(StudentWorld *world, int x, int y);
        virtual ~Earth();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
};

class Boulder : public Actor {
    public:
        Boulder(StudentWorld *world, int x, int y);
        virtual ~Boulder();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
    private:
        bool stable;
        bool fall;
        int ticks;
};

class Squirt : public Actor {
    public:
        Squirt(StudentWorld *world, int x, int y, Direction dir);
        virtual ~Squirt();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
    private:
        int traveldist;
};

class Goodies : public Actor {
    public:
        Goodies(StudentWorld *world, int imageID, int x, int y);
        virtual ~Goodies();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
};

class BarrelOfOil : public Goodies {
    public:
        BarrelOfOil(StudentWorld *world, int x, int y);
        virtual ~BarrelOfOil();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
};

class GoldNugget : public Goodies {
    public:
        GoldNugget(StudentWorld *world, int x, int y, bool visible, bool perm);
        virtual ~GoldNugget();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
    private:
        bool permanent;
        int ticks;
};

class SonarKit : public Goodies {
    public:
        SonarKit(StudentWorld *world, int x, int y);
        virtual ~SonarKit();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
    private:
        int Tmax;
        int ticks;
};

class WaterPool : public Goodies {
    public:
        WaterPool(StudentWorld *world, int x, int y);
        virtual ~WaterPool();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
    private:
        int Tmax;
        int ticks;
};

class Protester : public Actor {
    public:
        Protester(StudentWorld *world, int imageID);
        virtual ~Protester();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
        virtual void pickupNugget() = 0;
        int numSquaresToMove();
        int ticksToWait();
        void faceAndStep();
        void stepInDir();
        bool cantMove();
        Actor::Direction getrandomdir();
        bool perpValid(Actor::Direction &dir);
};

class RegularProtester : public Protester {
    public:
        RegularProtester(StudentWorld *world);
        virtual ~RegularProtester();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
        virtual void pickupNugget();
    private:
        int tickwait;
        int shutup;
        int hitpoints;
        bool leavefield;
        int noperp;
        int numtomove;
};

class HardcoreProtester : public Protester {
    public:
        HardcoreProtester(StudentWorld *world);
        virtual ~HardcoreProtester();
        virtual void doSomething();
        virtual void isAnnoyed(int ptslost);
        virtual void pickupNugget();
    private:
        int tickwait;
        int shutup;
        int hitpoints;
        bool leavefield;
        int noperp;
        int numtomove;
};

#endif
