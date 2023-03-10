#include "Pong.h"

Pong::Pong() {

}

Pong::~Pong() {

}

void Pong::init() {
	srand(1);

	//window 500, 800
	paused = true;
	trigger_value = 0.7;
	max_mutations = 4;
	epoche_length = 3000;
	step = epoche_length;
	epoche = 0;
	number_of_agents = 200;

	for (int i = 0; i < number_of_agents; i++) {
		Agent agent;
		agent.nn.init(5, 2);
		agents.push_back(agent);
		agents_list.push_back(i);
	}

	paddle_speed = 10;
	paddle_w = Data::window_width / 20;
	paddle_h = Data::window_height / 4;
	rect.setSize(sf::Vector2f(paddle_w, paddle_h));
	rect.setFillColor(sf::Color(255, 255, 255, 20));

	ball_speed = 8;
	ball_r = Data::window_height / 100;
	ball.setRadius(ball_r);
	ball.setFillColor(sf::Color(255, 255, 255, 20));
	ball.setOrigin(ball_r, ball_r);

	font.loadFromFile("res/arial.ttf");
	generation_text.setPosition(paddle_w * 1.5, 10);
	generation_text.setCharacterSize(30);
	generation_text.setFont(font);
	score_text.setPosition(paddle_w * 1.5, 50);
	score_text.setCharacterSize(30);
	score_text.setFont(font);
}

void Pong::on_resize() {

}

void Pong::handle_event(sf::Event& sf_event) {
	if (sf_event.type == sf::Event::KeyReleased) {
		if (sf_event.key.code == sf::Keyboard::Space) {
			paused = !paused;
		}
	}
}

bool Pong::compare(Agent &left, Agent &right) {
	if (left.score >= right.score) {
		if (left.score > right.score) {
			return true;
		}
		if (left.nn.number_of_hidden_nodes <= right.nn.number_of_hidden_nodes) {
			if (left.nn.number_of_hidden_nodes < right.nn.number_of_hidden_nodes) {
				return true;
			}
			return left.nn.number_of_connections < right.nn.number_of_connections;
		}
	}
	return false;
}

void Pong::update() {
	if (paused)
		return;

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

		std::cout << "score: " << agents[agents_list[agents.size() - 1]].score << "\nnodes: " << agents[agents_list[agents.size() - 1]].nn.number_of_hidden_nodes << "\nconnections: " << agents[agents_list[agents.size() - 1]].nn.number_of_connections << "\n";

		//restart epoche
		for (Agent &agent : agents) {
			agent.score = 0;
			agent.pos_y = (float(rand()) / RAND_MAX) * (Data::window_height - paddle_h);
			//agent.ball_x = Data::window_width / 2;
			//agent.ball_y = Data::window_height / 2;
			agent.ball_x = ball_r * 3 + rand() % int(Data::window_width - ball_r * 6);
			agent.ball_y = ball_r * 3 + rand() % int(Data::window_height - ball_r * 6);
			agent.ball_vel_x = -ball_speed;
			agent.ball_vel_y = rand() % 2 == 0 ? ball_speed : -ball_speed;
			agent.nn.clear_network();
		}

		step = 0;
		epoche++;
		std::cout << " === Epoche: " << epoche << " ===" << std::endl;
	}


	//step
	int m = sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? epoche_length : 1;
	for (int n = 0; n < m;n++) {
		for (Agent& agent : agents) {

			//set input
			agent.nn.set_input(0, agent.pos_y / (Data::window_height-paddle_h));
			agent.nn.set_input(1, agent.ball_x / Data::window_width);
			agent.nn.set_input(2, agent.ball_y / Data::window_height);
			agent.nn.set_input(3, agent.ball_vel_x / ball_speed);
			agent.nn.set_input(4, agent.ball_vel_y / ball_speed);

			//execute output
			agent.nn.forward();
			if (agent.nn.get_output(0) > trigger_value) {//up
				agent.pos_y -= paddle_speed;
			}
			if (agent.nn.get_output(1) > trigger_value) {//down
				agent.pos_y += paddle_speed;
			}


			if (agent.pos_y < 0) {
				agent.pos_y = 0;
			}
			if (agent.pos_y > Data::window_height - paddle_h) {
				agent.pos_y = Data::window_height - paddle_h;
			}

			agent.ball_x += agent.ball_vel_x;
			agent.ball_y += agent.ball_vel_y;

			if (agent.ball_x > Data::window_width - ball_r) {
				agent.ball_vel_x *= -1;
			}
			if (agent.ball_y < ball_r || agent.ball_y > Data::window_height - ball_r ) {
				agent.ball_vel_y *= -1;
			}

			if (agent.ball_x < paddle_w + 10 + ball_r) {
				if (agent.ball_y > agent.pos_y && agent.ball_y < agent.pos_y + paddle_h) {
					agent.score++;
					agent.ball_vel_x *= -1;
				}
				else {
					agent.score--;
					agent.ball_x = ball_r * 3 + rand() % int(Data::window_width - ball_r * 6);
					agent.ball_y = ball_r * 3 + rand() % int(Data::window_height - ball_r * 6);
					agent.ball_vel_x = -ball_speed;
					agent.ball_vel_y = rand() % 2 == 0 ? ball_speed : -ball_speed;
				}
			}


		}
		step++;
	}


}

void Pong::draw(sf::RenderTarget &window) {
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
		ball.setPosition(agents[agents_list[agents_list.size() - 1]].ball_x, agents[agents_list[agents_list.size() - 1]].ball_y);
		window.draw(ball);

		rect.setPosition(10, agents[agents_list[agents_list.size() - 1]].pos_y);
		window.draw(rect);
	}
	else {
		for (Agent& agent : agents) {
			ball.setPosition(agent.ball_x, agent.ball_y);
			window.draw(ball);

			rect.setPosition(10, agent.pos_y);
			window.draw(rect);
		}
	}

	generation_text.setString("Generation: " + std::to_string(epoche));
	window.draw(generation_text);

	score_text.setString("Score: " + std::to_string(agents[agents_list[number_of_agents-1]].score));
	window.draw(score_text);

}