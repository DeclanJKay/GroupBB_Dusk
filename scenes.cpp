#include "scenes.hpp"
#include "player.hpp"
#include "tile_level_loader/level_system.hpp"
#include "game_parameters.hpp"
#include "TDEnemy.hpp"
#include "EnemyStats.hpp"
#include "TurretType.hpp"
#include "TurretStats.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <unordered_map>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>


using ls = LevelSystem;
using param = Parameters;

// ------------------------------------------------------------
// Global scene handles (defined here, declared in scenes.hpp)
// ------------------------------------------------------------
std::shared_ptr<Scene>      Scenes::safehouse = nullptr;
std::shared_ptr<Scene>      Scenes::tower_defence = nullptr;
std::shared_ptr<Scene>      Scenes::end = nullptr;
std::shared_ptr<RunContext> Scenes::runContext = nullptr;

// ---------------------------------------------------------------------
// Key edge-trigger helper
//  - Returns true only on the exact frame the key goes from up -> down
//  - Used for actions like placing turrets and swapping scenes
// ---------------------------------------------------------------------
bool keyPressedOnce(sf::Keyboard::Key key) {
    static std::unordered_map<sf::Keyboard::Key, bool> keyStates;

    bool isPressed = sf::Keyboard::isKeyPressed(key);
    bool wasPressed = keyStates[key];

    keyStates[key] = isPressed;

    return (isPressed && !wasPressed);
}


// ============================================================================
// SafehouseScene (roguelite side)
// ============================================================================
void SafehouseScene::load() {
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });
    _background.setFillColor(sf::Color(30, 15, 15));

    // UI font 
    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("SAFEHOUSE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

    // Player HP text
    _hpText.setFont(_font);
    _hpText.setCharacterSize(24);
    _hpText.setFillColor(sf::Color::White);
    _hpText.setPosition(20.f, 60.f);
    _hpText.setString("HP: 0/0");

    // Wave / level text
    _waveText.setFont(_font);
    _waveText.setCharacterSize(24);
    _waveText.setFillColor(sf::Color::White);
    _waveText.setPosition(20.f, 90.f);
    _waveText.setString("Level 1 - Wave 1/5");

    // Money UI
    _moneyText.setFont(_font);
    _moneyText.setCharacterSize(24);
    _moneyText.setFillColor(sf::Color::Yellow);
    _moneyText.setPosition(param::game_width - 200.f, 20.f);
    _moneyText.setString("Money: $" + std::to_string(Scenes::runContext->currency));

    // Attack arc
    _attackArcShape.setPointCount(3);
    _attackArcShape.setFillColor(sf::Color(255, 255, 255, 60));

    // First time only: create player AND initialise shop
    if (!_initialised) {
        _player = std::make_shared<Player>();
        _player->set_use_tile_collision(false); // Safehouse ignores tiles
        _player->set_position({
            param::game_width * 0.5f,
            param::game_height * 0.5f
            });

        _shop.init(
            _font,
            sf::Vector2f(100.f, static_cast<float>(param::game_height) - 180.f)
        );

        _initialised = true;
    }

    // Inventory text setup
    _inventoryText.setFont(_font);
    _inventoryText.setCharacterSize(18);
    _inventoryText.setFillColor(sf::Color::White);

    // Hint text ("Press E to buy")
    _shopHintText.setFont(_font);
    _shopHintText.setCharacterSize(18);
    _shopHintText.setFillColor(sf::Color::White);
    _shopHintText.setString("Press E to buy");

    // Hook existing player into entity list
    _entities.clear();
    if (_player) {
        _entities.push_back(_player);
    }
}


void SafehouseScene::tick_simulation(float dt) {
    // If we’ve never been loaded / initialised, nothing to do
    if (!_initialised || !_player) return;

    // Cooldown for contact damage still counts down
    if (_damageCooldown > 0.f) {
        _damageCooldown -= dt;
        if (_damageCooldown < 0.f) _damageCooldown = 0.f;
    }

    // Move invaders and apply contact damage, same as in update()
    update_invaders(dt);

	update_enemy_bullets(dt); //keep enemy bullets updating too
    // Optional: if player can die off-screen, keep this
    if (_player->is_dead()) {
        _invaders.clear();

        if (!Scenes::end) {
            Scenes::end = std::make_shared<EndScene>();
        }

        GameSystem::set_active_scene(Scenes::end);
    }
}


void SafehouseScene::spawn_invaders(const std::vector<int>& enemyTypes) {
    for (int typeId : enemyTypes) {
        Invader inv;

        // Convert int -> EnemyType
        EnemyType type = static_cast<EnemyType>(typeId);


        inv.type = type;

        // Look up shared stats
        EnemyStats stats = get_enemy_stats(type);

        inv.type = type;
        inv.speed = stats.speed;
        inv.hp = stats.hp;
        inv.maxHp = stats.hp;
        inv.baseColor = stats.color;
        inv.isRanged = stats.isRanged;
        inv.rangeLimit = stats.rangeLimit;
        inv.damage = std::max(stats.damage, 1);
        inv.explodes = stats.explodes;
        inv.explosionRadius = stats.explosionRadius;
        inv.shootCooldown = 0.f; // ready to shoot

        inv.shape.setRadius(stats.radius);
        inv.shape.setOrigin(stats.radius, stats.radius);
        inv.shape.setFillColor(stats.color);

        // Stack them roughly in a column on the right
        float x = static_cast<float>(param::game_width) - 100.f;
        float y = 150.f + static_cast<float>(_invaders.size()) * 50.f;
        if (y > param::game_height - 100.f) {
            y = 150.f;
        }

        inv.shape.setPosition(x, y);

        _invaders.push_back(inv);
    }
}


