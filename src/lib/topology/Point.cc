#include "Point.hh"


long long operator*(Point const &a, Point const &b)
{
    return a.x * b.x + a.y * b.y;
}

Point operator-(Point const &a, Point const &b)
{
    return {a.x-b.x, a.y-b.y};
}

Point operator+(Point const &a, Point const &b)
{
    return {a.x+b.x, a.y+b.y};
}

Point operator/(Point const &a, long long d)
{
    return {a.x/d, a.y/d};
}

bool operator==(Point const &a, Point const &b)
{
    return a.x == b.x
        && a.y == b.y;
}

bool operator!=(Point const &a, Point const &b)
{
    return a.x != b.x
        || a.y != b.y;
}

long long square_distance(Point const &a, Point const &b)
{
    return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
}

std::ostream &operator<<(std::ostream& oss_p, Point const &p)
{
    oss_p<<"("<<p.x<<","<<p.y<<")"<<std::endl;
    return oss_p;
}
