#include "Tron.h"

Tron::Tron() {

}

Tron::~Tron() {

}

void Tron::init() {
	srand(666);//69 666

	trigger_value = 0.7;
	max_mutations = 10;
	epoche = 0;
	number_of_agents_pro_board = 2;//
	number_of_boards = 200;
	number_of_agents = number_of_agents_pro_board * number_of_boards;
	optimise_ever_n_epoches = 50;

	died_punishment = 1000;
	kill_bonus = 1000;
	board_width = 60;
	board_height = 60;
	best_board = 0;
	epoche_length = 1000;// board_width* board_height;
	step = epoche_length;


	empty_board = new unsigned int[board_width * board_height];
	for (int y = 0; y < board_height;y++) {
		for (int x = 0; x < board_width; x++) {
			empty_board[x + y * board_width] = (x == 0 || x == board_width - 1 || y == 0 || y == board_height - 1) ? 0xFF999999 : 0;
		}
	}

	for (int i = 0; i < number_of_boards; i++) {
		Board board;

		board.agents = new int[number_of_agents_pro_board];
		for (int n = 0; n < number_of_agents_pro_board; n++) {
			Agent agent;
			int r = 50 + rand() % 200;
			int g = 50 + rand() % 200;
			int b = 50 + rand() % 200;
			float trail_factor = 0.2;
			agent.color = sf::Color(255, b, g, r).toInteger();
			agent.trail_color = sf::Color(255, b * trail_factor, g * trail_factor, r * trail_factor).toInteger();
			agent.dx = -1;
			agent.dy = 0;
			agent.score = 0;
			agent.nn.min_weight = -2;
			agent.nn.max_weight = 2;
			agent.nn.init(9, 2);
			agents.push_back(agent);
			agents_list.push_back(i * number_of_agents_pro_board + n);
			board.agents[n] = i * number_of_agents_pro_board + n;
		}

		board.board = new unsigned int[board_width * board_height];
		boards.push_back(board);
	}


	texture.create(board_width, board_height);
	rect.setSize(sf::Vector2f(Data::window_width, Data::window_height));
	rect.setTexture(&texture);

	font.loadFromFile("res/pixelfont.ttf");
	gen_text.setFont(font);
	gen_text.setPosition(20,20);
	gen_text.setCharacterSize(30);

}

float constrain(float x, float min, float max) {
	return x < min ? min : x > max ? max : x;
}

void Tron::on_resize() {

}

void Tron::handle_event(sf::Event& sf_event) {

}

bool Tron::compare(Agent &left, Agent &right) {
	if (left.score >= right.score) {
		if (left.score > right.score) {
			return true;
		}
		//if (left.nn.number_of_hidden_nodes <= right.nn.number_of_hidden_nodes) {
		//	if (left.nn.number_of_hidden_nodes < right.nn.number_of_hidden_nodes) {
		//		return true;
		//	}
		//	return left.nn.number_of_connections < right.nn.number_of_connections;
		//}
	}
	return false;
}

void Tron::sort() {
	for (int max = agents_list.size() - 1; max > agents_list.size() / 2; max--) {
		for (int i = 0; i < max; i++) {
			if (compare(agents[agents_list[i]], agents[agents_list[i + 1]])) {
				int temp = agents_list[i];
				agents_list[i] = agents_list[i + 1];
				agents_list[i + 1] = temp;
			}
		}
	}
}

void Tron::mutate() {
	for (int i = 0; i < agents_list.size() / 2; i++) {
		agents[agents_list[i]].nn.mutate(1 + rand() % max_mutations, agents[agents_list[agents_list.size() - 1 - (rand() % int(agents.size() / 2))]].nn.building_code);
	}
}

void Tron::restart_epoche() {
	for (Board& board : boards) {
		memcpy(board.board, empty_board, board_width * board_height * sizeof(int));

		for (int i = 0; i < number_of_agents_pro_board; i++) {
			agents[board.agents[i]].nn.clear_network();
			agents[board.agents[i]].dx = -1;
			agents[board.agents[i]].dy = 0;
			agents[board.agents[i]].score = 0;
			agents[board.agents[i]].died = false;
			while (true) {
				int x = 1 + (rand() % (board_width - 2));
				int y = 1 + (rand() % (board_height - 2));
				if (board.board[x + y * board_width] == 0) {
					agents[board.agents[i]].pos_x = x;
					agents[board.agents[i]].pos_y = y;
					board.board[x + y * board_width] = agents[board.agents[i]].color;
					break;
				}
			}
		}
	}
}