// Move invaders towards the player, handle contact damage and hit flash
void SafehouseScene::update_invaders(float dt) {
    if (_invaders.empty() || !_player) return;

    sf::Vector2f playerPos = _player->get_position();
    float        playerR = _player->get_radius();

    for (auto& inv : _invaders) {
        sf::Vector2f pos = inv.shape.getPosition();
        sf::Vector2f dir = playerPos - pos;

        float lenSq = dir.x * dir.x + dir.y * dir.y;
        float len = (lenSq > 0.f) ? std::sqrt(lenSq) : 0.f;

        // ------------------------
        // Movement towards player
        // ------------------------
        if (lenSq > 1.0f) {
            sf::Vector2f norm = dir / len;

            // If ranged, we can stop closing quite so aggressively when in range
            bool shouldMove = true;
            if (inv.isRanged && inv.rangeLimit > 0.f) {
                // stop pushing too close once inside ~80% of range
                if (len <= inv.rangeLimit * 0.8f) {
                    shouldMove = false;
                }
            }

            if (shouldMove) {
                pos += norm * inv.speed * dt;
                inv.shape.setPosition(pos);
            }
        }

        // ------------------------
        // Ranged attack behaviour
        // ------------------------
        if (inv.isRanged && inv.rangeLimit > 0.f) {
            inv.shootCooldown -= dt;
            if (inv.shootCooldown < 0.f) inv.shootCooldown = 0.f;

            // Only shoot if player within range
            if (len > 0.f && len <= inv.rangeLimit && inv.shootCooldown <= 0.f) {
                EnemyBullet b;
                b.damage = (inv.damage > 0) ? inv.damage : 1;
                b.speed = 220.f;
                b.ttl = 3.f;

                // Small bullet, coloured like the invader
                b.shape.setRadius(4.f);
                b.shape.setOrigin(4.f, 4.f);
                b.shape.setFillColor(inv.baseColor);
                b.shape.setPosition(pos);

                sf::Vector2f shotDir = dir / len; // already have len from above
                b.vel = shotDir;

                _enemyBullets.push_back(b);

                // Cooldown between shots
                inv.shootCooldown = 1.2f; // tweak as needed
            }
        }

        // ------------------------
        // Contact damage to player
        // ------------------------
        sf::Vector2f diff = playerPos - pos;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        float combinedR = playerR + inv.shape.getRadius();

        if (distSq <= combinedR * combinedR && _damageCooldown <= 0.f) {
            // Use damage from EnemyStats so stronger enemies hurt more
            int dmg = (inv.damage > 0) ? inv.damage : 1;

            _player->take_damage(dmg);
            _damageCooldown = 1.0f; // 1 second of invulnerability
        }

        // ------------------------
        // Hit flash 
        // ------------------------
        if (inv.flashTimer > 0.f) {
            inv.flashTimer -= dt;
            float t = std::max(inv.flashTimer / 0.15f, 0.f);

            sf::Color c;
            c.r = static_cast<sf::Uint8>(
                inv.baseColor.r + (255 - inv.baseColor.r) * t
                );
            c.g = static_cast<sf::Uint8>(
                inv.baseColor.g + (255 - inv.baseColor.g) * t
                );
            c.b = static_cast<sf::Uint8>(
                inv.baseColor.b + (255 - inv.baseColor.b) * t
                );
            c.a = 255;
            inv.shape.setFillColor(c);
        }
        else {
            inv.shape.setFillColor(inv.baseColor);
        }
    }
}


void SafehouseScene::update_enemy_bullets(float dt) {
    if (_enemyBullets.empty() || !_player) return;

    std::vector<EnemyBullet> alive;
    alive.reserve(_enemyBullets.size());

    sf::Vector2f playerPos = _player->get_position();
    float        playerR = _player->get_radius();

    for (auto& b : _enemyBullets) {
        b.ttl -= dt;
        if (b.ttl <= 0.f) {
            continue; // bullet expired
        }

        // Move bullet
        b.shape.move(b.vel * b.speed * dt);

        // Check collision with player
        sf::Vector2f diff = playerPos - b.shape.getPosition();
        float distSq = diff.x * diff.x + diff.y * diff.y;
        float combinedR = playerR + b.shape.getRadius();

        bool hitPlayer = false;
        if (distSq <= combinedR * combinedR) {
            // Only apply damage if player's invulnerability is down
            if (_damageCooldown <= 0.f) {
                int dmg = (b.damage > 0) ? b.damage : 1;
                _player->take_damage(dmg);
                _damageCooldown = 0.6f; 
            }
            hitPlayer = true;
        }

        if (!hitPlayer) {
            alive.push_back(b);
        }
    }

    _enemyBullets.swap(alive);
}


