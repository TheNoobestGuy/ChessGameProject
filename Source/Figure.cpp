#include "Figure.h"

Figure::Figure(std::string name, int ID, Field_ID field_ID, bool color, int size)
{
	this->name = name;
	this->ID = ID;
	this->occupied_field = field_ID;
	this->color = color;
	this->size = size;

	if (color == 0)
		this->player = 1;
	else if (color == 1)
		this->player = 2;

	this->entangled = false;
	this->en_passant = false;
	this->first_move = true;
	this->picked_up = false;

	this->figure_rect = GameEngine::CreateRectangle(field_ID.x, field_ID.y, size);
	this->motion_rect = GameEngine::CreateRectangle(0, 0, size);
}

Figure::~Figure() {}

void Figure::PickUp(bool &figure_picked_up)
{

	if (GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &figure_rect) && GameEngine::mouse_left && !figure_picked_up)
	{
		picked_up = true;
		figure_picked_up = true;
	}
	else if (picked_up && !GameEngine::mouse_left && figure_picked_up)
	{
		picked_up = false;
		figure_picked_up = false;
	}
	
	if (picked_up)
	{
		motion_rect.x = GameEngine::mouse_x - (size / 2);
		motion_rect.y = GameEngine::mouse_y - (size / 2);
	}
}

void Figure::ChangePosition(Field_ID field)
{
	this->occupied_field = field;

	figure_rect.x = occupied_field.x * size;
	figure_rect.y = occupied_field.y * size;

	if (first_move)
	{
		first_move = false;
	}
	else
	{
		en_passant = false;
	}
}
