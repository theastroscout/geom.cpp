#ifndef GEOM_UTILS_HPP
#define GEOM_UTILS_HPP
// #pragma once
// #include "main.h"

namespace surfy::geom::utils {
	
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


	
	Clipping Polygon by Mask
	Sutherland-Hodgman algorithm

	Mask Polygon should be sorted couterclockwise



	*/	

	std::vector<Point> clip(const std::vector<Point>& input, const std::vector<Point>& mask) {

		std::vector<Point> output = input;
		
		for (int i = 0; i < mask.size(); ++i) {
			std::vector<Point> input = output;
			output.clear();
			
			const Point& a = mask[i];
			const Point& b = mask[(i + 1) % mask.size()];

			for (int j = 0; j < input.size(); ++j) {
				const Point& p1 = input[j];
				const Point& p2 = input[(j + 1) % input.size()];

				float p1Side = (a.x - b.x) * (p1.y - a.y) - (a.y - b.y) * (p1.x - a.x);
				float p2Side = (a.x - b.x) * (p2.y - a.y) - (a.y - b.y) * (p2.x - a.x);

				if (p1Side >= 0)
					output.push_back(p1);
				if (p1Side * p2Side < 0) {
					Point intersect;
					intersect.x = (p1.x * p2Side - p2.x * p1Side) / (p2Side - p1Side);
					intersect.y = (p1.y * p2Side - p2.y * p1Side) / (p2Side - p1Side);
					output.push_back(intersect);
				}
			}
		}

		return output;
	}
}

#endif