void SafehouseScene::update(const float& dt) {
    // Update player entity (movement etc.)
    Scene::update(dt);

    // TD simulation continues in the background
    if (Scenes::tower_defence) {
        auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);
        td->tick_simulation(dt);

        // Pull escaped enemy types and spawn them as invaders here
        auto escapedTypes = td->consume_escaped_enemies();
        if (!escapedTypes.empty()) {
            spawn_invaders(escapedTypes);
        }
    }

    // --- Update HP text from player health ---
    if (_player) {
        int hp = _player->get_health();
        int maxHp = _player->get_max_health();

        _hpText.setString(
            "HP: " + std::to_string(hp) + "/" + std::to_string(maxHp)
        );
    }

    // Update money display
    _moneyText.setString("Money: $" + std::to_string(Scenes::runContext->currency));


    // --- Wave / Level UI (TowerDefenceScene) ---
    if (Scenes::tower_defence) {
        auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);

        if (!td->hasFinishedAllWaves()) {
            int levelIdx = td->getCurrentLevelIndex() + 1; // 0-based -> 1-based
            int waveIdx = td->getCurrentWaveIndex() + 1;
            int totalWaves = td->getWavesInCurrentLevel();

            std::string extra;
            if (td->isWaitingForPlayer()) {
                extra = "  (Press F in TD to start)";
            }

            _waveText.setString(
                "Level " + std::to_string(levelIdx) +
                " - Wave " + std::to_string(waveIdx) +
                "/" + std::to_string(totalWaves) +
                extra
            );
        }
        else {
            _waveText.setString("All waves complete");
        }

        // --- Decide if the shop should be available this frame ---
        _canUseShop = _invaders.empty();   // must have no enemies in safehouse

        if (Scenes::tower_defence) {
            auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);
            if (!td->isWaitingForPlayer()) {
                // TD wave is actively running -> hide/lock the shop
                _canUseShop = false;
            }
        }

        // --- Shop hint text logic ---
        _showShopHint = false;
        if (_canUseShop && _player) {
            sf::Vector2f playerPos = _player->get_position();

            // Ask the shop if there is an active item near the player
            if (_shop.hasItemNear(playerPos)) {   // NEW Shop method
                _showShopHint = true;

                // Position the hint slightly above the player
                sf::Vector2f textPos = playerPos;
                textPos.y -= 40.f;
                _shopHintText.setPosition(textPos);
            }
        }



        // --- Debug / testing: spawn specific invader types with number keys ---
        auto spawnTestEnemy = [this](EnemyType type)
            {
                std::vector<int> v;
                v.push_back(static_cast<int>(type));
                spawn_invaders(v);
            };

        if (keyPressedOnce(sf::Keyboard::Num1)) {
            spawnTestEnemy(EnemyType::Basic);
        }
        if (keyPressedOnce(sf::Keyboard::Num2)) {
            spawnTestEnemy(EnemyType::Fast);
        }
        if (keyPressedOnce(sf::Keyboard::Num3)) {
            spawnTestEnemy(EnemyType::Tank);
        }
        if (keyPressedOnce(sf::Keyboard::Num4)) {
            spawnTestEnemy(EnemyType::shortRanged);
        }
        if (keyPressedOnce(sf::Keyboard::Num5)) {
            spawnTestEnemy(EnemyType::Exploder);
        }
        if (keyPressedOnce(sf::Keyboard::Num6)) {
            spawnTestEnemy(EnemyType::Medium);
        }
        if (keyPressedOnce(sf::Keyboard::Num7)) {
            spawnTestEnemy(EnemyType::RangedMelee);
        }
        if (keyPressedOnce(sf::Keyboard::Num8)) {
            spawnTestEnemy(EnemyType::FastExploder);
        }
        if (keyPressedOnce(sf::Keyboard::Num9)) {
            spawnTestEnemy(EnemyType::LongRange);
        }

    }

    // --- Attack + damage timers ---
    if (_attackCooldown > 0.f)    _attackCooldown -= dt;
    if (_attackEffectTimer > 0.f) _attackEffectTimer -= dt;
    if (_damageCooldown > 0.f)    _damageCooldown -= dt;

    // Melee attack in a 90-degree arc towards the mouse
    bool doAttack = false;
    if (_attackCooldown <= 0.f && keyPressedOnce(sf::Keyboard::Space)) {
        doAttack = true;
        _attackCooldown = 0.5f;
    }

    if (doAttack && _player) {
        const float attackRadius = 80.f;
        const float attackRadiusSq = attackRadius * attackRadius;
        const float cosHalfAngle = 0.70710678f; // cos(45°) = 90° cone

        sf::Vector2f center = _player->get_position();

        // Mouse position in world coords
        sf::RenderWindow& window = GameSystem::get_window();
        sf::Vector2i mousePix = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePix);

        sf::Vector2f toMouse = mousePos - center;
        float        lenSqMouse = toMouse.x * toMouse.x + toMouse.y * toMouse.y;
        sf::Vector2f forward(1.f, 0.f);

        if (lenSqMouse > 1.f) {
            float lenMouse = std::sqrt(lenSqMouse);
            forward = toMouse / lenMouse;
        }

        std::vector<Invader> survivors;
        survivors.reserve(_invaders.size());

        for (auto& inv : _invaders) {
            sf::Vector2f d = inv.shape.getPosition() - center;
            float        lenSq = d.x * d.x + d.y * d.y;

            bool hit = false;

            if (lenSq < 1.f) {
                // Very close = auto-hit
                hit = true;
            }
            else if (lenSq <= attackRadiusSq) {
                float        len = std::sqrt(lenSq);
                sf::Vector2f nd = d / len;

                float dot = nd.x * forward.x + nd.y * forward.y;

                if (dot >= cosHalfAngle) {
                    // Inside 90° arc
                    hit = true;
                }
            }

            if (hit) {
                // Basic attack does 1 damage
                inv.hp -= 1;
                inv.flashTimer = 0.15f; // brief flash

                // Exploders deal AoE damage to player on death
                if (inv.hp <= 0 && inv.explodes && _player) {
                    sf::Vector2f centerInv = inv.shape.getPosition();
                    sf::Vector2f toPlayer = _player->get_position() - centerInv;
                    float        distSq = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
                    float        blastR = inv.explosionRadius + _player->get_radius();
                    float        blastRSq = blastR * blastR;

                    if (distSq <= blastRSq && _damageCooldown <= 0.f) {
                        int dmg = (inv.damage > 0) ? inv.damage : 1;
                        _player->take_damage(dmg);
                        _damageCooldown = 1.0f; // reuse same i-frames as contact
                    }
                }

                if (inv.hp <= 0 && Scenes::runContext) {
                    EnemyStats stats = get_enemy_stats(inv.type);
                    int reward = std::max(stats.cost / 2, 1); // or any value you prefer
                    Scenes::runContext->currency += reward;
                    std::cout << "Safehouse kill: +" << reward
                        << " gold (total " << Scenes::runContext->currency << ")\n";
                }
            }

            if (inv.hp > 0) {
                survivors.push_back(inv);
            }
            // if hp <= 0: enemy dies (and may have exploded above)
        }

        _invaders.swap(survivors);

        // Visual arc setup
        const float cosA = 0.70710678f;
        const float sinA = 0.70710678f;

        sf::Vector2f left(
            forward.x * cosA - forward.y * sinA,
            forward.x * sinA + forward.y * cosA
        );
        sf::Vector2f right(
            forward.x * cosA + forward.y * sinA,
            -forward.x * sinA + forward.y * cosA
        );

        _attackArcShape.setPoint(0, center);
        _attackArcShape.setPoint(1, center + left * attackRadius);
        _attackArcShape.setPoint(2, center + right * attackRadius);

        _attackEffectTimer = 0.12f;
    }

    // --- Shop interaction (buy with E) ---
    if (_canUseShop && _player && Scenes::runContext && keyPressedOnce(sf::Keyboard::E)) {
        sf::Vector2f playerPos = _player->get_position();
        bool bought = _shop.tryPurchaseAt(playerPos, *Scenes::runContext, *_player);
        if (bought) {
            // no reroll here – wave end handles rerolls
        }
    }

    // --- Toggle inventory overlay with I ---
    if (keyPressedOnce(sf::Keyboard::I)) {
        _showInventory = !_showInventory;
    }


    // --- Update invaders and their bullets ---
    update_invaders(dt);
    update_enemy_bullets(dt);

    // --- Death check ---
    if (_player && _player->is_dead()) {
        _invaders.clear();

        if (!Scenes::end) {
            Scenes::end = std::make_shared<EndScene>();
        }

        GameSystem::set_active_scene(Scenes::end);
        return;
    }

    // Swap between Safehouse and Tower Defence using Shift
    if (keyPressedOnce(sf::Keyboard::LShift) || keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::tower_defence);
        return;
    }
}

void SafehouseScene::rerollShop() {
    _shop.regenerateItems();
}

void SafehouseScene::render(sf::RenderWindow& window) {
    window.draw(_background);
    Scene::render(window); // player

    for (const auto& inv : _invaders) {
        window.draw(inv.shape);
    }

    for (const auto& b : _enemyBullets) {
        window.draw(b.shape);
    }

    if (_attackEffectTimer > 0.f) {
        window.draw(_attackArcShape);
    }

    bool tdIsActive = false;
    if (Scenes::tower_defence) {
        auto td = std::static_pointer_cast<TowerDefenceScene>(Scenes::tower_defence);
        tdIsActive = !td->isWaitingForPlayer(); // wave currently running
    }

    // --- Shop UI ---
    if (_canUseShop) {
        _shop.render(window);
    }

    // --- Inventory UI ---
    if (_showInventory && Scenes::runContext) {
        std::string invText = "Inventory:\n";

        int index = 1;
        for (auto t : Scenes::runContext->turretInventory) {
            invText += std::to_string(index) + ") "
                + Shop::turretName(t) + "\n";
            ++index;
        }

        _inventoryText.setString(invText);
        _inventoryText.setPosition(50.f, 150.f);
        window.draw(_inventoryText);
    }

    // --- Shop hint ("Press E to buy") ---
    if (_showShopHint) {
        window.draw(_shopHintText);
    }


    window.draw(_label);
    window.draw(_hpText);   // show HP of player 
    window.draw(_waveText);
    window.draw(_moneyText);

}

