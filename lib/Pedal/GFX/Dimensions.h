#pragma once

class Position
{
public:
    int x = 0, y = 0;
    Position(int x = 0, int y = 0) : x(x), y(y) { }
    Position operator+(const Position& rhs);
    Position operator-(const Position& rhs);
    bool operator==(const Position& rhs);
    bool operator!=(const Position& rhs);
};

Position Position::operator+(const Position& rhs)
{
    Position pos;
    pos.x = this->x + rhs.x;
    pos.y = this->y + rhs.y;
    return pos;
}

Position Position::operator-(const Position& rhs)
{
    Position pos;
    pos.x = this->x - rhs.x;
    pos.y = this->y - rhs.y;
    return pos;
}

bool Position::operator==(const Position& rhs)
{
    return x == rhs.x && y == rhs.y;
}

bool Position::operator!=(const Position& rhs)
{
    return !operator==(rhs);
}

class Size
{
public:
    int width = 0, height = 0;
    
    Size(int width = 0, int height = 0)
    {
        this->width = width;
        this->height = height;
    }

    Size operator+(const Size& rhs);
    Size operator-(const Size& rhs);
    bool operator==(const Size& s);
    bool operator!=(const Size& s);
};

Size Size::operator+(const Size& rhs)
{
    Size size;
    size.width = this->width + rhs.width;
    size.height = this->height + rhs.height;
    return size;
}

Size Size::operator-(const Size& rhs)
{
    Size size;
    size.width = this->width - rhs.width;
    size.height = this->height - rhs.height;
    return size;
}

bool Size::operator==(const Size& s)
{
    return width == s.width && height == s.height;
}

bool Size::operator!=(const Size& s)
{
    return !operator==(s);
}