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
		void douglasPeucker(const Coords& points, const double& epsilon, Coords& simplified) {
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

	void Shape::simplify(const double& intolerance = 1.) {

		if (type == "Point") {

		} else if (type == "Line") {

			if (geom.line.coords.size() > 2) {
				Coords coords;
				simplify::douglasPeucker(geom.line.coords, intolerance, coords);
				geom.line.coords = coords;
			}

		} else if (type == "MultiLine") {

			for (int i = 0; i < size; ++i) {
				types::Line& line = geom.multiLine.items[i];
				if (!line.empty) {
					Coords coords;
					simplify::douglasPeucker(line.coords, intolerance, coords);
					line.coords = coords;
				}
			}

		} else if (type == "Polygon") {

			if (!geom.polygon.outer.empty) {
				Coords coords;
				simplify::douglasPeucker(geom.polygon.outer.coords, intolerance, coords);
				geom.polygon.outer.coords = coords;
			}

			if (!geom.polygon.inner.empty) {
				Coords coords;
				simplify::douglasPeucker(geom.polygon.inner.coords, intolerance,coords);
				geom.polygon.inner.coords = coords;
			}

		} else if (type == "MultiPolygon") {

			for (int i = 0; i < size; ++i) {
				types::Polygon& poly = geom.multiPolygon.items[i];
				
				if (!poly.outer.empty) {
					Coords coords;
					simplify::douglasPeucker(poly.outer.coords, intolerance, coords);
					poly.outer.coords = coords;
				}

				if (!poly.inner.empty) {
					Coords coords;
					simplify::douglasPeucker(poly.inner.coords, intolerance, coords);
					poly.inner.coords = coords;
				}
			}			

		}

		refresh();

		// return result;
	}
}