// ============================================================================
// TowerDefenceScene
// ============================================================================
void TowerDefenceScene::load() {
    _background.setSize({
        static_cast<float>(param::game_width),
        static_cast<float>(param::game_height)
        });
    _background.setFillColor(sf::Color(5, 5, 20));

    if (!_font.loadFromFile("res/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font: res/fonts/ARIAL.TTF\n";
    }

    _label.setFont(_font);
    _label.setString("TOWER DEFENCE");
    _label.setCharacterSize(32);
    _label.setFillColor(sf::Color::White);
    _label.setPosition(20.f, 20.f);

    // Wave UI text
    _waveText.setFont(_font);
    _waveText.setCharacterSize(24);
    _waveText.setFillColor(sf::Color::White);
    _waveText.setPosition(20.f, 60.f);
    _waveText.setString("Wave 0/0");


    // === MONEY UI ===
    _moneyText.setFont(_font);
    _moneyText.setCharacterSize(24);
    _moneyText.setFillColor(sf::Color::Yellow);
    _moneyText.setPosition(param::game_width - 200.f, 20.f);
    _moneyText.setString("Money: $" + std::to_string(Scenes::runContext->currency));

    // === Controls hint ===
    _controlsText.setFont(_font);
    _controlsText.setCharacterSize(18);
    _controlsText.setFillColor(sf::Color(200, 200, 200));
    _controlsText.setPosition(20.f, 90.f);
    _controlsText.setString("I: Inventory (Safehouse)   E: Place turret   Esc: Cancel");

    const float tileSize = 50.f;

    // Configure level tile colours for TD
    if (!_initialised) {
        ls::set_color(ls::EMPTY, sf::Color(10, 10, 30));
        ls::set_color(ls::WALL, sf::Color(60, 60, 80));
        ls::set_color(ls::WAYPOINT, sf::Color(120, 120, 120));
        ls::set_color(ls::START, sf::Color(80, 255, 80));
        ls::set_color(ls::END, sf::Color(255, 80, 80));

		// Load the TD level file
        ls::load_level(param::td_1, tileSize);

        _turrets.clear();
        _enemies.clear();
        _bullets.clear();
        _enemyPath.clear();
        _escapedEnemyTypes.clear();

        // Build the path (+ tiles) enemies will follow
        build_enemy_path();

        // Reset wave manager at the start of a new run / level
        _waveManager.reset();
        _lastLevelIndex = _waveManager.getCurrentLevelIndex();


        // Create the shared player for TD mode
        _entities.clear();
        _player = std::make_shared<Player>();
        _player->set_use_tile_collision(true);
        _player->set_position({ 150.f, 100.f });
        _entities.push_back(_player);

        _initialised = true;
        std::cout << "[TD] Initialised once.\n";
    }
    else {
        // When re-entering TD, keep existing state and just re-hook the player
        _entities.clear();
        if (_player) {
            _entities.push_back(_player);
        }
        std::cout << "[TD] Resumed with existing state.\n";
    }

    // Placement inventory heading
    _placementHeading.setFont(_font);
    _placementHeading.setCharacterSize(24);
    _placementHeading.setFillColor(sf::Color::White);
    _placementHeading.setString("Choose a turret to place: Space to place turret. Esc to cancel");

    // Range preview default style (transparent circle with outline)
    _rangePreview.setFillColor(sf::Color(0, 0, 0, 0));
    _rangePreview.setOutlineThickness(2.f);
    _rangePreview.setOutlineColor(sf::Color(0, 255, 0, 160));
}
void TowerDefenceScene::tick_simulation(float dt) {
    if (_enemyPath.empty()) return;

    // 1) WaveManager handles spawning when an active wave is running
    _waveManager.update(
        dt,
        static_cast<int>(_enemies.size()),
        [this](EnemyType type)
        {
            // When WaveManager wants a new enemy, spawn it at the start of the path
            const sf::Vector2f startPos = _enemyPath.front();
            _enemies.emplace_back(type, startPos);
        }
    );

    // 2) Normal TD simulation
    update_enemies(dt);
    update_turrets(dt);
    update_bullets(dt);
}

// Open the "place turret" inventory overlay in TD.
// We snapshot the current turretInventory and show up to 9 entries.
void TowerDefenceScene::openPlacementInventory()
{
    _placementInvLines.clear();
    _placementInvTypes.clear();
    _choosingTurret = false;

    if (!Scenes::runContext) return;

    const auto& inv = Scenes::runContext->turretInventory;
    if (inv.empty()) {
        std::cout << "No turrets in inventory to place.\n";
        return;
    }

    const float startY = 230.f;
    const float lineSpacing = 30.f;
    const float x = 90.f;

    std::size_t maxSlots = std::min<std::size_t>(9, inv.size());

    for (std::size_t i = 0; i < maxSlots; ++i) {
        TurretType type = inv[i];
        _placementInvTypes.push_back(type);

        sf::Text line;
        line.setFont(_font);
        line.setCharacterSize(22);
        line.setFillColor(sf::Color::White);

        std::string label =
            std::to_string(i + 1) + ") " + Shop::turretName(type);

        line.setString(label);
        line.setPosition(x, startY + static_cast<float>(i) * lineSpacing);

        _placementInvLines.push_back(line);
    }

    _choosingTurret = !_placementInvTypes.empty();
}

// Update the range preview circle for the currently pending turret.
void TowerDefenceScene::updateRangePreview()
{
    _rangeTileIsValid = false;

    if (!_player || !_hasPendingTurret) return;

    const float tileSize = 50.f;

    sf::Vector2f pos = _player->get_position();
    sf::Vector2i grid(
        static_cast<int>(pos.x / tileSize),
        static_cast<int>(pos.y / tileSize)
    );

    // Centre of this tile in world space
    sf::Vector2f tileWorld =
        ls::get_tile_position(grid) +
        sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

    // Is the tile EMPTY and not already occupied by a turret?
    LevelSystem::Tile tileType;
    try {
        tileType = ls::get_tile(grid);
    }
    catch (...) {
        tileType = ls::WALL; // treat as invalid
    }

    bool canPlace = (tileType == ls::EMPTY);
    if (canPlace) {
        for (const auto& t : _turrets) {
            if (t.getGrid() == grid) {
                canPlace = false;
                break;
            }
        }
    }

    // Get range from turret stats
    TurretStats stats = get_turret_stats(_pendingTurretType);
    float radius = stats.rangeTiles * tileSize;

    _rangePreview.setRadius(radius);
    _rangePreview.setOrigin(radius, radius);
    _rangePreview.setPosition(tileWorld);

    if (canPlace) {
        _rangePreview.setOutlineColor(sf::Color(0, 255, 0, 160)); // green
        _rangeTileIsValid = true;
    }
    else {
        _rangePreview.setOutlineColor(sf::Color(255, 0, 0, 160)); // red
        _rangeTileIsValid = false;
    }
}


// Build list of world-space positions enemies move through (from + tiles)
void TowerDefenceScene::build_enemy_path() {
    _enemyPath.clear();

    const int w = ls::get_width();
    const int h = ls::get_height();
    const float tileSize = 50.f;

    std::vector<sf::Vector2i> waypoints;
    waypoints.reserve(w * h);

    // Collect all WAYPOINT tiles from the level
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            sf::Vector2i grid(x, y);
            if (ls::get_tile(grid) == ls::WAYPOINT) {
                waypoints.push_back(grid);
            }
        }
    }

    if (waypoints.empty()) {
        std::cerr << "No WAYPOINT tiles found for enemy path.\n";
        return;
    }

    // Pick the "start" waypoint: smallest x, then smallest y
    sf::Vector2i start = waypoints[0];
    for (const auto& p : waypoints) {
        if (p.x < start.x || (p.x == start.x && p.y < start.y)) {
            start = p;
        }
    }

    auto index = [w](sf::Vector2i p) {
        return p.y * w + p.x;
        };

    std::vector<bool> visited(static_cast<size_t>(w * h), false);
    std::vector<sf::Vector2i> ordered;
    ordered.reserve(waypoints.size());

    sf::Vector2i current = start;
    ordered.push_back(current);
    visited[static_cast<size_t>(index(current))] = true;

    // 4-connected neighbor directions
    const sf::Vector2i dirs[4] = {
        { 1,  0 },
        { -1, 0 },
        { 0,  1 },
        { 0, -1 }
    };

    // Walk the connected chain of WAYPOINT tiles
    bool extended = true;
    while (extended) {
        extended = false;

        for (const auto& d : dirs) {
            sf::Vector2i next = current + d;
            if (next.x < 0 || next.y < 0 || next.x >= w || next.y >= h) {
                continue;
            }

            if (ls::get_tile(next) == ls::WAYPOINT &&
                !visited[static_cast<size_t>(index(next))]) {

                ordered.push_back(next);
                visited[static_cast<size_t>(index(next))] = true;
                current = next;
                extended = true;
                break;
            }
        }
    }

    // Convert grid coords to world positions (center of each tile)
    _enemyPath.reserve(ordered.size());
    for (const auto& grid : ordered) {
        sf::Vector2f tilePos = ls::get_tile_position(grid);
        _enemyPath.push_back(tilePos + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f));
    }

    std::cout << "Enemy path built with " << _enemyPath.size() << " nodes.\n";
}


