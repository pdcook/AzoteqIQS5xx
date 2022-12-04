#ifndef FINGER_H
#define FINGER_H

class Finger
{
    private:
        int _id;
        bool _is_touching = false;
        int _x = 0;
        int _y = 0;
        int _force = 0;
        int _area = 0;
        int _relative_x = 0;
        int _relative_y = 0;
    public:
        Finger(int id);
        const int& id = _id;
        const bool& is_touching = _is_touching;
        const int& x = _x;
        const int& y = _y;
        const int& force = _force;
        const int& area = _area;
        const int& relative_x = _relative_x;
        const int& relative_y = _relative_y;

        void update(bool is_touching, int x, int y, int force, int area);
};


#endif // FINGER_H
