// WaveGeneration.hpp
#pragma once

#include <vector>
#include <functional>
#include "EnemyType.hpp"

// A single wave configuration (point budget + which enemies can appear)
struct WaveConfig {
    int   levelIndex = 0;      // 0..4
    int   waveIndex = 0;      // 0..4
    int   pointBudget = 0;      // how many "points" we can spend
    float spawnInterval = 1.0f;   // seconds between spawns

    std::vector<EnemyType> allowedTypes;  // pool we pick from randomly

    bool      hasBoss = false;          // true only on last wave of level
    EnemyType bossType = EnemyType::Boss1;
};

// Handles progression across 5 levels * 5 waves, using a point system.
class WaveManager {
public:
    // Only defined ONCE here – remove any duplicates below
    static constexpr int kTotalLevels = 5;
    static constexpr int kWavesPerLevel = 5;

    WaveManager();

    // Reset to Level 1, Wave 1 waiting for player to press E
    void reset();

    // Update the wave logic:
    //  - dt: delta time
    //  - currentEnemyCount: how many enemies are alive in the scene
    //  - spawnEnemy: callback invoked to spawn a new enemy
    void update(float dt,
        int currentEnemyCount,
        const std::function<void(EnemyType)>& spawnEnemy);

    // UI helpers
    bool isWaitingForPlayer()      const { return _waitingForPlayer; }
    bool hasFinishedAllWaves()     const { return _allWavesDone; }

    int  getCurrentLevelIndex()    const { return _currentLevel; }     // 0-based
    int  getCurrentWaveIndex()     const { return _currentWave; }      // 0-based
    int  getWavesInCurrentLevel()  const { return kWavesPerLevel; }

    // Called when the player presses E to begin / continue
    void startNextWave();

private:
    int   _currentLevel = 0;   // 0..4
    int   _currentWave = 0;   // 0..4
    bool  _waitingForPlayer = true;
    bool  _allWavesDone = false;

    WaveConfig _currentConfig{};
    int        _remainingPoints = 0;   // points left to spend this wave
    float      _timeSinceSpawn = 0.f;
    bool       _bossSpawnedThisWave = false;

    // Random engine shared across waves
    unsigned int _rngSeed = 0u;

    void      setupCurrentWave();
    EnemyType chooseRandomEnemyType();
};
