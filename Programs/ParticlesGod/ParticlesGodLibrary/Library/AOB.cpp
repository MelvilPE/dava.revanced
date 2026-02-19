#include "AOB.h"

std::string AOB::GetModuleName()
{
    WCHAR WC[MAX_PATH];
    GetModuleFileNameW(NULL, WC, MAX_PATH);
    std::wstring WCSTR(WC);
    std::string EXEPATH = std::string(WCSTR.begin(), WCSTR.end());
    /* Here we take the second part after splitting by "\\/" which is exe filename */
    if (EXEPATH.find_last_of("\\/") != std::string::npos)
    {
        return EXEPATH.substr(EXEPATH.find_last_of("\\/") + 1);
    }

    return EXEPATH;
}

MODULEINFO AOB::GetModuleInfo(char* szModule)
{
	MODULEINFO modInfo = { NULL };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == NULL)
	{
		return modInfo;
	}

	GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}

std::vector<std::string> AOB::SplitPattern(std::string pattern)
{
    std::istringstream split(pattern);
    std::vector<std::string> tokens;
    std::string token;

    while (split >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}

uintptr_t AOB::Scan(char* module, char* pattern, char* mask)
{
    MODULEINFO modInfo = AOB::GetModuleInfo(module);
    uintptr_t base = (uintptr_t)modInfo.lpBaseOfDll;
    uintptr_t size = (uintptr_t)modInfo.SizeOfImage;
    uintptr_t patternLength = (uintptr_t)strlen(mask);

    for (uintptr_t i = 0; i < size - patternLength; i++)
    {
        bool resultFound = true;
        for (uintptr_t k = 0; k < patternLength; k++)
        {
            resultFound &= mask[k] == '?' || pattern[k] == *(char*)(base + i + k);
        }
        if (resultFound)
        {
            return (uintptr_t)(base + i);
        }
    }
    return 0;
}

uintptr_t AOB::Scan(std::vector<std::string> patternTokens)
{
    std::string moduleName = GetModuleName();
    MODULEINFO modInfo = GetModuleInfo((char*)moduleName.c_str());
    uintptr_t base = (uintptr_t)modInfo.lpBaseOfDll;
    uintptr_t size = (uintptr_t)modInfo.SizeOfImage;
    uintptr_t patternLength = (uintptr_t)(patternTokens.size());

    for (uintptr_t i = 0; i < size - patternLength; i++)
    {
        bool resultFound = true;
        for (uintptr_t k = 0; k < patternLength; k++)
        {
            if (patternTokens[k] != "?" && patternTokens[k] != "??")
            {
                int intValue = std::stoi(patternTokens[k], nullptr, 16);
                char charValue = static_cast<char>(intValue);
                if (charValue != *reinterpret_cast<char*>(base + i + k))
                {
                    resultFound = false;
                    break;
                }
            }
        }
        if (resultFound)
        {
            return base + i;
        }
    }
    return 0;
}