void TowerDefenceScene::update_enemies(float dt) {
    if (_enemyPath.size() < 2) return;

    const float tileSize = 50.f;

    std::vector<TDEnemy> alive;
    alive.reserve(_enemies.size());

    for (auto& enemy : _enemies) {
        // Skip enemies that have already been killed by turrets/bullets
        if (enemy.isDead()) {
            continue;
        }

        // Let TDEnemy handle movement + flashing
        bool reachedEnd = enemy.update(dt, _enemyPath, tileSize);

        if (reachedEnd) {
            // Tell the Safehouse what type escaped
            _escapedEnemyTypes.push_back(static_cast<int>(enemy.getType()));
        }
        else {
            alive.push_back(enemy);
        }
    }

    _enemies.swap(alive);
}


void TowerDefenceScene::place_turret(TurretType type) {
    if (!_player || !Scenes::runContext) return;

    const float tileSize = 50.f;

    // --- 1) Snap player position to a tile ---
    sf::Vector2f pos = _player->get_position();
    sf::Vector2i grid(
        static_cast<int>(pos.x / tileSize),
        static_cast<int>(pos.y / tileSize)
    );

    // --- 2) Check tile is valid and EMPTY ---
    LevelSystem::Tile tile;
    try {
        tile = ls::get_tile(grid);
    }
    catch (...) {
        std::cout << "Cannot place turret: grid out of bounds at ("
            << grid.x << ", " << grid.y << ")\n";
        return;
    }

    if (tile != ls::EMPTY) {
        std::cout << "Cannot place turret: tile not EMPTY at ("
            << grid.x << ", " << grid.y << ")\n";
        return;
    }

    // --- 3) Prevent stacking: check if any turret already occupies this grid ---
    for (const auto& t : _turrets) {
        sf::Vector2i existing = t.getGrid();
        if (existing == grid) {
            std::cout << "Cannot place turret: tile already occupied at ("
                << grid.x << ", " << grid.y << ")\n";
            return;
        }
    }

    // --- 4) Must have one copy in inventory ---
    auto& inv = Scenes::runContext->turretInventory;
    auto it = std::find(inv.begin(), inv.end(), type);

    if (it == inv.end()) {
        std::cout << "No turret of this type in inventory to place.\n";
        return;
    }

    // Only now do we consume ONE copy from inventory
    inv.erase(it);

    // --- 5) World position for this tile ---
    sf::Vector2f worldPos = ls::get_tile_position(grid);

    // --- 6) Create the turret ---
    _turrets.emplace_back(grid, worldPos, tileSize, type);
    _turretIncomeTimers.push_back(0.f);

    std::cout << "Placed turret " << static_cast<int>(type)
        << " at grid (" << grid.x << ", " << grid.y << ")\n";
}



