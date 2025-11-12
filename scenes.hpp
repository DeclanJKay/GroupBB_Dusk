#pragma once
#include <memory>
#include <string>
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
	// later: player entity, vendor, doors, etc.
};

// Tower defence scene (turret placement / waves)
class TowerDefenceScene : public Scene {
public:
	TowerDefenceScene() = default;

	void load() override;
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;

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
	};
	std::vector<Turret> _turrets;

	void place_turret();            // helper function
};


struct Scenes {

	static std::shared_ptr<Scene> maze;
	static std::shared_ptr<Scene> safehouse;
	static std::shared_ptr<Scene> tower_defence;
	static std::shared_ptr<Scene> end;

	static std::shared_ptr<RunContext> runContext;
};

