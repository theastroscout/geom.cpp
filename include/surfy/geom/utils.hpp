#ifndef GEOM_UTILS_HPP
#define GEOM_UTILS_HPP
// #pragma once

namespace surfy::geom::utils {

	/*

	Is Closed

	*/

	bool isClosed(const Coords& coords) {
		Point front = coords.front();
		Point back = coords.back();

		return (front.x == back.x && front.y == back.y);
	}

	BBox bbox(const Coords& coords) {
		
		if (coords.empty()) {
			return {.0, .0, .0, .0};
		}
		
		double minX = coords[0].x;
		double minY = coords[0].y;
		double maxX = coords[0].x;
		double maxY = coords[0].y;
		
		for (const auto& point : coords) {
			minX = std::min(minX, point.x);
			minY = std::min(minY, point.y);
			maxX = std::max(maxX, point.x);
			maxY = std::max(maxY, point.y);
		}
		
		return { minX, minY, maxX, maxY };
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

	Coords parseCoordsString(const std::string& str) {

		Coords coords;
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

	double length(const Coords& coords, size_t size = 0) {
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

	float area(const Coords& coords, size_t size = 0) {
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
	
	Ray-Casting
	Point inside Polygon

	*/

	bool inside(const Point& point, const Coords& coords) {
		for (const Point& vertex : coords) {
			if (vertex.x == point.x && vertex.y == point.y) {
				return true;
			}
		}

		bool inside = false;

		for (int i = 0, j = coords.size() - 1; i < coords.size(); j = i++) {
			bool isAboveI = (coords[i].y > point.y);
			bool isAboveJ = (coords[j].y > point.y);
			bool yIntersect = (isAboveI != isAboveJ);
			
			double slope = (coords[j].x - coords[i].x) / (coords[j].y - coords[i].y);
			double intersectX = slope * (point.y - coords[i].y) + coords[i].x;
			bool xIntersect = (point.x <= intersectX); // Include equality

			if (yIntersect && xIntersect) {
				inside = !inside;
			}
		}
		return inside;
	}

	// Function to check if three points are collinear (lie on the same line)
	bool collinear(const Point& p1, const Point& p2, const Point& p3, const double& epsilon) {
		// return (p2.y - p1.y) * (p3.x - p2.x) == (p3.y - p2.y) * (p2.x - p1.x);

		double slope1 = (p2.y - p1.y) * (p3.x - p2.x);
		double slope2 = (p3.y - p2.y) * (p2.x - p1.x);
		// double epsilon = ; // We can adjust epsilon based on tolerance requirements
		// std::cout << std::abs(slope1 - slope2) << std::endl;
		return std::abs(slope1 - slope2) < epsilon;
	}

	/*

	Prune
	Erase vertices laying on the same line

	*/

	void prune(Coords& coords, const double& epsilon = 1e-10) {
		if (coords.size() < 3) {
			// Not enough vertices
			return;
		}

		// Iterate through each vertex of the coords excluding the first and last
		auto it = coords.begin() + 1;
		while (it != coords.end() - 1) {
			// Get the current vertex and its adjacent vertices
			const Point& p1 = *(it - 1);
			const Point& p2 = *it;
			const Point& p3 = *(it + 1);

			// Check if the 3 vertices are collinear
			if (collinear(p1, p2, p3, epsilon)) {
				// If collinear remove the middle vertex
				it = coords.erase(it);
			} else {
				// Move to the next vertex
				++it;
			}
		}
		
	}

	/*

	Find the point with the maximum distance from the line segment

	*/

	double maxDistance(Point p1, Point p2, Point p) {
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		double dist;

		if (dx == 0 && dy == 0) {
			// p1 and p2 are the same point, so we just calculate distance between two points
			dist = distance(p1, p);
		} else {
			double t = ((p.x - p1.x) * dx + (p.y - p1.y) * dy) / (dx * dx + dy * dy);
			if (t < 0) {
				// p1 is the closest point
				dist = distance(p, p1);
			} else if (t > 1) {
				// p2 is the closest point
				dist = distance(p, p2);
			} else {
				// Closest point is on the line segment
				Point closest;
				closest.x = p1.x + t * dx;
				closest.y = p1.y + t * dy;
				dist = distance(p, closest);
			}
		}

		return dist;
	}

	/*

	Douglas-Peucker simplification algorithm

	*/

	void simplify(const Coords& points, const double& epsilon, Coords& simplified) {
		// Find the point with the maximum distance
		double maxDist = 0;
		int index = 0;
		int end = points.size();

		for (int i = 1; i < end - 1; ++i) {
			double dist = maxDistance(points[0], points[end - 1], points[i]);
			if (dist > maxDist) {
				maxDist = dist;
				index = i;
			}
		}

		// If max distance is greater than epsilon, recursively simplify
		if (maxDist > epsilon) {
			Coords firstHalf(points.begin(), points.begin() + index + 1);
			Coords secondHalf(points.begin() + index, points.end());
			Coords simplifiedFirstHalf;
			Coords simplifiedSecondHalf;
			simplify(firstHalf, epsilon, simplifiedFirstHalf);
			simplify(secondHalf, epsilon, simplifiedSecondHalf);

			// Avoid duplicating the endpoint of the first half and the starting point of the second half
			simplified.insert(simplified.end(), simplifiedFirstHalf.begin(), simplifiedFirstHalf.end() - 1);
			simplified.insert(simplified.end(), simplifiedSecondHalf.begin(), simplifiedSecondHalf.end());
		} else {
			// Keep the endpoints
			simplified.push_back(points[0]);
			simplified.push_back(points[end - 1]);
		}
	}

}

#endif