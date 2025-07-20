#include "ScriptUtil.h"

void ScriptUtil::Load(std::string FileName) {
	if (FileExist)
		return;

	if (!Doc.LoadFile(FileName.c_str(), TIXML_ENCODING_UTF8)) 
		return;

	Root = Doc.RootElement();
	std::cout << "Script util opened file: " << FileName << std::endl;
	FileExist = true;
}

void ScriptUtil::Release() {
	Doc.Clear();
	FileExist = false;
}

int ScriptUtil::GetCategoryNum() {
	int CatCount{};
	for (TiXmlElement* Element = Root->FirstChildElement(); Element != nullptr; Element = Element->NextSiblingElement())
		CatCount++;

	return CatCount;
}

TiXmlElement* ScriptUtil::GetRoot() {
	return Root;
}

float ScriptUtil::LoadDigitData(TiXmlElement* Category, std::string DataName) {
	return std::stof(Category->Attribute(DataName.c_str()));
}

std::string ScriptUtil::LoadStringData(TiXmlElement* Category, std::string DataName) {
	return Category->Attribute(DataName.c_str());
}

float ScriptUtil::LoadDigitData(std::string CategoryName, std::string DataName) {
	return GetDigitData(FindCategory(CategoryName), DataName);
}

std::string ScriptUtil::LoadStringData(std::string CategoryName, std::string DataName) {
	return GetStringData(FindCategory(CategoryName), DataName);
}

//////////////////////////////// private
float ScriptUtil::GetDigitData(TiXmlElement* CategoryVar, std::string DataName) {
	const char* DataValue = CategoryVar->Attribute(DataName.c_str());
	if (DataValue)
		return std::stof(DataValue);
	else
		return 0.0;
}

std::string ScriptUtil::GetStringData(TiXmlElement* CategoryVar, std::string DataName) {
	const char* DataValue = CategoryVar->Attribute(DataName.c_str());
	if (DataValue)
		return (std::string)DataValue;
	else 
		return "";
}

TiXmlElement* ScriptUtil::FindCategory(std::string CategoryName) {
	return Root->FirstChildElement(CategoryName.c_str());
}

std::string ScriptUtil::FindData(std::string CategoryName, std::string DataName) {
	TiXmlElement* FoundCategory = FindCategory(CategoryName);
	if (!FoundCategory) 
		return "";
	
	else {
		const char* DataValue = FoundCategory->Attribute(DataName.c_str());
		return (std::string)DataValue;
	}
}