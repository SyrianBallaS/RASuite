#pragma once

#include "RA_Defs.h"

typedef void* HANDLE;
typedef void* LPVOID;

enum HTTPRequestMethod
{
	Post,
	Get,
	_TerminateThread,

	NumHTTPRequestMethods
};

enum RequestType
{
	//	Login
	RequestLogin,

	//	Fetch
	RequestScore,
	RequestNews,
	RequestPatch,
	RequestLatestClientPage,
	RequestRichPresence,
	RequestAchievementInfo,
	RequestLeaderboardInfo,
	RequestCodeNotes,
	RequestFriendList,
	RequestBadgeIter,
	RequestUnlocks,
	RequestHashLibrary,
	RequestGamesList,
	RequestAllProgress,
	RequestGameID,

	//	Submit
	RequestPing,
	RequestPostActivity,
	RequestSubmitAwardAchievement,
	RequestSubmitCodeNote,
	RequestSubmitLeaderboardEntry,
	RequestSubmitAchievementData,
	RequestSubmitTicket,
	RequestSubmitNewTitle,

	//	Media:
	RequestUserPic,
	RequestBadge,

	//	Special:
	StopThread,

	NumRequestTypes
};

enum UploadType
{
	//	Upload:
	RequestUploadBadgeImage,

	NumUploadTypes
};


// You sure you don't want it unordered? (hash_table)
using PostArgs = std::map<char, std::string>;

extern std::string PostArgsToString(const PostArgs& args);

class RequestObject
{
public:
	RequestObject(RequestType nType, const PostArgs& PostArgs = PostArgs{},
		const std::string& sData = "") :
		m_nType{ nType }, m_PostArgs{ PostArgs }, m_sData{ sData }
	{}
	RequestObject(const RequestObject&) = delete;
	RequestObject& operator=(const RequestObject&) = delete;
	RequestObject(RequestObject&& b) noexcept :
	m_nType{ b.m_nType },
		m_PostArgs{ std::move(b.m_PostArgs) },
		m_sData{ std::move(b.m_sData) },
		m_sResponse{ std::move(m_sResponse) }
	{
		b.m_nType = RequestType{};
	}
	RequestObject& operator=(RequestObject&& b) noexcept
	{
		m_nType     = b.m_nType;
		m_PostArgs  = std::move(b.m_PostArgs);
		m_sData     = std::move(b.m_sData);
		m_sResponse = std::move(b.m_sResponse);

		b.m_nType = RequestType{};
	}
public:
	const RequestType GetRequestType() const { return m_nType; }
	const PostArgs& GetPostArgs() const { return m_PostArgs; }
	const std::string& GetData() const { return m_sData; }

	DataStream& GetResponse() { return m_sResponse; }
	const DataStream& GetResponse() const { return m_sResponse; }
	void SetResponse(const DataStream& sResponse) { m_sResponse = sResponse; }

	BOOL ParseResponseToJSON(Document& rDocOut);

private:
	// Instead of making this const I deleted copying
	RequestType m_nType{ RequestType{} };
	PostArgs m_PostArgs;
	std::string m_sData;
	DataStream m_sResponse;
};

class HttpResults
{
public:
	//	Caller must manage: SAFE_DELETE when finished
	RequestObject * PopNextItem();
	const RequestObject* PeekNextItem() const;
	void PushItem(RequestObject* pObj);
	void Clear();
	size_t Count() const;
	BOOL PageRequestExists(RequestType nType, const std::string& sData) const;

private:
	std::deque<RequestObject*> m_aRequests;
};

class RAWeb
{
public:
	static void RA_InitializeHTTPThreads();
	static void RA_KillHTTPThreads();

	static void LogJSON(const Document& doc);

	static void CreateThreadedHTTPRequest(RequestType nType, const PostArgs& PostData = PostArgs(), const std::string& sData = "");
	static BOOL HTTPRequestExists(RequestType nType, const std::string& sData);
	static BOOL HTTPResponseExists(RequestType nType, const std::string& sData);

	static BOOL DoBlockingRequest(RequestType nType, const PostArgs& PostData, Document& JSONResponseOut);
	static BOOL DoBlockingRequest(RequestType nType, const PostArgs& PostData, DataStream& ResponseOut);

	static BOOL DoBlockingHttpGet(const std::string& sRequestedPage, DataStream& ResponseOut, bool bIsImageRequest);
	static BOOL DoBlockingHttpPost(const std::string& sRequestedPage, const std::string& sPostString, DataStream& ResponseOut);

	static BOOL DoBlockingImageUpload(UploadType nType, const std::string& sFilename, Document& ResponseOut);

	static DWORD WINAPI HTTPWorkerThread(LPVOID lpParameter);

	static HANDLE Mutex() { return ms_hHTTPMutex; }
	static RequestObject* PopNextHttpResult() { return ms_LastHttpResults.PopNextItem(); }

private:
	static HANDLE ms_hHTTPMutex;
	static HttpResults ms_LastHttpResults;
};
