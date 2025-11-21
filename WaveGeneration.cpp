// WaveGeneration.cpp
#include "WaveGeneration.hpp"
#include "EnemyStats.hpp"

#include <random>
#include <algorithm>
#include <cassert>

// Small helper: clamp float
static float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

static EnemyType boss_for_level(int levelIndex) {
    switch (levelIndex) {
    case 0: return EnemyType::Boss1;
    case 1: return EnemyType::Boss2;
    case 2: return EnemyType::Boss3;
    case 3: return EnemyType::Boss4;
    case 4: default: return EnemyType::Boss5;
    }
}

// Decide which enemy types are unlocked on a given level
static std::vector<EnemyType> allowed_for_level(int levelIndex) {
    std::vector<EnemyType> types;

    // Level 1
    types.push_back(EnemyType::Basic);
    types.push_back(EnemyType::Fast);

    if (levelIndex >= 1) {
        // Level 2 unlock
        types.push_back(EnemyType::Tank);
    }
    if (levelIndex >= 2) {
        // Level 3 unlocks
        types.push_back(EnemyType::shortRanged);
        types.push_back(EnemyType::Exploder);
    }
    if (levelIndex >= 3) {
        // Level 4 unlocks
        types.push_back(EnemyType::Medium);
        types.push_back(EnemyType::RangedMelee);
        types.push_back(EnemyType::FastExploder);
    }
    if (levelIndex >= 4) {
        // Level 5 unlocks
        types.push_back(EnemyType::LongRange);
        types.push_back(EnemyType::HeavyTank);
    }

    return types;
}

// Build a point budget + spawn interval for this level/wave
static WaveConfig make_wave_config(int levelIndex, int waveIndex) {
    WaveConfig cfg{};
    cfg.levelIndex = levelIndex;
    cfg.waveIndex = waveIndex;

    // Base budget increases with level and wave:
    // e.g. L1W1 ~12 points, L5W5 much higher.
    int base = 10;
    int perLevel = 8 * levelIndex;   // 0, 8, 16, ...
    int perWave = 3 * waveIndex;    // 0, 3, 6, 9, 12
    cfg.pointBudget = base + perLevel + perWave;

    // Spawn interval gets slightly faster over time, clamped to 0.25s
    float baseInterval = 1.0f;
    float speedUp = 0.05f * (levelIndex + waveIndex); // small reduction
    cfg.spawnInterval = clampf(baseInterval - speedUp, 0.25f, 1.0f);

    cfg.allowedTypes = allowed_for_level(levelIndex);

    // Boss only on the last wave of the level
    cfg.hasBoss = (waveIndex == WaveManager::kWavesPerLevel - 1);
    if (cfg.hasBoss) {
        cfg.bossType = boss_for_level(levelIndex);
    }

    return cfg;
}

// --------------------------
// WaveManager implementation
// --------------------------

WaveManager::WaveManager() {
    // Simple deterministic seed so behaviour is repeatable between runs.
    // We can swap this to std::random_device later for true randomness.
    _rngSeed = 123456u;
    reset();
}

void WaveManager::reset() {
    _currentLevel = 0;
    _currentWave = 0;
    _waitingForPlayer = true;
    _allWavesDone = false;

    _timeSinceSpawn = 0.f;
    _bossSpawnedThisWave = false;

    setupCurrentWave();
}

void WaveManager::setupCurrentWave() {
    if (_currentLevel >= kTotalLevels) {
        _allWavesDone = true;
        return;
    }

    _currentConfig = make_wave_config(_currentLevel, _currentWave);
    _remainingPoints = _currentConfig.pointBudget;
    _timeSinceSpawn = 0.f;
    _bossSpawnedThisWave = false;
}

EnemyType WaveManager::chooseRandomEnemyType() {
    // Filter to enemies we can afford with remaining points
    std::vector<EnemyType> candidates;
    candidates.reserve(_currentConfig.allowedTypes.size());

    for (auto t : _currentConfig.allowedTypes) {
        EnemyStats st = get_enemy_stats(t);
        int cost = std::max(st.cost, 1);
        if (cost <= _remainingPoints) {
            candidates.push_back(t);
        }
    }

    // If nothing fits in the remaining budget, end the wave
    if (candidates.empty()) {
        _remainingPoints = 0;
        // Just return something; caller will see 0 points and not spawn further
        return _currentConfig.allowedTypes.front();
    }

    // Simple LCG-style RNG for repeatable behaviour
    _rngSeed = _rngSeed * 1664525u + 1013904223u;
    unsigned int idx = _rngSeed % static_cast<unsigned int>(candidates.size());
    return candidates[idx];
}

void WaveManager::startNextWave() {
    if (_allWavesDone) return;

    // Only meaningful if we were waiting
    _waitingForPlayer = false;
    _timeSinceSpawn = 0.f;
}

void WaveManager::update(float dt,
    int currentEnemyCount,
    const std::function<void(EnemyType)>& spawnEnemy)
{
    if (_allWavesDone) return;

    // If we're waiting for the player to press E, do nothing
    if (_waitingForPlayer) return;

    // Check if the current wave has fully finished:
    //  - no points left
    //  - no enemies alive
    //  - boss has spawned (if this is a boss wave)
    bool waveBudgetDone = (_remainingPoints <= 0);
    bool bossRequirementMet =
        (!_currentConfig.hasBoss) || _bossSpawnedThisWave;

    if (waveBudgetDone && bossRequirementMet && currentEnemyCount == 0) {
        // Advance wave/level
        _currentWave++;
        if (_currentWave >= kWavesPerLevel) {
            _currentWave = 0;
            _currentLevel++;
        }

        if (_currentLevel >= kTotalLevels) {
            _allWavesDone = true;
            _waitingForPlayer = false;
            return;
        }

        setupCurrentWave();
        _waitingForPlayer = true;   // wait for E again
        return;
    }

    // Still mid-wave: handle spawn timing
    _timeSinceSpawn += dt;
    if (_timeSinceSpawn < _currentConfig.spawnInterval) {
        return;
    }
    _timeSinceSpawn = 0.f;

    // Boss spawn: on boss waves we spawn the boss first, then normal enemies
    if (_currentConfig.hasBoss && !_bossSpawnedThisWave) {
        spawnEnemy(_currentConfig.bossType);
        _bossSpawnedThisWave = true;
        return;
    }

    // No points left -> nothing else to spawn, just wait for enemies to die/escape
    if (_remainingPoints <= 0) {
        return;
    }

    // Pick a type that fits our remaining budget
    EnemyType type = chooseRandomEnemyType();
    EnemyStats stats = get_enemy_stats(type);
    int cost = std::max(stats.cost, 1);

    if (cost > _remainingPoints) {
        // Should be rare because chooseRandomEnemyType has already filtered,
        // but guard anyway.
        _remainingPoints = 0;
        return;
    }

    _remainingPoints -= cost;
    spawnEnemy(type);
}
