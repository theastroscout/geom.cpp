#ifndef GEOM_HPP
#define GEOM_HPP
#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace surfy::geom {

	struct Geometry {
		unsigned int size = 0;
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
		bool closed;
		std::vector<Point> coords;
	};

	struct MultiLine : public Geometry {
		std::vector<Line> items;
	};

	struct Polygon : public Geometry {
		Line inner;
		Line outer;
	};

	struct MultiPolygon : public Geometry {
		std::vector<Polygon> items;
	};

	struct Dummy {
		std::string type = "Dummy";
	};

	namespace utils {};
}

#include "utils.h"
#include "print.h"

namespace surfy::geom {

	class Shape {
	public:
		std::string type;
		std::string source;
		unsigned int size = 0;
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
				print::point(os, geom.point);
				os << ")";

			} else if (type == "Line") {

				os << "LINESTRING (";
				print::line(os, geom.line.coords);
				os << ")";

			} else if (type == "Polygon") {
				os << "POLYGON ";
				print::polygon(os, geom.polygon);
			}

			return os.str();
		}

		/*

		Update size, Length, and Area

		*/

		void refresh() {
			size = 0;
			length = 0;
			area = .0;

			if (type == "Point") {
				size = 1;
			} else if (type == "Line") {
				size_t lineSize = geom.line.coords.size();
				geom.line.size = lineSize;
				geom.line.length = utils::length(geom.line.coords, lineSize);

				// Update Shape
				size = geom.line.size;
				length = geom.line.length;

			} else if (type == "Polygon") {

				geom.polygon.size = 0;
				geom.polygon.length = .0;
				geom.polygon.area = .0;

				if(!geom.polygon.outer.coords.empty()){
					size_t outerSize = geom.polygon.outer.coords.size();
					geom.polygon.outer.size = outerSize;
					geom.polygon.outer.length = utils::length(geom.polygon.outer.coords, outerSize);
					geom.polygon.outer.area = utils::area(geom.polygon.outer.coords, outerSize);



					// Update Shape
					geom.polygon.size += geom.polygon.outer.size;
					geom.polygon.length += geom.polygon.outer.length;
					geom.polygon.area += geom.polygon.outer.area;
				}

				if(!geom.polygon.inner.coords.empty()){
					size_t innerSize = geom.polygon.inner.coords.size();
					geom.polygon.inner.size = innerSize;
					geom.polygon.inner.length = utils::length(geom.polygon.inner.coords, innerSize);
					geom.polygon.inner.area = utils::area(geom.polygon.inner.coords, innerSize);

					// Update Shape
					geom.polygon.size += geom.polygon.inner.size;
					geom.polygon.length += geom.polygon.inner.length;
					geom.polygon.area += geom.polygon.inner.area;
				}

				size = geom.polygon.size;
				length = geom.polygon.length;
				area = geom.polygon.area;
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
						geom.polygon.outer.coords = coords;
					} else {
						geom.polygon.inner.coords = coords;
					}

					pass++;
				}
				
				// utils::refreshPolygon(geom.polygon);

			} else {
				type = "Error";
			}

			// Update size, Length, and Area
			refresh();
		}

		~Shape() {
			
		}
	};
}

#endif