// Ask each turret if it wants to fire this frame and spawn bullets
void TowerDefenceScene::update_turrets(float dt) {
    if (_turrets.empty()) return;

    float _incomeTimer = 0.f;
    const float tileSize = 50.f;
    const size_t count = _turrets.size();

    // Per-turret multipliers, start at 1.0 (no buff)
    std::vector<float> damageMult(count, 1.f);
    std::vector<float> fireRateMult(count, 1.f);


    // ---------------------------
    // 1) First pass: apply Buff turret auras
    // ---------------------------
    for (size_t i = 0; i < count; ++i) {
        const TDTurret& buffTurret = _turrets[i];
        const TurretStats& buffStats = buffTurret.getStats();

        if (!buffStats.isBuff) {
            continue; // only Buff turrets grant auras
        }

        // Aura radius in world units
        float buffRadius = buffStats.rangeTiles * tileSize;
        float buffRadiusSq = buffRadius * buffRadius;

        // Centre position of this buff turret (tile centre)
        sf::Vector2i buffGrid = buffTurret.getGrid();
        sf::Vector2f buffPos = ls::get_tile_position(buffGrid)
            + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

        for (size_t j = 0; j < count; ++j) {
            // If you DON'T want the buff turret to buff itself, uncomment:
            // if (i == j) continue;

            sf::Vector2i otherGrid = _turrets[j].getGrid();
            sf::Vector2f otherPos = ls::get_tile_position(otherGrid)
                + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

            sf::Vector2f d = otherPos - buffPos;
            float distSq = d.x * d.x + d.y * d.y;

            if (distSq <= buffRadiusSq) {
                damageMult[j] *= buffStats.buffDamageMult;
                fireRateMult[j] *= buffStats.buffFireRateMult;
            }
        }
    }

    // 1.5) Apply global upgrades from RunContext (level rewards)
    if (Scenes::runContext) {
        float gDmg = Scenes::runContext->turretDamageMult;
        float gRate = Scenes::runContext->turretFireRateMult;

        for (size_t i = 0; i < count; ++i) {
            damageMult[i] *= gDmg;
            fireRateMult[i] *= gRate;
        }
    }


    // ---------------------------
// 2) Second pass: update turrets & spawn bullets
// ---------------------------
// Make sure our income timer vector matches the number of turrets
    if (_turretIncomeTimers.size() < _turrets.size()) {
        _turretIncomeTimers.resize(_turrets.size(), 0.f);
    }

    for (size_t i = 0; i < count; ++i) {
        auto& t = _turrets[i];
        const TurretStats& stats = t.getStats();

        float dmgM = damageMult[i];
        if (dmgM < 0.f) dmgM = 0.f;

        float frM = fireRateMult[i];
        if (frM <= 0.f) frM = 0.01f; // avoid zero/negative

        // --- Income turrets (e.g. Banana Farm)
        if (stats.generatesIncome) {
            // ONLY earn money when there are enemies on the map
            if (!_enemies.empty()) {
                // Use fireInterval as the "tick" period
                _turretIncomeTimers[i] += dt * frM;
                if (_turretIncomeTimers[i] >= stats.fireInterval) {
                    _turretIncomeTimers[i] = 0.f;

                    if (Scenes::runContext) {
                        Scenes::runContext->currency += stats.incomePerTick;
                        std::cout << "Banana Farm income: +" << stats.incomePerTick
                            << " (total " << Scenes::runContext->currency << ")\n";
                    }
                }
            }

            // Income turrets don't shoot bullets
            continue;
        }

        sf::Vector2f bulletPos;
        sf::Vector2f bulletDir;

        // Scale dt to speed up / slow down fire rate via buff
        float dtForTurret = dt * frM;

        // TDTurret handles range, cooldown, target selection.
        if (!t.update(dtForTurret, _enemies, bulletPos, bulletDir)) {
            continue;
        }

        TurretType type = t.getType();
        // Buff turrets themselves don't shoot bullets, they’re just auras.
        if (stats.isBuff) {
            continue;
        }

        // Base damage from stats * buff multiplier
        int bulletDamage = static_cast<int>(stats.damage * dmgM + 0.5f);
        if (bulletDamage < 0) bulletDamage = 0;

        // ---------------------------
        // AOE turret: pulse around itself
        // ---------------------------
        if (type == TurretType::AOE) {
            sf::Vector2i grid = t.getGrid();
            sf::Vector2f centre = ls::get_tile_position(grid)
                + sf::Vector2f(tileSize * 0.5f, tileSize * 0.5f);

            float radius = stats.explosionRadius;
            float radiusSq = radius * radius;

            for (auto& e : _enemies) {
                if (e.isDead()) continue;

                sf::Vector2f enemyPos = e.getPosition();
                float        enemyRad = e.getRadius();

                sf::Vector2f d = enemyPos - centre;
                float distSq = d.x * d.x + d.y * d.y;
                float r = radius + enemyRad;

                if (distSq <= r * r) {
                    if (bulletDamage > 0) {
                        e.applyDamage(bulletDamage);
                    }
                    if (stats.dotDuration > 0.f && stats.damageOverTime > 0.f) {
                        e.applyDot(stats.dotDuration, stats.damageOverTime);
                    }
                    if (stats.slowDownTime > 0.f && stats.slowDownPercent > 0.f) {
                        e.applySlow(stats.slowDownTime, stats.slowDownPercent);
                    }
                    if (stats.stunTime > 0.f) {
                        e.applyStun(stats.stunTime);
                    }
                }
            }

            // Optional: visual flash using a bullet in "explosion only" mode
            if (stats.explosionRadius > 0.f) {
                TDBullet visual(
                    centre,
                    sf::Vector2f(0.f, 0.f),
                    0.f,                 // no movement
                    0,                   // no extra damage
                    stats.bulletTtl,     // ttl not really used in explosion phase
                    stats.explosionRadius,
                    0.f, 0.f,            // no DoT
                    0.f, 0.f,            // no slow
                    0.f                  // no stun
                );
                visual.startExplosionVisual();
                _bullets.push_back(visual);
            }

            continue; // skip normal bullet creation
        }

        // ---------------------------
        // Scatter turret: multiple pellets
        // ---------------------------
        if (type == TurretType::Scatter) {
            int   pelletCount = stats.pelletCount;
            if (pelletCount <= 0) pelletCount = 1;

            float spreadDegrees = stats.spreadAngleDeg;
            if (spreadDegrees < 0.f) spreadDegrees = 0.f;
            const float halfSpreadRad = (spreadDegrees * 3.14159265f / 180.f) * 0.5f;

            float baseAngle = std::atan2(bulletDir.y, bulletDir.x);
            float startAngle = baseAngle - halfSpreadRad;
            float step = (pelletCount > 1)
                ? (2.f * halfSpreadRad) / static_cast<float>(pelletCount - 1)
                : 0.f;

            for (int p = 0; p < pelletCount; ++p) {
                float angle = startAngle + step * static_cast<float>(p);
                sf::Vector2f dir(std::cos(angle), std::sin(angle));

                _bullets.emplace_back(
                    bulletPos,
                    dir,
                    stats.bulletSpeed,
                    bulletDamage,
                    stats.bulletTtl,
                    stats.explosionRadius,
                    stats.dotDuration,
                    stats.damageOverTime,
                    stats.slowDownTime,
                    stats.slowDownPercent,
                    stats.stunTime
                );
            }
        }
        else {
            // ---------------------------
            // All other turrets: one bullet
            // ---------------------------
            _bullets.emplace_back(
                bulletPos,
                bulletDir,
                stats.bulletSpeed,
                bulletDamage,
                stats.bulletTtl,
                stats.explosionRadius,
                stats.dotDuration,
                stats.damageOverTime,
                stats.slowDownTime,
                stats.slowDownPercent,
                stats.stunTime
            );
        }
    }

    // Clean out enemies that died from turret/bullet damage
    _enemies.erase(
        std::remove_if(
            _enemies.begin(), _enemies.end(),
            [](const TDEnemy& e)
            {
                if (e.isDead())
                {
                    // Lookup enemy stats to get reward value
                    EnemyStats stats = get_enemy_stats(e.getType());
                    int reward = std::max(stats.cost / 2, 1); // reward = half cost minimum 1

                    if (Scenes::runContext) {
                        Scenes::runContext->currency += reward;
                        std::cout << "Earned " << reward << " gold! Total: "
                            << Scenes::runContext->currency << "\n";
                    }

                    return true; // remove dead enemy
                }
                return false;
            }
        ),
        _enemies.end()
    );
}

