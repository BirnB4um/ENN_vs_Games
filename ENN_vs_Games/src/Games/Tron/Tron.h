#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include "../../Data.h"
#include "../../EvolutionaryNeuralNetwork.h"


class Tron
{

public:

	struct Board {
		unsigned int* board = nullptr;
		int* agents = nullptr;
	};

	struct Agent {
		int score;
		int pos_x, pos_y;
		int dx, dy;
		unsigned int color, trail_color;
		bool died;
		EvolutionaryNeuralNetwork nn;
	};

	Tron();
	~Tron();

	void init();
	void on_resize();
	void handle_event(sf::Event &sf_event);
	void update();
	void draw(sf::RenderTarget &window);

	bool compare(Agent &left, Agent &right);
	void sort();
	void restart_epoche();
	void mutate();
	void simulation_step();


private:

	int max_mutations;
	int epoche;
	int step;
	int epoche_length;
	float trigger_value;
	int number_of_agents_pro_board;
	int number_of_boards;
	int number_of_agents;
	int optimise_ever_n_epoches;

	std::vector<Agent> agents;
	std::vector<int> agents_list;

	std::vector<Board> boards;

	int min_steps;
	int died_punishment;
	int kill_bonus;
	int board_width, board_height;
	int best_board;
	unsigned int *empty_board;
	sf::Texture texture;
	sf::RectangleShape rect;

	sf::Font font;
	sf::Text gen_text;


};

