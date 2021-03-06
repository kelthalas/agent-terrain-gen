#include "generator.h"

#include "heightmap.h"
#include "agents/iagent.h"
#include "agents/coastlineagent.h"
#include "agents/mountainagent.h"
#include "agents/smoothagent.h"
#include "agents/riveragent.h"
#include "agents/beachagent.h"

#include <QFile>
#include <iostream>

Generator::Generator() : m_heightmap{nullptr}, m_phaseAgents(0), m_isRunning{false},
                        m_hasStarted{false}, m_nextPhase{0}, m_tickCount{0}
{
    setOnFinish([](){});
}

Generator::~Generator()
{
    for (auto& phase : m_phaseAgents) {
        for (auto& agent : phase) {
            delete agent;
        }
    }
}

void Generator::load(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Generator::load : impossible de charger le fichier");
    }

    QTextStream in(&file);
    m_phaseAgents.clear();
    m_phaseAgents.push_back(std::vector<IAgent*>());
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line == QString("newPhase")) {
            m_phaseAgents.push_back(std::vector<IAgent*>());
        } else {
            QStringList l = line.split("!", QString::SkipEmptyParts);
            if (l.at(0) == "CoastLine") {
                IAgent* agent = new CoastLineAgent();
                agent->fromString(line);
                m_phaseAgents.back().push_back(agent);
            } else if (l.at(0) == "Mountain") {
                IAgent* agent = new MountainAgent();
                agent->fromString(line);
                m_phaseAgents.back().push_back(agent);
            } else if (l.at(0) == "Smooth") {
                IAgent* agent = new SmoothAgent();
                agent->fromString(line);
                m_phaseAgents.back().push_back(agent);
            }else if (l.at(0) == "River") {
                IAgent* agent = new RiverAgent();
                agent->fromString(line);
                m_phaseAgents.back().push_back(agent);
            }
            else if (l.at(0) == "Beach") {
                IAgent* agent = new BeachAgent();
                agent->fromString(line);
                m_phaseAgents.back().push_back(agent);
            }
        }
    }
    reset();
}

void Generator::save(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Generator::save : impossible de sauvegarder le fichier");
    }

    QTextStream out(&file);
    for (unsigned int i = 0; i < m_phaseAgents.size(); ++i) {
        if (i != 0) {
            out << "newPhase\n";
        }
        for (auto& agent : m_phaseAgents[i]) {
            out << agent->toString() << "\n";
        }
    }
}

void Generator::addAgent(int phase, IAgent* agent)
{
    while((unsigned int)phase >= m_phaseAgents.size()){
        m_phaseAgents.push_back(std::vector<IAgent*>());
    }
    m_phaseAgents[phase].push_back(agent);

}

std::vector<IAgent*> Generator::getAgents(int phase)
{
    return m_phaseAgents[phase];
}

void Generator::reset()
{
    m_isRunning = false;
    m_hasStarted = false;
    m_nextPhase = 0;
    m_tickCount = 0;
    m_agents.clear();
    if (m_heightmap != nullptr) {
        m_heightmap->reset();
    }
}

void Generator::tick()
{
    m_hasStarted = true;
    if (m_agents.size() == 0) {
        if ((unsigned int)m_nextPhase < m_phaseAgents.size()) {
            populateNextStep();
        } else {
            m_isRunning = false;
            m_onFinish();
            m_heightmap->smoothAll();
            m_heightmap->computeNormals();
        }
    }
    auto it = m_agents.begin();
    while (it != m_agents.end()) {
        (*it)->run();
        if ((*it)->isDead()) {
            it = m_agents.erase(it);
        } else {
            ++it;
        }
    }
    m_tickCount++;
}

void Generator::runAll()
{
    m_heightmap->setComputeNormals(false);
    m_isRunning = true;
    while (m_isRunning) {
        tick();
    }
    m_heightmap->setComputeNormals(true);
    m_heightmap->smoothAll();

    m_heightmap->computeNormals();
}

bool Generator::isStarted() const
{
    return m_hasStarted;
}

bool Generator::isOver() const
{
    return m_hasStarted && !m_isRunning;
}

HeightMap* Generator::getHeightMap() const
{
    return m_heightmap;
}

void Generator::setHeightMap(HeightMap* heightmap)
{
    m_heightmap = heightmap;
}

int Generator::getHeightMapSize() const
{
    if (m_heightmap != nullptr) {
        m_heightmap->getSize();
    }
    return 0;
}

void Generator::setHeightMapSize()
{
    // TODO
}

void Generator::populateNextStep()
{
    m_agents.clear();
    if ((unsigned int)m_nextPhase < m_phaseAgents.size()) {
        for (unsigned int i = 0; i < m_phaseAgents[m_nextPhase].size(); ++i) {
            auto& templateAgent = m_phaseAgents[m_nextPhase][i];
            int count = templateAgent->getValue("count");
            for (int j = 0; j < count; ++j) {
                m_agents.push_back(templateAgent->copy());
                m_agents.back()->spawn(m_heightmap);
            }
        }
        m_nextPhase++;
    }
}

void Generator::setOnFinish(std::function<void()> onFinish)
{
    m_onFinish = onFinish;
}

int Generator::getPhasesCount()
{
    return m_phaseAgents.size();
}

int Generator::getCurrentPhase() const
{
    return m_nextPhase - 1;
}

int Generator::getTickCount() const
{
    return m_tickCount;
}
