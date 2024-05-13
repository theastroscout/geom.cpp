/*

Clippers

*/

namespace surfy::geom {

	namespace clippers {

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

		std::vector<Point> line(const std::vector<Point>& line, const std::vector<Point>& mask) {
			std::vector<Point> clipped;
			size_t maskSize = mask.size();
			for (int i = 0, l = line.size(); i < l; ++i) {
				Point p = line[i];
				bool isInside = inside(p, mask);

				if (isInside) {
					clipped.push_back(p);
				} else if (i > 0) {
					Point intersection;
					Point p1 = line[i];
					Point p2 = line[i - 1];

					for (int m=0, l = maskSize - 1; m < l; ++m) {
						Point p3 = mask[m];
						Point p4 = mask[m + 1];
						bool intersected = segmentIntersection(p1, p2, p3, p4, intersection);
						if (intersected) {
							clipped.push_back(intersection);
							break;
						}
					}

				}
			}
			
			return clipped;
		}

	}

	/*

	Clip

	*/

	Shape clip(const Shape& shape, const Shape& maskSrc) {
		Shape result;

		std::vector<Point> mask = maskSrc.geom.polygon.outer.coords;

		if (shape.type == "Line") {
			result.type = "Line";
			new (&result.geom.line) Line(); // Initialise Geometry::Line
			result.geom.line.coords = clippers::line(shape.geom.line.coords, mask);

		} else if (shape.type == "Polygon") {
			result.type = "Polygon";
			new (&result.geom.polygon) Polygon(); // Initialise Geometry::Polygon

			result.geom.polygon.outer.coords = utils::clip(shape.geom.polygon.outer.coords, mask);

			if(!shape.geom.polygon.inner.coords.empty()){
				result.geom.polygon.inner.coords = utils::clip(shape.geom.polygon.inner.coords, mask);
			}
		}

		result.refresh();
		
		return result;
	}
}