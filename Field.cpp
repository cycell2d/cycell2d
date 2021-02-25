#include "Field.h"
#include "Window_Manager.h"
using namespace std;




void Field::acceptImageAndInitializeEverything(sf::Image im, string name)
{
	filename = name;
	int r= im.getSize().y;
	int c= im.getSize().x;
	max_row = r - 1;
	max_col = c - 1;
	FieldData_row_col = vector < vector <int> >(r, vector <int>(c, 0));

	field_RenderTexture.create(c, r);
	field_RenderTexture.clear(sf::Color(255, 255, 255, 255));

	sf::RectangleShape single_pixel;
	single_pixel.setSize(sf::Vector2f(1.0, 1.0));
	sf::Color probe;


	for (int x = 0; x < c; x++) {
		for (int y = 0; y < r; y++) {
			probe = im.getPixel(x, y);
			if ((probe.r == 1) || (probe.g == 1) || (probe.b == 1)) {
				default_location = sf::Vector2f((float)x,(float)y);
				probe.r *= 255;
				probe.g *= 255;
				probe.b *= 255;
			}
			single_pixel.setFillColor(probe);
			single_pixel.setPosition((float)x, (float)y);
			field_RenderTexture.draw(single_pixel);
		}
	}
	field_RenderTexture.display();

	field_sprite.setTexture(field_RenderTexture.getTexture());
	update_field_data_from_rendertexture();
	}

void Field::update_field_data_from_rendertexture()
{
	
	field_image = field_RenderTexture.getTexture().copyToImage();
	int image_width = field_image.getSize().x;
	int image_height = field_image.getSize().y;
	FieldData_row_col = vector < vector <int> >(image_height, vector <int>(image_width, 0));
	for (int x = 0; x < image_width; x++) {
		for (int y = 0; y < image_height; y++) {
		

			if (field_image.getPixel(x, y)==sf::Color::White) {
				FieldData_row_col[y][x] = 0;
			}
			if (field_image.getPixel(x, y) == sf::Color::Red) {
				FieldData_row_col[y][x] = 1;
			}
			if (field_image.getPixel(x, y) == sf::Color::Green) {
				FieldData_row_col[y][x] = 2;
			}
			if (field_image.getPixel(x, y) == sf::Color::Blue) {
				FieldData_row_col[y][x] = 3;
			}
			if (field_image.getPixel(x, y) == sf::Color::Yellow) {
				FieldData_row_col[y][x] = 4;
			}
		}
	}
	isFieldDataInSynchWithTexture = 1;

	//create sprite for displaying in iteration mode.
	field_image_with_white_transparent = field_RenderTexture.getTexture().copyToImage();
	field_image_with_white_transparent.createMaskFromColor(sf::Color::White);
	field_texture_with_white_transparent.loadFromImage(field_image_with_white_transparent);
	field_sprite_with_white_transparent.setTexture(field_texture_with_white_transparent);
}




void Field::clear()
{
	field_RenderTexture.clear(sf::Color(255, 255, 255, 255));
	update_field_data_from_rendertexture();
	filename = "*.png";
}

Field::Field(int r, int c)
{
	max_row = r - 1;
	max_col = c - 1;
	FieldData_row_col =  vector < vector <int> >(r, vector <int>(c, 0));
	field_RenderTexture.create(c,r);
	field_RenderTexture.clear(sf::Color(255, 255, 255, 255));
	field_sprite.setTexture(field_RenderTexture.getTexture());
	update_field_data_from_rendertexture();
	default_location = sf::Vector2f((float)(max_col / 2),(float)(max_row / 2));
}


Field::~Field()
{
}
