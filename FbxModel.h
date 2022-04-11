#pragma once

#include <string>

struct Node {

};

class FbxModel {
public:
	friend class FbxLoader;
private:
	std::string name;
};