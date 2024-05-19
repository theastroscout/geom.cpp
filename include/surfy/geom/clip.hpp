/*

Clippers

*/

namespace surfy::geom {

	namespace clippers {

		/*
	
		Ray-Casting
		Check if point is inside our mask

		*/

		bool insidePolygon(const Point& p, const Coords& poly) {
			int n = poly.size();
			bool result = false;
			for (int i = 0, j = n - 1; i < n; j = i++) {
				if (((poly[i].y > p.y) != (poly[j].y > p.y)) &&
					(p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)) {
					result = !result;
				}
			}
			return result;
		}

		// Find the intersection point of two line segments
		bool segmentIntersection(const Point& p1, const Point& p2, const Point& p3, const Point& p4, Point& intersection) {
			double x1 = p1.x, y1 = p1.y;
			double x2 = p2.x, y2 = p2.y;
			double x3 = p3.x, y3 = p3.y;
			double x4 = p4.x, y4 = p4.y;

			double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
			if (denom == 0) {
				return false; // Lines are parallel or coincident
			}

			double ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / denom;
			double ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / denom;

			if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
				intersection.x = x1 + ua * (x2 - x1);
				intersection.y = y1 + ua * (y2 - y1);
				return true; // Intersection exists within line segments
			}

			return false; // Intersection is outside the line segments
		}

		/*

		Clip a line by mask (polygon)
		Cyrus-Beck algorithm

		*/

		Coords line(const Coords& line, const Coords& mask) {
			Coords clipped;
			size_t maskSize = mask.size();
			for (size_t i = 0; i < line.size(); ++i) {
				Point p = line[i];
				bool isInside = insidePolygon(p, mask);

				if (isInside) {
					clipped.push_back(p);
					if (i > 0 && !insidePolygon(line[i - 1], mask)) {
						Point intersection;
						for (size_t m = 0; m < maskSize - 1; ++m) {
							if (segmentIntersection(line[i - 1], p, mask[m], mask[m + 1], intersection)) {
								clipped.insert(clipped.end() - 1, intersection);
							}
						}
					}
				} else if (i > 0 && insidePolygon(line[i - 1], mask)) {
					Point intersection;
					for (size_t m = 0; m < maskSize - 1; ++m) {
						if (segmentIntersection(line[i - 1], p, mask[m], mask[m + 1], intersection)) {
							clipped.push_back(intersection);
						}
					}
				}
			}
			return clipped;
		}

		// Find intersection of two segments (lines);
		Point intersect(const Point& p1, const Point& p2, const Point& q1, const Point& q2) {
			double a1 = p2.y - p1.y;
			double b1 = p1.x - p2.x;
			double c1 = a1 * p1.x + b1 * p1.y;

			double a2 = q2.y - q1.y;
			double b2 = q1.x - q2.x;
			double c2 = a2 * q1.x + b2 * q1.y;

			double determinant = a1 * b2 - a2 * b1;

			if (determinant == 0) {
				return {0, 0}; // Lines are parallel
			} else {
				double x = (b2 * c1 - b1 * c2) / determinant;
				double y = (a1 * c2 - a2 * c1) / determinant;
				return {x, y};
			}
		}

		/*
	
		Cross Product
		Returns true if the point is on the left side of the line segment, false otherwise.

		*/

		bool crossProduct(const Point& p, const Point& p1, const Point& p2) {
			return (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) >= 0;
		}

		// Sutherland-Hodgman polygon clipping algorithm
		Coords sutherlandHodgman(const Coords& subjectPolygon, const Coords& clipPolygon) {
			Coords outputList = subjectPolygon;
			int clipPolygonSize = clipPolygon.size();

			for (int i = 0; i < clipPolygonSize; i++) {
				Coords inputList = outputList;
				outputList.clear();

				Point A = clipPolygon[i];
				Point B = clipPolygon[(i + 1) % clipPolygonSize];

				int inputListSize = inputList.size();
				for (int j = 0; j < inputListSize; j++) {
					Point P = inputList[j];
					Point Q = inputList[(j + 1) % inputListSize];

					if (crossProduct(Q, A, B)) {
						if (!crossProduct(P, A, B)) {
							outputList.push_back(intersect(P, Q, A, B));
						}
						outputList.push_back(Q);
					} else if (crossProduct(P, A, B)) {
						outputList.push_back(intersect(P, Q, A, B));
					}
				}
			}

			return outputList;
		}

	}

	/*

	Clip

	*/

	void Shape::clip(const Coords& mask) {
		// Shape result;

		// std::vector<Point> mask = maskSrc.geom.polygon.outer.coords;

		if (type == "Point") {

			if (!utils::inside(geom.point, mask)) {
				type = "Dummy";
				new (&geom.point) types::Point();
			}

		} else if (type == "Line") {

			geom.line.coords = clippers::line(geom.line.coords, mask);

		} else if (type == "MultiLine") {

			for (int i = 0; i < size; ++i) {
				geom.multiLine.items[i].coords = clippers::line(geom.multiLine.items[i].coords, mask);
			}

		} else if (type == "Polygon") {

			if (!geom.polygon.outer.empty) {
				Coords coords = clippers::sutherlandHodgman(geom.polygon.outer.coords, mask);
				geom.polygon.outer.coords = coords;
			}

			if (!geom.polygon.inner.empty) {
				Coords coords = clippers::sutherlandHodgman(geom.polygon.inner.coords, mask);
				geom.polygon.inner.coords = coords;
			}

		} else if (type == "MultiPolygon") {

			for (int i = 0; i < size; ++i) {
				types::Polygon& poly = geom.multiPolygon.items[i];
				
				if (!poly.outer.empty) {
					Coords coords = clippers::sutherlandHodgman(poly.outer.coords, mask);
					poly.outer.coords = coords;
				}
				
				if (!poly.inner.empty) {
					Coords coords = clippers::sutherlandHodgman(poly.inner.coords, mask);
					poly.inner.coords = coords;
				}
			}

		}

		refresh();
	}
}