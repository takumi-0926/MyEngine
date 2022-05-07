//#include "PMXLoader.h"
//#include <fstream>
//#include <array>
//
//bool GetPMXStringUTF16(std::ifstream& file, std::wstring& output) {
//    std::array<wchar_t, 512> wBuffer = {};
//    int texSize;
//
//    file.read(reinterpret_cast<char*>(&texSize), 4);
//
//    file.read(reinterpret_cast<char*>(&wBuffer), texSize);
//
//    output = std::wstring(&wBuffer[0], &wBuffer[0] + texSize / 2);
//}
//
//bool LoadPmx(PMXModelData& data, const std::wstring& _filePath)
//{
//    if (_filePath.empty()) { return false; }
//
//
//
//    return false;
//
//
//}
