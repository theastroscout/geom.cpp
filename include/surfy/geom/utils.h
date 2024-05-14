#ifndef GEOM_UTILS_HPP
#define GEOM_UTILS_HPP
// #pragma once
// #include "main.h"

namespace surfy::geom::utils {

	/*

	Is Closed

	*/

	bool isClosed(const std::vector<Point>& coords) {
		Point front = coords.front();
		Point back = coords.back();

		return (front.x == back.x && front.y == back.y);
	}
	
	/*

	Calc Distance Between Two Points

	*/

	double distance(const Point& p1, const Point& p2) {
		return std::sqrt(std::fabs((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));
	}

	/*

	Parse Coordinates String

	*/

	std::vector<Point> parseCoordsString(const std::string& str) {

		std::vector<Point> coords;
		double x, y;
		char comma;

		// Use stringstream to parse coordinates
		std::stringstream ss(str);
		while (ss >> x >> y) {
			Point p;
			p.x = x;
			p.y = y;
			coords.push_back(p);
			// Check for comma, if not found, break the loop
			if (!(ss >> comma)) {
				break;
			}
		}

		return coords;
	}

	/*

	Length

	*/

	double length(const std::vector<Point>& coords, size_t size = 0) {
		double length = 0;
		if (size == 0) {
			size = coords.size();
		}

		if (size > 1) {
			--size;
			for (size_t i = 0; i < size; ++i) {
				length += distance(coords[i], coords[i+1]);
			}
		}
		
		return length;
	}

	/*
	
	Calculate Area, Gauss's area

	*/

	float area(const std::vector<Point>& coords, size_t size = 0) {
		float area = .0;
		if (size == 0) {
			size = coords.size();
		}
		for (int i = 0; i < size; ++i) {
			int j = (i + 1) % size;
			area += std::fabs(coords[i].x * coords[j].y - coords[j].x * coords[i].y);
		}
		return area / 2;
	}

	/*

	Point inside Polygon

	*/

	bool inside(const Point& point, const std::vector<Point>& polygon) {
		for (const Point& vertex : polygon) {
			if (vertex.x == point.x && vertex.y == point.y) {
				return true;
			}
		}

		bool inside = false;

		for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
			bool isAboveI = (polygon[i].y > point.y);
			bool isAboveJ = (polygon[j].y > point.y);
			bool yIntersect = (isAboveI != isAboveJ);
			
			double slope = (polygon[j].x - polygon[i].x) / (polygon[j].y - polygon[i].y);
			double intersectX = slope * (point.y - polygon[i].y) + polygon[i].x;
			bool xIntersect = (point.x <= intersectX); // Include equality

			if (yIntersect && xIntersect) {
				inside = !inside;
			}
		}
		return inside;
	}

}

#endif