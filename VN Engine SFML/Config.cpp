#include "Config.h"
#include "ConfigParser.h"
#include <iostream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const std::string TITLE = "Visual Novel";

Parameters Config::parse(std::string path) {
	Parameters param;

	param.height = SCREEN_HEIGHT;
	param.width = SCREEN_WIDTH;
	param.title = TITLE;

	ConfigParser settings;
	if (!settings.loadFromFile(path)) {
		std::cout << "Error of load config file" << std::endl;
		return param;
	}

	settings.get("width", param.width);
	settings.get("height", param.height);
	settings.get("title", param.title);

	return param;
}