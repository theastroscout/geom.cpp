# Surfy.Geom C++
Demo test/test.cpp

## Installation

```cpp
#include "/include/surfy/geom/geom.hpp"
namespace sg = surfy::geom;
```

## Shape
Shape is the main object of this library. It includes all basic functions and represents all geometry types such as Point, Line, MultiLine, Polygon and MultiPolygon that are defined during construction in Shape.geom.

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
sg::Shape point("POINT (1. 1.)");

```

## Point
```cpp

// Structure
struct Point : public Geometry {
	double x, y;
}

// Create
sg::Shape point("POINT (1 2)");
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
sg::Shape line("LINESTRING (1 1, 2 2)");

std::string line.type // Line
bool line.empty
unsigned int line.vertices // Number of vertices
double line.length // Length

// Line params, which are the same since there is only one Line
unsigned int line.geom.line.vertices // Number of Line's vertices
double line.geom.line.length // Line length

bool line.geom.line.closed
// To String
std::string wkt = line.wkt(); // "LINESTRING (1 1, 2 2)"

// Print
std::cout << line << std::endl; // "LINESTRING (1 1, 2 2)"

// Extract Points
Point p1 = line.geom.line.coords[0];
Point p2 = line.geom.line.coords[1];

// Get Coords
double p1.x // 1
double p2.y // 2

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

sg::Shape poly("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
std::string poly.type // Polygon

// Overall (Outer + Inner)
// poly.size is an alias for a poly.geom.polygon.size
unsigned int poly.vertices // Overall number of vertices
double poly.length // Overall length
double poly.area // Overall area

// Outer Polygon
bool poly.geom.outer.empty
bool poly.geom.outer.closed
unsigned int poly.geom.outer.vertices // Outer Polygon vertices
double poly.geom.outer.length // Outer Polygon length
double poly.geom.outer.area // Outer Polygon area

// Inner Polygon
bool poly.geom.inner.empty
bool poly.geom.inner.closed
unsigned int poly.geom.inner.vertices // Inner Polygon vertices
double poly.geom.inner.length // Inner Polygon length
double poly.geom.inner.area // Inner Polygon area

// To String
std::string wkt = poly.wkt(); // "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"

// Print
std::cout << poly << std::endl; // "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"

double poly.geom.polygon.outer.coords[1].y // 10.
double poly.geom.polygon.inner.coords[1].y // 5.

```

## Clip
Clip takes Shape and Mask defined by four points representing a rectangular box, and clips Shape according to the mask, ensuring that Shape stays within the boundaries of the mask. Returns new Shape.

```cpp

// Counterclockwise Mask: TopLeft, BottomLeft, BottomRight, TopRight
std::vector<sg::Point> mask = {{0, 0}, {0, 6}, {6, 6}, {6, 0}};

// Clip Polygon
sg::Shape poly("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
sg::clip(poly, mask);
// Geometry in poly.geom.polygon
std::cout << poly << std::end; // POLYGON ((0 0, 0 6, 6 6, 6 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))

// Clip Line
sg::Shape line("LINESTRING (0 0, 5 5, 11 10, 15 15)");
sg::clip(line, mask);
// Geometry in line.geom.line
std::cout << line << std::end; // LINESTRING ((0 0, 5 5, 6 5.83333))

```

## Simplify
Simplify uses the Douglas-Peucker simplification algorithm for reducing the number of points in a curve while preserving its general shape. It works by recursively dividing the curve into line segments and retaining only those points that are sufficiently far from the line segments.

Tolerance in the context of the Douglas-Peucker algorithm refers to the maximum distance allowed between the original curve and the simplified approximation.

Simplify returns new Shape.

```cpp
sg::Shape complexLine("LINESTRING (0 0, 2 2, 5 5, 6 6, 7 7)");
complexLine.simplify(1.);
std::cout << complexLine << std::endl; // "LINESTRING (0 0, 7 7)"

sg::Shape complexPolygon("POLYGON ()");
complexPolygon.simplify(1.);
std::cout << complexPolygon << std::endl;

```
