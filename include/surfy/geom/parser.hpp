/*

Surfy::geom::parser
Parsing string

*/

namespace surfy::geom::parser {

	types::Polygon polygon(const std::string& body) {

		types::Polygon poly;

		int pass = 1;
		size_t pos = 0;
		while (pos < body.size()) {
			size_t start = body.find("(", pos);
			size_t end = body.find(")", start);
			if (start == std::string::npos || end == std::string::npos) {
				break; // No more polygons found
			}

			std::string polyStr = body.substr(start + 1, end - start - 1);

			pos = end + 1;

			Coords coords = utils::parseCoordsString(polyStr);

			if (pass == 1) {
				poly.outer.coords = coords;
			} else {
				poly.inner.coords = coords;
			}

			pass++;
		}

		return poly;
	}
}