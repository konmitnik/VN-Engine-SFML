#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

struct Parameters {
	int width;
	int height;
	std::string title;
};

class Config {
	public:
		Parameters parse(std::string path);
};

#endif // !_CONFIG_H_
