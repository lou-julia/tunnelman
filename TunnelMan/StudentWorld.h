#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"

#include <string>
#include <vector>
#include <algorithm>

class StudentWorld : public GameWorld {
    public:
        //given functions
        StudentWorld(std::string assetDir);
        virtual ~StudentWorld();
        virtual int init();
        virtual int move();
        virtual void cleanUp();
    
        //game text tool
        void setDisplaytext();
        std::string formatting(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score);
    
        //spawners
        void spawnBoulder(int numBoulders);
        void spawnNugget(int numNuggets);
        void dropNugget(int x, int y);
        void spawnBarrel(int numBarrels);
        void spawnSquirt(int x, int y, Actor::Direction dir);
        void spawnSonar();
        void spawnWater();
        void spawnProtester();
        
        //removal tools
        bool removeEarth(int x, int y);
        void removeDead();
    
        //check if present tools
        bool boulderPresent(int x, int y);
        bool tunnelmanPresent(int x, int y, int dist);
        Protester* protesterPresent(int x, int y, int dist);
        bool earthPresent(int x, int y);
        bool boulderSupported(int x, int y);
    
        //calculation tools
        int numInLevel(std::string item);
        double distance(int x1, int y1, int x2, int y2);
        bool validPos(int x, int y, const std::vector<Actor*>& actors);
    
        //game play tools
        void illuminate(int x, int y);
        bool facingTunnelMan(int x, int y, Actor::Direction dir);
        bool lineOfSight(int x, int y);
    
        //getters
        TunnelMan* getTunnelMan();
    
    private:
        TunnelMan* m_tunnelman;
        std::vector<std::vector<Earth*>> m_earth;
        std::vector<Actor*> m_actors;
        int ticks;
        int Tmax;
        int prob;
        int protesters;
};

#endif
