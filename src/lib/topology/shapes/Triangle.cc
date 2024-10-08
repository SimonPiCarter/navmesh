#include "Triangle.hh"
#include<sstream>


Triangle::Triangle(std::array<Point, 3> const &points_p) : points(points_p)
{
    center = (points[0]+points[1]+points[2])/3;
}

Triangle::Triangle(Point const &a, Point const &b, Point const &c) : Triangle(std::array<Point, 3> {a,b,c}) {}


bool are_connected(Triangle const &a, Triangle const &b)
{
    int  equalities_l = 0;
    for(int  i = 0 ; i < 3 ; ++ i)
    {
        for(int  j = 0 ; j < 3 ; ++ j)
        {
            if(a.points[i] == b.points[j])
            {
                equalities_l += 1;
            }
        }
    }
    return equalities_l >= 2;
}

/*************** Geometric Predicates for Delaunay Diagrams *****************/
inline long long triangle_area(const Point &a, const Point &b, const Point &c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool ccw (const Point &a, const Point &b, const Point &c)
{
    return (b.x - a.x)*(c.y - a.y)-(c.x - a.x)*(b.y - a.y) > 0;
}

bool is_inside_circle(Triangle const &tr, Point const &p)
{
    Point const &a = tr.points[0];
    Point const &b = tr.points[1];
    Point const &c = tr.points[2];
    if(ccw(a, b, c))
    {
        return (a.x * a.x + a.y * a.y) * triangle_area(b, c, p) -
                (b.x * b.x + b.y * b.y) * triangle_area(a, c, p) +
                (c.x * c.x + c.y * c.y) * triangle_area(a, b, p) -
                (p.x * p.x + p.y * p.y) * triangle_area(a, b, c) >
            0;
    }
    else
    {
        return (a.x * a.x + a.y * a.y) * triangle_area(b, c, p) -
                (b.x * b.x + b.y * b.y) * triangle_area(a, c, p) +
                (c.x * c.x + c.y * c.y) * triangle_area(a, b, p) -
                (p.x * p.x + p.y * p.y) * triangle_area(a, b, c) <
            0;
    }
}

long long sign (Point p1, Point p2, Point p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool is_inside(Triangle const &tr, Point const &pt)
{
    Point const &a = tr.points[0];
    Point const &b = tr.points[1];
    Point const &c = tr.points[2];
    long long d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, a, b);
    d2 = sign(pt, b, c);
    d3 = sign(pt, c, a);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

std::string draw_line(Point const &a, Point const &b)
{
    std::stringstream ss_l;
    ss_l<<"<line x1=\""<<a.x<<"\" x2=\""<<b.x<<"\" y1=\""<<a.y<<"\" y2=\""<<b.y<<"\" stroke=\"white\" stroke-width=\"0.01\"/>"<<std::endl;
    return ss_l.str();
}

std::string generate_svg(std::vector<Triangle> const &triangles_p)
{
    std::stringstream ss_l;
    ss_l<<"<svg width=\"10\" height=\"10\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"<<std::endl;
    for(Triangle const &tr : triangles_p)
    {
        ss_l<<draw_line(tr.points[0], tr.points[1]);
        ss_l<<draw_line(tr.points[1], tr.points[2]);
        ss_l<<draw_line(tr.points[2], tr.points[0]);
    }
    ss_l<<"</svg>"<<std::endl;
    return ss_l.str();
}