void Tron::simulation_step() {

	bool all_dead = true;
	bool all_best_dead = true;

	int b = -1;
	for (Board &board : boards) {
		b++;

		for (int i = 0; i < number_of_agents_pro_board; i++) {

			if (!agents[board.agents[i]].died) {
				all_dead = false;
				if (b == best_board) {
					all_best_dead = false;
				}
				Agent& agent = agents[board.agents[i]];

				//// ===== set input =====
				float dist, min_dist=1;
				bool changed_dir = false;
				int d, dx, dy;

				//dist FORWARD
				dist = 0;
				dx = agent.pos_x + agent.dx;
				dy = agent.pos_y + agent.dy;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx += agent.dx;
					dy += agent.dy;
				}
				if (dist < min_dist)
					min_dist = dist;
				//agent.nn.set_input(0, dist / board_width);

				//dist LEFT
				dist = 0;
				dx = agent.pos_x + agent.dy;
				dy = agent.pos_y - agent.dx;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx += agent.dy;
					dy += -agent.dx;
				}
				if (dist < min_dist)
					min_dist = dist;
				agent.nn.set_input(1, dist / board_width);

				//dist RIGHT
				dist = 0;
				dx = agent.pos_x - agent.dy;
				dy = agent.pos_y + agent.dx;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx += -agent.dy;
					dy += agent.dx;
				}
				if (dist < min_dist)
					min_dist = dist;
				agent.nn.set_input(2, dist / board_width);

				//dist LEFT FORWARD
				dist = 0;
				dx = agent.pos_x + agent.dy + agent.dx;
				dy = agent.pos_y + agent.dy - agent.dx;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx += agent.dy + agent.dx;
					dy += agent.dy - agent.dx;
				}
				if (dist < min_dist)
					min_dist = dist;
				//agent.nn.set_input(3, dist / board_width);

				//dist RIGHT FORWARD
				dist = 0;
				dx = agent.pos_x + agent.dx - agent.dy;
				dy = agent.pos_y + agent.dx + agent.dy;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx += agent.dx - agent.dy;
					dy += agent.dx + agent.dy;
				}
				if (dist < min_dist)
					min_dist = dist;
				//agent.nn.set_input(4, dist / board_width);

				//dist LEFT BACK
				dist = 0;
				dx = agent.pos_x + agent.dy - agent.dx;
				dy = agent.pos_y - agent.dx - agent.dy;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx+= agent.dy - agent.dx;
					dy+= -agent.dx - agent.dy;
				}
				if (dist < min_dist)
					min_dist = dist;
				agent.nn.set_input(5, dist / board_width);

				//dist RIGHT BACK
				dist = 0;
				dx = agent.pos_x -agent.dy - agent.dx;
				dy = agent.pos_y + agent.dx - agent.dy;
				while (board.board[dx + dy * board_width] == 0) {
					dist++;
					dx+= -agent.dy - agent.dx;
					dy+= agent.dx - agent.dy;
				}
				if (dist < min_dist)
					min_dist = dist;
				agent.nn.set_input(6, dist / board_width);

				//position of enemy
				agent.nn.set_input(7, 0);
				agent.nn.set_input(8, 0);
				for (int n = 0; n < number_of_agents_pro_board; n++) {
					if (n != i) {
						int d_x = agents[board.agents[n]].pos_x - agent.pos_x;
						int d_y = agents[board.agents[n]].pos_y - agent.pos_y;

						//horizontal position of enemy (relative to agent)
						if (agent.dx == 0) {
							dist = agent.dy < 0 ? d_x : -d_x;
						}
						else {
							dist = agent.dx < 0 ? -d_y : d_y;
						}
						agent.nn.set_input(7, dist / board_width);


						//vertical position of enemy (relative to agent)
						if (agent.dx == 0) {
							dist = agent.dy < 0 ? -d_y : d_y;
						}
						else {
							dist = agent.dx < 0 ? -d_x : d_x;
						}
						agent.nn.set_input(8, dist / board_width);
					}
				}



				//time
				//agent.nn.set_input(7, (step / 100.0f) - int(step / 100.0f));

				//coords on board
				//agent.nn.set_input(8, float(agent.pos_x) / board_width);
				//agent.nn.set_input(9, float(agent.pos_y) / board_height);
					


				// ===== run ENN =====
				agent.nn.forward();

				// ===== execute output =====
				float highest = trigger_value;

				if (agent.nn.get_output(0) > highest) {//left
					highest = agent.nn.get_output(0);
					int x = agent.dx;
					agent.dx = agent.dy;
					agent.dy = -x;
					changed_dir = true;
				}
				if (agent.nn.get_output(1) > highest) {//right
					highest = agent.nn.get_output(1);
					int x = agent.dx;
					agent.dx = -agent.dy;
					agent.dy = x;
					changed_dir = true;
				}
				


				if (board.board[agent.pos_x + agent.dx + (agent.pos_y + agent.dy) * board_width] != 0) {//died
					agent.died = true;
					agent.score -= died_punishment;
					board.board[agent.pos_x + agent.pos_y * board_width] = agent.trail_color;
					
					//give boost to others
					for (int n = 0; n < number_of_agents_pro_board; n++) {
						if (n != i) {
							agents[board.agents[n]].score += kill_bonus * (epoche_length-step)/epoche_length;
						}
					}

				}
				else {//go step
					board.board[agent.pos_x + agent.pos_y * board_width] = agent.trail_color;
					agent.pos_x += agent.dx;
					agent.pos_y += agent.dy;
					board.board[agent.pos_x + agent.pos_y * board_width] = agent.color;
					
					agent.score++;

					//if (min_dist > 3.0/board_width) {
					//	agent.score += 10;
					//}
					//if (changed_dir) {
					//	agent.score += 10;
					//}
				}

			}

		}

	}

	if (all_dead) {
		step = epoche_length;
	}
	if (all_best_dead) {
		min_steps = epoche_length;
	}

}

