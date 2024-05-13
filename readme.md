# Surfy.Geom C++


## Installation

```cpp
#include "/include/surfy/geom/geom.h"
using sg = surfy::geom;
```

## Shape
```cpp
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

sg::Shape point = sg::Shape("POINT (-0.035706 51.484804)");
```

## Point
```cpp
sg::Shape point = sg::Shape("POINT (-0.035706 51.484804)");
std::string point.type // Point
double point.geom.point.x
double point.geom.point.y
```