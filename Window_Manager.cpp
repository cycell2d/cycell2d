#include "Window_Manager.h"
#include "User_Input.h"
#include "parameters.h"



void Window_Manager::update(User_Input * ui)
{

	if (ui->isCloseWindowNeeded()) {
		window.close();
		return;
	}

	resize_needed = 0;
	if (ui->isResizeNeeded()) {
		resize_needed = 1;
		zoom_size = sf::Vector2f((float)getScreenWidth(), (float)getScreenHeight());
		sf::Vector2u window_size;
		window_size = window.getSize();
		fixed_view.reset(sf::FloatRect(0, 0, (float)window_size.x, (float)window_size.y));
		zoomable_view.reset(sf::FloatRect(0, 0, (float)window_size.x, (float)window_size.y));
	}



	//*******************
	//Checks for zoom
	//*******************
	if ( (ui->getMouseRawPosition(window).x > 300) && (!ui->isMouseOnLargerWidgets()) ){
				if (ui->getMouseWheelDelta() < 0) {
				zoom_size += sf::Vector2f(80.0f, 80.0f * float(getScreenHeight()) / float(getScreenWidth()));
				}
			if (ui->getMouseWheelDelta() > 0) {
				zoom_size -= sf::Vector2f(80.0f, 80.0f * float(getScreenHeight()) / float(getScreenWidth()));
				if (zoom_size.x < 50) {
					zoom_size = sf::Vector2f(50.0f, 50.0f * float(getScreenHeight()) / float(getScreenWidth()));
				}
			}
	}
	


	//*******************
	//Zooms zoomable_view.
	//*******************
	if (zoom_size.x > window.getSize().x) { zoom_size = sf::Vector2f(window.getSize()); }
	zoomable_view.setSize(sf::Vector2f(zoom_size.x, zoom_size.y));

	float zoom_factor = float(zoomable_view.getSize().x) / float(window.getSize().x);
	if (zoomable_view.getCenter().x < zoom_factor*float(window.getSize().x) / 2) {
		zoomable_view.setCenter(sf::Vector2f(zoom_factor*float(window.getSize().x) / 2, zoomable_view.getCenter().y));
	}
	if (zoomable_view.getCenter().x >(2 - zoom_factor)*(float(window.getSize().x) / 2)) {
		zoomable_view.setCenter(sf::Vector2f((2 - zoom_factor)*(float(window.getSize().x) / 2), zoomable_view.getCenter().y));
	}
	if (zoomable_view.getCenter().y < zoom_factor*float(window.getSize().y) / 2) {
		zoomable_view.setCenter(sf::Vector2f(zoomable_view.getCenter().x, zoom_factor*float(window.getSize().y) / 2));
	}
	if (zoomable_view.getCenter().y >(2 - zoom_factor)*(float(window.getSize().y) / 2)) {
		zoomable_view.setCenter(sf::Vector2f(zoomable_view.getCenter().x, (2 - zoom_factor)*(float(window.getSize().y) / 2)));
	}


	//*******************
	//Positions zoomable_view.
	//*******************
	if (!ui->getGUI_Momentary(gui_MOUSE_RIGHT_BUTTON)) {
		old_mouse_pan_location = ui->getMouseRawPosition(window);
	}
	else {
		if ((ui->getMouseRawPosition(window).x > 300) && (!ui->isMouseOnLargerWidgets())) {
			zoom_factor = float(zoomable_view.getSize().x) / float(window.getSize().x);
			new_mouse_pan_location = ui->getMouseRawPosition(window);
			zoomable_view.move((old_mouse_pan_location.x - new_mouse_pan_location.x)*    zoom_factor, (old_mouse_pan_location.y - new_mouse_pan_location.y)*   zoom_factor);
			if (
				(zoomable_view.getCenter().x > (2 - zoom_factor)*(float(window.getSize().x) / 2))
				||
				(zoomable_view.getCenter().x < zoom_factor*float(window.getSize().x) / 2)
				||
				(zoomable_view.getCenter().y > (2 - zoom_factor)*(float(window.getSize().y) / 2))
				||
				(zoomable_view.getCenter().y < zoom_factor*float(window.getSize().y) / 2)
				)
			{
				zoomable_view.move((new_mouse_pan_location.x - old_mouse_pan_location.x)*    zoom_factor, (new_mouse_pan_location.y - old_mouse_pan_location.y)*    zoom_factor);
			}
			old_mouse_pan_location = new_mouse_pan_location;
		}
	}

}

Window_Manager::Window_Manager(int width, int height) {
	window.create(sf::VideoMode(width, height), "CYCELL2d");
	r_texture.create(width, height);
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(200, 20));
	window.setKeyRepeatEnabled(true);
	window.setMouseCursorVisible(1);
	zoom_size = sf::Vector2f((float)getScreenWidth(), (float)getScreenHeight());
	sf::Vector2u window_size;
	window_size = window.getSize();
	fixed_view.reset(sf::FloatRect(0, 0, (float)window_size.x, (float)window_size.y));
	zoomable_view.reset(sf::FloatRect(0, 0, (float)window_size.x, (float)window_size.y));

	if (!font.loadFromFile("consola.ttf"))
	{
		cout << "Couldn't load font.\n";
		sp();
	}
}