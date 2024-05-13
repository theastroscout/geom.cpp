# Surfy.Geom C++


## Installation

```cpp
#include "/include/surfy/geom/geom.h"
using sg = surfy::geom;
```

## Shape
```cpp

// Structure
class Shape {
public:
	std::string type; // Point, Line, MultiLine, Polygon, MultiPolygon, Error
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

```

## Line
```cpp

// Structure
struct Line : public Geometry {
	std::vector<Point> coords;
}

// Create
sg::Shape line = sg::Shape("LINESTRING (1 1, 2 2)");
std::string line.type // Line
double point.geom.line[0].x // 1
double point.geom.line[1].y // 2

```

## Polygon
```cpp

// Structure
struct Polygon : public Geometry {
	std::vector<Point> outer;
	std::vector<Point> inner;
}

/*

Create
Only Outer: "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0))"
Both Outer and Inner: "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))"
*/

sg::Shape line = sg::Shape("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
std::string line.type // Polygon
double point.geom.polygon.outer[1].y // 10
double point.geom.polygon.inner[1].y // 5

```