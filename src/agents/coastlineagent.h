#ifndef COASTLINEAGENT_H
#define COASTLINEAGENT_H

#include "abstractagent.h"
#include "../noise/noise.h"

#include <map>
#include <memory>
#include <list>
#include <utility>

class CoastLineAgent : public AbstractAgent
{
public:
    CoastLineAgent();

    void spawn(HeightMap* world);
    void run();
    bool isDead();

    QString getTypeName() const;
    std::vector<QString> getProperties();
    std::unique_ptr<IAgent> copy();

private:
    void spawn(HeightMap* world, int verticeLimit);
    float getScore(int x, int y);
    std::unique_ptr<CoastLineAgent> copyCoastLine();
    std::pair<int, int> getRandomPosition();
    std::pair<int, int> getRandomInlandPosition();
    std::pair<int, int> getRandomDirection();
    float getSquareDistance(int x, int y, int x2, int y2);

    int m_life;
    int m_vertices;

    HeightMap* m_world;
    std::unique_ptr<CoastLineAgent> m_children[2];

    int m_x;
    int m_y;
    bool m_terminal;
    bool m_root;

    int m_defaultDirectionX;
    int m_defaultDirectionY;
    int m_attractorX;
    int m_attractorY;
    int m_repulsorX;
    int m_repulsorY;
    int m_maxLife;


    SimplexNoise m_noise;
};

#endif // COASTLINEAGENT_H
