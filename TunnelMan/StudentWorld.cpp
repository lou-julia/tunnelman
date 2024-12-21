#include "StudentWorld.h"
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  GIVEN FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir),m_tunnelman(nullptr), m_earth(NULL), m_actors(NULL) 
{
    protesters = 0;
    ticks = 0;
}

StudentWorld::~StudentWorld()
{
   cleanUp();
}

int StudentWorld::init() {
    Tmax = std::max(25,200 - static_cast<int>(getLevel()));
    int level = getLevel();
    prob = min(90, level*10 + 30);
    
    //CREATE EARTH OBJECTS
    {
        m_earth.resize(VIEW_HEIGHT - SPRITE_HEIGHT);
        for (int i = 0; i < VIEW_HEIGHT - SPRITE_HEIGHT; i++) {
            m_earth[i].resize(VIEW_WIDTH);
            for (int j = 0; j < VIEW_WIDTH; j++) {
                if (j < 30 || j > 33 || i < 4 || i > 59) {
                    m_earth[i][j] = new Earth(this, j, i);
                }
            }
        }
    }
    //CREATE TUNNELMAN OBJECT
    {
        m_tunnelman = new TunnelMan(this);
    }
    //CREATE ACTOR OBJECTS
    {
        spawnBoulder(numInLevel("b"));
        spawnNugget(numInLevel("g"));
        spawnBarrel(numInLevel("o"));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    setDisplaytext();
    
    //CALL ACTORS TO DO SOMETHING
    for (int i = 0; i < m_actors.size(); i++) {
        m_actors[i]->doSomething();
    }
    
    int G = getLevel()*25+300;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(1, G);
    uniform_int_distribution<> distr2(1, 5);
    if (distr(gen) == G) {
        if (distr2(gen) == 1) {
            spawnSonar();
        }
        else {
            spawnWater();
        }
    }
    
    //ADD PROTESTERS
    ticks++;
    if (protesters == 0) {
        spawnProtester();
        protesters++;
    }
    if (protesters < numInLevel("p") && ticks > Tmax) {
            spawnProtester();
            protesters++;
            ticks = 0;
    }
    
    //REMOVE DEAD
    removeDead();
    
    //CALL TUNNELMAN TO DO SOMETHING
    if (m_tunnelman->getAlive()) {
        m_tunnelman->doSomething();
    }
    else {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    //ADVANCE TO NEXT LEVEL
    if (getTunnelMan()->getBarrels() == numInLevel("o")) {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
    
}

void StudentWorld::cleanUp() {
    //DESTRUCT EARTH
    for (int i = 0; i < VIEW_HEIGHT - SPRITE_HEIGHT; i++) {
        for (int j = 0; j < VIEW_WIDTH; j++) {
            if (m_earth[i][j] != nullptr) {
                delete m_earth[i][j];
            }
        } m_earth[i].clear();
    } m_earth.clear();
    
    protesters = 0;
    
    //DESTRUCT TUNNELMAN
    if (m_tunnelman != nullptr) {
        delete m_tunnelman;
    }
    
    //DESTRUCT ACTOR OBJECTS
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i] != nullptr) {
            delete m_actors[i];
        }
    } m_actors.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  DISPLAY TEXT FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////

void StudentWorld::setDisplaytext() {
    int level = getLevel();
    int lives = getLives();
    int health = getTunnelMan()->getHitPoints();
    int squirts = getTunnelMan()->getWater();
    int gold = getTunnelMan()->getNuggets();
    int barrelsleft = numInLevel("o") - getTunnelMan()->getBarrels();
    int sonar = getTunnelMan()->getSonar();
    int score = getScore();
    
    string s = formatting(level, lives, health, squirts, gold, barrelsleft, sonar, score);
    setGameStatText(s);
}

string StudentWorld::formatting(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score) {
    health = health * 10;
    ostringstream oss;
    oss << "Lvl: " << setw(2) << setfill(' ') << level << "  "
        << "Lives: " << lives << "  "
        << "Hlth: " << setw(3) << setfill(' ') << health << "%  "
        << "Wtr: " << setw(2) << setfill(' ') << squirts << "  "
        << "Gld: " << setw(2) << setfill(' ') << gold << "  "
        << "Oil Left: " << setw(2) << setfill(' ') << barrelsLeft << "  "
        << "Sonar: " << setw(2) << setfill(' ') << sonar << "  "
        << "Scr: " << setw(6) << setfill('0') << score;

    return oss.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SPAWNERS
/////////////////////////////////////////////////////////////////////////////////////////////

void StudentWorld::spawnBoulder(int numBoulders) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrX(0, 60);
    uniform_int_distribution<> distrY(20, 56);
    for (int i = 0; i < numBoulders; i++) {
        int x = distrX(gen); 
        int y = distrY(gen);
        while (!validPos(x, y, m_actors) && !(x < 30 || x > 33 || y < 4 || y > 59)) {
            x = distrX(gen);
            y = distrY(gen);
        }
        m_actors.push_back(new Boulder(this, x, y));
        removeEarth(x, y);
    }
    
}

void StudentWorld::spawnNugget(int numNuggets) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrX(0, 60);
    uniform_int_distribution<> distrY(0, 56);
    for (int i = 0; i < numNuggets; i++) {
        int x = distrX(gen);
        int y = distrY(gen);
        while (!validPos(x, y, m_actors)) {
            x = distrX(gen);
            y = distrY(gen);
        }
        m_actors.push_back(new GoldNugget(this, x, y, false, true));
    }
}

