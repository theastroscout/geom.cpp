# Surfy.Geom C++


### Installation

```cpp
#include "/include/surfy/geom/geom.h"
using sg = surfy::geom;
```

## Point
```cpp
sg::Shape point = sg::Shape("POINT (-0.035706 51.484804)");
std::string point.type // Point
double point.geom.point.x
double point.geom.point.y
```