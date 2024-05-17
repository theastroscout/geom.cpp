/*

Printer

*/

#include <sstream>
#include <iomanip>

namespace surfy::geom {

	namespace print {
		/*

		Point

		*/

		void point(std::ostream& os, const Point& point, const bool& compressed) {

			if (compressed) {
				os << std::fixed << std::setprecision(0) << std::round(point.x * 1e6) << " " << std::round(point.y * 1e6);
			} else {
				os << std::setprecision(8) << std::noshowpoint << point.x << " " << point.y;
			}
			// os << std::to_string(point.x) << " " << std::to_string(point.y);
		}

		/*

		Line

		*/

		void line(std::ostream& os, const Coords& coords, const bool& compressed) {
			os << "(";
			size_t length = coords.size();
			for (size_t i = 0; i < length; ++i) {
				point(os, coords[i], compressed);
				if (i != length - 1) {
					os << ", ";
				}
			}
			os << ")";
		}

		/*

		Polygon

		*/

		void polygon(std::ostream& os, const types::Polygon& poly, const bool& compressed) {
			
			os << "(";

			line(os, poly.outer.coords, compressed);

			if (!poly.inner.coords.empty()) {
				os << ",";
				line(os, poly.inner.coords, compressed);
			}

			os << ")";
		}
	}

	/*

	Point

	*/

	std::ostream& operator<<(std::ostream& os, const Point& point) {
		os << "(";
		print::point(os, point);
		os << ")";
		return os;
	}

	/*

	Coords

	*/

	std::ostream& operator<<(std::ostream& os, const Coords& coords) {
		print::line(os, coords);
		return os;
	}

	/*

	Point Type

	*/

	std::ostream& operator<<(std::ostream& os, const types::Point& point) {
		os << "POINT (";
		print::point(os, point);
		os << ")";
		return os;
	}

	/*

	Line Type

	*/

	std::ostream& operator<<(std::ostream& os, const types::Line& line) {
		os << "LINESTRING ";
		print::line(os, line.coords);
		return os;
	}

	/*

	MultiLine Type

	*/

	std::ostream& operator<<(std::ostream& os, const types::MultiLine& multiLine) {
		os << "MULTILINESTRING (";
		for (int i = 0; i < multiLine.size; ++i) {
			print::line(os, multiLine.items[i].coords);
			if (i != multiLine.size - 1) {
				os << ",";
			}
		}
		os << ")";
		return os;
	}

	/*

	Polygon Type

	*/

	std::ostream& operator<<(std::ostream& os, const types::Polygon& poly) {
		os << "POLYGON ";
		print::polygon(os, poly);
		return os;
	}

	/*

	Shape

	*/

	std::ostream& operator<<(std::ostream& os, const Shape& shape) {

		if (shape.type == "Point") {

			os << "POINT (";
			print::point(os, shape.geom.point);
			os << ")";

		} else if (shape.type == "Line") {

			os << "LINESTRING ";
			print::line(os, shape.geom.line.coords);

		} else if (shape.type == "MultiLine") {

			os << "MULTILINESTRING (";
			for (int i = 0; i < shape.size; ++i) {
				print::line(os, shape.geom.multiLine.items[i].coords);
				if (i != shape.size - 1) {
					os << ",";
				}
			}
			os << ")";

		} else if (shape.type == "Polygon") {

			os << "POLYGON ";
			print::polygon(os, shape.geom.polygon);

		} else if (shape.type == "MultiPolygon") {

			os << "MULTIPOLYGON (";
			for (int i = 0; i < shape.size; ++i) {
				print::polygon(os, shape.geom.multiPolygon.items[i]);
				if (i != shape.size - 1) {
					os << ",";
				}
			}
			os << ")";
		}

		return os;
	}
}