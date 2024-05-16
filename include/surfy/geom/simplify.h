/*


Simplify
Douglas-Peucker algorithm


*/

namespace surfy::geom {

	namespace simplify {

		// Find the point with the maximum distance from the line segment
		double maxDistance(Point p1, Point p2, Point p) {
			double dx = p2.x - p1.x;
			double dy = p2.y - p1.y;
			double dist;

			if (dx == 0 && dy == 0) {
				// p1 and p2 are the same point
				dist = utils::distance(p1, p);
			} else {
				double t = ((p.x - p1.x) * dx + (p.y - p1.y) * dy) / (dx * dx + dy * dy);
				if (t < 0) {
					// Closest point is p1
					dist = utils::distance(p, p1);
				} else if (t > 1) {
					// Closest point is p2
					dist = utils::distance(p, p2);
				} else {
					// Closest point is on the line segment
					Point closest;
					closest.x = p1.x + t * dx;
					closest.y = p1.y + t * dy;
					dist = utils::distance(p, closest);
				}
			}

			return dist;
		}

		// Douglas-Peucker simplification algorithm
		void douglasPeucker(const std::vector<Point>& points, const double& epsilon, std::vector<Point>& simplified) {
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
				std::vector<Point> firstHalf(points.begin(), points.begin() + index + 1);
				std::vector<Point> secondHalf(points.begin() + index, points.end());
				std::vector<Point> simplifiedFirstHalf;
				std::vector<Point> simplifiedSecondHalf;
				douglasPeucker(firstHalf, epsilon, simplifiedFirstHalf);
				douglasPeucker(secondHalf, epsilon, simplifiedSecondHalf);

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

	Shape Shape::simplify(const double& intolerance = 1.) {
		
		Shape result;

		if (type == "Point") {
			result.type = "Point";
			new (&result.geom.line) Line(); // Initialise Geometry::Point

			result.geom.point.x = geom.point.x;
			result.geom.point.y = geom.point.y;

		} else if (type == "Line") {

			result.type = "Line";
			new (&result.geom.line) Line(); // Initialise Geometry::Line

			if (result.geom.line.coords.size() > 2) {
				simplify::douglasPeucker(geom.line.coords, intolerance, result.geom.line.coords);
			} else {
				result.geom.line.coords = geom.line.coords;
			}

		} else if (type == "MultiLine") {

			result.type = "MultiLine";
			new (&result.geom.multiLine) MultiLine(); // Initialise Geometry::Line

			for (int i = 0; i < size; ++i) {
				const Line& srcLine = geom.multiLine.items[i];
				Line line;
				if (!srcLine.empty) {
					simplify::douglasPeucker(srcLine.coords, intolerance, line.coords);
				}
				result.geom.multiLine.items.push_back(line);
			}

		} else if (type == "Polygon") {

			result.type = "Polygon";
			new (&result.geom.polygon) Polygon(); // Initialise Geometry::Polygon

			if (!geom.polygon.outer.coords.empty()) {
				simplify::douglasPeucker(geom.polygon.outer.coords, intolerance, result.geom.polygon.outer.coords);
			}

			if (!geom.polygon.inner.coords.empty()) {
				simplify::douglasPeucker(geom.polygon.inner.coords, intolerance, result.geom.polygon.inner.coords);
			}

		} else if (type == "MultiPolygon") {

			result.type = "MultiPolygon";
			new (&result.geom.multiPolygon) MultiPolygon(); // Initialise Geometry::Polygon

			for (int i = 0; i < size; ++i) {
				const Polygon& srcPoly = geom.multiPolygon.items[i];
				Polygon poly;
				
				if (!srcPoly.outer.empty) {
					simplify::douglasPeucker(srcPoly.outer.coords, intolerance, poly.outer.coords);
				}

				if (!srcPoly.inner.empty) {
					simplify::douglasPeucker(srcPoly.inner.coords, intolerance, poly.inner.coords);
				}

				result.geom.multiPolygon.items.push_back(poly);
			}

			

		}

		result.refresh();

		return result;
	}
}