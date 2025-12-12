///Includes
#include <SFML/Graphics.hpp>
#include <cstdlib> //for rand
#include <ctime> //for seeding rand

#include "gameSys.hpp"
#include "gameParams.hpp"
#include <tuple>
#include "MouseHelper.hpp"
#include "KeyboardHelper.hpp"

int test;

int main () {
	srand(time(0));

	//create the window
	sf::RenderWindow window(sf::VideoMode({Params::gameW, Params::gameH}), "Dusk");
	window.setVerticalSyncEnabled(true);

	MouseHelper::SetWindow(&window);

    //initialise and load
	GameSys::init();

	while (window.isOpen())
	{
		//process window events
      	sf::Event event;
      	while (window.pollEvent(event))
	  	{
			KeyboardHelper::AssembleKeysPressed(&event);
			MouseHelper::HandleEvents(&event);
      		if (event.type == sf::Event::Closed)
			{
        		window.close();
      		}
    	}

		//Calculate dt
		static sf::Clock clock;
		const float dt = clock.restart().asSeconds();
		window.clear();
		GameSys::update(dt);
		GameSys::render(window);
		window.display();
		KeyboardHelper::Clear();
		MouseHelper::Clear();
	}

	//Unload and shutdown
	GameSys::clean();
}