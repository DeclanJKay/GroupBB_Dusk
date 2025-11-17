#pragma once
#include <memory>
#include <string>
#include <vector>
#include "game_systems.hpp"
#include <SFML/Graphics.hpp>
#include "run_context.hpp"
#include "scenes.hpp"

class Player;

// Maze scene
class MazeScene : public Scene {
public:
	MazeScene() = default;

	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;
	void load() override;

	void reset();
	void set_file_path(const std::string& file_path);

private:
	std::string _file_path;
};

// Simple ending screen
class EndScene : public Scene {
public:
	EndScene() = default;
	void load() override;
	void render(sf::RenderWindow& window) override;

private:
	sf::Text _win_text;
	sf::Font _font;
};

// Safehouse scene (roguelite action side)
class SafehouseScene : public Scene {
public:
	SafehouseScene() = default;

	void load() override;
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;

private:
	sf::RectangleShape _background;

	sf::Font _font;
	sf::Text _label;

	std::shared_ptr<Player> _player = nullptr;		// Persistent player for this scene
	bool _initialised = false;

	// enemies that made it out of the tower defence
	struct Invader {
		sf::CircleShape shape;
		float speed = 60.f;   // movement speed in px/sec	
	};
	std::vector<Invader> _invaders;

	void spawn_invaders(int count);
	void update_invaders(float dt);
};

// Tower defence scene (turret placement / waves)
class TowerDefenceScene : public Scene {
public:
	TowerDefenceScene() = default;

	void load() override;
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;
	void tick_simulation(float dt); //run the TDF sim while in the sfaehouse 
	int consume_escaped_enemies();

private:
	sf::RectangleShape _background;
	//sf::RectangleShape _enemy_path; (ignore this think i dont need it but commenting it out for the now) 

	sf::Font _font;
	sf::Text _label;
	// later: turret slots, enemies, bullets, UI, etc.

	std::shared_ptr<Player> _player;

	// simple turret representation
	struct Turret {
		sf::Vector2i grid;          // tile coordinates
		sf::RectangleShape shape;   // visual
		float cooldown = 0.f;		//time until fire again 
	};
	std::vector<Turret> _turrets;

	// Enemy path in world space (centres of the + tiles)
	std::vector<sf::Vector2f> _enemyPath;

	// Simple enemy representation
	struct Enemy {
		float t = 0.f;              // position along the path (0 .. path_length-1)
		sf::CircleShape shape;      // visual
		int hp = 3;					//inital health of basic enemy 
		float flashTimer = 0.f;		//flash effect when enemy gets hit
	};
	std::vector<Enemy> _enemies;

	// Bullets that the turrets fire
	struct Bullet {
		sf::Vector2f pos;
		sf::Vector2f vel;       // normalized direction
		float speed = 300.f;    // px/sec
		int   damage = 1;
		float ttl = 2.0f;       // seconds to live
		sf::CircleShape shape;  // visual
	};
	std::vector<Bullet> _bullets;

	float _spawnTimer = 0.f;        // time since last spawn

	bool _initialised = false;		//only initialise TDF once
	
	int _escapedEnemies = 0;		//How many enemies reach the end of the maze 

	void place_turret();            // helper function
	void build_enemy_path();        // generate ordered path from + tiles
	void spawn_enemy();             // create a new enemy at start of path
	void update_enemies(float dt);  // move enemies along the path
	void update_turrets(float dt);  //turrets fire 
	void update_bullets(float dt);  //bullet update for them moving 
};


struct Scenes {

	static std::shared_ptr<Scene> maze;
	static std::shared_ptr<Scene> safehouse;
	static std::shared_ptr<Scene> tower_defence;
	static std::shared_ptr<Scene> end;

	static std::shared_ptr<RunContext> runContext;
};