void Tron::update() {

	if (step >= epoche_length) {

		for (Agent &agent : agents) {
			//agent.score -= 1 * agent.nn.number_of_hidden_nodes;
			//agent.score -= 1 * agent.nn.number_of_connections;

		}

		//if (epoche == 427) {
		//	std::cout << "\ncol: " << agents[boards[best_board].agents[6]].color;
		//	agents[boards[best_board].agents[6]].nn.print();
		//	agents[boards[best_board].agents[6]].nn.optimise_network();
		//	agents[boards[best_board].agents[6]].nn.optimise_building_code();
		//	auto myfile = std::fstream("muster_nn.dat", std::ios::out | std::ios::binary);
		//	myfile.write((char*)&(agents[boards[best_board].agents[6]].nn.building_code[0]), agents[boards[best_board].agents[6]].nn.building_code.size() * sizeof(float));
		//	myfile.close();
		//}

		sort();

		best_board = int(agents_list[agents.size() - 1] / number_of_agents_pro_board);

		std::cout << "score: " << agents[agents_list[agents.size() - 1]].score << "\nnodes: " << agents[agents_list[agents.size() - 1]].nn.number_of_hidden_nodes << "\nconnections: " << agents[agents_list[agents.size() - 1]].nn.number_of_connections << "\n";
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
			agents[agents_list[agents.size() - 1]].nn.print();
		}

		mutate();

		if (epoche % optimise_ever_n_epoches == 0) {
			for (int i = 0; i < number_of_agents; i++) {
				agents[i].nn.optimise_network();
				agents[i].nn.optimise_building_code();
			}
			std::cout << "========================================= OPTIMISED!" << std::endl;
		}

		restart_epoche();

		min_steps = 1;
		step = 0;
		epoche++;
		std::cout << " === Epoche: " << epoche << " ===" << std::endl;
	}


	//step
	int m = sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? epoche_length : min_steps;
	//if (min_steps == 1 && (
	//	epoche == 31 ||
	//	epoche == 136 ||
	//	epoche == 183 ||
	//	epoche == 199 ||
	//	epoche == 223 ||
	//	epoche == 235 ||
	//	epoche == 283)) {
	//	m = sf::Keyboard::isKeyPressed(sf::Keyboard::P) ? epoche_length : 1;
	//}
	//if (min_steps == 1 && (
	//	epoche <= 12 ||
	//	epoche == 250 ||
	//	epoche == 370 ||
	//	epoche == 389 ||
	//	epoche == 396 ||
	//	epoche == 419 ||
	//	epoche == 427 ||
	//	epoche == 432 ||
	//	epoche == 436 ||
	//	epoche == 451 ||
	//	epoche == 490 ||
	//	epoche == 630 ||
	//	epoche == 640 ||
	//	epoche == 691 ||
	//	epoche == 1022 ||
	//	epoche == 1094 ||
	//	epoche == 1635 ||
	//	epoche == 1636 ||
	//	epoche == 1637 ||
	//	epoche == 1638 ||
	//	epoche == 1639 ||
	//	epoche == 2500 ||
	//	false)) {
	//	m = sf::Keyboard::isKeyPressed(sf::Keyboard::P) ? epoche_length : 1;
	//}
	for (int n = 0; n < m; n++) {
		simulation_step();
		step++;
	}


}

void Tron::draw(sf::RenderTarget &window) {
	
	texture.update((uint8_t*) boards[best_board].board);
	window.draw(rect);

	gen_text.setString("Generation: " + std::to_string(epoche));
	window.draw(gen_text);

}