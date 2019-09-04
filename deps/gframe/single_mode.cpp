#include "single_mode.h"
#include "duelclient.h"
#include "game.h"
#include <random>

namespace ygo {

void* SingleMode::pduel = 0;
bool SingleMode::is_closing = false;
bool SingleMode::is_continuing = false;
Replay SingleMode::last_replay;
Replay SingleMode::new_replay;
ReplayStream SingleMode::replay_stream;
Signal SingleMode::singleSignal;

bool SingleMode::StartPlay() {
	if(mainGame->dInfo.isSingleMode)
		return false;
	std::thread(SinglePlayThread).detach();
	return true;
}
void SingleMode::StopPlay(bool is_exiting) {
	is_closing = is_exiting;
	is_continuing = false;
	mainGame->actionSignal.Set();
	if(is_closing)
		singleSignal.SetNoWait(true);
	else
		singleSignal.Set();
}
void SingleMode::SetResponse(unsigned char* resp, unsigned int len) {
	if(!pduel)
		return;
	last_replay.Write<int8_t>(len);
	last_replay.WriteData(resp, len);
	set_responseb(pduel, resp, len);
}
int SingleMode::SinglePlayThread() {
	mainGame->dInfo.isSingleMode = true;
	const int start_lp = 8000;
	const int start_hand = 5;
	const int draw_count = 1;
	const int opt = 0;
	time_t seed = time(0);
	DuelClient::rnd.seed(seed);
	pduel = mainGame->SetupDuel(DuelClient::rnd());
	mainGame->dInfo.compat_mode = false;
	set_player_info(pduel, 0, start_lp, start_hand, draw_count);
	set_player_info(pduel, 1, start_lp, start_hand, draw_count);
	mainGame->dInfo.lp[0] = start_lp;
	mainGame->dInfo.lp[1] = start_lp;
	mainGame->dInfo.startlp = start_lp;
	mainGame->dInfo.strLP[0] = fmt::to_wstring(mainGame->dInfo.lp[0]);
	mainGame->dInfo.strLP[1] = fmt::to_wstring(mainGame->dInfo.lp[1]);
	mainGame->dInfo.hostname.push_back(mainGame->ebNickName->getText());
	mainGame->dInfo.clientname.push_back(L"");
	mainGame->dInfo.player_type = 0;
	mainGame->dInfo.turn = 0;
	bool loaded = true;
	std::string script_name = "";
	if(open_file) {
		open_file = false;
		script_name = Utils::ToUTF8IfNeeded(open_file_name);
		if(!preload_script(pduel, (char*)script_name.c_str(), 0, 0, nullptr)) {
			script_name = Utils::ToUTF8IfNeeded(TEXT("./puzzles/") + open_file_name);
			if(!preload_script(pduel, (char*)script_name.c_str(), 0, 0, nullptr))
				loaded = false;
		}
	} else {
		script_name = BufferIO::EncodeUTF8s(mainGame->lstSinglePlayList->getListItem(mainGame->lstSinglePlayList->getSelected(), true));
		if(!preload_script(pduel, (char*)script_name.c_str(), 0, 0, nullptr))
			loaded = false;
	}
	if(!loaded) {
		end_duel(pduel);
		mainGame->dInfo.isSingleMode = false;
		return 0;
	}
	ReplayHeader rh;
	rh.id = REPLAY_YRP1;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_SINGLE_MODE + REPLAY_LUA64;
	rh.seed = seed;
	mainGame->gMutex.lock();
	mainGame->HideElement(mainGame->wSinglePlay);
	mainGame->ClearCardInfo();
	mainGame->mTopMenu->setVisible(false);
	mainGame->wCardImg->setVisible(true);
	mainGame->wInfos->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(dataManager.GetSysString(1210).c_str());
	mainGame->wPhase->setVisible(true);
	mainGame->dField.Clear();
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isInDuel = true;
	mainGame->dInfo.isStarted = true;
	mainGame->dInfo.isCatchingUp = false;
	mainGame->SetMesageWindow();
	mainGame->device->setEventReceiver(&mainGame->dField);
	mainGame->gMutex.unlock();
	std::vector<uint8> duelBuffer;
	is_closing = false;
	is_continuing = true;
	last_replay.BeginRecord(false);
	last_replay.WriteHeader(rh);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.id = REPLAY_YRPX;
	rh.flag |= REPLAY_NEWREPLAY;
	new_replay.WriteHeader(rh);
	replay_stream.clear();
	unsigned short buffer[20];
	BufferIO::CopyWStr(mainGame->dInfo.hostname[0].c_str(), buffer, 20);
	last_replay.WriteData(buffer, 40, false);
	new_replay.WriteData(buffer, 40, false);
	BufferIO::CopyWStr(mainGame->dInfo.clientname[0].c_str(), buffer, 20);
	last_replay.WriteData(buffer, 40, false);
	new_replay.WriteData(buffer, 40, false);
	last_replay.Write<int32_t>(start_lp, false);
	last_replay.Write<int32_t>(start_hand, false);
	last_replay.Write<int32_t>(draw_count, false);
	last_replay.Write<int32_t>(opt, false);
	last_replay.Write<int16_t>(script_name.size(), false);
	last_replay.WriteData(script_name.c_str(), script_name.size(), false);
	last_replay.Flush();
	new_replay.Write<int32_t>((mainGame->GetMasterRule(opt, 0)) | (opt & SPEED_DUEL) << 8);
	int len = get_message(pduel, nullptr);
	if (len > 0){
		duelBuffer.resize(len);
		get_message(pduel, duelBuffer.data());
		is_continuing = SinglePlayAnalyze((char*)duelBuffer.data(), len);
	}
	start_duel(pduel, opt);
	while (is_continuing) {
		/*int flag = */process(pduel);
		len = get_message(pduel, nullptr);
		/* int flag = result >> 16; */
		if (len > 0) {
			duelBuffer.resize(len);
			get_message(pduel, duelBuffer.data());
			is_continuing = SinglePlayAnalyze((char*)duelBuffer.data(), len);
		}
	}
	last_replay.EndRecord(0x1000);
	std::vector<unsigned char> oldreplay;
	oldreplay.insert(oldreplay.end(), (unsigned char*)&last_replay.pheader, ((unsigned char*)&last_replay.pheader) + sizeof(ReplayHeader));
	oldreplay.insert(oldreplay.end(), last_replay.comp_data.begin(), last_replay.comp_data.end());
	new_replay.WritePacket(ReplayPacket(OLD_REPLAY_MODE, (char*)oldreplay.data(), oldreplay.size()));

	new_replay.EndRecord();
	if(is_closing)
		return 0;
	end_duel(pduel);
	time_t nowtime = time(NULL);
	tm* localedtime = localtime(&nowtime);
	wchar_t timetext[40];
	wcsftime(timetext, 40, L"%Y-%m-%d %H-%M-%S", localedtime);
	mainGame->gMutex.lock();
	mainGame->ebRSName->setText(timetext);
	mainGame->wReplaySave->setText(dataManager.GetSysString(1340).c_str());
	mainGame->PopupElement(mainGame->wReplaySave);
	mainGame->gMutex.unlock();
	mainGame->replaySignal.Reset();
	mainGame->replaySignal.Wait();
	if(mainGame->saveReplay)
		new_replay.SaveReplay(Utils::ParseFilename(mainGame->ebRSName->getText()));
	new_replay.Reset();
	last_replay.Reset();
	mainGame->gMutex.lock();
	mainGame->dField.Clear();
	mainGame->gMutex.unlock();
	if(!is_closing) {
		mainGame->gMutex.lock();
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isSingleMode = false;
		mainGame->gMutex.unlock();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.lock();
		mainGame->closeDoneSignal.Wait();
		mainGame->closeSignal.unlock();
		mainGame->gMutex.lock();
		mainGame->ShowElement(mainGame->wSinglePlay);
		mainGame->stTip->setVisible(false);
		mainGame->SetMesageWindow();
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->gMutex.unlock();
		if(exit_on_return)
			mainGame->device->closeDevice();
	}
	return 0;
}
bool SingleMode::SinglePlayAnalyze(char* msg, unsigned int len) {
	char* offset, *pbuf = msg;
	int player, count;
	while (pbuf - msg < (int)len) {
		replay_stream.clear();
		bool record = true;
		if(is_closing || !is_continuing)
			return false;
		offset = pbuf;
		mainGame->dInfo.curMsg = BufferIO::Read<uint8_t>(pbuf);
		ReplayPacket p(mainGame->dInfo.curMsg, pbuf, len - 1);
		switch (mainGame->dInfo.curMsg) {
		case MSG_RETRY: {
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(L"Error occurs.");
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			return false;
		}
		case MSG_HINT: {
			int type = BufferIO::Read<uint8_t>(pbuf);
			int player = BufferIO::Read<uint8_t>(pbuf);
			/*int data = */BufferIO::Read<int64_t>(pbuf);
			if(player == 0)
				DuelClient::ClientAnalyze(offset, pbuf - offset);
			if (type > 0 && type < 6 && type != 4)
				record = false;
			break;
		}
		case MSG_WIN: {
			pbuf += 2;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			replay_stream.push_back(p);
			return false;
		}
		case MSG_SELECT_BATTLECMD: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 18;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 8 + 2;
			SinglePlayRefresh();
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_IDLECMD: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 7;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 18 + 3;
			SinglePlayRefresh();
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_EFFECTYN: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 22;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 8;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<uint8_t>(pbuf);
			pbuf += count * 8;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_CARD: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 9;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 14;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 9;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 11;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_UNSELECT_CARD: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 10;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 14;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 14;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += 10 + count * 23;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 4;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 9;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SELECT_SUM: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 13;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 14;
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 14;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 13;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::Read<uint8_t>(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh(player, LOCATION_DECK, 0x181fff);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			/*int oplayer = */BufferIO::Read<uint8_t>(pbuf);
			int count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::Read<uint8_t>(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh(player, LOCATION_GRAVE, 0x181fff);
			break;
		}
		case MSG_REVERSE_DECK: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh(0, LOCATION_DECK, 0x181fff);
			SinglePlayRefresh(1, LOCATION_DECK, 0x181fff);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			pbuf++;
			count = BufferIO::Read<uint8_t>(pbuf);
			pbuf += count * 20;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_NEW_TURN: {
			player = BufferIO::Read<uint8_t>(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_MOVE: {
			pbuf += 4;
			loc_info previous = ClientCard::read_location_info(pbuf);
			loc_info current = ClientCard::read_location_info(pbuf);
			pbuf += 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			if(previous.location && !(current.location & 0x80) && (previous.location != current.location || previous.controler != current.controler))
				SinglePlayRefreshSingle(current.controler, current.location, current.sequence);
			break;
		}
		case MSG_POS_CHANGE: {
			pbuf += 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SET: {
			pbuf += 14;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SWAP: {
			pbuf += 28;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 14;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 14;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_FLIPSUMMONING: {
			pbuf += 14;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_FLIPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_CHAINING: {
			pbuf += 32;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_CHAIN_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			SinglePlayRefresh(0, LOCATION_DECK, 0x181fff);
			SinglePlayRefresh(1, LOCATION_DECK, 0x181fff);
			break;
		}
		case MSG_CHAIN_NEGATED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CARD_SELECTED:
		case MSG_RANDOM_SELECTED: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 20;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 20;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 20;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 20;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 38;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK_DISABLED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_MISSED_EFFECT: {
			pbuf += 14;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<uint8_t>(pbuf);
			pbuf += count;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<uint8_t>(pbuf);
			pbuf += count;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::Read<uint8_t>(pbuf);
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_ANNOUNCE_CARD:
		case MSG_ANNOUNCE_NUMBER: {
			player = BufferIO::Read<uint8_t>(pbuf);
			count = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 8 * count;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			record = false;
			break;
		}
		case MSG_CARD_HINT: {
			pbuf += 19;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_TAG_SWAP: {
			player = BufferIO::Read<uint8_t>(pbuf);
			/*int mcount = */BufferIO::Read<int32_t>(pbuf);
			int ecount = BufferIO::Read<int32_t>(pbuf);
			/*int pcount = */BufferIO::Read<int32_t>(pbuf);
			int hcount = BufferIO::Read<int32_t>(pbuf);
			pbuf += ecount * 4 + hcount * 4 + 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh(player, LOCATION_DECK, 0x181fff);
			SinglePlayRefresh(player, LOCATION_EXTRA, 0x181fff);
			break;
		}
		case MSG_MATCH_KILL: {
			pbuf += 4;
			break;
		}
		case MSG_REMOVE_CARDS: {
			int count = BufferIO::Read<int32_t>(pbuf);
			pbuf += count * 10;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_RELOAD_FIELD: {
			pbuf++;
			for(int p = 0; p < 2; ++p) {
				pbuf += 4;
				for(int seq = 0; seq < 7; ++seq) {
					int val = BufferIO::Read<uint8_t>(pbuf);
					if(val)
						pbuf += 5;
				}
				for(int seq = 0; seq < 8; ++seq) {
					int val = BufferIO::Read<uint8_t>(pbuf);
					if(val)
						pbuf++;
				}
				pbuf += 24;
			}
			int count = BufferIO::Read<int32_t>(pbuf);
			pbuf += 20 * count;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayReload();
			mainGame->gMutex.lock();
			mainGame->dField.RefreshAllCards();
			mainGame->gMutex.unlock();
			break;
		}
		case MSG_AI_NAME: {
			int len = BufferIO::Read<uint16_t>(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			std::string namebuf;
			namebuf.resize(len);
			memcpy(&namebuf[0], begin, len + 1);
			mainGame->dInfo.clientname[0] = BufferIO::DecodeUTF8s(namebuf);
			break;
		}
		case MSG_SHOW_HINT: {
			char msgbuf[1024];
			wchar_t msg[1024];
			int len = BufferIO::Read<uint16_t>(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			memcpy(msgbuf, begin, len + 1);
			BufferIO::DecodeUTF8(msgbuf, msg);
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(msg);
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			break;
		}
		}
		//setting the length again in case of multiple messages in a row,
		//when the packet will be written in the replay, the extra data registered previously will be discarded
		p.data.resize((pbuf - offset) - 1);
		if (record)
			replay_stream.insert(replay_stream.begin(), p);
		new_replay.WriteStream(replay_stream);
		new_replay.Flush();
		replay_stream.clear();
	}
	return is_continuing;
}
void SingleMode::SinglePlayRefresh(int player, int location, int flag) {
	std::vector<uint8_t> buffer;
	int len = query_field_card(pduel, player, location, flag, nullptr, FALSE, TRUE);
	buffer.resize(len);
	get_cached_query(pduel, buffer.data());
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), location, (char*)buffer.data());
	buffer.insert(buffer.begin(), location);
	buffer.insert(buffer.begin(), player);
	replay_stream.push_back(ReplayPacket(MSG_UPDATE_DATA, (char*)buffer.data(), buffer.size()));
}
void SingleMode::SinglePlayRefreshSingle(int player, int location, int sequence, int flag) {
	std::vector<uint8_t> buffer;
	int len = query_card(pduel, player, location, sequence, flag, nullptr, FALSE, TRUE);
	buffer.resize(len);
	get_cached_query(pduel, buffer.data());
	mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)buffer.data());
	buffer.insert(buffer.begin(), sequence);
	buffer.insert(buffer.begin(), location);
	buffer.insert(buffer.begin(), player);
	ReplayPacket p(MSG_UPDATE_CARD, (char*)buffer.data(), buffer.size());
	replay_stream.push_back(p);
}
void SingleMode::SinglePlayRefresh(int flag) {
	for(int p = 0; p < 2; p++)
		for(int loc = LOCATION_HAND; loc != LOCATION_GRAVE; loc *= 2)
			SinglePlayRefresh(p, loc, flag);
}
void SingleMode::SinglePlayReload() {
	for(int p = 0; p < 2; p++)
		for(int loc = LOCATION_DECK; loc != LOCATION_OVERLAY; loc *= 2)
			SinglePlayRefresh(p, loc, 0xffdfff);
}

}
