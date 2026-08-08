#include <Windows.h>
#include <Xinput.h>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

// Logger
static fs::path g_logPath;

static std::string GetTimestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

static void WriteLog(const std::string& msg)
{
    if (g_logPath.empty())
        return;

    std::ofstream file(g_logPath, std::ios::app);
    if (file.is_open())
    {
        file << "[" << GetTimestamp() << "] " << msg << "\n";
    }
}

// XInput function pointers
using XInputGetState_t              = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
using XInputSetState_t              = DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*);
using XInputGetCapabilities_t       = DWORD(WINAPI*)(DWORD, DWORD, XINPUT_CAPABILITIES*);
using XInputEnable_t                = void (WINAPI*)(BOOL);
using XInputGetBatteryInformation_t = DWORD(WINAPI*)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);
using XInputGetKeystroke_t          = DWORD(WINAPI*)(DWORD, DWORD, PXINPUT_KEYSTROKE);
using XInputGetAudioDeviceIds_t     = DWORD(WINAPI*)(DWORD, LPWSTR, UINT*, LPWSTR, UINT*);
using XInputGetDSoundAudioDeviceGuids_t = DWORD(WINAPI*)(DWORD, GUID*, GUID*);

static HMODULE g_realXInput = nullptr;

static XInputGetState_t               Real_XInputGetState              = nullptr;
static XInputSetState_t               Real_XInputSetState              = nullptr;
static XInputGetCapabilities_t        Real_XInputGetCapabilities       = nullptr;
static XInputEnable_t                 Real_XInputEnable                = nullptr;
static XInputGetBatteryInformation_t  Real_XInputGetBatteryInformation = nullptr;
static XInputGetKeystroke_t           Real_XInputGetKeystroke          = nullptr;
static XInputGetAudioDeviceIds_t      Real_XInputGetAudioDeviceIds     = nullptr;
static XInputGetDSoundAudioDeviceGuids_t Real_XInputGetDSoundAudioDeviceGuids = nullptr;

static bool LoadRealXInput()
{
    if (g_realXInput)
        return true;

    wchar_t sysPath[MAX_PATH] = {};
    GetSystemDirectoryW(sysPath, MAX_PATH);
    std::wstring fullPath = std::wstring(sysPath) + L"\\XInput1_4.dll";

    g_realXInput = LoadLibraryW(fullPath.c_str());
    if (!g_realXInput)
    {
        WriteLog("Failed to load real system XInput1_4.dll");
        return false;
    }

    Real_XInputGetState              = (XInputGetState_t)GetProcAddress(g_realXInput, "XInputGetState");
    Real_XInputSetState              = (XInputSetState_t)GetProcAddress(g_realXInput, "XInputSetState");
    Real_XInputGetCapabilities       = (XInputGetCapabilities_t)GetProcAddress(g_realXInput, "XInputGetCapabilities");
    Real_XInputEnable                = (XInputEnable_t)GetProcAddress(g_realXInput, "XInputEnable");
    Real_XInputGetBatteryInformation = (XInputGetBatteryInformation_t)GetProcAddress(g_realXInput, "XInputGetBatteryInformation");
    Real_XInputGetKeystroke          = (XInputGetKeystroke_t)GetProcAddress(g_realXInput, "XInputGetKeystroke");
    Real_XInputGetAudioDeviceIds     = (XInputGetAudioDeviceIds_t)GetProcAddress(g_realXInput, "XInputGetAudioDeviceIds");
    Real_XInputGetDSoundAudioDeviceGuids = (XInputGetDSoundAudioDeviceGuids_t)GetProcAddress(g_realXInput, "XInputGetDSoundAudioDeviceGuids");

    if (!Real_XInputGetState)
    {
        WriteLog("Failed to resolve XInputGetState");
        return false;
    }

    return true;
}

extern "C" {

__declspec(dllexport) DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
    if (!LoadRealXInput() || !Real_XInputGetState)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetState(dwUserIndex, pState);
}

__declspec(dllexport) DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
    if (!LoadRealXInput() || !Real_XInputSetState)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputSetState(dwUserIndex, pVibration);
}

__declspec(dllexport) DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
    if (!LoadRealXInput() || !Real_XInputGetCapabilities)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}

__declspec(dllexport) void WINAPI XInputEnable(BOOL enable)
{
    if (LoadRealXInput() && Real_XInputEnable)
        Real_XInputEnable(enable);
}

__declspec(dllexport) DWORD WINAPI XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
    if (!LoadRealXInput() || !Real_XInputGetBatteryInformation)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
}

__declspec(dllexport) DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke)
{
    if (!LoadRealXInput() || !Real_XInputGetKeystroke)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetKeystroke(dwUserIndex, dwReserved, pKeystroke);
}

__declspec(dllexport) DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount)
{
    if (!LoadRealXInput() || !Real_XInputGetAudioDeviceIds)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetAudioDeviceIds(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);
}

__declspec(dllexport) DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
    if (!LoadRealXInput() || !Real_XInputGetDSoundAudioDeviceGuids)
        return ERROR_DEVICE_NOT_CONNECTED;
    return Real_XInputGetDSoundAudioDeviceGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

} // extern "C"

// Load mods
static void LoadMods(HMODULE hSelf)
{
    wchar_t modulePath[MAX_PATH] = {};
    GetModuleFileNameW(hSelf, modulePath, MAX_PATH);

    fs::path dllDir = fs::path(modulePath).parent_path();
    g_logPath = dllDir / L"dumbLoader.log";

    fs::path modsDir = dllDir / L"mods";

    if (!fs::exists(modsDir) || !fs::is_directory(modsDir))
        return;

    // Safe mode
    if (fs::exists(modsDir / L"disable") || fs::exists(modsDir / L"disable.txt"))
    {
        WriteLog("Safe mode active (disable file found) - skipping all mods");
        return;
    }

    // Collect all .dll files
    std::vector<fs::path> dlls;
    for (const auto& entry : fs::directory_iterator(modsDir))
    {
        if (!entry.is_regular_file())
            continue;

        auto ext = entry.path().extension();
        if (ext == L".dll" || ext == L".DLL")
            dlls.push_back(entry.path());
    }

    // Priority: alphabetical order (use 01_, 02_ etc. if you need specific order)
    std::sort(dlls.begin(), dlls.end(), [](const fs::path& a, const fs::path& b) {
        return a.filename().wstring() < b.filename().wstring();
    });

    // Load in order
    for (const auto& path : dlls)
    {
        HMODULE loaded = LoadLibraryW(path.c_str());
        if (!loaded)
        {
            WriteLog("Failed to load: " + path.string() + " (error " + std::to_string(GetLastError()) + ")");
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        LoadMods(hModule);
        break;

    case DLL_PROCESS_DETACH:
        if (g_realXInput)
        {
            FreeLibrary(g_realXInput);
            g_realXInput = nullptr;
        }
        break;
    }
    return TRUE;
}
