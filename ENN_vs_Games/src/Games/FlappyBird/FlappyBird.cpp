#include "FlappyBird.h"

FlappyBird::FlappyBird() {

}

FlappyBird::~FlappyBird() {

}

void FlappyBird::init() {
	srand(333);

	//window 500, 800

	trigger_value = 0.7;
	max_mutations = 10;
	epoche_length = 10000;
	step = epoche_length;
	epoche = 0;
	number_of_agents = 200;

	for (int i = 0; i < number_of_agents; i++) {
		Agent agent;
		agent.score = 0;
		agent.nn.init(4, 1);
		agents.push_back(agent);
		agents_list.push_back(i);
	}


	bird_width = 60;
	bird_height = 40;
	bird_x = 50;
	pipe_width = 70;
	flapp_vel = 10;
	max_vel = 10;
	gravity = 1;
	pipe_speed = 4;
	pipe_space = bird_height + flapp_vel * 8;
	all_dead = false;
	perfect_score = false;

	upper_pipe.setPosition(-1,0);
	upper_pipe.setSize(sf::Vector2f(pipe_width, 100));
	upper_pipe.setFillColor(sf::Color(0, 255, 0, 255));
	lower_pipe.setPosition(-1,0);
	lower_pipe.setSize(sf::Vector2f(pipe_width, 100));
	lower_pipe.setFillColor(sf::Color(0,255,0,255));
	bird.setSize(sf::Vector2f(bird_width, bird_height));
	bird.setFillColor(sf::Color(255,0,0,100));

	font.loadFromFile("res/arial.ttf");
	gen_text.setFont(font);
	gen_text.setPosition(20,20);
	gen_text.setCharacterSize(30);
	score_text.setFont(font);
	score_text.setPosition(20,60);
	score_text.setCharacterSize(30);

}

void FlappyBird::on_resize() {

}

void FlappyBird::handle_event(sf::Event& sf_event) {

}

bool FlappyBird::compare(Agent &left, Agent &right) {
	if (left.score >= right.score) {
		if (left.score > right.score) {
			return true;
		}
		if (left.score > 1000) {
			if (left.nn.number_of_nodes < right.nn.number_of_hidden_nodes ||
				left.nn.number_of_connections < right.nn.number_of_connections) {
				return true;
			}
		}
	}
	return false;
}

void FlappyBird::update() {

	if (all_dead) {
		step = epoche_length;
	}

	if (step >= epoche_length) {

		//sort
		for (int max = agents_list.size() - 1; max > agents_list.size() / 2; max--) {
			for (int i = 0; i < max;i++) {
				if (compare(agents[agents_list[i]], agents[agents_list[i+1]])) {
					int temp = agents_list[i];
					agents_list[i] = agents_list[i + 1];
					agents_list[i + 1] = temp;
				}
			}
		}

		//mutate
		for (int i = 0; i < agents_list.size() / 2; i++) {
			agents[agents_list[i]].nn.mutate(1 + rand() % max_mutations, agents[agents_list[agents_list.size() - 1 - (rand() % int(agents.size() / 2))]].nn.building_code);
		}
		if (agents[agents_list[agents.size() - 1]].score == 10000) {
			perfect_score = true;
		}
		std::cout << "score: " << agents[agents_list[agents.size() - 1]].score << "\nnodes: " << agents[agents_list[agents.size() - 1]].nn.number_of_hidden_nodes << "\nconnections: " << agents[agents_list[agents.size() - 1]].nn.number_of_connections << "\n";
		//if (agents[agents_list[agents.size() - 1]].nn.number_of_connections == 2 &&
		//	agents[agents_list[agents.size() - 1]].nn.number_of_hidden_nodes == 0) {
		//	agents[agents_list[agents.size() - 1]].nn.print();
		//}

		//restart epoche
		for (Agent &agent : agents) {
			agent.score = 0;
			agent.pos_y = Data::window_height / 2;
			agent.vel_y = 0;
			agent.died = false;
			agent.nn.clear_network();
		}
		upper_pipe.setPosition(-pipe_width - 1, 0);
		lower_pipe.setPosition(-pipe_width-1, 0);

		step = 0;
		epoche++;
		std::cout << " === Epoche: " << epoche << " ===" << std::endl;
	}


	//step
	all_dead = true;
	int m = sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? epoche_length : 1;
	if (perfect_score) {
		m = 1;
	}
	for (int n = 0; n < m;n++) {
		for (Agent& agent : agents) {

			if (!agent.died) {
				all_dead = false;

				agent.score++;

				//set input
				agent.nn.set_input(0, agent.vel_y/max_vel);
				agent.nn.set_input(1, agent.pos_y / Data::window_height);
				agent.nn.set_input(2, float(upper_pipe.getPosition().x - bird_x - bird_width) / Data::window_width);
				agent.nn.set_input(3, float(upper_pipe.getSize().y) / Data::window_height);

				//execute output
				agent.nn.forward();
				if (agent.nn.get_output(0) > trigger_value) {
					agent.vel_y = -flapp_vel;
				}

			}

			//update vel/pos
			agent.vel_y += gravity;
			if (agent.vel_y > max_vel) {
				agent.vel_y = max_vel;
			}
			agent.pos_y += agent.vel_y;

			//hit top/bottom
			if (agent.pos_y < 0) {
				agent.died = true;
				agent.pos_y = 0;
			}
			else if (agent.pos_y > Data::window_height - bird_height) {
				agent.died = true;
				agent.pos_y = Data::window_height - bird_height;
			}

			//hit pipe
			if (bird_x + bird_width > upper_pipe.getPosition().x && bird_x < upper_pipe.getPosition().x + pipe_width) {
				if (agent.pos_y < upper_pipe.getSize().y) {
					agent.died = true; 
				}
				else if (agent.pos_y + bird_height > lower_pipe.getPosition().y) {
					agent.died = true;
				}
			}

		}

		//update pipes
		upper_pipe.setPosition(upper_pipe.getPosition().x - pipe_speed, 0);
		lower_pipe.setPosition(lower_pipe.getPosition().x - pipe_speed, lower_pipe.getPosition().y);

		//spawn new pipe
		if (upper_pipe.getPosition().x < -pipe_width) {
			
			float off_y = 0.1 * Data::window_height + (float(rand()) / RAND_MAX) * (0.8 * Data::window_height - pipe_space);
			upper_pipe.setSize(sf::Vector2f(pipe_width, off_y));
			lower_pipe.setSize(sf::Vector2f(pipe_width, Data::window_height - pipe_space - off_y));

			upper_pipe.setPosition(Data::window_width, 0);
			lower_pipe.setPosition(Data::window_width, off_y + pipe_space);
		}

		step++;
	}


}

void FlappyBird::draw(sf::RenderTarget &window) {
	
	window.draw(upper_pipe);
	window.draw(lower_pipe);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
		bird.setPosition(bird_x, agents[agents_list[agents.size()-1]].pos_y);
		window.draw(bird);
	}
	else {
		for (Agent& agent : agents) {
			bird.setPosition(bird_x, agent.pos_y);
			window.draw(bird);
		}
	}

	gen_text.setString("Generation: " + std::to_string(epoche));
	window.draw(gen_text);

	score_text.setString("Score: " + std::to_string( int((agents[agents_list[number_of_agents-1]].score+20) / ((Data::window_width + pipe_width)/pipe_speed)) ));
	window.draw(score_text);

}