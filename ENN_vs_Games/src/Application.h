#pragma once
#include <SFML/Graphics.hpp>
#include "Data.h"
#include "Games/Pong/Pong.h"
#include "Games/FlappyBird/FlappyBird.h"
#include "Games/Tron/Tron.h"

class Application
{

public:

	Application();
	~Application();

	bool init();
	void run();

private:

	sf::RenderWindow window;
	sf::Event sf_event;

	Tron scene;

	void on_resize();
	void handle_event();
	void update();
	void draw();


};

