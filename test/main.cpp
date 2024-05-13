#include "include/surfy/print/print.h"
#include "include/surfy/geom/geom.h"

int main() {
	surfy::geom::Shape pointShape = surfy::geom::Shape("POINT (-0.035706 51.484804)");
	print("Type:", pointShape.type);
	std::string pointWKT = pointShape.wkt();
	print(pointWKT, "\n\n");

	surfy::geom::Shape lineShape = surfy::geom::Shape("LINESTRING (-0.035706 51.484804, -0.035706 51.484804)");
	print("Type:", lineShape.type);
	std::string lineWKT = lineShape.wkt();
	print(lineWKT, "\n\n");

	

	surfy::geom::Shape polyShape = surfy::geom::Shape("POLYGON ((-4.274372 55.858587, -4.273536 55.858584, -4.273530 55.858397, -4.273970 55.858397, -4.273975 55.858190, -4.273627 55.858177, -4.273637 55.858000, -4.274458 55.858009, -4.274426 55.858539, -4.274372 55.858587))");
	print("Type:", polyShape.type);
	print("Vertices", polyShape.geom.polygon.vertices);
	print("Area", polyShape.geom.polygon.area);
	print("Length", polyShape.geom.polygon.length);
	std::string polyWKT = polyShape.wkt();
	print(polyWKT, "\n\n");

	surfy::geom::Shape polyShape2 = surfy::geom::Shape("POLYGON ((-0.035706 52.484804, -0.025706 51.484804))");
	print("Type:", polyShape2.type);
	print("Area", polyShape2.geom.polygon.area);
	print("Length", polyShape2.geom.polygon.length);
	std::string polyWKT2 = polyShape2.wkt();
	print(polyWKT2, "\n\n");
}