void TowerDefenceScene::generateUpgradeChoices() {
    _upgradeChoices.clear();

    // Pool of possible upgrades
    std::vector<UpgradeType> pool = {
        UpgradeType::TurretDamage,
        UpgradeType::FireRate,
        UpgradeType::TurretCost
    };

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(pool.begin(), pool.end(), rng);

    const float startY = 220.f;
    const float lineSpacing = 40.f;
    const float x = 80.f;

    for (int i = 0; i < 3 && i < static_cast<int>(pool.size()); ++i) {
        UpgradeChoice choice;
        choice.type = pool[i];

        choice.text.setFont(_font);
        choice.text.setCharacterSize(22);
        choice.text.setFillColor(sf::Color::White);

        std::string label;

        switch (i) {
        case 0: label = "1) "; break;
        case 1: label = "2) "; break;
        case 2: label = "3) "; break;
        default: label = "-) "; break;
        }

        switch (choice.type) {
        case UpgradeType::TurretDamage:
            label += "Turret Damage +20%";
            break;
        case UpgradeType::FireRate:
            label += "Fire Rate +20%";
            break;
        case UpgradeType::TurretCost:
            label += "Turret cost -20%";
            break;
        }

        choice.text.setString(label);
        choice.text.setPosition(x, startY + i * lineSpacing);

        _upgradeChoices.push_back(choice);
    }

    _showUpgradeChoices = true;
}

void TowerDefenceScene::applyUpgrade(UpgradeType type) {
    if (!Scenes::runContext) return;

    switch (type) {
    case UpgradeType::TurretDamage:
        Scenes::runContext->turretDamageMult *= 1.2f;   // +20%
        std::cout << "Upgrade: turret damage -> x"
            << Scenes::runContext->turretDamageMult << "\n";
        break;

    case UpgradeType::FireRate:
        Scenes::runContext->turretFireRateMult *= 1.2f; // +20%
        std::cout << "Upgrade: fire rate -> x"
            << Scenes::runContext->turretFireRateMult << "\n";
        break;

    case UpgradeType::TurretCost:
        Scenes::runContext->turretCostMult *= 0.8f;     // -20% cost
        if (Scenes::runContext->turretCostMult < 0.3f)
            Scenes::runContext->turretCostMult = 0.3f;
        std::cout << "Upgrade: turret cost -> "
            << static_cast<int>(Scenes::runContext->turretCostMult * 100.f)
            << "% of base\n";
        break;
    }
}







// Move bullets, apply damage, and cull dead bullets + enemies
void TowerDefenceScene::update_bullets(float dt) {
    if (_bullets.empty()) return;

    std::vector<TDBullet> alive;
    alive.reserve(_bullets.size());

    for (auto& b : _bullets) {
        // TDBullet::update handles movement + collision + enemy damage.
        if (b.update(dt, _enemies)) {
            alive.push_back(b);   // still alive this frame
        }
        // else: bullet expired or hit something -> drop it
    }
    _bullets.swap(alive);

    // Clean out enemies that died from bullet damage
    _enemies.erase(
        std::remove_if(
            _enemies.begin(), _enemies.end(),
            [](const TDEnemy& e)
            {
                if (e.isDead())
                {
                    // Lookup enemy stats to get reward value
                    EnemyStats stats = get_enemy_stats(e.getType());
                    int reward = std::max(stats.cost / 2, 1); // reward = half cost minimum 1

                    if (Scenes::runContext) {
                        Scenes::runContext->currency += reward;
                        std::cout << "Earned " << reward << " gold! Total: "
                            << Scenes::runContext->currency << "\n";
                    }

                    return true; // remove dead enemy
                }
                return false;
            }
        ),
        _enemies.end()
    );
}


// Return and clear list of types of enemies that reached the end of the path
std::vector<int> TowerDefenceScene::consume_escaped_enemies() {
    // SafehouseScene calls this each frame to request "who escaped this tick".
    // We return the current list and then clear it so we don't respawn the same
    // enemies again on the next frame.
    auto result = _escapedEnemyTypes;
    _escapedEnemyTypes.clear();
    return result;
}