void StudentWorld::dropNugget(int x, int y) {
    m_actors.push_back(new GoldNugget(this, x,y, true, false));
}

void StudentWorld::spawnBarrel(int numBarrels) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrX(0, 60);
    uniform_int_distribution<> distrY(0, 56);
    for (int i = 0; i < numBarrels; i++) {
        int x = distrX(gen);
        int y = distrY(gen);
        while (!validPos(x, y, m_actors)) {
            x = distrX(gen);
            y = distrY(gen);
        }
        m_actors.push_back(new BarrelOfOil(this, x, y));
    }
}

void StudentWorld::spawnSquirt(int x, int y, Actor::Direction dir) {
    m_actors.push_back(new Squirt(this, x,y, dir));
}

void StudentWorld::spawnSonar() {
    m_actors.push_back(new SonarKit(this, 0, 60));
}
void StudentWorld::spawnWater() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrX(0, 60);
    uniform_int_distribution<> distrY(0, 56);
        int x = distrX(gen);
        int y = distrY(gen);
        while (!validPos(x, y, m_actors) || earthPresent(x, y)) {
            x = distrX(gen);
            y = distrY(gen);
        }
        m_actors.push_back(new WaterPool(this, x, y));
}

void StudentWorld::spawnProtester() {
    int pick = rand() % 100;
    if (pick < prob) {
        m_actors.push_back(new HardcoreProtester(this));
    }
    else {
        m_actors.push_back(new RegularProtester(this));
    }
}

///////////////////////////////////////////////////////////////////////////
//  REMOVAL TOOLS
///////////////////////////////////////////////////////////////////////////

bool StudentWorld::removeEarth (int x, int y) {
    bool removed = false;
    int a = VIEW_HEIGHT - SPRITE_HEIGHT;
    int b = VIEW_WIDTH;
    
    for (int i = y; i < y + SPRITE_HEIGHT; i++) {
        for (int j = x; j < x + SPRITE_WIDTH; j++) {
            if (i > -1 && i < a && j > -1 && j < b && m_earth[i][j] != nullptr) {
                delete m_earth[i][j];
                m_earth[i][j] = nullptr;
                removed = true;
            }
        }
    }
    return removed;
}

