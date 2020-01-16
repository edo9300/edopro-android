#ifndef GAME_H
#define GAME_H

#include "dllinterface.h"
#include "config.h"
#include "client_field.h"
#include "deck_con.h"
#include "menu_handler.h"
#include "discord_wrapper.h"
#include <unordered_map>
#include <vector>
#include <list>
#include "CGUIImageButton/CGUIImageButton.h"
#include "CGUITTFont/CGUITTFont.h"
#include "CGUISkinSystem/CGUISkinSystem.h"
#include "CGUICustomText/CGUICustomText.h"
#include "CGUIFileSelectListBox/CGUIFileSelectListBox.h"
#include "CGUICustomContextMenu/CGUICustomContextMenu.h"
#include "CGUICustomContextMenu/CGUICustomMenu.h"
#include "CGUICustomTabControl/CGUICustomTabControl.h"
#include "CProgressBar/CProgressBar.h"
#include "ResizeablePanel/ResizeablePanel.h"
#include "deck_manager.h"
#include "sound_manager.h"
#include "repo_manager.h"
#include "windbot_panel.h"

namespace ygo {

struct Config {
	bool use_d3d;
	bool use_vsync;
	float dpi_scale;
	int max_fps;
	int game_version;
	bool fullscreen;
	unsigned short antialias;
	std::wstring serverport;
	unsigned char textfontsize;
	std::wstring lasthost;
	std::wstring lastport;
	std::wstring nickname;
	std::wstring gamename;
	std::wstring lastdeck;
	unsigned int lastlflist;
	unsigned int lastallowedcards;
	std::wstring textfont;
	std::wstring numfont;
	std::wstring roompass;
	//settings
	int chkMAutoPos;
	int chkSTAutoPos;
	int chkRandomPos;
	int chkAutoChain;
	int chkWaitChain;
	int chkIgnore1;
	int chkIgnore2;
	int chkHideSetname;
	int chkHideHintButton;
	int draw_field_spell;
	int quick_animation;

	int chkAnime;
	bool enablemusic;
	bool enablesound;
	double musicVolume;
	double soundVolume;
	int skin_index;
};

struct DuelInfo {
	bool isInDuel;
	bool isStarted;
	bool isReplay;
	bool isOldReplay;
	bool isCatchingUp;
	bool isFirst;
	bool isRelay;
	bool isInLobby;
	bool isSingleMode;
	bool compat_mode;
	bool is_shuffling;
	int current_player[2];
	int lp[2];
	int startlp;
	int duel_field;
	int duel_params;
	int extraval;
	int turn;
	short curMsg;
	int team1;
	int team2;
	int best_of;
	std::vector<std::wstring> clientname;
	std::vector<std::wstring> hostname;
	std::wstring strLP[2];
	std::wstring vic_string;
	unsigned char player_type;
	unsigned char time_player;
	unsigned short time_limit;
	unsigned short time_left[2];
	DiscordWrapper::DiscordSecret secret;
	bool isReplaySwapped;
};

struct FadingUnit {
	bool signalAction;
	bool isFadein;
	float fadingFrame;
	int autoFadeoutFrame;
	irr::gui::IGUIElement* guiFading;
	irr::core::recti fadingSize;
	irr::core::vector2di fadingUL;
	irr::core::vector2di fadingLR;
	irr::core::vector2di fadingDest;
};

class Game {

public:
	bool Initialize();
	void MainLoop();
	void BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
	void LoadZipArchives();
	void LoadExpansionDB();
	void LoadArchivesDB();
	void RefreshDeck(irr::gui::IGUIComboBox* cbDeck);
	void RefreshLFLists();
	void RefreshAiDecks();
	void RefreshReplay();
	void RefreshSingleplay();
	void DrawSelectionLine(irr::video::S3DVertex* vec, bool strip, int width, irr::video::SColor color);
	void DrawSelectionLine(irr::gui::IGUIElement* element, int width, irr::video::SColor color);
	void DrawBackGround();
	void DrawLinkedZones(ClientCard* pcard);
	bool CheckMutual(ClientCard* pcard, int mark);
	void DrawCards();
	void DrawCard(ClientCard* pcard);
	void DrawShadowText(irr::gui::CGUITTFont* font, const core::stringw& text, const core::rect<s32>& shadowposition, const core::rect<s32>& padding, video::SColor color = 0xffffffff, video::SColor shadowcolor = 0xff000000, bool hcenter = false, bool vcenter = false, const core::rect<s32>* clip = nullptr);
	void DrawMisc();
	void DrawStatus(ClientCard* pcard);
	void DrawPendScale(ClientCard* pcard);
	void DrawStackIndicator(const std::wstring& text, S3DVertex* v, bool opponent);
	void DrawGUI();
	void DrawSpec();
	void DrawBackImage(irr::video::ITexture* texture);
	void ShowElement(irr::gui::IGUIElement* element, int autoframe = 0);
	void HideElement(irr::gui::IGUIElement* element, bool set_action = false);
	void PopupElement(irr::gui::IGUIElement* element, int hideframe = 0);
	void WaitFrameSignal(int frame);
	void DrawThumb(CardDataC* cp, position2di pos, LFList* lflist, bool drag = false, recti* cliprect = nullptr, bool loadimage = true);
	void DrawDeckBd();
	void LoadConfig();
	void SaveConfig();
	void LoadPicUrls();
	void AddGithubRepositoryStatusWindow(const RepoManager::GitRepo& repo);
	void LoadGithubRepositories();
	void LoadServers();
	void ShowCardInfo(int code, bool resize = false);
	void ClearCardInfo(int player = 0);
	void AddChatMsg(const std::wstring& msg, int player, int type);
	void AddLog(const std::wstring& msg, int param = 0);
	void ClearChatMsg();
	void AddDebugMsg(const std::string& msg);
	void ClearTextures();
	void CloseDuelWindow();
	void PopupMessage(const std::wstring& text, const std::wstring& caption = L"");
	static irr::video::SColor GetSkinColor(const std::wstring& value, irr::video::SColor fallback = NULL);

