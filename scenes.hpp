#pragma once
#include <memory>
#include <string>
#include "game_systems.hpp"
#include <SFML/Graphics.hpp>

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

struct Scenes {
	static std::shared_ptr<Scene> maze;
	static std::shared_ptr<Scene> end;
};