void StudentWorld::removeDead() {
    for (int i = 0; i < m_actors.size(); i++) {
        if (!m_actors[i]->getAlive()) {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//  CHECK IF PRESENT TOOLS
///////////////////////////////////////////////////////////////////////////

bool StudentWorld::boulderPresent(int x, int y) {
    bool boulder = false;
    
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->getID() == TID_BOULDER) {
            if (distance(x, y, m_actors[i]->getX(), m_actors[i]->getY()) < 3) {
                boulder = true;
            }
        }
    }
    return boulder;
}

bool StudentWorld::tunnelmanPresent(int x, int y, int dist) {
    if (distance(x,y,m_tunnelman->getX(),m_tunnelman->getY()) <= dist) {
        return true;
    }
    return false;
}

Protester* StudentWorld::protesterPresent(int x, int y, int dist) {
    for (int i = 0; i < m_actors.size(); i++) {
        if ((m_actors[i]->getID() == TID_PROTESTER) || m_actors[i]->getID() == TID_HARD_CORE_PROTESTER)
        if (distance(x,y,m_actors[i]->getX(),m_actors[i]->getY()) <= dist) {
            return dynamic_cast<Protester*>(m_actors[i]);
        }
    }
    return nullptr;
}

bool StudentWorld::earthPresent(int x, int y) {
    if (x < 0 || y < 0 || x + SPRITE_WIDTH > 64 || y + SPRITE_HEIGHT > 64) {
        return true;
    }
    else if (y + SPRITE_HEIGHT > 60 && y + SPRITE_HEIGHT <= 64) {
        return false;
    }
    for (int i = y; i < y + SPRITE_HEIGHT; i++) {
        for (int j = x; j < x + SPRITE_WIDTH; j++) {
            if (m_earth[i][j] != nullptr) {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::boulderSupported(int x, int y) {
    if (x < 0 || y < 1 || x + SPRITE_WIDTH > 64 || y + SPRITE_HEIGHT > 64) {
        return false;
    }
    for (int i = x; i < x + SPRITE_WIDTH; i++)
    {
        if (m_earth[y - 1][i] != nullptr)
            return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
//  STUPID MATHS STUFF
///////////////////////////////////////////////////////////////////////////

int StudentWorld::numInLevel(string item) {
    int level = getLevel();
    
    if (item == "b") {
        return min(level / 2 + 2, 9);
    }
    if (item == "g") {
        return max(5 - level / 2, 2);
    }
    if (item == "o") {
        return min(2 + level, 21);
    }
    if (item == "p") {
        return min(2 + level * 3/2, 15);
    }
    return 0;
}

double StudentWorld::distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}

bool StudentWorld::validPos(int x, int y, const std::vector<Actor*>& actors) {
    for (int i = 0; i < actors.size(); i++) {
        if (distance(x, y, actors[i]->getX(), actors[i]->getY()) < 6) {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////
//  GAME PLAY TOOLS
///////////////////////////////////////////////////////////////////////////

void StudentWorld::illuminate(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) < 12) {
            m_actors[i]->setVisible(true);
        }
    }
}

bool StudentWorld::facingTunnelMan(int x, int y, Actor::Direction dir) {
    if (dir == Actor::left) {
        if (x > getTunnelMan()->getX()) {
            return true;
        }
        return false;
    }
    else if (dir == Actor::right) {
        if (x < getTunnelMan()->getX()) {
            return true;
        }
        return false;
    }
    else if (dir == Actor::up) {
        if (y < getTunnelMan()->getY()) {
            return true;
        }
        return false;
    }
    else if (dir == Actor::down) {
        if (y > getTunnelMan()->getY()) {
            return true;
        }
        return false;
    }
    else {
        return false;
    }
}

bool StudentWorld::lineOfSight(int x,  int y) {
    if (x == getTunnelMan()->getX()) {
        if (y > getTunnelMan()->getY()) {
            for (int i = getTunnelMan()->getY(); i < y; i++) {
                if (boulderPresent(x, i) || earthPresent(x, i)) {
                    return false;
                }
            }
            return true;
        }
        else {
            for (int i = y; i < getTunnelMan()->getY(); i++) {
                if (boulderPresent(x, i) || earthPresent(x, i)) {
                    return false;
                }
            }
            return true;
        }
    }
    if (y == getTunnelMan()->getY()) {
        if (x > getTunnelMan()->getX()) {
            for (int i = getTunnelMan()->getX(); i < x; i++) {
                if (boulderPresent(i, y) || earthPresent(i, y)) {
                    return false;
                }
            }
            return true;
        }
        else {
            for (int i = y; i < getTunnelMan()->getY(); i++) {
                if (boulderPresent(i, y) || earthPresent(i, y)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

TunnelMan* StudentWorld::getTunnelMan() {
    return m_tunnelman;
}
