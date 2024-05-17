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

		Coords line(const Coords& line, const Coords& mask) {
			Coords clipped;
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

		Point intersect(Point s, Point e, Point cp1, Point cp2) {
			double A1 = e.y - s.y;
			double B1 = s.x - e.x;
			double C1 = A1 * s.x + B1 * s.y;

			double A2 = cp2.y - cp1.y;
			double B2 = cp1.x - cp2.x;
			double C2 = A2 * cp1.x + B2 * cp1.y;

			double det = A1 * B2 - A2 * B1;

			if (det == 0) {
				return {0, 0}; // Lines are parallel, no intersection
			}

			double x = (B2 * C1 - B1 * C2) / det;
			double y = (A1 * C2 - A2 * C1) / det;
			return {x, y};
		}

		bool inside(Point p, Point cp1, Point cp2) {
			return (cp2.x - cp1.x) * (p.y - cp1.y) >= (cp2.y - cp1.y) * (p.x - cp1.x);
		}

		Coords sutherlandHodgman(const Coords& subject, const Coords& clip) {
			Coords output = subject;

			for (size_t i = 0; i < clip.size(); ++i) {
				Coords input = output;
				output.clear();

				Point cp1 = clip[i];
				Point cp2 = clip[(i + 1) % clip.size()];

				for (size_t j = 0; j < input.size(); ++j) {
					Point s = input[j];
					Point e = input[(j + 1) % input.size()];

					if (inside(e, cp1, cp2)) {
						if (!inside(s, cp1, cp2)) {
							output.push_back(intersect(s, e, cp1, cp2));
						}
						output.push_back(e);
					} else if (inside(s, cp1, cp2)) {
						output.push_back(intersect(s, e, cp1, cp2));
					}
				}
			}

			return output;
		}



	}

	/*

	Clip

	*/

	void Shape::clip(const Coords& mask) {
		
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