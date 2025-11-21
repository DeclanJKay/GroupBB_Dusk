#include "WaveGeneration.hpp"

// -----------------------------
// Level / wave definitions
// -----------------------------
std::vector<Level> getWaveDefinitions()
{
    std::vector<Level> levels;

    // For now: 1 level with 5 waves (last is a boss).
    Level l1;

    l1.waves.push_back(Wave{ 5, 1.2f, EnemyType::Basic });
    l1.waves.push_back(Wave{ 7, 1.0f, EnemyType::Basic });
    l1.waves.push_back(Wave{ 8, 0.9f, EnemyType::Fast });
    l1.waves.push_back(Wave{ 10, 0.8f, EnemyType::Tank });
    l1.waves.push_back(Wave{ 1, 0.0f, EnemyType::Boss1 }); // boss wave

    levels.push_back(l1);

    return levels;
}

// -----------------------------
// WaveManager
// -----------------------------

WaveManager::WaveManager()
    : _levels(getWaveDefinitions())
{
    reset();
}

void WaveManager::reset()
{
    _currentLevel = 0;
    _currentWave = 0;
    _spawnTimer = 0.f;
    _spawnedThisWave = 0;

    if (_levels.empty() || _levels[0].waves.empty()) {
        _state = State::FinishedAll;
    }
    else {
        _state = State::WaitingForStart;
    }
}

const Wave& WaveManager::currentWave() const
{
    return _levels[_currentLevel].waves[_currentWave];
}

int WaveManager::getWavesInCurrentLevel() const
{
    if (_levels.empty()) return 0;
    return static_cast<int>(_levels[_currentLevel].waves.size());
}

bool WaveManager::isWaitingForPlayer() const
{
    return _state == State::WaitingForStart;
}

bool WaveManager::isSpawningWave() const
{
    return _state == State::Spawning;
}

bool WaveManager::hasFinishedAllWaves() const
{
    return _state == State::FinishedAll;
}

void WaveManager::startNextWave()
{
    if (_state != State::WaitingForStart) return;
    if (_levels.empty()) return;

    _spawnTimer = 0.f;
    _spawnedThisWave = 0;
    _state = State::Spawning;
}

void WaveManager::update(
    float dt,
    int activeEnemies,
    const std::function<void(EnemyType)>& spawnFn)
{
    if (_state == State::FinishedAll || _levels.empty()) return;

    // --- Spawning phase ---
    if (_state == State::Spawning) {
        _spawnTimer += dt;
        const Wave& wave = currentWave();

        // Spawn as many as the timer allows
        while (_spawnedThisWave < wave.numEnemies &&
            _spawnTimer >= wave.spawnInterval) {

            _spawnTimer -= wave.spawnInterval;
            spawnFn(wave.enemyType);
            _spawnedThisWave++;
        }

        // Once we've spawned them all, wait until the map is clear
        if (_spawnedThisWave >= wave.numEnemies) {
            _state = State::WaitingForClear;
        }
    }

    // --- Waiting for all enemies to be gone ---
    if (_state == State::WaitingForClear) {
        if (activeEnemies == 0) {
            // Advance wave / level
            int lastWaveIndex = static_cast<int>(_levels[_currentLevel].waves.size()) - 1;
            int lastLevelIndex = static_cast<int>(_levels.size()) - 1;

            if (_currentWave < lastWaveIndex) {
                // Next wave in same level
                _currentWave++;
                _state = State::WaitingForStart;
            }
            else if (_currentLevel < lastLevelIndex) {
                // Next level
                _currentLevel++;
                _currentWave = 0;
                _state = State::WaitingForStart;
            }
            else {
                // No more levels/waves
                _state = State::FinishedAll;
            }
        }
    }
}
