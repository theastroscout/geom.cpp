#include "../include/json.h"
using json = nlohmann::ordered_json;

#include "../include/surfy/print/print.h"
using surfy::print;

#include "../include/surfy/geom/geom.h"
namespace sg = surfy::geom;


// Global Config
json config;

void pointTest() {
	print("\n\n#### Point Test ####\n\n");

	sg::Shape point("POINT (2 3)");
	print("Point print:", point);

	/*

	Structure test

	*/

	json data = {
		{ "wkt", point.wkt() },
		{ "empty", point.empty },
		{ "x", point.geom.point.x },
		{ "y", point.geom.point.y }
	};
	print(data);
	print("\n");

	/*

	Clip test

	*/

	sg::Shape mask("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
	print("Mask:", mask);
	
	sg::Shape clippedPointOutside = sg::clip(point, mask);
	print("Clipped Point (Outside Mask)", clippedPointOutside);
	
	sg::Shape pointInside("POINT (.5 .5)");
	sg::Shape clippedPointInside = sg::clip(pointInside, mask);
	print("\nPoint (Inside Mask):", pointInside);
	print("Clipped Point (Inside Mask):", clippedPointInside);
	print("\n");

	// Simplify Test

	sg::Shape simplePoint = point.simplify(1);
	print("Simplified Point is the same Point:", simplePoint);


}

void lineTest() {
	print("\n\n#### Line Test ####\n\n");

	sg::Shape line("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)");
	print("Line print:", line);

	/*

	Structure test

	*/

	json data = {
		{ "wkt", line.wkt() },
		{ "empty", line.empty },
		{ "vertices", line.vertices },
		{ "length", line.length },
		{ "coords",
			{
				{ "closed", line.geom.line.closed },
				{ "empty", line.geom.line.empty }
			}
		}
	};
	print(data);
	print("\n");

	/*

	Clip test

	*/

	sg::Shape line4clip("LINESTRING (0 0, 5 5, 11 10, 15 15)");
	print("Clip line:", line4clip);

	sg::Shape mask("POLYGON ((0 0, 0 6, 6 6, 6 0, 0 0))");
	print("Mask:", mask);
	
	sg::Shape clippedLine = sg::clip(line4clip, mask);
	print("Clipped Line:", clippedLine);
	print("\n");

	/*

	Closed Line Clip

	*/

	sg::Shape line4clip_closed("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)");
	print("Clip Closed Line:", line4clip_closed);

	sg::Shape clippedClosedLine = sg::clip(line4clip_closed, mask);
	print("Clipped Closed Line:", clippedClosedLine);
	print("\n");

	/*

	Simplify test

	*/

	sg::Shape complexLine("LINESTRING (0 0, 2 2, 3 3, 10 2, 6 6, 7 7, 30 30)");
	print("Complex Line:", complexLine);

	sg::Shape simpleLine = complexLine.simplify(2);
	print("Simplified Line:", simpleLine);
}

void multiLineTest() {
	print("\n\n#### MultiLine Test ####\n\n");

	sg::Shape multiLine("MULTILINESTRING ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 2 2, 3 3, 10 2, 6 6, 7 7, 30 30),())");
	print("MultiLine print:", multiLine);
	
	/*

	Structure test

	*/

	json items = json::array();
	for (int i=0; i < multiLine.size; ++i) {
		sg::Line& line = multiLine.geom.multiLine.items[i];
		json item = {
			{ "wkt", line.wkt() },
			{ "empty", line.empty },
			{ "closed", line.closed },
			{ "vertices", line.vertices },
			{ "length", line.length }
		};
		items.push_back(item);
	}


	json data = {
		{ "wkt", multiLine.wkt() },
		{ "empty", multiLine.empty },
		{ "size", multiLine.size }, // Number of lines inside MultiLine
		{ "vertices", multiLine.vertices },
		{ "length", multiLine.length },
		{ "items", items }
	};
	print(data);

	/*

	Clip test

	*/

	sg::Shape mask("POLYGON ((0 0, 0 6, 6 6, 6 0, 0 0))");
	print("Mask: ", mask);
	
	sg::Shape clippedLine = sg::clip(multiLine, mask);
	print("Clipped Line:", clippedLine);
	print("\n");

	/*

	Simplify test

	*/

	sg::Shape simpleMultiLine = multiLine.simplify(2);
	print("Simplified MultiLine:", simpleMultiLine);
}

void polygonTest() {
	print("\n\n#### Polygon Test ####\n\n");

	sg::Shape poly("POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0),(0 0, 0 5, 5 5, 5 0, 0 0))");
	print("Polygon Print", poly);

	/*

	Structure test

	*/

	json data = {
		{ "wkt", poly.wkt()} ,
		{ "empty", poly.empty },
		{ "vertices", poly.vertices },
		{ "length", poly.length },
		{ "area", poly.area },
		{ "outer",
			{
				{ "empty", poly.geom.polygon.outer.empty },
				{ "closed", poly.geom.polygon.outer.closed },
				{ "vertices", poly.geom.polygon.outer.vertices },
				{ "length", poly.geom.polygon.outer.length },
				{ "area", poly.geom.polygon.outer.area }
			}
		},
		{ "inner",
			{
				{ "empty", poly.geom.polygon.inner.empty },
				{ "closed", poly.geom.polygon.inner.closed },
				{ "vertices", poly.geom.polygon.inner.vertices },
				{ "length", poly.geom.polygon.inner.length },
				{ "area", poly.geom.polygon.inner.area }
			}
		}
	};
	print(data);
	print("\n");

	// Clip Test

	sg::Shape mask("POLYGON ((0 0, 0 6, 6 6, 6 0, 0 0))");
	print("Mask: ", mask);
	
	sg::Shape clippedPolygon = sg::clip(poly, mask);
	print("Clipped Polygon:", clippedPolygon);
	print("\n");

	// Simplify Test

	sg::Shape complexPoly("POLYGON ((-0.0426899 51.5166536, -0.0426874 51.51564, -0.0415785 51.5156202, -0.0416071 51.5158, -0.0416962 51.5159493, -0.0421352 51.5163716, -0.0425499 51.5166216, -0.0426899 51.5166536))");
	print("Complex Polygon:", complexPoly);
	sg::Shape simplePoly = complexPoly.simplify(.0001);
	print("Simplified Polygon:", simplePoly);
}

void multiPolygonTest() {
	print("\n\n#### MultiPolygon Test ####\n\n");

	sg::Shape multiPolygon("MULTIPOLYGON (((40 40, 41 41, 20 45, 45 30, 40 40),(30 20, 20 15, 20 25, 30 20)),((40 40, 20 45, 45 30, 40 40)),((40 40, 20 45, 45 30, 50 50, 40 40)))");

	print("MultiPolygon Print:", multiPolygon);

	/*

	Structure test

	*/

	json items = json::array();

	for (int i=0; i < multiPolygon.size; ++i) {
		sg::Polygon& poly = multiPolygon.geom.multiPolygon.items[i];
		json item = {
			{ "wkt", poly.wkt() },
			{ "empty", poly.empty },
			{ "vertices", poly.vertices },
			{ "length", poly.length },
			{ "area", poly.area },
			{ "outer",
				{
					{ "empty", poly.outer.empty },
					{ "closed", poly.outer.closed },
					{ "vertices", poly.outer.vertices },
					{ "length", poly.outer.length },
					{ "area", poly.outer.area }
				}
			},
			{ "inner",
				{
					{ "empty", poly.inner.empty },
					{ "closed", poly.inner.closed },
					{ "vertices", poly.inner.vertices },
					{ "length", poly.inner.length },
					{ "area", poly.inner.area }
				}
			}
		};
		items.push_back(item);
	}

	json data = {
		{ "wkt", multiPolygon.wkt()} ,
		{ "empty", multiPolygon.empty },
		{ "vertices", multiPolygon.vertices },
		{ "length", multiPolygon.length },
		{ "area", multiPolygon.area },
		{ "items", items }
	};

	print(data);
	print("\n");

	// Clip Test

	sg::Shape mask("POLYGON ((20 20, 20 40, 40 40, 40 20, 20 20))");
	print("Mask: ", mask);
	
	sg::Shape clippedMultiPolygon = sg::clip(multiPolygon, mask);
	print("Clipped MultiPolygon:", clippedMultiPolygon);
	print("\n");

	// Simplify Test

	sg::Shape simpleMultiPoly = multiPolygon.simplify(2);
	print("Simplified MultiPolygon:", simpleMultiPoly);


}

/*

Prune Test
Remove points lying on the same line

*/

void prune() {
	print("\n\n#### Prune Test ####\n\n");
	sg::Shape line("LINESTRING (0 0, 1 1, 2 2, 3 3, 4 3, 5 2, 6 1, 7 0)");
	print("Line for pruning:", line);
	sg::utils::prune(line.geom.line.coords);
	print("Pruned Line", line); // LINESTRING (0 0, 3 3, 4 3, 7 0)
}

int main() {

	// pointTest();
	// lineTest();
	// multiLineTest();
	// polygonTest();
	// multiPolygonTest();
	prune();

	return 0;
}