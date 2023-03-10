#include "Application.h"


Application::Application() {

}

Application::~Application() {

}

bool Application::init() {

	Data::window_width = 800;
	Data::window_height = 800;

	sf::ContextSettings window_settings;
	window_settings.depthBits = 24;
	window_settings.stencilBits = 8;
	window_settings.antialiasingLevel = 4;
	window_settings.majorVersion = 3;
	window_settings.minorVersion = 0;
	window.create(sf::VideoMode(Data::window_width, Data::window_height), "ENN vs Games",
		sf::Style::Close | sf::Style::Resize, window_settings);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(false);


	scene.init();


	return true;
}

void Application::run() {
	while (window.isOpen()) {
		handle_event();
		update();
		draw();
	}
}

void Application::on_resize() {
	window.setView(sf::View(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2), (sf::Vector2f)window.getSize()));
	Data::window_width = window.getSize().x;
	Data::window_height = window.getSize().y;

	scene.on_resize();
}

void Application::handle_event() {
	while (window.pollEvent(sf_event)) {
		if (sf_event.type == sf::Event::Closed) {
			window.close();
		}
		else if (sf_event.type == sf::Event::Resized) {
			on_resize();
		}

		scene.handle_event(sf_event);
	}
}

void Application::update() {
	scene.update();
}

void Application::draw() {
	window.clear();

	scene.draw(window);

	window.display();
}