	uint8 LocalPlayer(uint8 player);
	std::wstring LocalName(int local_player);
	void UpdateDuelParam();
	void UpdateExtraRules();
	int GetMasterRule(uint32 param, uint32 forbidden, int* truerule = 0);
	void SetPhaseButtons();
	void SetMesageWindow();

	bool HasFocus(EGUI_ELEMENT_TYPE type) const {
		irr::gui::IGUIElement* focus = env->getFocus();
		return focus && focus->hasType(type);
	}

	void OnResize();
	template<typename T>
	T Scale(T val);
	template<typename T, typename T2, typename T3, typename T4>
	rect<T> Scale(T x, T2 y, T3 x2, T4 y2);
	template<typename T>
	rect<T> Scale(rect<T> rect);
	template<typename T>
	vector2d<T> Scale(vector2d<T> vec);
	template<typename T>
	T ResizeX(T x);
	template<typename T>
	T ResizeY(T y);
	template<typename T, typename T2>
	vector2d<T> Scale(T x, T2 y);
	recti Resize(s32 x, s32 y, s32 x2, s32 y2);
	recti Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2);
	vector2d<s32> Resize(s32 x, s32 y, bool reverse = false);
	recti ResizeElem(s32 x, s32 y, s32 x2, s32 y2, bool scale = true);
	recti ResizePhaseHint(s32 x, s32 y, s32 x2, s32 y2, s32 width);
	recti ResizeWinFromCenter(s32 x, s32 y, s32 x2, s32 y2);
	recti ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat = false);
	void SetCentered(irr::gui::IGUIElement* elem);
	void ValidateName(irr::gui::IGUIElement* box);
	
	std::wstring ReadPuzzleMessage(const std::wstring& script_name);
	OCG_Duel SetupDuel(OCG_DuelOptions opts);
	std::vector<char> LoadScript(const std::string& script_name);
	bool LoadScript(OCG_Duel pduel, const std::string& script_name);
	static int ScriptReader(void* payload, OCG_Duel duel, const char* name);
	static void MessageHandler(void* payload, const char* string, int type);

	path_string working_directory;
	std::unique_ptr<SoundManager> soundManager;
	std::mutex gMutex;
	std::mutex analyzeMutex;
	Signal frameSignal;
	Signal actionSignal;
	Signal replaySignal;
	std::mutex closeSignal;
	Signal closeDoneSignal;
	Config gameConf;
	DuelInfo dInfo;
	DiscordWrapper discord;
