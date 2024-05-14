/*

Clippers

*/

namespace surfy::geom {

	namespace clippers {

		/*

		Finds Intersection of two Segments

		*/

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

		Clip Line by Mask

		*/

		std::vector<Point> line(const std::vector<Point>& line, const std::vector<Point>& mask) {
			std::vector<Point> clipped;
			size_t maskSize = mask.size();
			for (int i = 0, l = line.size(); i < l; ++i) {
				Point p = line[i];
				bool isInside = utils::inside(p, mask);

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

		/*


		
		Clipping Polygon by Mask
		Sutherland-Hodgman algorithm

		Mask Polygon should be sorted couterclockwise



		*/	

		std::vector<Point> polygon(const std::vector<Point>& input, const std::vector<Point>& mask) {

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

	/*

	Clip

	*/

	Shape clip(const Shape& shape, const Shape& maskSrc) {
		Shape result;

		std::vector<Point> mask = maskSrc.geom.polygon.outer.coords;

		if (shape.type == "Point") {

			result.type = "Point";
			new (&result.geom.point) Point(); // Initialise Geometry::Point

			if (utils::inside(shape.geom.point, mask)) {
				result.geom.point.x = shape.geom.point.x;
				result.geom.point.y = shape.geom.point.y;
			}

		} else if (shape.type == "Line") {

			result.type = "Line";
			new (&result.geom.line) Line(); // Initialise Geometry::Line
			result.geom.line.coords = clippers::line(shape.geom.line.coords, mask);

		} else if (shape.type == "MultiLine") {
			
			result.type = "MultiLine";
			new (&result.geom.multiLine) MultiLine(); // Initialise Geometry::MultiLine

			for (int i = 0; i < shape.size; ++i) {
				Line line;
				line.coords = clippers::line(shape.geom.multiLine.items[i].coords, mask);
				result.geom.multiLine.items.push_back(line);
			}

		} else if (shape.type == "Polygon") {

			result.type = "Polygon";
			new (&result.geom.polygon) Polygon(); // Initialise Geometry::Polygon

			if (!shape.geom.polygon.outer.empty) {
				result.geom.polygon.outer.coords = clippers::polygon(shape.geom.polygon.outer.coords, mask);
			}

			if (!shape.geom.polygon.inner.empty) {
				result.geom.polygon.inner.coords = clippers::polygon(shape.geom.polygon.inner.coords, mask);
			}
		} else if (shape.type == "MultiPolygon") {
			
			result.type = "MultiPolygon";
			new (&result.geom.multiPolygon) MultiPolygon(); // Initialise Geometry::MultiPolygon

			for (int i = 0; i < shape.size; ++i) {
				const Polygon& srcPoly = shape.geom.multiPolygon.items[i];
				Polygon poly;
				
				if (!srcPoly.outer.empty) {
					poly.outer.coords = clippers::polygon(srcPoly.outer.coords, mask);
				}
				
				if (!srcPoly.inner.empty) {
					poly.inner.coords = clippers::polygon(srcPoly.inner.coords, mask);
				}

				result.geom.multiPolygon.items.push_back(poly);
			}

		}

		result.refresh();
		
		return result;
	}
}