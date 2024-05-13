#ifndef GEOM_HPP
#define GEOM_HPP
#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace surfy::geom {

	struct Geometry {
		unsigned int vertices = 0;
		double length = 0;
		double area = 0;
		std::string wkt() const; // Return WKT string
	};

	std::string Geometry::wkt() const {
		return "Blimey!";
	}

	struct Point : public Geometry {
		double x;
		double y;
	};

	struct Line : public Geometry {
		std::vector<Point> coords;
	};

	struct MultiLine : public Geometry {
		std::vector<Line> items;
	};

	struct Polygon : public Geometry {
		std::vector<Point> inner;
		std::vector<Point> outer;
	};

	struct MultiPolygon : public Geometry {
		std::vector<Polygon> items;
	};

	struct Dummy {
		std::string type = "Dummy";
	};

	namespace utils {};



	void test();
}

// #include "types.h"
#include "utils.h"

namespace surfy::geom {

	class Shape {
	public:
		std::string type;
		std::string source;
		unsigned int vertices = 0;
		double length = 0;
		double area = 0;

		union Geometry {
			Point point;
			Line line;
			MultiLine multiline;
			Polygon polygon;
			MultiPolygon multiPolygon;

			Geometry(){}
			~Geometry(){}

		} geom;

		/*

		WKT
		Stringify geometry, e.g. POINT (1 2) or LINESTRING (0 0, 2 2)

		*/

		std::string wkt() {
			std::stringstream os;		

			if (type == "Point") {

				os << "POINT (";
				utils::printPoint(os, geom.point);
				os << ")";

			} else if (type == "Line") {

				os << "LINESTRING (";
				utils::printCoords(os, geom.line.coords);
				os << ")";
			}

			return os.str();
		}

		/*

		Update Vertices, Length, and Area

		*/

		void refresh() {
			vertices = 0;
			length = .0;
			area = .0;

			if (type == "Point") {
				vertices = 1;
			} else if (type == "Line") {
				size_t lineSize = geom.line.coords.size();
				vertices = lineSize;

			} else if (type == "Polygon") {
				if(!geom.polygon.outer.empty()){
					size_t outerSize = geom.polygon.outer.size();
					geom.polygon.vertices += outerSize;
					geom.polygon.length += utils::length(geom.polygon.outer, outerSize);
					geom.polygon.area += utils::area(geom.polygon.outer, outerSize);
				}

				if(!geom.polygon.inner.empty()){
					size_t innerSize = geom.polygon.inner.size();
					geom.polygon.vertices += innerSize;
					geom.polygon.length += utils::length(geom.polygon.inner, innerSize);
					geom.polygon.area += utils::area(geom.polygon.inner, innerSize);
				}

				vertices += geom.polygon.vertices;
				length += geom.polygon.length;
				area += geom.polygon.area;
			}
		}		

		Shape (const std::string& src) {
			source = src; // Store source just in case

			// Find the start and end positions of the coordinates substring
			size_t startPos = src.find("(");
			size_t endPos = src.rfind(")");
			std::string body = src.substr(startPos + 1, endPos - startPos - 1);

			if (startPos == std::string::npos || endPos == std::string::npos) {
				type = "Error";
				return;
			}

			if (src.find("POINT") != std::string::npos) {
				
				type = "Point";
				new (&geom.point) Point();

				std::vector<Point> coords = utils::parseCoordsString(body);
				geom.point = coords[0];

			} else if (src.find("LINE") != std::string::npos) {

				type = "Line";
				new (&geom.line) Line();

				geom.line.coords = utils::parseCoordsString(body);

			} else if (src.find("POLYGON") != std::string::npos) {

				type = "Polygon";
				new (&geom.polygon) Polygon();

				int pass = 1;
				size_t pos = 0;
				while (pos < body.size()) {
					size_t start = body.find("(", pos);
					size_t end = body.find(")", start);
					if (start == std::string::npos || end == std::string::npos) {
						break; // No more polygons found
					}

					std::string polyStr = body.substr(start + 1, end - start - 1);

					pos = end + 1;

					std::vector<Point> coords = utils::parseCoordsString(polyStr);

					if (pass == 1) {
						geom.polygon.outer = coords;
					} else {
						geom.polygon.inner = coords;
					}

					pass++;
				}
				
				// utils::refreshPolygon(geom.polygon);

			} else {
				type = "Error";
			}

			// Update Vertices, Length, and Area
			refresh();
		}

		~Shape() {
			
		}
	};
}

#endif