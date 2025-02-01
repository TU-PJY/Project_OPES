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

float ScriptUtil::LoadDigitData(std::string CategoryName, std::string DataName) {
	return GetDigitData(FindCategory(CategoryName), DataName);
}

std::string ScriptUtil::LoadStringData(std::string CategoryName, std::string DataName) {
	return GetStringData(FindCategory(CategoryName), DataName);
}

DigitDataVec ScriptUtil::LoadCategoryDigitData(std::string CategoryName) {
	DigitDataVec LoadedData{};
	TiXmlElement* Category = FindCategory(CategoryName);

	if (!Category)
		return {};

	TiXmlAttribute* Attribute = Category->FirstAttribute();
	while (Attribute) {
		LoadedData.emplace_back(std::stof(Attribute->Value()));
		Attribute = Attribute->Next();
	}

	return LoadedData;
}

StringDataVec ScriptUtil::LoadCategoryStringData(std::string CategoryName) {
	StringDataVec LoadedData{};
	TiXmlElement* Category = FindCategory(CategoryName);

	if (!Category) 
		return {};

	TiXmlAttribute* Attribute = Category->FirstAttribute();
	while (Attribute) {
		LoadedData.emplace_back((std::string)Attribute->Value());
		Attribute = Attribute->Next();
	}

	return LoadedData;
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