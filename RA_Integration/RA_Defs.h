#pragma once

// We don't need all of it
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#include <ShlObj.h>
#include <tchar.h>
#include <queue>
#include <array>
#include <map>
#include <unordered_map>
#include <chrono>
// just in-case
#ifdef WIN32_LEAN_AND_MEAN
#include <ShellAPI.h>
#include <MMSystem.h>
#endif // WIN32_LEAN_AND_MEAN


// Using a pch is too complicated now, just gonna try to reduce includes
#include <fstream>

// the C98 version has fixed params and might cause errors on parsing 
#define TINYFORMAT_USE_VARIADIC_TEMPLATES 
#include "tinyformat.h"

#define _RA ::ra::

#ifndef RA_EXPORTS

//	Version Information is integrated into tags

#else

//NB. These must NOT be accessible from the emulator!
//#define RA_INTEGRATION_VERSION	"0.053"


#define RAPIDJSON_HAS_STDSTRING 1


//	RA-Only
#pragma warning(push, 1)
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/filestream.h"
#include "rapidjson/include/rapidjson/ostreamwrapper.h"
#include "rapidjson/include/rapidjson/istreamwrapper.h"
#include "rapidjson/include/rapidjson/error/en.h"

// super strech
#include "rapidjson/include/rapidjson/filereadstream.h"
#include "rapidjson/include/rapidjson/filewritestream.h"
#pragma warning(pop)

using namespace rapidjson;
extern GetParseErrorFunc GetJSONParseErrorStr;

using namespace std::string_literals;
using namespace std::chrono_literals;

namespace ra {} // just so it doesn't complain
using namespace ra;
#endif	//RA_EXPORTS


#define RA_KEYS_DLL						"RA_Keys.dll"
#define RA_PREFERENCES_FILENAME_PREFIX	"RAPrefs_"
#define RA_UNKNOWN_BADGE_IMAGE_URI		"00000"

#define RA_DIR_OVERLAY					".\\Overlay\\"
#define RA_DIR_BASE						".\\RACache\\"
#define RA_DIR_DATA						RA_DIR_BASE##"Data\\"
#define RA_DIR_BADGE					RA_DIR_BASE##"Badge\\"
#define RA_DIR_USERPIC					RA_DIR_BASE##"UserPic\\"
#define RA_DIR_BOOKMARKS				RA_DIR_BASE##"Bookmarks\\"

#define RA_GAME_HASH_FILENAME			RA_DIR_DATA##"gamehashlibrary.txt"
#define RA_GAME_LIST_FILENAME			RA_DIR_DATA##"gametitles.txt"
#define RA_MY_PROGRESS_FILENAME			RA_DIR_DATA##"myprogress.txt"
#define RA_MY_GAME_LIBRARY_FILENAME		RA_DIR_DATA##"mygamelibrary.txt"

#define RA_OVERLAY_BG_FILENAME			RA_DIR_OVERLAY##"overlayBG.png"
#define RA_NEWS_FILENAME				RA_DIR_DATA##"ra_news.txt"
#define RA_TITLES_FILENAME				RA_DIR_DATA##"gametitles.txt"
#define RA_LOG_FILENAME					RA_DIR_DATA##"RALog.txt"


#define RA_HOST_URL						"retroachievements.org"
#define RA_HOST_IMG_URL					"i.retroachievements.org"

#define SIZEOF_ARRAY( ar )	( sizeof( ar ) / sizeof( ar[ 0 ] ) )
#define SAFE_DELETE( x )	{ if( x != nullptr ) { delete x; x = nullptr; } }

using ARGB = DWORD;

//namespace RA
//{

// The function that was here (RAClamp) has a function in the standard
// (std::clamp)

class RARect : public RECT
{
public:
    RARect() {}
    RARect(LONG nX, LONG nY, LONG nW, LONG nH)
    {
        left = nX;
        right = nX + nW;
        top = nY;
        bottom = nY + nH;
    }

public:
    inline int Width() const { return(right - left); }
    inline int Height() const { return(bottom - top); }
};



class RASize
{
public:
    RASize() : m_nWidth(0), m_nHeight(0) {}
    RASize(const RASize& rhs) : m_nWidth(rhs.m_nWidth), m_nHeight(rhs.m_nHeight) {}
    RASize(int nW, int nH) : m_nWidth(nW), m_nHeight(nH) {}

public:
    inline int Width() const { return m_nWidth; }
    inline int Height() const { return m_nHeight; }
    inline void SetWidth(int nW) { m_nWidth = nW; }
    inline void SetHeight(int nH) { m_nHeight = nH; }

private:
    int m_nWidth;
    int m_nHeight;
};