#ifdef YGOPRO_BUILD_DLL
	void* ocgcore;
#endif
	bool coreloaded;
	bool is_fullscreen;
	std::list<FadingUnit> fadingList;
	std::vector<int> logParam;
	std::wstring chatMsg[8];
	struct RepoGui {
		std::string path;
		IProgressBar* progress1;
		IProgressBar* progress2;
		irr::gui::IGUIButton* history_button1;
		irr::gui::IGUIButton* history_button2;
		std::wstring commit_history_full;
		std::wstring commit_history_partial;
	};
	std::map<std::string, RepoGui> repoInfoGui;

	uint32 delta_time;
	uint32 delta_frames;
	int hideChatTimer;
	bool hideChat;
	float chatTiming[8];
	int chatType[8];
	unsigned short linePatternD3D;
	unsigned short linePatternGL;
	float waitFrame;
	int signalFrame;
	int saveReplay;
	int showcard;
	int showcardcode;
	float showcarddif;
	float showcardp;
	bool is_attacking;
	float attack_sv;
	irr::core::vector3df atk_r;
	irr::core::vector3df atk_t;
	float atkdy;
	int lpframe;
	float lpd;
	int lpplayer;
	int lpccolor;
	int lpcalpha;
	std::wstring lpcstring;
	bool always_chain;
	bool ignore_chain;
	bool chain_when_avail;

	bool is_building;
	bool is_siding;
	int forbiddentypes;
	unsigned short extra_rules;
	uint32 duel_param;
	uint32 showingcard;
	bool cardimagetextureloading;


	irr::core::dimension2d<irr::u32> window_size;
	irr::core::vector2d<irr::f32> window_scale;

	CGUISkinSystem *skinSystem;

	ClientField dField;
	DeckBuilder deckBuilder;
	MenuHandler menuHandler;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::scene::ICameraSceneNode* camera;
	io::IFileSystem* filesystem;
	void PopulateResourcesDirectories();
	std::vector<path_string> field_dirs;
	std::vector<path_string> pic_dirs;
	std::vector<path_string> cover_dirs;
	std::vector<path_string> script_dirs;
	std::vector<path_string> cores_to_load;
	std::mutex popupCheck;
	std::wstring queued_msg;
	std::wstring queued_caption;
	//GUI
	irr::gui::IGUIEnvironment* env;
	irr::gui::CGUITTFont* guiFont;
	irr::gui::CGUITTFont* textFont;
	irr::gui::CGUITTFont* numFont;
	irr::gui::CGUITTFont* adFont;
	irr::gui::CGUITTFont* lpcFont;
	std::map<irr::gui::CGUIImageButton*, int> imageLoading;
	//card image
	irr::gui::IGUIStaticText* wCardImg;
	irr::gui::IGUIImage* imgCard;
	//hint text
	irr::gui::IGUIStaticText* stHintMsg;
	irr::gui::IGUIStaticText* stTip;
	irr::gui::IGUIStaticText* stCardListTip;
	//infos
	int infosExpanded; //0: not expanded, 1: expanded and to be shown as expanded, 2: expanded but not to be shown as expanded
	irr::gui::IGUITabControl* wInfos;
	irr::gui::IGUIStaticText* stName;
	irr::gui::IGUIStaticText* stInfo;
	irr::gui::IGUIStaticText* stDataInfo;
	irr::gui::IGUIStaticText* stSetName;
	irr::gui::IGUIStaticText* stText;
	irr::gui::IGUIStaticText* stMusicVolume;
	irr::gui::IGUIStaticText* stSoundVolume;
	irr::gui::IGUITab* tabLog;
	irr::gui::IGUIListBox* lstLog;
	irr::gui::IGUITab* tabChat;
	irr::gui::IGUIListBox* lstChat;
	irr::gui::IGUIButton* btnClearLog;
	irr::gui::IGUIButton* btnExpandLog;
	irr::gui::IGUIButton* btnClearChat;
	irr::gui::IGUIButton* btnExpandChat;
	irr::gui::IGUIButton* btnSaveLog;
	irr::gui::IGUITab* tabRepositories;
	irr::gui::IGUIContextMenu* mTabRepositories;
	irr::gui::Panel* tabSystem;
	irr::gui::IGUICheckBox* chkMAutoPos;
	irr::gui::IGUICheckBox* chkSTAutoPos;
	irr::gui::IGUICheckBox* chkRandomPos;
	irr::gui::IGUICheckBox* chkAutoChain;
	irr::gui::IGUICheckBox* chkWaitChain;
	irr::gui::IGUICheckBox* chkQuickAnimation;
	irr::gui::IGUICheckBox* chkHideSetname;
	irr::gui::IGUICheckBox* chkHideHintButton;
	irr::gui::IGUICheckBox* chkEnableSound;
	irr::gui::IGUICheckBox* chkEnableMusic;
	irr::gui::IGUIScrollBar* scrMusicVolume;
	irr::gui::IGUIScrollBar* scrSoundVolume;
	//main menu
	irr::gui::IGUIWindow* wMainMenu;
	irr::gui::IGUIWindow* wCommitsLog;
	irr::gui::IGUIContextMenu* mTopMenu;
	irr::gui::IGUIContextMenu* mRepositoriesInfo;
	irr::gui::IGUIContextMenu* mAbout;
	irr::gui::IGUIWindow* wAbout;
	irr::gui::IGUIStaticText* stAbout;
	irr::gui::IGUIButton* btnOnlineMode;
	irr::gui::IGUIButton* btnLanMode;
	irr::gui::IGUIButton* btnSingleMode;
	irr::gui::IGUIButton* btnReplayMode;
	irr::gui::IGUIButton* btnTestMode;
	irr::gui::IGUIButton* btnDeckEdit;
	irr::gui::IGUIButton* btnModeExit;
	irr::gui::IGUIButton* btnCommitLogExit;
	irr::gui::IGUIStaticText* stCommitLog;
	//lan
	irr::gui::IGUIWindow* wLanWindow;
	irr::gui::IGUIEditBox* ebNickName;
	irr::gui::IGUIListBox* lstHostList;
	irr::gui::IGUIButton* btnLanRefresh;
	irr::gui::IGUIEditBox* ebJoinHost;
	irr::gui::IGUIEditBox* ebJoinPort;
	irr::gui::IGUIEditBox* ebJoinPass;
	irr::gui::IGUIButton* btnJoinHost;
	irr::gui::IGUIButton* btnJoinCancel;
	irr::gui::IGUIButton* btnCreateHost;
	//create host
	irr::gui::IGUIWindow* wCreateHost;
	irr::gui::IGUIComboBox* cbLFlist;
	irr::gui::IGUIButton* btnRelayMode;
	irr::gui::IGUIComboBox* cbMatchMode;
	irr::gui::IGUIComboBox* cbRule;
	irr::gui::IGUIEditBox* ebTimeLimit;
	irr::gui::IGUIEditBox* ebTeam1;
	irr::gui::IGUIEditBox* ebTeam2;
	irr::gui::IGUIEditBox* ebBestOf;
	irr::gui::IGUIEditBox* ebOnlineTeam1;
	irr::gui::IGUIEditBox* ebOnlineTeam2;
	irr::gui::IGUIEditBox* ebOnlineBestOf;
	irr::gui::IGUIEditBox* ebStartLP;
	irr::gui::IGUIEditBox* ebStartHand;
	irr::gui::IGUIEditBox* ebDrawCount;
	irr::gui::IGUIEditBox* ebServerName;
	irr::gui::IGUIEditBox* ebServerPass;
	irr::gui::IGUIButton* btnRuleCards;
	irr::gui::IGUIWindow* wRules;
	irr::gui::IGUICheckBox* chkRules[13];
	irr::gui::IGUIButton* btnRulesOK;
	irr::gui::IGUIComboBox* cbDuelRule;
	irr::gui::IGUIButton* btnCustomRule;
	irr::gui::IGUICheckBox* chkCustomRules[7];
	irr::gui::IGUICheckBox* chkTypeLimit[5];
	irr::gui::IGUIWindow* wCustomRules;
	irr::gui::IGUIButton* btnCustomRulesOK;
	irr::gui::IGUICheckBox* chkNoCheckDeck;
	irr::gui::IGUICheckBox* chkNoShuffleDeck;
	irr::gui::IGUIButton* btnHostConfirm;
	irr::gui::IGUIButton* btnHostCancel;
	irr::gui::IGUIStaticText* stHostPort;
	irr::gui::IGUIEditBox* ebHostPort;
	irr::gui::IGUIStaticText* stHostNotes;
	irr::gui::IGUIEditBox* ebHostNotes;
	//host panel
	irr::gui::IGUIWindow* wHostPrepare;
	irr::gui::IGUIWindow* wHostPrepare2;
	WindBotPanel gBot;
	irr::gui::IGUIStaticText* stHostCardRule;
	irr::gui::IGUIButton* btnHostPrepDuelist;
	irr::gui::IGUIButton* btnHostPrepWindBot;
	irr::gui::IGUIButton* btnHostPrepOB;
	irr::gui::IGUIStaticText* stHostPrepDuelist[6];
	irr::gui::IGUICheckBox* chkHostPrepReady[6];
	irr::gui::IGUIButton* btnHostPrepKick[6];
	irr::gui::IGUIComboBox* cbDeckSelect;
	irr::gui::IGUIComboBox* cbDeckSelect2;
	irr::gui::IGUIStaticText* stHostPrepRule;
	irr::gui::IGUIStaticText* stHostPrepRule2;
	irr::gui::IGUIStaticText* stHostPrepOB;
	irr::gui::IGUIStaticText* stDeckSelect;
	irr::gui::IGUIButton* btnHostPrepReady;
	irr::gui::IGUIButton* btnHostPrepNotReady;
	irr::gui::IGUIButton* btnHostPrepStart;
	irr::gui::IGUIButton* btnHostPrepCancel;
	//replay
	irr::gui::IGUIWindow* wReplay;
	irr::gui::CGUIFileSelectListBox* lstReplayList;
	irr::gui::IGUIStaticText* stReplayInfo;
	irr::gui::IGUICheckBox* chkYrp;
	irr::gui::IGUIButton* btnLoadReplay;
	irr::gui::IGUIButton* btnDeleteReplay;
	irr::gui::IGUIButton* btnRenameReplay;
	irr::gui::IGUIButton* btnReplayCancel;
	irr::gui::IGUIButton* btnExportDeck;
	irr::gui::IGUIEditBox* ebRepStartTurn;
	//single play
	irr::gui::IGUIWindow* wSinglePlay;
	irr::gui::CGUIFileSelectListBox* lstSinglePlayList;
	irr::gui::IGUIStaticText* stSinglePlayInfo;
	irr::gui::IGUIButton* btnLoadSinglePlay;
	irr::gui::IGUIButton* btnSinglePlayCancel;
	//hand
	irr::gui::IGUIWindow* wHand;
	irr::gui::CGUIImageButton* btnHand[3];
	//
	irr::gui::IGUIWindow* wFTSelect;
	irr::gui::IGUIButton* btnFirst;
	irr::gui::IGUIButton* btnSecond;
	//message
	irr::gui::IGUIWindow* wMessage;
	irr::gui::IGUIStaticText* stMessage;
	irr::gui::IGUIButton* btnMsgOK;
	//auto close message
	irr::gui::IGUIWindow* wACMessage;
	irr::gui::IGUIStaticText* stACMessage;
	//yes/no
	irr::gui::IGUIWindow* wQuery;
	irr::gui::IGUIStaticText* stQMessage;
	irr::gui::IGUIButton* btnYes;
	irr::gui::IGUIButton* btnNo;
	//options
	irr::gui::IGUIWindow* wOptions;
	irr::gui::IGUIStaticText* stOptions;
	irr::gui::IGUIButton* btnOptionp;
	irr::gui::IGUIButton* btnOptionn;
	irr::gui::IGUIButton* btnOptionOK;
	irr::gui::IGUIButton* btnOption[5];
	irr::gui::IGUIScrollBar *scrOption;
	//pos selection
	irr::gui::IGUIWindow* wPosSelect;
	irr::gui::CGUIImageButton* btnPSAU;
	irr::gui::CGUIImageButton* btnPSAD;
	irr::gui::CGUIImageButton* btnPSDU;
	irr::gui::CGUIImageButton* btnPSDD;
	//card selection
	irr::gui::IGUIWindow* wCardSelect;
	irr::gui::CGUIImageButton* btnCardSelect[5];
	irr::gui::IGUIStaticText *stCardPos[5];
	irr::gui::IGUIScrollBar *scrCardList;
	irr::gui::IGUIButton* btnSelectOK;
	//card display
	irr::gui::IGUIWindow* wCardDisplay;
	irr::gui::CGUIImageButton* btnCardDisplay[5];
	irr::gui::IGUIStaticText *stDisplayPos[5];
	irr::gui::IGUIScrollBar *scrDisplayList;
	irr::gui::IGUIButton* btnDisplayOK;
	//announce number
	irr::gui::IGUIWindow* wANNumber;
	irr::gui::IGUIComboBox* cbANNumber;
	irr::gui::IGUIButton* btnANNumberOK;
	//announce card
	irr::gui::IGUIWindow* wANCard;
	irr::gui::IGUIEditBox* ebANCard;
	irr::gui::IGUIListBox* lstANCard;
	irr::gui::IGUIButton* btnANCardOK;
	//announce attribute
	irr::gui::IGUIWindow* wANAttribute;
	irr::gui::IGUICheckBox* chkAttribute[7];
	//announce race
	irr::gui::IGUIWindow* wANRace;
	irr::gui::IGUICheckBox* chkRace[25];
	//cmd menu
	irr::gui::IGUIWindow* wCmdMenu;
	irr::gui::IGUIButton* btnActivate;
	irr::gui::IGUIButton* btnSummon;
	irr::gui::IGUIButton* btnSPSummon;
	irr::gui::IGUIButton* btnMSet;
	irr::gui::IGUIButton* btnSSet;
	irr::gui::IGUIButton* btnRepos;
	irr::gui::IGUIButton* btnAttack;
	irr::gui::IGUIButton* btnShowList;
	irr::gui::IGUIButton* btnOperation;
	irr::gui::IGUIButton* btnReset;
	irr::gui::IGUIButton* btnShuffle;
	//chat window
	irr::gui::IGUIWindow* wChat;
	irr::gui::IGUIListBox* lstChatLog;
	irr::gui::IGUIEditBox* ebChatInput;
	irr::gui::IGUICheckBox* chkIgnore1;
	irr::gui::IGUICheckBox* chkIgnore2;
	//phase button
	irr::gui::IGUIStaticText* wPhase;
	irr::gui::IGUIButton* btnDP;
	irr::gui::IGUIButton* btnSP;
	irr::gui::IGUIButton* btnM1;
	irr::gui::IGUIButton* btnBP;
	irr::gui::IGUIButton* btnM2;
	irr::gui::IGUIButton* btnEP;
	//deck edit
	irr::gui::IGUIStaticText* wDeckEdit;
	irr::gui::IGUIComboBox* cbDBLFList;
	irr::gui::IGUIComboBox* cbDBDecks;
	irr::gui::IGUIButton* btnHandTest;
	irr::gui::IGUIButton* btnClearDeck;
	irr::gui::IGUIButton* btnSortDeck;
	irr::gui::IGUIButton* btnShuffleDeck;
	irr::gui::IGUIButton* btnSaveDeck;
	irr::gui::IGUIButton* btnDeleteDeck;
	irr::gui::IGUIButton* btnSaveDeckAs;
	irr::gui::IGUIButton* btnRenameDeck;
	irr::gui::IGUIButton* btnSideOK;
	irr::gui::IGUIButton* btnSideShuffle;
	irr::gui::IGUIButton* btnSideSort;
	irr::gui::IGUIButton* btnSideReload;
	irr::gui::IGUIEditBox* ebDeckname;
	irr::gui::IGUIStaticText* stBanlist;
	irr::gui::IGUIStaticText* stDeck;
	irr::gui::IGUIStaticText* stCategory;
	irr::gui::IGUIStaticText* stLimit;
	irr::gui::IGUIStaticText* stAttribute;
	irr::gui::IGUIStaticText* stRace;
	irr::gui::IGUIStaticText* stAttack;
	irr::gui::IGUIStaticText* stDefense;
	irr::gui::IGUIStaticText* stStar;
	irr::gui::IGUIStaticText* stSearch;
	irr::gui::IGUIStaticText* stScale;
	//filter
	irr::gui::IGUIStaticText* wFilter;
	irr::gui::IGUIScrollBar* scrFilter;
	irr::gui::IGUIComboBox* cbCardType;
	irr::gui::IGUIComboBox* cbCardType2;
	irr::gui::IGUIComboBox* cbRace;
	irr::gui::IGUIComboBox* cbAttribute;
	irr::gui::IGUIComboBox* cbLimit;
	irr::gui::IGUIEditBox* ebStar;
	irr::gui::IGUIEditBox* ebScale;
	irr::gui::IGUIEditBox* ebAttack;
	irr::gui::IGUIEditBox* ebDefense;
	irr::gui::IGUIEditBox* ebCardName;
	irr::gui::IGUIButton* btnEffectFilter;
	irr::gui::IGUIButton* btnStartFilter;
	irr::gui::IGUIButton* btnClearFilter;
	irr::gui::IGUIWindow* wCategories;
	irr::gui::IGUICheckBox* chkCategory[32];
	irr::gui::IGUIButton* btnCategoryOK;
	irr::gui::IGUIButton* btnMarksFilter;
	irr::gui::IGUIWindow* wLinkMarks;
	irr::gui::IGUIButton* btnMark[8];
	irr::gui::IGUIButton* btnMarksOK;
	irr::gui::IGUICheckBox* chkAnime;
	//sort type
	irr::gui::IGUIStaticText* wSort;
	irr::gui::IGUIComboBox* cbSortType;
	//replay save
	irr::gui::IGUIWindow* wReplaySave;
	irr::gui::IGUIEditBox* ebRSName;
	irr::gui::IGUIButton* btnRSYes;
	irr::gui::IGUIButton* btnRSNo;
	//replay control
	irr::gui::IGUIStaticText* wReplayControl;
	irr::gui::IGUIButton* btnReplayStart;
	irr::gui::IGUIButton* btnReplayPause;
	irr::gui::IGUIButton* btnReplayStep;
	irr::gui::IGUIButton* btnReplayUndo;
	irr::gui::IGUIButton* btnReplayExit;
	irr::gui::IGUIButton* btnReplaySwap;
	//surrender/leave
	irr::gui::IGUIButton* btnLeaveGame;
	//restart
	irr::gui::IGUIButton* btnRestartSingle;
	//swap
	irr::gui::IGUIButton* btnSpectatorSwap;
	//chain control
	irr::gui::IGUIButton* btnChainIgnore;
	irr::gui::IGUIButton* btnChainAlways;
	irr::gui::IGUIButton* btnChainWhenAvail;

	//cancel or finish
	irr::gui::IGUIButton* btnCancelOrFinish;

	//server lobby
	bool isHostingOnline;
	irr::gui::IGUITable* roomListTable;
	irr::gui::IGUIStaticText* wRoomListPlaceholder;
	irr::gui::IGUIComboBox* serverChoice;
	irr::gui::IGUIEditBox* ebNickNameOnline;
	irr::gui::IGUIButton* btnCreateHost2;
	irr::gui::IGUIComboBox* cbFilterRule;
	irr::gui::IGUIComboBox* cbFilterBanlist;
	irr::gui::IGUIStaticText* ebRoomNameText;
	irr::gui::IGUIEditBox* ebRoomName;
	irr::gui::IGUICheckBox* chkShowPassword;
	irr::gui::IGUICheckBox* chkShowActiveRooms;
	irr::gui::IGUIButton* btnLanRefresh2;
	irr::gui::IGUIWindow* wRoomPassword;
	irr::gui::IGUIEditBox* ebRPName;
	irr::gui::IGUIButton* btnFilterRelayMode;
	irr::gui::IGUIButton* btnRPYes;
	irr::gui::IGUIButton* btnRPNo;
	irr::gui::IGUIButton* btnJoinHost2;
	irr::gui::IGUIButton* btnJoinCancel2;

