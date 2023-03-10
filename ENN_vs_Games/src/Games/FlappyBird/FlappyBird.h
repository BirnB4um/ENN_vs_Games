#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../../Data.h"
#include "../../EvolutionaryNeuralNetwork.h"


class FlappyBird
{

public:

	struct Agent {
		int score;
		float pos_y;
		float vel_y;
		bool died;
		EvolutionaryNeuralNetwork nn;

	};

	FlappyBird();
	~FlappyBird();

	void init();
	void on_resize();
	void handle_event(sf::Event &sf_event);
	void update();
	void draw(sf::RenderTarget &window);

	bool compare(Agent &left, Agent &right);


private:

	int max_mutations;
	int number_of_agents;
	int epoche;
	int step;
	int epoche_length;
	float trigger_value;

	std::vector<Agent> agents;
	std::vector<int> agents_list;

	sf::RectangleShape upper_pipe;
	sf::RectangleShape lower_pipe;
	sf::RectangleShape bird;

	float bird_width, bird_height;
	float bird_x;
	float pipe_width;
	float pipe_speed;
	float pipe_space;
	float flapp_vel;
	float max_vel;
	float gravity;
	bool all_dead;

	sf::Font font;
	sf::Text gen_text;
	sf::Text score_text;

};

