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
		double x, y;
	};

	struct Line : public Geometry {
		bool closed;
		std::vector<Point> coords = {};
	};

	struct MultiLine : public Geometry {
		unsigned int size = 0;
		std::vector<Line> items = {};
	};

	struct Polygon : public Geometry {
		Line inner;
		Line outer;
	};

	struct MultiPolygon : public Geometry {
		unsigned int size = 0;
		std::vector<Polygon> items = {};
	};

	namespace utils {
		double distance(const Point& p1, const Point& p2);
		std::vector<Point> parseCoordsString(const std::string& str);
		double length(const std::vector<Point>& coords, size_t size);
		float area(const std::vector<Point>& coords, size_t size);
	};

	namespace print {
		void point(std::ostream& os, const Point& point);
		void line(std::ostream& os, const std::vector<Point>& coords);
		void polygon(std::ostream& os, const Polygon& poly);
	};

	/*

	Shape

	*/

	class Shape {
	public:
		std::string type = "Dummy";
		std::string source;
		unsigned int vertices = 0;
		unsigned int size = 0;
		double length = 0;
		double area = 0;

		union Geometry {
			Point point;
			Line line;
			MultiLine multiLine;
			Polygon polygon;
			MultiPolygon multiPolygon;

			Geometry() {}
			~Geometry() {}

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

				os << "LINESTRING";
				print::line(os, geom.line.coords);

			} else if (type == "MultiLine") {

				os << "MULTILINESTRING (";

				for (int i = 0; i < geom.multiLine.size; ++i) {
					Line& line = geom.multiLine.items[i];
					print::line(os, line.coords);
				}
				os << ")";

			} else if (type == "Polygon") {
				os << "POLYGON ";
				print::polygon(os, geom.polygon);
			}

			return os.str();
		}

		/*

		Update Vertices, Length, and Area

		*/

		void refresh() {
			vertices = 0;
			length = 0;
			area = .0;

			if (type == "Point") {
				vertices = 1;
			} else if (type == "Line") {
				size_t lineSize = geom.line.coords.size();
				geom.line.vertices = lineSize;
				geom.line.length = utils::length(geom.line.coords, lineSize);

				// Update Shape
				vertices = geom.line.vertices;
				length = geom.line.length;

			} else if (type == "MultiLine") {
				
				geom.multiLine.size = geom.multiLine.items.size();

				for (int i = 0; i < geom.multiLine.size; ++i) {
					Line& line = geom.multiLine.items[i];
					size_t lineSize = line.coords.size();
					line.vertices = lineSize;
					line.length = utils::length(line.coords, lineSize);

					geom.multiLine.vertices += line.vertices;
					geom.multiLine.length += line.length;
				}

				// Update Shape
				size = geom.multiLine.size;
				vertices = geom.multiLine.vertices;
				length = geom.multiLine.length;

			} else if (type == "Polygon") {

				geom.polygon.vertices = 0;
				geom.polygon.length = .0;
				geom.polygon.area = .0;

				if (!geom.polygon.outer.coords.empty()) {
					size_t outerSize = geom.polygon.outer.coords.size();
					geom.polygon.outer.vertices = outerSize;
					geom.polygon.outer.length = utils::length(geom.polygon.outer.coords, outerSize);
					geom.polygon.outer.area = utils::area(geom.polygon.outer.coords, outerSize);


					// Update Polygon
					geom.polygon.vertices += geom.polygon.outer.vertices;
					geom.polygon.length += geom.polygon.outer.length;
					geom.polygon.area += geom.polygon.outer.area;
				}

				if (!geom.polygon.inner.coords.empty()) {
					size_t innerSize = geom.polygon.inner.coords.size();
					geom.polygon.inner.vertices = innerSize;
					geom.polygon.inner.length = utils::length(geom.polygon.inner.coords, innerSize);
					geom.polygon.inner.area = utils::area(geom.polygon.inner.coords, innerSize);


					// Update Polygon
					geom.polygon.vertices += geom.polygon.inner.vertices;
					geom.polygon.length += geom.polygon.inner.length;
					geom.polygon.area += geom.polygon.inner.area;
				}

				// Update Shape
				vertices = geom.polygon.vertices;
				length = geom.polygon.length;
				area = geom.polygon.area;
			}
		}		

		Shape (const std::string& src = "") {
			source = src; // Store source just in case

			if (src.empty()) {
				// Dummy Geometry
				return;
			}

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

			} else if (src.find("MULTILINESTRING") != std::string::npos) {

				type = "MultiLine";
				new (&geom.multiLine) MultiLine();

				size_t pos = 0;
				while (pos < body.size()) {
					size_t start = body.find("(", pos);
					size_t end = body.find(")", start);
					if (start == std::string::npos || end == std::string::npos) {
						break; // No more polygons found
					}

					std::string lineStr = body.substr(start + 1, end - start - 1);

					Line line;
					line.coords = utils::parseCoordsString(lineStr);

					Point front = line.coords.front();
					Point back = line.coords.back();

					if(front.x == back.x && front.y == back.y) {
						line.closed = true;
					}
					
					geom.multiLine.items.push_back(line);

					pos = end + 1;
				}

			} else if (src.find("LINESTRING") != std::string::npos) {

				type = "Line";
				new (&geom.line) Line();

				geom.line.coords = utils::parseCoordsString(body);

				Point front = geom.line.coords.front();
				Point back = geom.line.coords.back();

				if(front.x == back.x && front.y == back.y) {
					geom.line.closed = true;
				}

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

			}

			// Update size, Length, and Area
			refresh();
		}

		Shape simplify(const double& intolerance);

		/*
		Shape (const Polygon& poly) {
			new (&geom.polygon) Polygon(poly);
		}*/

		Shape(const Shape& other) {
			std::memcpy(&geom, &other.geom, sizeof(Geometry));
		}

		~Shape() {
			
		}
	};
}

#include "print.h"
#include "utils.h"
#include "clip.h"
#include "simplify.h"

#endif