#ifdef __ANDROID__
	ANDROID_APP appMain;
	int glversion;
	bool isPSEnabled;
	bool isNPOTSupported;
	s32 ogles2Solid;
	s32 ogles2TrasparentAlpha;
	s32 ogles2BlendTexture;
	Signal externalSignal;
	IGUIStaticText* fpsCounter;
#endif
};

extern Game* mainGame;

template<typename T>
inline vector2d<T> Game::Scale(vector2d<T> vec) {
	return vector2d<T>(vec.X * gameConf.dpi_scale, vec.Y * gameConf.dpi_scale );
}
template<typename T>
inline T Game::ResizeX(T x) {
	return Scale<T>(x * window_scale.X);
}
template<typename T>
inline T Game::ResizeY(T y) {
	return Scale<T>(y * window_scale.Y);
}
template<typename T, typename T2>
inline vector2d<T> Game::Scale(T x, T2 y) {
	return vector2d<T>((T)(x * gameConf.dpi_scale), (T)(y * gameConf.dpi_scale));
}
template<typename T>
inline T Game::Scale(T val) {
	return T(val * gameConf.dpi_scale);
}
template<typename T, typename T2, typename T3, typename T4>
rect<T> Game::Scale(T x, T2 y, T3 x2, T4 y2) {
	auto& scale = gameConf.dpi_scale;
	return { (T)std::roundf(x * scale),(T)std::roundf(y * scale), (T)std::roundf(x2 * scale), (T)std::roundf(y2 * scale) };
}
template<typename T>
rect<T> Game::Scale(rect<T> rect) {
	return Scale(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X, rect.LowerRightCorner.Y);
}

}

