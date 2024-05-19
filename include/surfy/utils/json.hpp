#ifndef SURFY_UTILS_JSON_HPP
#define SURFY_UTILS_JSON_HPP

#include <fstream>
namespace surfy::utils::json {

	using surfy::json;

	/*

	Load JSON

	*/

	json load(const std::string& path) {
		std::ifstream file(path);

		if (!file.is_open()) {
			std::cerr << "Error opening Config file. " << path << std::endl;
			return 1;
		}

		std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// Parse the JSON string
		json jsonData;
		try {
			jsonData = json::parse(jsonString);
		} catch (const std::exception& e) {
			std::cerr << "Error parsing JSON: " << e.what() << std::endl;
			return jsonData;
		}

		return jsonData;
	}

	/*

	Save JSON

	*/

	bool save(const std::string& path, const json& data, const char& indent = '\t', const int& indentSize = 1) {
		// Save JSON to file
		std::ofstream outputFile(path);
		if (!outputFile.is_open()) {
			return false;	
		}

		outputFile << json(data).dump(indentSize, indent); // Dump the JSON object to the file
		outputFile.close();
		return true;
	}
}

#endif