void TowerDefenceScene::update(const float& dt) {
    // Update the player in this scene
    Scene::update(dt);

    // Run safehouse simulation in the background too
    if (Scenes::safehouse) {
        auto sh = std::static_pointer_cast<SafehouseScene>(Scenes::safehouse);
        sh->tick_simulation(dt);
    }

    // ============================================================
    // 1) Level-up upgrade panel: ONLY handle 1–3, then early-return
    // ============================================================
    if (_showUpgradeChoices && !_upgradeChoices.empty()) {
        int chosenIndex = -1;

        if (keyPressedOnce(sf::Keyboard::Num1) ||
            keyPressedOnce(sf::Keyboard::Numpad1)) {
            chosenIndex = 0;
        }
        else if (keyPressedOnce(sf::Keyboard::Num2) ||
            keyPressedOnce(sf::Keyboard::Numpad2)) {
            chosenIndex = 1;
        }
        else if (keyPressedOnce(sf::Keyboard::Num3) ||
            keyPressedOnce(sf::Keyboard::Numpad3)) {
            chosenIndex = 2;
        }

        if (chosenIndex >= 0 &&
            chosenIndex < static_cast<int>(_upgradeChoices.size())) {
            applyUpgrade(_upgradeChoices[chosenIndex].type);
            _upgradeChosenThisRun = true;
            _showUpgradeChoices = false;
        }

        // While the reward screen is up, ignore all other TD input
        return;
    }

    // ============================================================
    // 1.5) Turret placement inventory overlay
    //      (E already pressed, we are choosing which turret)
    // ============================================================
    if (_choosingTurret && !_placementInvTypes.empty()) {
        int chosenIndex = -1;

        if (keyPressedOnce(sf::Keyboard::Num1) ||
            keyPressedOnce(sf::Keyboard::Numpad1)) chosenIndex = 0;
        else if (keyPressedOnce(sf::Keyboard::Num2) ||
            keyPressedOnce(sf::Keyboard::Numpad2)) chosenIndex = 1;
        else if (keyPressedOnce(sf::Keyboard::Num3) ||
            keyPressedOnce(sf::Keyboard::Numpad3)) chosenIndex = 2;
        else if (keyPressedOnce(sf::Keyboard::Num4) ||
            keyPressedOnce(sf::Keyboard::Numpad4)) chosenIndex = 3;
        else if (keyPressedOnce(sf::Keyboard::Num5) ||
            keyPressedOnce(sf::Keyboard::Numpad5)) chosenIndex = 4;
        else if (keyPressedOnce(sf::Keyboard::Num6) ||
            keyPressedOnce(sf::Keyboard::Numpad6)) chosenIndex = 5;
        else if (keyPressedOnce(sf::Keyboard::Num7) ||
            keyPressedOnce(sf::Keyboard::Numpad7)) chosenIndex = 6;
        else if (keyPressedOnce(sf::Keyboard::Num8) ||
            keyPressedOnce(sf::Keyboard::Numpad8)) chosenIndex = 7;
        else if (keyPressedOnce(sf::Keyboard::Num9) ||
            keyPressedOnce(sf::Keyboard::Numpad9)) chosenIndex = 8;

        if (chosenIndex >= 0 &&
            chosenIndex < static_cast<int>(_placementInvTypes.size())) {

            _pendingTurretType = _placementInvTypes[chosenIndex];
            _hasPendingTurret = true;
            _choosingTurret = false;
        }

        // Cancel with Esc
        if (keyPressedOnce(sf::Keyboard::Escape)) {
            _choosingTurret = false;
        }

        // While the placement inventory is up, pause other TD controls
        return;
    }

    // ============================================================
    // 2) Normal TD controls
    // ============================================================

    // Handle starting the next wave with F (only while waiting)
    if (_waveManager.isWaitingForPlayer() && keyPressedOnce(sf::Keyboard::F)) {
        _waveManager.startNextWave();
    }

    // --- Update money UI every frame ---
    if (Scenes::runContext) {
        _moneyText.setString(
            "Money: $" + std::to_string(Scenes::runContext->currency)
        );
    }

    // Swap back to Safehouse with Shift (LShift or RShift)
    if (keyPressedOnce(sf::Keyboard::LShift) ||
        keyPressedOnce(sf::Keyboard::RShift)) {
        GameSystem::set_active_scene(Scenes::safehouse);
        return;
    }

    // ------------------------------------------------------------
    // Open turret placement inventory:
    //   - Press E while a wave is RUNNING
    // ------------------------------------------------------------
    if (!_hasPendingTurret &&
        keyPressedOnce(sf::Keyboard::E)) {
        openPlacementInventory();
    }

    // ------------------------------------------------------------
    // Pending turret: show range preview + confirm / cancel
    // ------------------------------------------------------------
    if (_hasPendingTurret) {
        updateRangePreview();

        bool confirm =
            keyPressedOnce(sf::Keyboard::Enter) ||
            keyPressedOnce(sf::Keyboard::Space);

        bool cancel = keyPressedOnce(sf::Keyboard::Escape);

        if (confirm) {
            if (_rangeTileIsValid) {
                place_turret(_pendingTurretType);
                _hasPendingTurret = false;
            }
        }
        else if (cancel) {
            _hasPendingTurret = false;
        }
    }

    // Run full TD sim (spawning, movement, turrets, bullets)
    tick_simulation(dt);

    if (Scenes::safehouse) {
        auto sh = std::static_pointer_cast<SafehouseScene>(Scenes::safehouse);
        sh->tick_simulation(dt);
    }

    // ============================================================
    // 3) Wave-end / level-end detection + shop reroll + upgrades
    // ============================================================
    int currentLevel = _waveManager.getCurrentLevelIndex();
    bool nowWaiting = _waveManager.isWaitingForPlayer();

    if (nowWaiting && !_wasWaitingForPlayer) {
        // We have just transitioned from "wave running" -> "waiting"
        if (Scenes::safehouse) {
            auto sh = std::static_pointer_cast<SafehouseScene>(Scenes::safehouse);
            sh->rerollShop();
        }

        // Level index changed -> finished a whole LEVEL
        if (currentLevel != _lastLevelIndex) {
            _upgradeChosenThisRun = false;
            generateUpgradeChoices();
        }
    }

    _wasWaitingForPlayer = nowWaiting;
    _lastLevelIndex = currentLevel;

    // --- Update wave UI text ---
    if (!_waveManager.hasFinishedAllWaves()) {
        int levelIdx = _waveManager.getCurrentLevelIndex() + 1;
        int waveIdx = _waveManager.getCurrentWaveIndex() + 1;
        int totalWaves = _waveManager.getWavesInCurrentLevel();

        std::string extra;
        if (_waveManager.isWaitingForPlayer()) {
            extra = "  (Press F to start)";
        }

        _waveText.setString(
            "Level " + std::to_string(levelIdx) +
            " - Wave " + std::to_string(waveIdx) +
            "/" + std::to_string(totalWaves) +
            extra
        );
    }
    else {
        _waveText.setString("All waves complete");
    }
}

void TowerDefenceScene::render(sf::RenderWindow& window) {
    // Background colour for TD scene
    window.draw(_background);

    // Draw the tile grid (walls, path, etc.)
    ls::render(window);

    // Draw the player (from Scene base class)
    Scene::render(window);

    // Draw turrets, bullets, and enemies
    for (const auto& turret : _turrets) turret.render(window);
    for (const auto& b : _bullets)      b.render(window);
    for (const auto& enemy : _enemies)  window.draw(enemy.getShape());

    // Range preview (drawn on top of path but under UI)
    if (_hasPendingTurret) {
        window.draw(_rangePreview);
    }

    // Scene label + basic UI
    window.draw(_label);
    window.draw(_waveText);
    window.draw(_moneyText);
    window.draw(_controlsText);

    // --------------------------------------------------------
    // Turret placement inventory overlay
    // --------------------------------------------------------
    if (_choosingTurret && !_placementInvLines.empty()) {
        sf::RectangleShape panel;
        panel.setSize(sf::Vector2f(
            static_cast<float>(param::game_width) - 120.f,
            220.f
        ));
        panel.setFillColor(sf::Color(0, 0, 0, 190));
        panel.setPosition(60.f, 150.f);
        window.draw(panel);

        window.draw(_placementHeading);

        for (const auto& line : _placementInvLines) {
            window.draw(line);
        }
    }

    // --------------------------------------------------------
    // Upgrade choice overlay when a level is beaten
    // --------------------------------------------------------
    if (_showUpgradeChoices && !_upgradeChoices.empty()) {
        sf::RectangleShape panel;
        panel.setSize(sf::Vector2f(
            static_cast<float>(param::game_width) - 120.f,
            160.f
        ));
        panel.setFillColor(sf::Color(0, 0, 0, 190));
        panel.setPosition(60.f, 180.f);
        window.draw(panel);

        sf::Text heading;
        heading.setFont(_font);
        heading.setCharacterSize(24);
        heading.setFillColor(sf::Color::White);
        heading.setString("Level complete! Choose one upgrade (1-3):");
        heading.setPosition(80.f, 190.f);
        window.draw(heading);

        for (const auto& choice : _upgradeChoices) {
            window.draw(choice.text);
        }
    }
}


// ============================================================================
// EndScene (simple game-over screen)
// ============================================================================

void EndScene::load() {
    // Load font and configure "Game Over" text
    if (_font.loadFromFile("res/fonts/arial.ttf")) {
        _win_text.setFont(_font);
        _win_text.setString("GAME OVER\nPress R to restart");
        _win_text.setCharacterSize(36);
        _win_text.setFillColor(sf::Color::White);

        // Roughly centred (hard-coded for now)
        _win_text.setPosition(200.f, 200.f);
    }
}

void EndScene::update(const float& dt) {
    // No entities to update, so just handle input
    (void)dt; // silence unused warning if any

    // Press R to restart a fresh run
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        // Recreate the core scenes from scratch.
        // we go straight back to Safehouse + TowerDefence.
        Scenes::safehouse = std::make_shared<SafehouseScene>();
        Scenes::tower_defence = std::make_shared<TowerDefenceScene>();

        // Jump back to the start of the run (Safehouse)
        GameSystem::set_active_scene(Scenes::safehouse);
    }
}


void EndScene::render(sf::RenderWindow& window) {
    // Only draw if we have text set up
    if (_win_text.getString().isEmpty()) return;
    window.draw(_win_text);
}
