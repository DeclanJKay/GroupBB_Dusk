#pragma once

#include <vector>
#include <functional>
#include "EnemyType.hpp"

// A single wave: how many enemies, how fast they spawn, and which type.
struct Wave {
    int       numEnemies = 0;
    float     spawnInterval = 1.0f;        // seconds between spawns
    EnemyType enemyType = EnemyType::Basic;
};

// A level is just a list of waves.
struct Level {
    std::vector<Wave> waves;
};

// Returns all level/wave definitions for the run.
std::vector<Level> getWaveDefinitions();

// ---------------------------------------------
// WaveManager
// ---------------------------------------------
class WaveManager {
public:
    WaveManager();

    // Reset back to level 0, wave 0, no active wave.
    void reset();

    // Called every frame:
    //  - dt: delta time
    //  - activeEnemies: number of enemies currently alive on the map
    //  - spawnFn: callback to actually spawn an enemy of a given type
    void update(float dt,
        int activeEnemies,
        const std::function<void(EnemyType)>& spawnFn);

    // Called when the player presses E to start the next wave.
    void startNextWave();

    bool isWaitingForPlayer() const;     // ready, waiting for E
    bool isSpawningWave() const;         // currently spawning enemies
    bool hasFinishedAllWaves() const;    // run is fully done

    // 0-based indices (add +1 for UI)
    int getCurrentLevelIndex() const { return _currentLevel; }
    int getCurrentWaveIndex()  const { return _currentWave; }
    int getWavesInCurrentLevel() const;

private:
    enum class State {
        WaitingForStart,   // waiting for player to press E
        Spawning,          // spawning enemies on a timer
        WaitingForClear,   // all enemies for this wave spawned, waiting for 0 alive
        FinishedAll        // no more waves/levels
    };

    const Wave& currentWave() const;

    std::vector<Level> _levels;

    int   _currentLevel = 0;
    int   _currentWave = 0;
    float _spawnTimer = 0.f;
    int   _spawnedThisWave = 0;
    State _state = State::WaitingForStart;
};