#define FIELD_X			4.2f	
#define FIELD_Y			8.0f
#define FIELD_Z			7.8f
#define FIELD_ANGLE		atan(FIELD_Y / FIELD_Z)

#define CAMERA_LEFT		-0.90f
#define CAMERA_RIGHT	0.45f
#define CAMERA_BOTTOM	-0.42f
#define CAMERA_TOP		0.42f

#define CARD_IMG_WIDTH		177
#define CARD_IMG_HEIGHT		254
#define CARD_THUMB_WIDTH	44
#define CARD_THUMB_HEIGHT	64

#define UEVENT_EXIT			0x1
#define UEVENT_TOWINDOW		0x2

#define COMMAND_ACTIVATE	0x0001
#define COMMAND_SUMMON		0x0002
#define COMMAND_SPSUMMON	0x0004
#define COMMAND_MSET		0x0008
#define COMMAND_SSET		0x0010
#define COMMAND_REPOS		0x0020
#define COMMAND_ATTACK		0x0040
#define COMMAND_LIST		0x0080
#define COMMAND_OPERATION	0x0100
#define COMMAND_RESET		0x0200

#define POSITION_HINT		0x8000

#define DEFAULT_DUEL_RULE			5

#define CARD_ARTWORK_VERSIONS_OFFSET	10

#define DECK_SEARCH_SCROLL_STEP		100

#endif // GAME_H
