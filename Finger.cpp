#include "Finger.h"

Finger::Finger(int id)
{
    this->_id = id;
}

void Finger::update(bool is_touching, int x, int y, int force, int area)
{
    if (this->is_touching && is_touching)
    {
        // update relative position
        this->_relative_x = x - this->x;
        this->_relative_y = y - this->y;
    }
    else
    {
        // reset relative position
        this->_relative_x = 0;
        this->_relative_y = 0;
    }

    this->_is_touching = is_touching;
    this->_x = x;
    this->_y = y;
    this->_force = force;
    this->_area = area;
}
