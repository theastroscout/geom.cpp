# Surfy.Geom C++


## Installation

```cpp
#include "/include/surfy/geom/geom.h"
namespace sg = surfy::geom;
```

## Shape
```cpp

// Structure
class Shape {
public:
	std::string type; // Point, Line, MultiLine, Polygon, MultiPolygon, Dummy
	std::string source; // Storing source string
	
	union Geometry {
		Point point;
		Line line;
		MultiLine multiLine;
		Polygon polygon;
		MultiPolygon multiPolygon;
	} geom;

	std::string wkt();

	Shape(const std::string& src) {

	}
};

// Create
sg::Shape point = sg::Shape("POINT (1. 1.)");

```

## Point
```cpp

// Structure
struct Point : public Geometry {
	double x;
	double y;
}

// Create
sg::Shape point = sg::Shape("POINT (1 2)");
std::string point.type // Point
double point.geom.point.x // 1
double point.geom.point.y // 2

// To String
std::string wkt = point.wkt(); // "POINT (1 2)"

// Print
std::cout << point << std::endl; // "POINT (1 2)"

```

## Line
```cpp

// Structure
struct Line : public Geometry {
	bool closed; // If first point == end point
	unsigned int size; // Vertices
	double length;
	std::vector<Point> coords;
}

// Create
sg::Shape line = sg::Shape("LINESTRING (1 1, 2 2)");
std::string line.type // Line

// Extract Points
Point p1 = line.geom.line.coords[0];
Point p2 = line.geom.line.coords[1];

// Get Coords
double p1.x // 1
double p2.y // 2

unsigned int line.size // Overall size
double line.length // Overall length

// Line params, which are the same since there is only one Line
unsigned int line.geom.line.size // Line size
double line.geom.line.length // Line length

bool line.geom.line.closed // 1 or 0

// To String
std::string wkt = line.wkt(); // "LINESTRING (1 1, 2 2)"

// Print
std::cout << line << std::endl; // "LINESTRING (1 1, 2 2)"

```

## Polygon
```cpp

// Structure
struct Polygon : public Geometry {
	Line outer;
	Line inner;
}

/*

Create
Only Outer: "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0))"
Both Outer and Inner: "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"
*/

sg::Shape poly = sg::Shape("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
std::string poly.type // Polygon
double poly.geom.polygon.outer.coords[1].y // 10.
double poly.geom.polygon.inner.coords[1].y // 5.

// Overall (Outer + Inner)
// poly.size is an alias for poly.geom.polygon.size
unsigned int poly.size // Overall size
double poly.length // Overall length
double poly.area // Overall area

// Outer Polygon
unsigned int poly.geom.outer.size // Inner Polygon size
double poly.geom.outer.length // Inner Polygon length
double poly.geom.outer.area // Inner Polygon area

// Inner Polygon
unsigned int poly.geom.inner.size // Inner Polygon size
double poly.geom.inner.length // Inner Polygon length
double poly.geom.inner.area // Inner Polygon area

// To String
std::string wkt = poly.wkt(); // "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"


// Print
std::cout << poly << std::endl; // "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"

```

## Clip
Function takes a Shape and a Mask defined by four points representing a rectangular box. It clips the Shape according to the mask, ensuring that the Shape stays within the boundaries of the Mask. The function returns new Shape.

```cpp

// Counterclockwise Mask: TopLeft, BottomLeft, BottomRight, TopRight
std::vector<sg::Point> mask = {{0, 0}, {0, 6}, {6, 6}, {6, 0}};

// Clip Polygon
sg::Shape poly = sg::Shape("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
sg::Shape clippedPolygon = sg::clip(poly, mask);
// Geometry in clippedPolygon.geom.polygon
std::cout << clippedPolygon << std::end; // POLYGON ((0 0, 0 6, 6 6, 6 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))

// Clip Line
sg::Shape line = sg::Shape("LINESTRING (0 0, 5 5, 11 10, 15 15)");
sg::Shape clippedLine = sg::clip(line, mask);
// Geometry in clippedLine.geom.line
std::cout << clippedLine << std::end; // LINESTRING ((0 0, 5 5, 6 5.83333))

```