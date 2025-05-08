#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "DirectX_3D.h"
#include "tinyxml/tinyxml.h"
#include <iostream>
#include <string>

using CategoryPtr = TiXmlElement*;

// read only
class ScriptUtil {
public:
	TiXmlDocument Doc{};
	TiXmlElement* Root{};
	bool          FileExist{};

public:
	ScriptUtil() {}

	void Load(std::string FileName);
	float LoadDigitData(std::string CategoryName, std::string DataName);
	std::string LoadStringData(std::string CategoryName, std::string DataName);
	DigitDataVec LoadCategoryDigitData(std::string CategoryName);
	StringDataVec LoadCategoryStringData(std::string CategoryName);

	template<typename Function>
	void LoadAllData(Function Func) {
		TiXmlElement* Category = Root->FirstChildElement();
		while (Category) {
			Func(Category);
			Category = Category->NextSiblingElement();
		}
	}

	void Release();
	int GetCategoryNum();
	TiXmlElement* GetRoot();
	float LoadDigitData(TiXmlElement* Category, std::string DataName);
	std::string LoadStringData(TiXmlElement* Category, std::string DataName);
	TiXmlElement* FindCategory(std::string CategoryName);
	std::string FindData(std::string CategoryName, std::string DataName);
	float GetDigitData(TiXmlElement* CategoryVar, std::string DataName);
	std::string GetStringData(TiXmlElement* CategoryVar, std::string DataName);
};