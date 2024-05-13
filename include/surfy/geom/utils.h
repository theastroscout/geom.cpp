#ifndef GEOM_UTILS_HPP
#define GEOM_UTILS_HPP
// #pragma once
// #include "main.h"

namespace surfy::geom::utils {
	
	/*

	Calc Distance Between Two Points

	*/

	double distance(const Point& p1, const Point& p2) {
		return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
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

	Print Coordinates

	*/

	void printPoint(std::ostream& os, const Point& point) {
		os << point.x << " " << point.y;
	}

	void printCoords(std::ostream& os, const std::vector<Point>& coords) {
		os << "(";
		size_t length = coords.size();
		for (size_t i = 0; i < length; ++i) {
			printPoint(os, coords[i]);
			if (i != length - 1) {
				os << ", ";
			}
		}
		os << ")";
	}

	/*

	Length

	*/

	double length(const std::vector<Point>& coords, size_t size = 0) {
		double length = 0;
		if (size == 0) {
			size = coords.size();
		}
		for (size_t i = 0; i < size; ++i) {
			length += distance(coords[i], coords[i+1]);
		}
		return length;
	}

	/*
	
	Calculate Area, Gauss's area

	*/

	float area(const std::vector<Point>& coords, size_t size = 0) {
		double area = .0;
		if (size == 0) {
			size = coords.size();
		}
		for (int i = 0; i < size; ++i) {
			int j = (i + 1) % size;
			// area += coords[i].x * coords[j].y - coords[j].x * coords[i].y;
			area += coords[i].x * coords[j].y - coords[j].x * coords[i].y;
		}
		return area / 2;
	}
}

#endif