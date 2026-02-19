#pragma once
#include "Includes.h"
#include <PSapi.h>

class AOB
{
public:
    static std::string GetModuleName();
	static MODULEINFO GetModuleInfo(char* szModule);
    static std::vector<std::string> SplitPattern(std::string pattern);
	static uintptr_t Scan(char* module, char* pattern, char* mask);
    static uintptr_t Scan(std::vector<std::string> patternTokens);
};