const RASize RA_BADGE_PX(64, 64);
const RASize RA_USERPIC_PX(64, 64);

class ResizeContent
{
public:
    enum AlignType
    {
        NO_ALIGN,
        ALIGN_RIGHT,
        ALIGN_BOTTOM,
        ALIGN_BOTTOM_RIGHT
    };

public:
    HWND hwnd;
    POINT pLT;
    POINT pRB;
    AlignType nAlignType;
    int nDistanceX;
    int nDistanceY;
    bool bResize;

    ResizeContent(HWND parentHwnd, HWND contentHwnd, AlignType newAlignType, bool isResize)
    {
        hwnd = contentHwnd;
        nAlignType = newAlignType;
        bResize = isResize;

        RARect rect;
        GetWindowRect(hwnd, &rect);

        pLT.x = rect.left;	pLT.y = rect.top;
        pRB.x = rect.right; pRB.y = rect.bottom;

        ScreenToClient(parentHwnd, &pLT);
        ScreenToClient(parentHwnd, &pRB);

        GetWindowRect(parentHwnd, &rect);
        nDistanceX = rect.Width() - pLT.x;
        nDistanceY = rect.Height() - pLT.y;

        if (bResize)
        {
            nDistanceX -= (pRB.x - pLT.x);
            nDistanceY -= (pRB.y - pLT.y);
        }
    }

    void Resize(int width, int height)
    {
        int xPos, yPos;

        switch (nAlignType)
        {
        case ResizeContent::ALIGN_RIGHT:
            xPos = width - nDistanceX - (bResize ? pLT.x : 0);
            yPos = bResize ? (pRB.y - pLT.x) : pLT.y;
            break;
        case ResizeContent::ALIGN_BOTTOM:
            xPos = bResize ? (pRB.x - pLT.x) : pLT.x;
            yPos = height - nDistanceY - (bResize ? pLT.y : 0);
            break;
        case ResizeContent::ALIGN_BOTTOM_RIGHT:
            xPos = width - nDistanceX - (bResize ? pLT.x : 0);
            yPos = height - nDistanceY - (bResize ? pLT.y : 0);
            break;
        default:
            xPos = bResize ? (pRB.x - pLT.x) : pLT.x;
            yPos = bResize ? (pRB.y - pLT.x) : pLT.y;
            break;
        }

        if (!bResize)
            SetWindowPos(hwnd, NULL, xPos, yPos, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
        else
            SetWindowPos(hwnd, NULL, 0, 0, xPos, yPos, SWP_NOMOVE | SWP_NOZORDER);
    }
};

enum AchievementSetType
{
    Core,
    Unofficial,
    Local,

