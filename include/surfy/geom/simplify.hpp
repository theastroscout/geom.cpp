/*


Simplify
Douglas-Peucker algorithm


*/

namespace surfy::geom {
	void Shape::simplify(const double& intolerance = 1.) {

		if (type == "Point") {

		} else if (type == "Line") {

			if (geom.line.coords.size() > 2) {
				Coords coords;
				utils::simplify(geom.line.coords, intolerance, coords);
				geom.line.coords = coords;
			}

		} else if (type == "MultiLine") {

			for (int i = 0; i < size; ++i) {
				types::Line& line = geom.multiLine.items[i];
				if (!line.empty) {
					Coords coords;
					utils::simplify(line.coords, intolerance, coords);
					line.coords = coords;
				}
			}

		} else if (type == "Polygon") {

			if (!geom.polygon.outer.empty) {
				Coords coords;
				utils::simplify(geom.polygon.outer.coords, intolerance, coords);
				geom.polygon.outer.coords = coords;
			}

			if (!geom.polygon.inner.empty) {
				Coords coords;
				utils::simplify(geom.polygon.inner.coords, intolerance,coords);
				geom.polygon.inner.coords = coords;
			}

		} else if (type == "MultiPolygon") {

			for (int i = 0; i < size; ++i) {
				types::Polygon& poly = geom.multiPolygon.items[i];
				
				if (!poly.outer.empty) {
					Coords coords;
					utils::simplify(poly.outer.coords, intolerance, coords);
					poly.outer.coords = coords;
				}

				if (!poly.inner.empty) {
					Coords coords;
					utils::simplify(poly.inner.coords, intolerance, coords);
					poly.inner.coords = coords;
				}
			}			

		}

		refresh();

		// return result;
	}
}