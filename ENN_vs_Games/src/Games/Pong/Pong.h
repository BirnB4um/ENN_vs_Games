#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../../Data.h"
#include "../../EvolutionaryNeuralNetwork.h"


class Pong
{

public:

	struct Agent {
		float pos_y;
		int score;
		EvolutionaryNeuralNetwork nn;

		float ball_x, ball_y;
		float ball_vel_x, ball_vel_y;

	};

	Pong();
	~Pong();

	void init();
	void on_resize();
	void handle_event(sf::Event &sf_event);
	void update();
	void draw(sf::RenderTarget &window);

	bool compare(Agent &left, Agent &right);


private:
	bool paused;

	int max_mutations;
	int number_of_agents;
	int epoche;
	int step;
	int epoche_length;
	float trigger_value;

	std::vector<Agent> agents;
	std::vector<int> agents_list;

	sf::RectangleShape rect;
	sf::CircleShape ball;
	float paddle_w, paddle_h;
	float paddle_speed;
	float ball_r;
	float ball_speed;

	bool perfect_score;

	sf::Font font;
	sf::Text generation_text;
	sf::Text score_text;

};

