#pragma once
#pragma once
#define SFML_NO_DEPRECATED_WARNINGS
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include<stdlib.h>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include <locale>
#include <list>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <queue>
using namespace std;
class User_Input;
class Window_Manager {


	// this class is designed for a window with two views.
	// The first is called fixed_view and is not affected by zoom, pan, etc. Suitable for placement of GUI stuff.
	// The second is called zoomable_view and is affected, upon update, by mouse zoom and pan activity.
	// zoomable_view has 'smart' zooming whereby the view will pan as needed to prevent 'unused' areas of the view
	// showing up. In order to do this, zoomable_view needs to have a pre-defined size which, in the implementation, will
	// be set to the parent window size. Other implementations should be used if one wishes to have an infinite scrolling
	// plane.

private:
	sf::RenderTexture r_texture;
	sf::RenderWindow window;
	sf::View fixed_view;
	sf::View zoomable_view;
	sf::Vector2f zoom_size;
	sf::Vector2i new_mouse_pan_location;
	sf::Vector2i old_mouse_pan_location;
	sf::Font font;
	bool resize_needed = 0;
public:
	sf::View get_fixed_view() { return fixed_view; }
	sf::View get_zoomable_view() { return zoomable_view; }
	sf::Font getFont() { return font; }
	int getScreenWidth() { return window.getSize().x; }
	int getScreenHeight() { return window.getSize().y; }
	void change_to_Fixed_View() { window.setView(fixed_view); r_texture.setView(fixed_view); }
	void change_to_Zoomable_View() { window.setView(zoomable_view); r_texture.setView(zoomable_view);
	}
	void update(User_Input * ui);
	bool isResizeNeeded() { return resize_needed; }
	sf::RenderWindow& getWindow() { return window; }
	sf::RenderTexture& get_r_texture() { return r_texture; }
	Window_Manager(int width, int height);


};