    NumAchievementSetTypes
};

// Sematic reasons
using DataStream    = std::basic_string<BYTE>;
using cstring       = const char*;
using ByteAddress   = std::size_t;
using AchievementID = std::size_t;
using LeaderboardID = std::size_t;
using GameID        = std::size_t;

// just call c_str() as needed
std::string DataStreamAsString(const DataStream& stream);

extern void RADebugLogNoFormat(const char* data);


// This function was a disaster, all those lines into two...
template<typename... Args>
void RADebugLog(const char* fmt, const Args&... args)
{
    // it couldn't deduce it
    std::string buf{ tfm::format(fmt, args...) };
    RADebugLogNoFormat(buf.c_str());
}

extern BOOL DirectoryExists(const char* sPath);

constexpr int SERVER_PING_DURATION{ 2 * 60 };
//};
//using namespace RA;

#define RA_LOG RADebugLog

#ifdef _DEBUG
#undef ASSERT
#define ASSERT( x ) assert( x )
#else
#undef ASSERT
#define ASSERT( x ) {}
#endif

#ifndef UNUSED
#define UNUSED( x ) ( x );
#endif

extern std::string Narrow(const wchar_t* wstr);
extern std::string Narrow(const std::wstring& wstr);
extern std::wstring Widen(const char* str);
extern std::wstring Widen(const std::string& str);

//	No-ops to help convert:
//	No-ops to help convert:
extern std::wstring Widen(const wchar_t* wstr);
extern std::wstring Widen(const std::wstring& wstr);
extern std::string Narrow(const char* str);
extern std::string Narrow(const std::string& wstr);

using tstring = std::basic_string<TCHAR>;


#ifdef UNICODE
#define NativeStr(x) Widen(x)
#define NativeStrType std::wstring
#else
#define NativeStr(x) Narrow(x)
#define NativeStrType std::string
#endif

// Don't feel like casting non-stop
// There's also standard literals for strings on clock types

// Use it if you need an unsigned int 
// Not using _s because that's the literal for std::string
// usage: auto a{19_z};
constexpr std::size_t operator "" _z(unsigned long long n)
{
    return static_cast<std::size_t>(n);
}

// Use it if you need a signed int
// usage: auto a{9_i};
constexpr std::intptr_t operator "" _i(unsigned long long n)
{
    return static_cast<std::intptr_t>(n);
}

namespace ra {


// NOW I remember, you can use std::to_string now as well.
template<typename CharT = char>
std::basic_string<CharT> TimeToString(time_t the_time) noexcept
{
    std::basic_ostringstream<CharT> oss;
    ss << the_time;
    return ss.str();
}

template<typename Enum, class = std::enable_if_t<std::is_enum_v<Enum>>>
constexpr auto etoi(Enum e) -> typename std::underlying_type_t<Enum>
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

// alias
template<typename Enum>
constexpr auto to_integral = etoi<Enum>;

// Stuff down here is to allow the use of strongly typed enums while still able to compare
// They do work as intended but there's too many logic errors so it'll  be commented out for now
// The SIZEOF_ARR is NOT a constant expression


// For something to be EqualityComparable, A == B, B == A must be implicitly
// convertible to bool
//#pragma region EqualityComparable
//template<
//    typename EqualityComparable,
//    typename EqualityComparable2,
//    class = std::void_t<>
//>
//struct is_equality_comparable : std::false_type {};
//
//
//template<
//    typename EqualityComparable,
//    typename EqualityComparable2
//>
//struct is_equality_comparable<EqualityComparable, EqualityComparable2,
//    std::enable_if_t<true, 
//    decltype(std::declval<EqualityComparable&>() == std::declval<EqualityComparable2&>())
//    >> : std::true_type {};
//
//template<typename EqualityComparable, typename EqualityComparable2 = EqualityComparable>
//constexpr bool is_equality_comparable_v = is_equality_comparable<EqualityComparable, EqualityComparable2>::value;
//
//template<typename EqualityComparable, typename EqualityComparable2 = EqualityComparable>
//constexpr auto operator==(const EqualityComparable& eq, const EqualityComparable2& eq2) noexcept
//-> decltype(std::declval<EqualityComparable&>() == std::declval<EqualityComparable2&>())
//{
//    return{ eq == eq2 };
//}
//#pragma endregion
//
//#pragma region LessThanComparable 
//template<
//    typename LessThanComparable,
//    typename LessThanComparable2,
//    class = std::void_t<>
//>
//struct is_lessthan_comparable : std::false_type {};
//
//
//template<
//    typename LessThanComparable,
//    typename LessThanComparable2
//>
//struct is_lessthan_comparable<LessThanComparable, LessThanComparable2,
//    std::enable_if_t<true,
//    decltype(std::declval<LessThanComparable&>() < std::declval<LessThanComparable2&>())
//    >> : std::true_type {};
//
//template<typename LessThanComparable, typename LessThanComparable2 = LessThanComparable>
//constexpr bool is_lessthan_comparable_v = is_lessthan_comparable<LessThanComparable, LessThanComparable2>::value;
//
//template<typename LessThanComparable, typename LessThanComparable2 = LessThanComparable>
//constexpr auto operator<(const LessThanComparable& a, const LessThanComparable2& b) noexcept
//-> decltype(std::declval<LessThanComparable&>() == std::declval<LessThanComparable2&>())
//{
//    static_assert(is_lessthan_comparable_v<LessThanComparable, LessThanComparable2>);
//    return{ a < b };
//}
//
//// Now that we have those the rest are handled by the standard
//using namespace std::rel_ops;





#pragma endregion


} // namespace ra


//  // Seems we need more
//template<
//    typename Enum,
//    typename Integral,
//    class = std::enable_if_t<std::is_enum_v<Enum> && std::is_integral_v<Integral>>>
//    constexpr auto operator|(const Enum& a, const Integral& b) noexcept
//    -> decltype(std::declval<Enum&>() | std::declval<Integral&>())
//{
//    auto result{ etoi(a) | b };
//    return result;
//}
//
//
//
//// Seems we need more
//template<
//    typename Enum,
//    typename Integral,
//    class = std::enable_if_t<std::is_enum_v<Enum> && std::is_integral_v<Integral>>>
//    constexpr auto operator|=(const Enum& a, const Integral& b) noexcept
//    -> decltype(std::declval<Enum&>() |= std::declval<Integral&>())
//{
//    auto result{ etoi(a) |= b };
//    return result;
//}
