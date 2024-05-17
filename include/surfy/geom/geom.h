#ifndef SURFY_GEOM_HPP
#define SURFY_GEOM_HPP
#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace surfy::geom {

	using BBox = std::array<double, 4>;

	struct Point {
		double x, y;
	};

	using Coords = std::vector<Point>;

	namespace types {
		struct Geometry {
			unsigned int vertices = 0;
			double length = .0;
			double area = .0;
			bool empty = true;
		};

		struct Point : public Geometry, public surfy::geom::Point {
			// double x, y;
			int typeID = 1;
			std::string wkt();
		};

		struct Line : public Geometry {
			int typeID = 2;
			bool closed = false;
			surfy::geom::Coords coords;
			std::string wkt();
		};

		struct MultiLine : public Geometry {
			int typeID = 3;
			unsigned int size = 0;
			std::vector<Line> items;
			std::string wkt();
		};

		struct Polygon : public Geometry {
			int typeID = 4;
			Line inner;
			Line outer;
			std::string wkt();

			Polygon() : inner(), outer() {}
			Polygon(const Polygon& other) : inner(other.inner), outer(other.outer) {}
		};

		struct MultiPolygon : public Geometry {
			int typeID = 5;
			unsigned int size = 0;
			std::vector<Polygon> items;
			std::string wkt();
		};
	}

	namespace utils {
		bool isClosed(const Coords& coords);
		double distance(const Point& p1, const Point& p2);
		BBox bbox(const Coords& coords);
		Coords parseCoordsString(const std::string& str);
		double length(const std::vector<Point>& coords, size_t size);
		float area(const std::vector<Point>& coords, size_t size);
		void prune(Coords& coords, const double& epsilon);
	};

	namespace parser {
		types::Polygon polygon(const std::string& body);
	};

	namespace print {
		void point(std::ostream& os, const Point& point, const bool& compressed = false);
		void line(std::ostream& os, const Coords& coords, const bool& compressed = false);
		void polygon(std::ostream& os, const types::Polygon& poly, const bool& compressed = false);
	};

	std::string types::Point::wkt() {
		std::stringstream os;
		os << "POINT (";
		print::point(os, *this);
		os << ")";
		return os.str();
	}

	std::string types::Line::wkt() {
		std::stringstream os;
		os << "LINESTRING ";
		print::line(os, coords);
		return os.str();
	}

	std::string types::MultiLine::wkt() {
		std::stringstream os;
		os << "MULTILINESTRING (";

		for (int i = 0; i < size; ++i) {
			Line& line = items[i];
			print::line(os, line.coords);
			if (i != size - 1) {
				os << ",";
			}
		}

		os << ")";
		return os.str();
	}

	std::string types::Polygon::wkt() {
		std::stringstream os;
		os << "POLYGON ";
		print::polygon(os, *this);
		return os.str();
	}

	std::string types::MultiPolygon::wkt() {
		std::stringstream os;
		
		os << "MULTIPOLYGON (";
		for (int i = 0; i < size; ++i) {
			print::polygon(os, items[i]);
			if (i != size - 1) {
				os << ",";
			}
		}
		os << ")";

		return os.str();
	}

	/*

	Shape

	*/

	class Shape {
	public:
		int typeID = 0;
		std::string type = "Dummy";
		std::string source;
		unsigned int vertices = 0;
		unsigned int size = 0;
		double length = .0;
		double area = .0;
		bool empty = true;
		BBox bbox = {.0, .0, .0, .0};


		union Geometry {
			types::Point point;
			types::Line line;
			types::MultiLine multiLine;
			types::Polygon polygon;
			types::MultiPolygon multiPolygon;

			Geometry() {}
			~Geometry() {}

		} geom;

		/*

		WKT
		Stringify geometry to "POINT (1 2)", "LINESTRING (0 0, 2 2)", etc.

		*/

		std::string wkt() {
			std::stringstream os;

			if (type == "Point") {

				os << "POINT (";
				print::point(os, geom.point);
				os << ")";

			} else if (type == "Line") {

				os << "LINESTRING ";
				print::line(os, geom.line.coords);

			} else if (type == "MultiLine") {

				os << "MULTILINESTRING (";

				for (int i = 0; i < size; ++i) {
					types::Line& line = geom.multiLine.items[i];
					print::line(os, line.coords);
					if (i != size - 1) {
						os << ",";
					}
				}
				os << ")";

			} else if (type == "Polygon") {

				os << "POLYGON ";
				print::polygon(os, geom.polygon);

			} else if (type == "MultiPolygon") {

				os << "MULTIPOLYGON (";
				for (int i = 0; i < size; ++i) {
					print::polygon(os, geom.multiPolygon.items[i]);
					if (i != size - 1) {
						os << ",";
					}
				}
				os << ")";

			}

			return os.str();
		}

		std::string compressed() {
			std::stringstream os;
			if (type == "Polygon") {
				print::polygon(os, geom.polygon, true);
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
			empty = true;

			if (type == "Point") {
				vertices = 1;
				empty = false;
			} else if (type == "Line") {
				size_t lineSize = geom.line.coords.size();
				geom.line.vertices = lineSize;
				geom.line.length = utils::length(geom.line.coords, lineSize);

				// Update Shape
				vertices = geom.line.vertices;
				length = geom.line.length;
				if (vertices != 0) {
					geom.line.closed = utils::isClosed(geom.line.coords);
					geom.line.empty = false;
					empty = false;
				}

				bbox = utils::bbox(geom.line.coords);

			} else if (type == "MultiLine") {
				
				geom.multiLine.size = geom.multiLine.items.size();

				for (int i = 0; i < geom.multiLine.size; ++i) {
					types::Line& line = geom.multiLine.items[i];
					size_t lineSize = line.coords.size();
					line.vertices = lineSize;

					if (line.vertices != 0) {
						line.empty = false;
						line.length = utils::length(line.coords, lineSize);
						line.closed = utils::isClosed(line.coords);

						geom.multiLine.vertices += line.vertices;
						geom.multiLine.length += line.length;
					}
				}

				// Update Shape
				size = geom.multiLine.size;
				vertices = geom.multiLine.vertices;
				length = geom.multiLine.length;

				if (vertices != 0) {
					empty = false;
				}

			} else if (type == "Polygon") {

				geom.polygon.vertices = 0;
				geom.polygon.length = .0;
				geom.polygon.area = .0;

				if (!geom.polygon.outer.coords.empty()) {
					size_t outerSize = geom.polygon.outer.coords.size();
					geom.polygon.outer.vertices = outerSize;

					if (geom.polygon.outer.vertices != 0){
						geom.polygon.outer.empty = false;

						geom.polygon.outer.closed = utils::isClosed(geom.polygon.outer.coords);

						geom.polygon.outer.length = utils::length(geom.polygon.outer.coords, outerSize);
						geom.polygon.outer.area = utils::area(geom.polygon.outer.coords, outerSize);

						// Update Polygon
						geom.polygon.vertices += geom.polygon.outer.vertices;
						geom.polygon.length += geom.polygon.outer.length;
						geom.polygon.area += geom.polygon.outer.area;
					}

					BBox outerBBox = utils::bbox(geom.polygon.outer.coords);
					bbox[0] += outerBBox[0];
					bbox[1] += outerBBox[1];
					bbox[2] += outerBBox[2];
					bbox[3] += outerBBox[3];
				}

				if (!geom.polygon.inner.coords.empty()) {
					size_t innerSize = geom.polygon.inner.coords.size();
					geom.polygon.inner.vertices = innerSize;

					if (geom.polygon.inner.vertices != 0) {
						geom.polygon.inner.empty = false;

						geom.polygon.inner.closed = utils::isClosed(geom.polygon.inner.coords);

						geom.polygon.inner.length = utils::length(geom.polygon.inner.coords, innerSize);
						geom.polygon.inner.area = utils::area(geom.polygon.inner.coords, innerSize);

						// Update Polygon
						geom.polygon.vertices += geom.polygon.inner.vertices;
						geom.polygon.length += geom.polygon.inner.length;
						geom.polygon.area += geom.polygon.inner.area;
					}

					BBox innerBBox = utils::bbox(geom.polygon.inner.coords);
					bbox[0] += innerBBox[0];
					bbox[1] += innerBBox[1];
					bbox[2] += innerBBox[2];
					bbox[3] += innerBBox[3];
				}

				// Update Shape
				vertices = geom.polygon.vertices;
				length = geom.polygon.length;
				area = geom.polygon.area;

				if (vertices != 0) {
					empty = false;
					geom.polygon.empty = false;
				}
			} else if (type == "MultiPolygon") {
				
				geom.multiPolygon.size = geom.multiPolygon.items.size();

				geom.multiPolygon.vertices = 0;
				geom.multiPolygon.length = .0;
				geom.multiPolygon.area = .0;

				for (int i = 0; i < geom.multiPolygon.size; ++i) {
					types::Polygon& polygon = geom.multiPolygon.items[i];
					
					polygon.vertices = 0;
					polygon.length = .0;
					polygon.area = .0;

					if (!polygon.outer.coords.empty()) {
						size_t outerSize = polygon.outer.coords.size();
						polygon.outer.vertices = outerSize;

						if (polygon.outer.vertices != 0){
							polygon.outer.empty = false;

							polygon.outer.closed = utils::isClosed(polygon.outer.coords);

							polygon.outer.length = utils::length(polygon.outer.coords, outerSize);
							polygon.outer.area = utils::area(polygon.outer.coords, outerSize);

							// Update Polygon
							polygon.vertices += polygon.outer.vertices;
							polygon.length += polygon.outer.length;
							polygon.area += polygon.outer.area;
						}
					}

					if (!polygon.inner.coords.empty()) {
						size_t outerSize = polygon.inner.coords.size();
						polygon.inner.vertices = outerSize;

						if (polygon.inner.vertices != 0){
							polygon.inner.empty = false;

							polygon.inner.closed = utils::isClosed(polygon.inner.coords);

							polygon.inner.length = utils::length(polygon.inner.coords, outerSize);
							polygon.inner.area = utils::area(polygon.inner.coords, outerSize);

							// Update Polygon
							polygon.vertices += polygon.inner.vertices;
							polygon.length += polygon.inner.length;
							polygon.area += polygon.inner.area;
						}
					}

					if (polygon.vertices != 0) {
						geom.multiPolygon.vertices += polygon.vertices;
						geom.multiPolygon.length += polygon.length;
						geom.multiPolygon.area += polygon.area;
					}
					
				}

				// Update Shape
				size = geom.multiPolygon.size;
				vertices = geom.multiPolygon.vertices;
				length = geom.multiPolygon.length;
				area = geom.multiPolygon.area;

				if (vertices != 0) {
					empty = false;
				}

			}
		}		

		Shape(const std::string& src = "", const bool optimized = false) {
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
				type = "Dummy";
				return;
			}

			if (src.find("POINT") != std::string::npos) {
				
				typeID = 1;
				type = "Point";
				new (&geom.point) types::Point();

				Coords coords = utils::parseCoordsString(body);
				geom.point.x = coords[0].x;
				geom.point.y = coords[0].y;

			} else if (src.find("MULTILINESTRING") != std::string::npos) {

				typeID = 2;
				type = "MultiLine";
				new (&geom.multiLine) types::MultiLine();

				size_t pos = 0;
				while (pos < body.size()) {
					size_t start = body.find("(", pos);
					size_t end = body.find(")", start);
					if (start == std::string::npos || end == std::string::npos) {
						break; // No more polygons found
					}

					std::string lineStr = body.substr(start + 1, end - start - 1);

					types::Line line;
					line.coords = utils::parseCoordsString(lineStr);

					geom.multiLine.items.push_back(line);

					pos = end + 1;
				}

			} else if (src.find("LINESTRING") != std::string::npos) {

				typeID = 3;
				type = "Line";
				new (&geom.line) types::Line();

				geom.line.coords = utils::parseCoordsString(body);

			}  else if (src.find("MULTIPOLYGON") != std::string::npos) {
				
				std::vector<types::Polygon> items;

				size_t pos = 0;
				while (pos < body.size()) {
					size_t start = body.find("((", pos);
					size_t end = body.find("))", start);
					if (start == std::string::npos || end == std::string::npos) {
						break; // No more polygons found
					}

					std::string polyStr = body.substr(start + 1, end - start);
					pos = end + 1;

					types::Polygon poly = parser::polygon(polyStr);
					items.push_back(poly);
				}

				/*

				Optimized

				*/

				if (optimized) {
					if (items.size() == 1) {

						typeID = 4;
						type = "Polygon";
						new (&geom.polygon) types::Polygon();
						geom.polygon = items[0];
						utils::prune(geom.polygon.outer.coords, 1e-10);
						utils::prune(geom.polygon.inner.coords, 1e-10);

					} else {

						typeID = 5;
						type = "MultiPolygon";
						new (&geom.multiPolygon) types::MultiPolygon();

						for (types::Polygon& item : items) {
							utils::prune(item.outer.coords, 1e-10);
							utils::prune(item.inner.coords, 1e-10);
						}

						geom.multiPolygon.items = items;

					}
				} else {

					typeID = 5;
					type = "MultiPolygon";
					new (&geom.multiPolygon) types::MultiPolygon();
					geom.multiPolygon.items = items;
				}


			} else if (src.find("POLYGON") != std::string::npos) {

				typeID = 4;
				type = "Polygon";
				types::Polygon poly = parser::polygon(body);
				new (&geom.polygon) types::Polygon(poly);

			} else {
				type = "Dummy";
				return;
			}

			// Update size, Length, and Area
			refresh();
		}

		void clip(const Coords& mask);

		void simplify(const double& intolerance);

		void optimize() {
			std::cout << "OPTI" << std::endl;
		};

		/*
		Shape (const Polygon& poly) {
			new (&geom.polygon) Polygon(poly);
		}*/

		Shape(const Shape& other) {
			type = other.type;
			source = other.source;
			vertices = other.vertices;
			size = other.size;
			length = other.length;
			area = other.area;
			empty = other.empty;
			// std::memcpy(&geom, &other.geom, sizeof(Geometry));
			if (type == "Point") {
				new (&geom.point) types::Point(other.geom.point);
			} else if (type == "Line") {
				new (&geom.line) types::Line(other.geom.line);
			} else if (type == "MultiLine") {
				new (&geom.multiLine) types::MultiLine(other.geom.multiLine);
			} else if (type == "Polygon") {
				new (&geom.polygon) types::Polygon(other.geom.polygon);
			} else if (type == "MultiPolygon") {
				new (&geom.multiPolygon) types::MultiPolygon(other.geom.multiPolygon);
			} else {
				new (&geom.point) types::Point();
			}
			
		}

		~Shape() {
			
		}
	};
}

#include "print.h"
#include "utils.h"
#include "parser.h"
#include "clip.h"
#include "simplify.h"

#endif