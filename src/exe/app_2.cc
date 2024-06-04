#include "topology/Point.hh"

class QuadEdge;
class Edge
{
    friend QuadEdge;
    friend void Splice(Edge *, Edge *);

private:
    int num;
    Edge *next;
    Point *data;

public:
    Edge() { data = 0; }
    Edge *Rot();
    Edge *invRot();
    Edge *Sym();
    Edge *Onext();
    Edge *Oprev();
    Edge *Dnext();
    Edge *Dprev();
    Edge *Lnext();
    Edge *Lprev();
    Edge *Rnext();
    Edge *Rprev();
    Point *Org();
    Point *Dest();
    const Point &Org2d() const;
    const Point &Dest2d() const;
    void EndPoints(Point *, Point *);
    QuadEdge *Qedge() { return (QuadEdge *)(this - num); }
};
class QuadEdge
{
    friend Edge *MakeEdge();

private:
    Edge e[4];

public:
    QuadEdge();
};
class Subdivision
{
private:
    Edge *startingEdge;
    Edge *Locate(const Point &);

public:
    Subdivision(const Point &, const Point &, const Point &);
    void InsertSite(const Point &);
    void Draw();
};
inline QuadEdge::QuadEdge()
{
    e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
    e[0].next = &(e[0]);
    e[1].next = &(e[3]);
    e[2].next = &(e[2]);
    e[3].next = &(e[1]);
}
/************************* Edge Algebra *************************************/
inline Edge *Edge::Rot()
// Return the dual of the current edge, directed from its right to its left.
{
    return (num < 3) ? this + 1 : this - 3;
}
inline Edge *Edge::invRot()
// Return the dual of the current edge, directed from its left to its right.
{
    return (num > 0) ? this - 1 : this + 3;
}
inline Edge *Edge::Sym()
// Return the edge from the destination to the origin of the current edge.
{
    return (num < 2) ? this + 2 : this - 2;
}
inline Edge *Edge::Onext()
// Return the next ccw edge around (from) the origin of the current edge.
{
    return next;
}
inline Edge *Edge::Oprev()
// Return the next cw edge around (from) the origin of the current edge.
{
    return Rot()->Onext()->Rot();
}
inline Edge *Edge::Dnext()
// Return the next ccw edge around (into) the destination of the current edge.
{
    return Sym()->Onext()->Sym();
}
inline Edge *Edge::Dprev()
// Return the next cw edge around (into) the destination of the current edge.
{
    return invRot()->Onext()->invRot();
}
inline Edge *Edge::Lnext()
// Return the ccw edge around the left face following the current edge.
{
    return invRot()->Onext()->Rot();
}
inline Edge *Edge::Lprev()
// Return the ccw edge around the left face before the current edge.
{
    return Onext()->Sym();
}
inline Edge *Edge::Rnext()
// Return the edge around the right face ccw following the current edge.
{
    return Rot()->Onext()->invRot();
}
inline Edge *Edge::Rprev()
// Return the edge around the right face ccw before the current edge.
{
    return Sym()->Onext();
}
/************** Access to data pointers *************************************/
inline Point *Edge::Org()
{
    return data;
}
inline Point *Edge::Dest()
{
    return Sym()->data;
}
inline const Point &Edge::Org2d() const
{
    return *data;
}
inline const Point &Edge::Dest2d() const
{
    return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
}
inline void Edge::EndPoints(Point * or_p, Point *de)
{
    data = or_p ;
    Sym()->data = de;
}
/*********************** Basic Topological Operators ************************/
Edge *MakeEdge()
{
    QuadEdge *ql = new QuadEdge;
    return ql->e;
}
void Splice(Edge *a, Edge *b)
// This operator affects the two edge rings around the origins of a and b,
// and, independently, the two edge rings around the left faces of a and b.
// In each case, (i) if the two rings are distinct, Splice will combine
// them into one; (ii) if the two are the same ring, Splice will break it
// into two separate pieces.
// Thus, Splice can be used both to attach the two edges together, and
// to break them apart. See Guibas and Stolfi (1985) p.96 for more details
// and illustrations.
{
    Edge *alpha = a->Onext()->Rot();
    Edge *beta = b->Onext()->Rot();
    Edge *t1 = b->Onext();
    Edge *t2 = a->Onext();
    Edge *t3 = beta->Onext();
    Edge *t4 = alpha->Onext();
    a->next = t1;
    b->next = t2;
    alpha->next = t3;
    beta->next = t4;
}
void DeleteEdge(Edge *e)
{
    Splice(e, e->Oprev());
    Splice(e->Sym(), e->Sym()->Oprev());
    delete e->Qedge();
}
/************* Topological Operations for Delaunay Diagrams *****************/
Subdivision::Subdivision(const Point &a, const Point &b, const Point &c)
// Initialize a subdivision to the triangle defined by the points a, b, c.
{
    Point *da, *db, *dc;
    da = new Point(a), db = new Point(b), dc = new Point(c);
    Edge *ea = MakeEdge();
    ea->EndPoints(da, db);
    Edge *eb = MakeEdge();
    Splice(ea->Sym(), eb);
    eb->EndPoints(db, dc);
    Edge *ec = MakeEdge();
    Splice(eb->Sym(), ec);
    ec->EndPoints(dc, da);
    Splice(ec->Sym(), ea);
    startingEdge = ea;
}
Edge *Connect(Edge *a, Edge *b)
// Add a new edge e connecting the destination of a to the
// origin of b, in such a way that all three have the same
// left face after the connection is complete.
// Additionally, the data pointers of the new edge are set.
{
    Edge *e = MakeEdge();
    Splice(e, a->Lnext());
    Splice(e->Sym(), b);
    e->EndPoints(a->Dest(), b->Org());
    return e;
}
void Swap(Edge *e)
// Essentially turns edge e counterclockwise inside its enclosing
// quadrilateral. The data pointers are modified accordingly.
{
    Edge *a = e->Oprev();
    Edge *b = e->Sym()->Oprev();
    Splice(e, a);
    Splice(e->Sym(), b);
    Splice(e, a->Lnext());
    Splice(e->Sym(), b->Lnext());
    e->EndPoints(a->Dest(), b->Dest());
}
/*************** Geometric Predicates for Delaunay Diagrams *****************/
inline long long TriArea(const Point &a, const Point &b, const Point &c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
int InCircle(const Point &a, const Point &b,
             const Point &c, const Point &d)
// Returns true if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
{
    return (a.x * a.x + a.y * a.y) * TriArea(b, c, d) -
               (b.x * b.x + b.y * b.y) * TriArea(a, c, d) +
               (c.x * c.x + c.y * c.y) * TriArea(a, b, d) -
               (d.x * d.x + d.y * d.y) * TriArea(a, b, c) >
           0;
}
int ccw(const Point &a, const Point &b, const Point &c)
// Returns true if the points a, b, c are in a counterclockwise order
{
    return (TriArea(a, b, c) > 0);
}
int RightOf(const Point &x, Edge *e)
{
    return ccw(x, e->Dest2d(), e->Org2d());
}
int LeftOf(const Point &x, Edge *e)
{
    return ccw(x, e->Org2d(), e->Dest2d());
}
int OnEdge(const Point &x, Edge *e)
// A predicate that determines if the point x is on the edge e.
// The point is considered on if it is in the EPS-neighborhood
// of the edge.
{
    if (x == e->Org2d() || x == e->Dest2d())
        return true;
    long long t1 = square_distance(x, e->Org2d());
    long long t2 = square_distance(x, e->Dest2d());
    long long t3 = square_distance(e->Org2d(), e->Dest2d());
    if (t1 > t3 || t2 > t3)
        return false;
    Point dir_1 = e->Dest2d() - x;
    Point dir_2 = x - e->Org2d();
    if(dir_1.x == 0)
    {
        return dir_2.x == 0;
    }
    if(dir_2.x == 0)
    {
        return false;
    }
    return dir_1.y/dir_1.x == dir_2.y/dir_2.x;
}

/************* An Incremental Algorithm for the Construction of *************/
/************************ Delaunay Diagrams *********************************/
Edge *Subdivision::Locate(const Point &x)
// Returns an edge e, s.t. either x is on e, or e is an edge of
// a triangle containing x. The search starts from startingEdge
// and proceeds in the general direction of x. Based on the
// pseudocode in Guibas and Stolfi (1985) p.121.
{
    Edge *e = startingEdge;
    while (true)
    {
        if (x == e->Org2d() || x == e->Dest2d())
            return e;
        else if (RightOf(x, e))
            e = e->Sym();
        else if (!RightOf(x, e->Onext()))
            e = e->Onext();
        else if (!RightOf(x, e->Dprev()))
            e = e->Dprev();
        else
            return e;
    }
}

void Subdivision::InsertSite(const Point &x)
// Inserts a new point into a subdivision representing a Delaunay
// triangulation, and fixes the affected edges so that the result
// is still a Delaunay triangulation. This is based on the
// pseudocode from Guibas and Stolfi (1985) p.120, with slight
// modifications and a bug fix.
{
    Edge *e = Locate(x);
    if ((x == e->Org2d()) || (x == e->Dest2d())) // point is already in
        return;
    else if (OnEdge(x, e))
    {
        e = e->Oprev();
        DeleteEdge(e->Onext());
    }
    // Connect the new point to the vertices of the containing
    // triangle (or quadrilateral, if the new point fell on an
    // existing edge.)
    Edge *base = MakeEdge();
    base->EndPoints(e->Org(), new Point(x));
    Splice(base, e);
    startingEdge = base;
    do
    {
        base = Connect(e, base->Sym());
        e = base->Oprev();
    } while (e->Lnext() != startingEdge);
    // Examine suspect edges to ensure that the Delaunay condition
    // is satisfied.
    do
    {
        Edge *t = e->Oprev();
        if (RightOf(t->Dest2d(), e) &&
            InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x))
        {
            Swap(e);
            e = e->Oprev();
        }
        else if (e->Onext() == startingEdge) // no more suspect edges
            return;
        else // pop a suspect edge
            e = e->Onext()->Lprev();
    } while (true);
}


int main()
{
    return 0;
}
