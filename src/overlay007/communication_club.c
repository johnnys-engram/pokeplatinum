#include "overlay007/communication_club.h"

#include <nitro.h>
#include <string.h>

#include "constants/communication/comm_type.h"
#include "generated/comm_club_ret_codes.h"

#include "field/field_system.h"

#include "bg_window.h"
#include "comm_manager.h"
#include "communication_information.h"
#include "communication_system.h"
#include "field_comm_manager.h"
#include "field_message.h"
#include "field_system.h"
#include "heap.h"
#include "list_menu.h"
#include "menu.h"
#include "message.h"
#include "render_text.h"
#include "render_window.h"
#include "save_player.h"
#include "sound_playback.h"
#include "string_gf.h"
#include "string_list.h"
#include "string_template.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "system.h"
#include "text.h"
#include "trainer_info.h"
#include "unk_02033200.h"
#include "comm_tool.h"

#include "res/text/bank/unk_0353.h"

typedef struct CommClubManager CommClubManager;
typedef void (*CommClubManTaskFunc)(SysTask *, void *);
typedef void (*UnkFuncPtr_ov7_02249C2C_1)(CommClubManager *);

typedef struct CommClubManager {
    String *stringBuffer[8];
    Window unk_20;
    Window unk_30;
    Window msgWindow;
    StringTemplate *unk_50;
    StringTemplate *unk_54;
    StringTemplate *strTempMsg;
    ListMenu *unk_5C;
    Menu *unk_60;
    StringList *unk_64;
    FieldSystem *fieldSystem;
    CommClubManTaskFunc commClubTask;
    UnkFuncPtr_ov7_02249C2C_1 unk_70;
    MessageLoader *msgLoader;
    TrainerInfo *trainerInfoPersonal;
    TrainerInfo *unk_7C;
    u8 unk_80[8];
    u32 retCode;
    u16 unk_8C;
    u16 connectIndex;
    u8 connectedCnt;
    u8 commType;
    u8 unk_92;
    u8 unk_93;
    u8 printMsgIndex;
    u8 unk_95;
    u8 msgDelay;
    u8 unk_97;
    u8 unk_98;
} CommClubManager;

static void CommClubMan_Run(SysTask *task, void *param1);
static void CommClubMan_SetTask(CommClubManTaskFunc param0);
static void CommClubMan_PrintMessage(int param0, BOOL param1);
static void CommClubMan_StartBattleClient(CommClubManager *param0);
static void CommClubMan_StartBattleServer(CommClubManager *param0);
static void CommClubMan_OnListCursorMove(ListMenu *param0, u32 param1, u8 param2);
static void CommClubMan_RedrawAllListItems(ListMenu *param0, u32 param1, u8 param2);
static void CommClubMan_DrawListItem(ListMenu *param0, u32 param1, u8 param2);
static void CommClubMan_PrintChooseJoinMsg(CommClubManager *param0);
static void CommClubMan_DisplayPersonalTrainerInfo(CommClubManager *param0);
static void CommClubTask_ClientBuildServerList(SysTask *param0, void *param1);
static void CommClubTask_ClientSelectServer(SysTask *param0, void *param1);
static void CommClubMan_RefreshServerList(CommClubManager *param0);
static void CommClubTask_SelectServerList(SysTask *param0, void *param1);
static void CommClubTask_ClientWaitHostResponse(SysTask *param0, void *param1);
static void CommClubTask_WaitConfirmLeaveGroup(SysTask *param0, void *param1);
static void CommClubTask_LeaveGroup(SysTask *param0, void *param1);
static void CommClubMan_Disconnect(void);
static void CommClubMan_StartRetailHostAwait(CommClubManager *param0);
static void CommClubMan_PrintAwaitingContactMsg(void);
static void CommClubMan_UpdatePlayerCountDisplay(CommClubManager *param0);
static void CommClubMan_DrawServerListItem(ListMenu *param0, u32 param1, u8 param2);
static void CommClubTask_ServerBuildList(SysTask *param0, void *param1);
static void CommClubTask_ServerPollPlayers(SysTask *param0, void *param1);
static void CommClubTask_ServerRespondToContact(SysTask *param0, void *param1);
static void CommClubTask_ServerPlayerLeft(SysTask *param0, void *param1);
static void CommClubTask_ServerWaitMsgThenPoll(SysTask *param0, void *param1);
static void CommClubTask_ServerPrintGroupConfirm(SysTask *param0, void *param1);
static void CommClubTask_ServerGroupConfirmWaitPrint(SysTask *param0, void *param1);
static void CommClubTask_ServerGroupConfirmInput(SysTask *param0, void *param1);
static void CommClubTask_ServerAskCancelGroup(SysTask *param0, void *param1);
static void CommClubTask_ServerGroupDropout(SysTask *param0, void *param1);
static void CommClubTask_ServerDropoutWaitButton(SysTask *param0, void *param1);
static void CommClubTask_ServerCancelPromptInput(SysTask *param0, void *param1);
static void CommClubTask_ServerCancelPromptWaitPrint(SysTask *param0, void *param1);
static void CommClubTask_ServerSyncWait(SysTask *param0, void *param1);
static void CommClubTask_ServerContactPrompt(SysTask *param0, void *param1);
static void CommClubTask_DifferentRegulation(SysTask *param0, void *param1);
static void CommClubMan_DestroyList(SysTask *param0, CommClubManager *param1);
static void CommClubMan_PrintPlayerContactMsg(int param0, CommClubManager *param1);
static void CommClubMan_ServerAcceptedDual(CommClubManager *param0);
static void CommClubMan_ClientAccepted(CommClubManager *param0);
static void CommClubTask_WaitAcceptMsgThenSync(SysTask *param0, void *param1);
static void CommClubTask_WaitForGroup(SysTask *param0, void *param1);
static void CommClubTask_ExitGuestRoom(SysTask *param0, void *param1);
static void CommClubTask_ExitGuestRoomEnd(SysTask *param0, void *param1);
static void CommClubMan_PlayerRefused(CommClubManager *param0);
static void CommClubTask_ClientRefusedWaitMsg(SysTask *param0, void *param1);
static void CommClubMan_ReconnectAndClearPlayers(CommClubManager *param0);
static void CommClubTask_WaitMsgThenRejoinList(SysTask *param0, void *param1);
static void CommClubMan_FinishSuccess(CommClubManager *param0);
static int CommClubMan_MaxPlayers(void);
static int CommClubMan_MinPlayers(void);
static int CommClubMan_Regulation(void);

static CommClubManager *sCommClubMan = NULL;

// WindowTemplate for yes/no choice box
static const WindowTemplate sYesNoWindowTemplate = {
    0x3,
    0x19,
    0xD,
    0x6,
    0x4,
    0xD,
    0x21F
};

static void CommClubMan_PrintMessage(int msgId, BOOL format)
{
    if (!FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        Text_RemovePrinter(sCommClubMan->printMsgIndex);
    }

    if (format) {
        MessageLoader_GetString(sCommClubMan->msgLoader, msgId, sCommClubMan->stringBuffer[4]);
        StringTemplate_Format(sCommClubMan->strTempMsg, sCommClubMan->stringBuffer[5], sCommClubMan->stringBuffer[4]);
    } else {
        MessageLoader_GetString(sCommClubMan->msgLoader, msgId, sCommClubMan->stringBuffer[5]);
    }

    if (!Window_IsInUse(&sCommClubMan->msgWindow)) {
        FieldMessage_AddWindow(sCommClubMan->fieldSystem->bgConfig, &sCommClubMan->msgWindow, 3);
    }

    FieldMessage_DrawWindow(&sCommClubMan->msgWindow, SaveData_GetOptions(sCommClubMan->fieldSystem->saveData));
    sCommClubMan->printMsgIndex = FieldMessage_Print(&sCommClubMan->msgWindow, sCommClubMan->stringBuffer[5], SaveData_GetOptions(sCommClubMan->fieldSystem->saveData), 1);
}

static inline void CommClubMan_PrintMessageFastSpeed(int msgId, BOOL format)
{
    if (!FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        Text_RemovePrinter(sCommClubMan->printMsgIndex);
    }

    if (format) {
        MessageLoader_GetString(sCommClubMan->msgLoader, msgId, sCommClubMan->stringBuffer[4]);
        StringTemplate_Format(sCommClubMan->strTempMsg, sCommClubMan->stringBuffer[5], sCommClubMan->stringBuffer[4]);
    } else {
        MessageLoader_GetString(sCommClubMan->msgLoader, msgId, sCommClubMan->stringBuffer[5]);
    }

    if (!Window_IsInUse(&sCommClubMan->msgWindow)) {
        FieldMessage_AddWindow(sCommClubMan->fieldSystem->bgConfig, &sCommClubMan->msgWindow, 3);
    }

    FieldMessage_DrawWindow(&sCommClubMan->msgWindow, SaveData_GetOptions(sCommClubMan->fieldSystem->saveData));
    RenderControlFlags_SetCanABSpeedUpPrint(TRUE);
    RenderControlFlags_SetAutoScrollFlags(AUTO_SCROLL_DISABLED);
    RenderControlFlags_SetSpeedUpOnTouch(FALSE);
    sCommClubMan->printMsgIndex = Text_AddPrinterWithParams(&sCommClubMan->msgWindow, FONT_MESSAGE, sCommClubMan->stringBuffer[5], 0, 0, TEXT_SPEED_FAST, NULL);
}

static void CommClubMan_CreateList(ListMenuTemplate param0, u8 param1, u8 param2, u8 param3, u8 param4, u16 param5)
{
    if (!Window_IsInUse(&sCommClubMan->unk_20)) {
        Window_Add(sCommClubMan->fieldSystem->bgConfig, &sCommClubMan->unk_20, 3, param1, param2, param3, param4, 13, param5);
    }

    Window_DrawStandardFrame(&sCommClubMan->unk_20, 1, 1024 - (18 + 12) - 9, 11);

    ListMenuTemplate v0 = param0;
    v0.choices = sCommClubMan->unk_64;
    v0.window = &sCommClubMan->unk_20;

    sCommClubMan->unk_5C = ListMenu_New(&v0, 0, 0, HEAP_ID_FIELD1);
    Window_CopyToVRAM(&sCommClubMan->unk_20);
}

static void CommClubMan_Init(FieldSystem *fieldSystem)
{
    int v0;

    GF_ASSERT(sCommClubMan == NULL);

    sCommClubMan = Heap_Alloc(HEAP_ID_FIELD1, sizeof(CommClubManager));
    MI_CpuFill8(sCommClubMan, 0, sizeof(CommClubManager));

    sCommClubMan->retCode = COMM_CLUB_RET_0;
    sCommClubMan->fieldSystem = fieldSystem;
    sCommClubMan->unk_97 = 0;
    sCommClubMan->msgLoader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_UNK_0353, HEAP_ID_FIELD1);
    sCommClubMan->trainerInfoPersonal = SaveData_GetTrainerInfo(FieldSystem_GetSaveData(sCommClubMan->fieldSystem));
    sCommClubMan->unk_7C = TrainerInfo_New(HEAP_ID_FIELD1);

    Window_Init(&sCommClubMan->unk_20);
    Window_Init(&sCommClubMan->unk_30);
    Window_Init(&sCommClubMan->msgWindow);

    sCommClubMan->unk_50 = StringTemplate_Default(HEAP_ID_FIELD1);
    sCommClubMan->unk_54 = StringTemplate_Default(HEAP_ID_FIELD1);
    sCommClubMan->strTempMsg = StringTemplate_Default(HEAP_ID_FIELD1);
    sCommClubMan->unk_98 = 0;

    for (v0 = 0; v0 < (7 + 1); v0++) {
        sCommClubMan->unk_80[v0] = 0;
    }

    for (v0 = 0; v0 < 8; v0++) {
        sCommClubMan->stringBuffer[v0] = String_Init((70 * 2), HEAP_ID_FIELD1);
    }
}

static void CommClubMan_Delete(void)
{
    for (int i = 0; i < 8; i++) {
        String_Free(sCommClubMan->stringBuffer[i]);
    }

    Heap_Free(sCommClubMan->unk_7C);

    if (sCommClubMan->unk_50) {
        StringTemplate_Free(sCommClubMan->unk_50);
        sCommClubMan->unk_50 = NULL;
    }

    if (sCommClubMan->unk_54) {
        StringTemplate_Free(sCommClubMan->unk_54);
        sCommClubMan->unk_54 = NULL;
    }

    if (sCommClubMan->strTempMsg) {
        StringTemplate_Free(sCommClubMan->strTempMsg);
        sCommClubMan->strTempMsg = NULL;
    }

    MessageLoader_Free(sCommClubMan->msgLoader);

    if (sCommClubMan->msgWindow.pixels != NULL) {
        Window_Remove(&sCommClubMan->msgWindow);
    }

    Heap_Free(sCommClubMan);
    sCommClubMan = NULL;
}

static void CommClubMan_StartBattleClient(CommClubManager *man)
{
    FieldCommManager_StartBattleClient(man->fieldSystem, man->commType, CommClubMan_Regulation());
}

// ListMenuTemplate for client server-select list
static const ListMenuTemplate sClientListMenuTemplate = {
    NULL,
    CommClubMan_OnListCursorMove,
    CommClubMan_RedrawAllListItems,
    NULL,
    0x10,
    0x5,
    0x0,
    0x8,
    0x0,
    0x0,
    0x1,
    0xF,
    0x2,
    0x0,
    0x10,
    0x1,
    0x0,
    0x0,
    NULL
};

// List callback: plays confirm SE on cursor move
static void CommClubMan_OnListCursorMove(ListMenu *param0, u32 param1, u8 param2)
{
    sCommClubMan->unk_98 = 1;

    if (param2 == 0) {
        Sound_PlayEffect(SEQ_SE_CONFIRM);
    }
}

// Redraws all visible list entries
static void CommClubMan_RedrawAllListItems(ListMenu *param0, u32 param1, u8 param2)
{
    for (int v0 = 0; v0 < ListMenu_GetAttribute(param0, 3); v0++) {
        CommClubMan_DrawListItem(param0, 0, v0);
    }
}

// Draws one list item (player number + name + trainer ID)
static void CommClubMan_DrawListItem(ListMenu *param0, u32 param1, u8 param2)
{
    int v0 = sub_02033808();
    u16 cnt = 0;

    if (sCommClubMan->unk_5C) {
        ListMenu_GetListAndCursorPos(sCommClubMan->unk_5C, &cnt, NULL);
    }

    cnt += param2;

    Window_FillRectWithColor(&sCommClubMan->unk_20, 15, 8, param2 * 16, Window_GetWidth(&sCommClubMan->unk_20) * 8 - 8, 16);

    if (cnt < v0) {
        sub_020339AC(cnt, sCommClubMan->unk_7C);

        StringTemplate_SetNumber(sCommClubMan->unk_50, 0, cnt + 1, 2, 2, 1);
        StringTemplate_SetPlayerName(sCommClubMan->unk_50, 1, sCommClubMan->unk_7C);
        MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00063, sCommClubMan->stringBuffer[1]); // "{num}:{player name}"

        StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[0], sCommClubMan->stringBuffer[1]);
        Text_AddPrinterWithParams(&sCommClubMan->unk_20, FONT_SYSTEM, sCommClubMan->stringBuffer[0], 8, param2 * 16, TEXT_SPEED_NO_TRANSFER, NULL);

        StringTemplate_SetNumber(sCommClubMan->unk_50, 2, TrainerInfo_ID(sCommClubMan->unk_7C) % 0x10000, 5, 2, 1);
        MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00065, sCommClubMan->stringBuffer[3]); // "ID{id}"

        StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[2], sCommClubMan->stringBuffer[3]);
        Text_AddPrinterWithParams(&sCommClubMan->unk_20, FONT_SYSTEM, sCommClubMan->stringBuffer[2], 11 * 8, param2 * 16, TEXT_SPEED_NO_TRANSFER, NULL);
    } else {
        StringTemplate_SetNumber(sCommClubMan->unk_50, 0, cnt + 1, 2, 2, 1);
        MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00064, sCommClubMan->stringBuffer[1]); // "{num}:"

        StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[0], sCommClubMan->stringBuffer[1]);
        Text_AddPrinterWithParams(&sCommClubMan->unk_20, FONT_SYSTEM, sCommClubMan->stringBuffer[0], 8, param2 * 16, TEXT_SPEED_NO_TRANSFER, NULL);
    }
}

static void CommClubMan_PrintChooseJoinMsg(CommClubManager *param0)
{
    if (!Window_IsInUse(&sCommClubMan->unk_30)) {
        Window_Add(sCommClubMan->fieldSystem->bgConfig, &sCommClubMan->unk_30, 3, 23, 2, 8, 4, 13, (1 + 20 * 5 * 2));
    }

    Window_DrawStandardFrame(&sCommClubMan->unk_30, 1, 1024 - (18 + 12) - 9, 11);
    Window_FillTilemap(&sCommClubMan->unk_30, 15);
    Window_CopyToVRAM(&sCommClubMan->unk_30);

    param0->unk_97 = 1;

    SysTask_Start(CommClubMan_Run, param0, 0);
    CommClubMan_DisplayPersonalTrainerInfo(sCommClubMan);

    int sChooseJoinMsg[] = {
        pl_msg_00000353_00040, // Choose the friend you want to trade Pokemon with.
        pl_msg_00000353_00041, // Choose the Single Battle you would like to join.
        pl_msg_00000353_00042, // Choose the Double Battle you would like to join.
        pl_msg_00000353_00044, // Choose the Mix Battle you would like to join
        pl_msg_00000353_00043, // Choose the Multi Battle you would like to join
        pl_msg_00000353_00043,
        pl_msg_00000353_00045, // Choose the friend(s) you want to cook with.
        pl_msg_00000353_00046, // Chose the friend(s) you want to mix records with
        pl_msg_00000353_00047, // Choose the friends you want to hold a Contest with.
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00048, // Choose the friend you want to take to the Battle Tower.
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00049, // Choose the friend you want to play with at the Battle Factory--Level 50
        pl_msg_00000353_00050, // Choose the friend you want to play with at the Battle Factory--Open Level
        pl_msg_00000353_00000,
        pl_msg_00000353_00051, // Choose the friend you want to play with at the Battle Hall.
        pl_msg_00000353_00052, // Choose the friend you want to play with at the Battle Castle.
        pl_msg_00000353_00053 // Chose the friend you want to play with at the Battle Factory
    };

    if (sCommClubMan->commType == COMM_TYPE_CONTEST) {
        //{contestt type} Contest! Please choose the leader.
        CommClubMan_PrintMessage(pl_msg_00000353_00103 + sCommClubMan->unk_92, FALSE);
    } else {
        CommClubMan_PrintMessage(sChooseJoinMsg[sCommClubMan->commType], FALSE);
    }

    CommClubMan_SetTask(CommClubTask_ClientBuildServerList);
}

static void CommClubMan_DisplayPersonalTrainerInfo(CommClubManager *param0)
{
    StringTemplate_SetPlayerName(sCommClubMan->unk_54, 0, sCommClubMan->trainerInfoPersonal);
    StringTemplate_SetNumber(sCommClubMan->unk_54, 1, TrainerInfo_ID(sCommClubMan->trainerInfoPersonal) % 0x10000, 5, 2, 1);
    MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00062, sCommClubMan->stringBuffer[6]); // "{name}\nID{id}"
    StringTemplate_Format(sCommClubMan->unk_54, sCommClubMan->stringBuffer[7], sCommClubMan->stringBuffer[6]);
    Text_AddPrinterWithParams(&sCommClubMan->unk_30, FONT_SYSTEM, sCommClubMan->stringBuffer[7], 2, 2, TEXT_SPEED_INSTANT, NULL);
}

// Waits for print, builds server StringList with numbered slots
static void CommClubTask_ClientBuildServerList(SysTask *task, void *data)
{
    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        int v2;

        sCommClubMan->unk_64 = StringList_New(16, HEAP_ID_FIELD1);

        for (v2 = 0; v2 < 16; v2++) {
            StringTemplate_SetNumber(sCommClubMan->unk_50, 0, v2 + 1, 2, 2, 1);
            MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00064, sCommClubMan->stringBuffer[1]); // "{num}:"
            StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[0], sCommClubMan->stringBuffer[1]);
            StringList_AddFromString(sCommClubMan->unk_64, sCommClubMan->stringBuffer[0], 0);
        }

        CommClubMan_CreateList(sClientListMenuTemplate, 1, 2, 20, 5 * 2, 1);
        CommClubMan_SetTask(CommClubTask_ClientSelectServer);
    }
}

// Processes list input / open-join autopick for server selection
static void CommClubTask_ClientSelectServer(SysTask *task, void *param1)
{
    u32 v0;
    int v1;
    CommClubManager *commClubMan = (CommClubManager *)param1;

    CommClubMan_DisplayPersonalTrainerInfo(commClubMan);

    if (CommSys_CheckError()) {
        sCommClubMan->retCode = COMM_CLUB_RET_ERROR;
        CommClubMan_Disconnect();
        CommClubMan_DestroyList(task, commClubMan);
    } else {
        if (!sub_02033870()) {
            v0 = ListMenu_ProcessInput(sCommClubMan->unk_5C);
        } else {
            v0 = 0xffffffff;
        }

        switch (v0) {
        case 0xffffffff:
            CommClubMan_RefreshServerList(commClubMan);
            break;
        case 0xfffffffe:
            Sound_PlayEffect(SEQ_SE_CONFIRM);
            sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
            CommClubMan_Disconnect();
            CommClubMan_DestroyList(task, commClubMan);
            break;
        default:
            Sound_PlayEffect(SEQ_SE_CONFIRM);
            ListMenu_CalcTrueCursorPos(commClubMan->unk_5C, &commClubMan->connectIndex);

            if (sub_02033808() > commClubMan->connectIndex) {
                CommClubMan_RefreshServerList(commClubMan);
                commClubMan->connectIndex = sub_0203383C(commClubMan->connectIndex);
                FieldCommManager_ConnectBattleClient(commClubMan->connectIndex);
                CommList_Refresh();
                CommClubMan_SetTask(CommClubTask_SelectServerList);
            }
            break;
        }
    }
}

// Refreshes server list window when BSS data changes
static void CommClubMan_RefreshServerList(CommClubManager *commClubMan)
{
    if (sub_02033870() || sCommClubMan->unk_98) {
        sCommClubMan->unk_98 = 0;
        sub_02033884();
        Window_FillRectWithColor(&sCommClubMan->unk_20, 15, 8, 0, 20 - 8, (5 * 2) * 8);
        CommClubMan_RedrawAllListItems(sCommClubMan->unk_5C, 0, 0);
        Window_CopyToVRAM(&sCommClubMan->unk_20);
    }
}

static void CommClubTask_SelectServerList(SysTask *task, void *param1)
{
    CommClubManager *v0 = (CommClubManager *)param1;

    if (CommSys_CheckError()) {
        CommClubMan_DestroyList(task, v0);
        CommClubMan_PrintMessage(0, FALSE);
        CommClubMan_ReconnectAndClearPlayers(v0);
        return;
    }

    if (CommInfo_TrainerInfo(CommSys_CurNetId()) == NULL) {
        return;
    }

    CommList_Refresh();

    sub_020339AC(v0->connectIndex, sCommClubMan->unk_7C);
    StringTemplate_SetPlayerName(v0->strTempMsg, 1, sCommClubMan->unk_7C);
    if (CommClubMan_MinPlayers() <= 2) {
        CommClubMan_PrintMessage(1, TRUE);
    } else {
        CommClubMan_PrintMessageFastSpeed(2, TRUE);
    }
    CommClubMan_SetTask(CommClubTask_ClientWaitHostResponse);
}

// Checks CommList for accept/reject/error stamps from host
static BOOL CommClubMan_CheckHostResponse(SysTask *task, void *data)
{
    CommClubManager *commClubMan = data;

    if (CommSys_CheckError()) {
        CommClubMan_DestroyList(task, commClubMan);
        CommClubMan_PrintMessage(pl_msg_00000353_00000, FALSE); // "There appears to be a problem..."
        CommClubMan_ReconnectAndClearPlayers(commClubMan);
    } else if (CommList_Get(0, 3) == CommSys_CurNetId()) {
        CommList_Refresh();
        CommClubMan_DestroyList(task, commClubMan);
        CommClubMan_PlayerRefused(commClubMan);
    } else if (CommList_Get(0, 5) != -1) {
        CommList_Refresh();
        CommClubMan_DestroyList(task, commClubMan);
        CommClubMan_PlayerRefused(commClubMan);
    } else if (CommList_Get(0, 4) == CommSys_CurNetId()) {
        CommClubMan_PrintMessage(pl_msg_00000353_00097, FALSE); // Players have choseen different rules. The same rules must be chosen.
        CommClubMan_SetTask(CommClubTask_DifferentRegulation);
    } else if (CommList_Get(0, 2) == CommSys_CurNetId()) {
        CommList_Refresh();

        commClubMan->connectedCnt = CommInfo_CountReceived();

        if (!FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
            Text_RemovePrinter(sCommClubMan->printMsgIndex);
        }

        CommClubMan_DestroyList(task, commClubMan);
        CommClubMan_ClientAccepted(commClubMan);
    } else if (CommClub_CheckCommError()) {
        CommClubMan_DestroyList(task, commClubMan);
        CommClubMan_PlayerRefused(commClubMan);
    } else {
        return FALSE;
    }

    return TRUE;
}

// Waits for host accept/reject (B = leave group prompt)
static void CommClubTask_ClientWaitHostResponse(SysTask *task, void *param1)
{
    if (!FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        return;
    }

    if (CommClubMan_CheckHostResponse(task, param1)) {
        return;
    }

    if ((PAD_BUTTON_B)&gSystem.pressedKeys) {
        CommClubMan_PrintMessage(pl_msg_00000353_00003, FALSE); // Do you want to leave the group?
        CommClubMan_SetTask(CommClubTask_WaitConfirmLeaveGroup);
    }
}

static void CommClubTask_WaitConfirmLeaveGroup(SysTask *task, void *data)
{
    CommClubManager *commClubMan = data;

    if (CommClubMan_CheckHostResponse(task, data)) {
        return;
    }

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        commClubMan->unk_60 = Menu_MakeYesNoChoice(sCommClubMan->fieldSystem->bgConfig, &sYesNoWindowTemplate, 1024 - (18 + 12) - 9, 11, 4);
        CommClubMan_SetTask(CommClubTask_LeaveGroup);
    }
}

static void CommClubTask_LeaveGroup(SysTask *task, void *data)
{
    CommClubManager *v0 = data;
    u32 v1;

    if (CommClubMan_CheckHostResponse(task, data)) {
        Menu_DestroyForExit(v0->unk_60, 4);
        v0->unk_60 = NULL;
        return;
    }

    v1 = Menu_ProcessInputAndHandleExit(v0->unk_60, 4);

    if (v1 == 0) {
        CommClubMan_DestroyList(task, v0);
        CommClubMan_PrintMessage(pl_msg_00000353_00006, FALSE); // You have left the group.
        CommClubMan_ReconnectAndClearPlayers(v0);
    } else if (v1 != 0xffffffff) {
        CommClubMan_SetTask(CommClubTask_SelectServerList);
    }
}

// ListMenuTemplate for server connected-players list
static const ListMenuTemplate sServerListMenuTemplate = {
    NULL,
    NULL,
    NULL,
    NULL,
    0x5,
    0x5,
    0x0,
    0x1,
    0x0,
    0x0,
    0x1,
    0xF,
    0x2,
    0x0,
    0x10,
    0x1,
    0x0,
    0x1,
    NULL
};

// Draws server list item (trainer name + ID)
static void CommClubMan_DrawServerListItem(ListMenu *param0, u32 param1, u8 param2)
{
    u16 v0 = 0;

    if (sCommClubMan->unk_5C) {
        ListMenu_GetListAndCursorPos(sCommClubMan->unk_5C, &v0, NULL);
    }

    v0 += param2;

    if (NULL != CommInfo_TrainerInfo(v0)) {
        StringTemplate_SetPlayerName(sCommClubMan->unk_50, 0, CommInfo_TrainerInfo(v0));
        MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00066, sCommClubMan->stringBuffer[1]); // "{player name}"

        StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[0], sCommClubMan->stringBuffer[1]);
        Text_AddPrinterWithParams(&sCommClubMan->unk_20, FONT_SYSTEM, sCommClubMan->stringBuffer[0], 8, param2 * 16, TEXT_SPEED_INSTANT, NULL);

        StringTemplate_SetNumber(sCommClubMan->unk_50, 2, TrainerInfo_ID_LowHalf(CommInfo_TrainerInfo(v0)), 5, 2, 1);
        MessageLoader_GetString(sCommClubMan->msgLoader, pl_msg_00000353_00065, sCommClubMan->stringBuffer[3]); // "ID{id}"

        StringTemplate_Format(sCommClubMan->unk_50, sCommClubMan->stringBuffer[2], sCommClubMan->stringBuffer[3]);
        Text_AddPrinterWithParams(&sCommClubMan->unk_20, FONT_SYSTEM, sCommClubMan->stringBuffer[2], 9 * 8, param2 * 16, TEXT_SPEED_INSTANT, NULL);
    }
}

static void CommClubMan_StartBattleServer(CommClubManager *commClubMan)
{
    FieldCommManager_StartBattleServer(commClubMan->fieldSystem, commClubMan->commType, CommClubMan_Regulation());
}

static void CommClubMan_Disconnect(void)
{
    FieldCommManager_EndBattleNoSync();
}

static void CommClubMan_SetTask(CommClubManTaskFunc stateFunc)
{
    sCommClubMan->commClubTask = stateFunc;
}

// Opens side window, prints awaiting-contact msg, starts SysTask
static void CommClubMan_StartRetailHostAwait(CommClubManager *man)
{
    int v0;

    SysTask_Start(CommClubMan_Run, man, 0);
    CommClubMan_PrintAwaitingContactMsg();

    if (!Window_IsInUse(&sCommClubMan->unk_30)) {
        Window_Add(sCommClubMan->fieldSystem->bgConfig, &sCommClubMan->unk_30, 3, 22, 2, 9, 4, 13, (1 + 17 * 6 * 2));
    }

    Window_DrawStandardFrame(&sCommClubMan->unk_30, 1, 1024 - (18 + 12) - 9, 11);
    Window_FillTilemap(&sCommClubMan->unk_30, 15);
    Window_CopyToVRAM(&sCommClubMan->unk_30);

    man->unk_97 = 1;

    sub_02032E1C(0);
    CommClubMan_SetTask(CommClubTask_ServerBuildList);
}

// Prints comm-type-specific "Awaiting contact..." message
static void CommClubMan_PrintAwaitingContactMsg(void)
{
    int sAwaitingContactMsg[] = {
        pl_msg_00000353_00007, // XXXXXXXXXXXXXXXXXXXXXX
        pl_msg_00000353_00008, // Single Battle! Awaiting contact from your friend.
        pl_msg_00000353_00009, // Double Battle! Awaiting contact from your friend.
        pl_msg_00000353_00011, // Mix Battle! Awaiting contact from your friend.
        pl_msg_00000353_00010, // Multi Battle! Awaiting contact from your friends.
        pl_msg_00000353_00010,
        pl_msg_00000353_00012, // When the group is ready: A Button: Proceed B Button: Cancel
        pl_msg_00000353_00013, // Record Corner! Awaiting contact from your friend(s).
        pl_msg_00000353_00014, // When the group is ready: A Button: Proceed B Button: Cancel
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00015, // Battle Tower! Awaiting contact from your friend.
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00016, // Battle Factory--Level 50! Awaiting contact from your friend.
        pl_msg_00000353_00017, // Battle Factory--Open Level! Awaiting contact from your friend.
        pl_msg_00000353_00000,
        pl_msg_00000353_00018, // Battle Hall! Awaiting contact from your friend.
        pl_msg_00000353_00019, // Battle Castle! Awaiting contact from your friend.
        pl_msg_00000353_00020 // Battle Arcade! Awaiting contact from your friend.
    };

    GF_ASSERT(sCommClubMan->commType < (sizeof(sAwaitingContactMsg) / sizeof(int)));

    if (sCommClubMan->commType == COMM_TYPE_CONTEST) {
        if (CommInfo_CountReceived() > 1) {
            //{contest type} Contest! Awaiting contact. A Button: When everyone's ready.
            CommClubMan_PrintMessage(pl_msg_00000353_00108 + sCommClubMan->unk_92, FALSE);
        } else {
            //{contest type} Contest! Awaiting contact from another player.
            CommClubMan_PrintMessage(pl_msg_00000353_00098 + sCommClubMan->unk_92, FALSE);
        }
    } else {
        CommClubMan_PrintMessage(sAwaitingContactMsg[sCommClubMan->commType], FALSE);
    }
}

// Updates "{N} players" / "{N} more needed" in side window
static void CommClubMan_UpdatePlayerCountDisplay(CommClubManager *commClubMan)
{
    int msg, playerCnt;

    if (CommInfo_CountReceived() >= CommClubMan_MinPlayers()) {
        playerCnt = CommInfo_CountReceived();
        msg = pl_msg_00000353_00068; //{cnt} players.
    } else {
        playerCnt = CommClubMan_MinPlayers() - CommInfo_CountReceived();
        msg = pl_msg_00000353_00067; //{cnt} more players needed.
    }

    StringTemplate_SetNumber(sCommClubMan->unk_54, 0, playerCnt, 2, 5, 1);
    Window_FillTilemap(&sCommClubMan->unk_30, 15);

    MessageLoader_GetString(sCommClubMan->msgLoader, msg, sCommClubMan->stringBuffer[7]);
    StringTemplate_Format(sCommClubMan->unk_54, sCommClubMan->stringBuffer[6], sCommClubMan->stringBuffer[7]);
    Text_AddPrinterWithParams(&sCommClubMan->unk_30, FONT_SYSTEM, sCommClubMan->stringBuffer[6], 2, 2, TEXT_SPEED_INSTANT, NULL);
}

static void CommClubMan_PrintPlayerContactMsg(int netId, CommClubManager *commClubMan)
{
    int sContactMsg[] = {
        pl_msg_00000353_00021, //{Player} has contacted you. Would you like to trade?
        pl_msg_00000353_00022, //{Player} has contacted you. Start a Single Battle?
        pl_msg_00000353_00023, //{Player} has contacted you. Start a Double Battle?
        pl_msg_00000353_00024, //{Player} has contacted you. Start a Mix Battle?
        pl_msg_00000353_00025, //{Player} has contacted you. Let {Player} join your group?
        pl_msg_00000353_00025,
        pl_msg_00000353_00031, //{Player} has contacted you. Let {Player} join your group?
        pl_msg_00000353_00025,
        pl_msg_00000353_00025,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00026, //{Player} has contacted you. Take the Battle Tower challenge?
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00027, //{Player} has contacted you. Start the Battle Factory challenge?
        pl_msg_00000353_00027,
        pl_msg_00000353_00000,
        pl_msg_00000353_00028, //{Player} has contacted you. Start the Battle Hall challenge?
        pl_msg_00000353_00029, //{Player} has contacted you. Start the Battle Castle challenge?
        pl_msg_00000353_00030 //{Player} has contacted you. Start the Battle Arcade challenge?
    };
    TrainerInfo *trainerInfo = CommInfo_TrainerInfo(netId);

    if (trainerInfo != NULL) {
        StringTemplate_SetPlayerName(commClubMan->strTempMsg, 1, trainerInfo);
    }

    CommClubMan_PrintMessage(sContactMsg[commClubMan->commType], TRUE);
}

static void CommClubMan_Run(SysTask *task, void *param1)
{
    CommClubManager *man = (CommClubManager *)param1;

    if (sCommClubMan->commClubTask != NULL) {
        CommClubManTaskFunc stateFunc = sCommClubMan->commClubTask;

        stateFunc(task, man);
    }
}

// Waits for msg print, builds connected-players StringList
static void CommClubTask_ServerBuildList(SysTask *task, void *param1)
{
    CommClubManager *v0 = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(v0);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        int netId;

        sCommClubMan->unk_64 = StringList_New(5, HEAP_ID_FIELD1);

        for (netId = 0; netId < 5; netId++) {
            StringList_AddFromMessageBank(sCommClubMan->unk_64, sCommClubMan->msgLoader, pl_msg_00000353_00069, 0); // (empty slot placeholder)
        }

        sCommClubMan->unk_98 = 1;

        for (netId = 0; netId < (7 + 1); netId++) {
            sCommClubMan->unk_80[netId] = 0;
        }

        CommClubMan_CreateList(sServerListMenuTemplate, 1, 2, 17, 6 * 2, 1);
        CommClubMan_SetTask(CommClubTask_ServerPollPlayers);
    }
}

// Main loop: detects new contacts, shows accept/reject prompt
static void CommClubTask_ServerPollPlayers(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;
    int v2;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    for (v2 = 0; v2 < (7 + 1); v2++) {
        if (sub_02032DC4(v2) && (v2 != 0)) {
            sCommClubMan->unk_80[v2] = 1;
            sCommClubMan->unk_98 = 1;
        } else if ((NULL != CommInfo_TrainerInfo(v2)) && !sCommClubMan->unk_80[v2]) {
            sCommClubMan->unk_80[v2] = 1;
            sCommClubMan->unk_98 = 1;
        } else if ((NULL == CommInfo_TrainerInfo(v2)) && sCommClubMan->unk_80[v2]) {
            sCommClubMan->unk_80[v2] = 0;
            sCommClubMan->unk_98 = 1;
        }
    }

    if (sCommClubMan->unk_98) {
        sCommClubMan->unk_98 = 0;
        ListMenu_Draw(sCommClubMan->unk_5C);

        for (v2 = 0; v2 < ListMenu_GetAttribute(sCommClubMan->unk_5C, 3); v2++) {
            CommClubMan_DrawServerListItem(sCommClubMan->unk_5C, 0, v2);
        }
    }

    v1 = ListMenu_ProcessInput(commClubMan->unk_5C);

    if (CommInfo_NewNetworkId() != 0xff) {
        commClubMan->unk_95 = CommInfo_NewNetworkId();
        Sound_PlayEffect(SEQ_SE_DP_PC_LOGIN);
        CommClubMan_PrintPlayerContactMsg(commClubMan->unk_95, commClubMan);
        CommClubMan_SetTask(CommClubTask_ServerContactPrompt);
        return;
    }

    if (CommSys_CheckError()) {
        CommClubMan_Disconnect();
        CommClubMan_DestroyList(task, commClubMan);
        sCommClubMan->retCode = COMM_CLUB_RET_ERROR;
    } else {
        switch (v1) {
        case 0xffffffff:
            break;
        case 0xfffffffe:
            CommClubMan_SetTask(CommClubTask_ServerAskCancelGroup);
            break;
        default:
            if (CommClubMan_MinPlayers() <= CommInfo_CountReceived()) {
                int sGroupConfirmMsg[] = {
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00032, // Multi Battle! Is this group OK?
                    pl_msg_00000353_00032,
                    pl_msg_00000353_00032,
                    pl_msg_00000353_00033, // Is this group OK?
                    pl_msg_00000353_00034, // Record Center! Is this group OK?
                    pl_msg_00000353_00035, // Contest! Is this group OK?
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000,
                    pl_msg_00000353_00000
                };

                commClubMan->connectedCnt = CommInfo_CountReceived();

                CommClubMan_PrintMessage(sGroupConfirmMsg[commClubMan->commType], FALSE);
                CommClubMan_SetTask(CommClubTask_ServerGroupConfirmWaitPrint);
            }

            break;
        }
    }
}

// Processes yes/no for accept/reject player
static void CommClubTask_ServerRespondToContact(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;
    int v2;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);
    ListMenu_ProcessInput(commClubMan->unk_5C);

    v1 = Menu_ProcessInputAndHandleExit(commClubMan->unk_60, 4);

    if (!CommSys_IsPlayerConnected(commClubMan->unk_95)) {
        if (v1 == 0xffffffff) {
            Menu_DestroyForExit(commClubMan->unk_60, 4);
        }

        if (commClubMan->commType == COMM_TYPE_CONTEST) {
            for (v2 = 1; v2 < CommClubMan_MaxPlayers(); v2++) {
                if (CommSys_IsPlayerConnected(v2)) {
                    CommInfo_InitPlayer(v2);
                    CommList_Send(5, v2);
                }
            }

            CommClubMan_PrintMessage(0, FALSE);
            CommClubMan_SetTask(CommClubTask_ServerPlayerLeft);
        } else {
            CommInfo_InitPlayer(commClubMan->unk_95);
            CommClubMan_PrintMessage(0, FALSE);
            CommClubMan_SetTask(CommClubTask_ServerPlayerLeft);
        }
    } else if (CommSys_CheckError() || CommClub_CheckCommError()) {
        if (v1 == 0xffffffff) {
            Menu_DestroyForExit(commClubMan->unk_60, 4);
        }

        CommInfo_InitPlayer(commClubMan->unk_95);
        CommClubMan_PrintMessage(0, FALSE);
        CommClubMan_SetTask(CommClubTask_ServerPlayerLeft);
    } else if (v1 == 0) {
        if ((commClubMan->commType == COMM_TYPE_SINGLE_BATTLE) || (commClubMan->commType == COMM_TYPE_DOUBLE_BATTLE)) {
            if (!CommInfo_CheckBattleRegulation()) {
                CommClubMan_PrintMessage(pl_msg_00000353_00097, FALSE); // Players have chosen different rules. The same rules must be chosen.
                CommClubMan_SetTask(CommClubTask_DifferentRegulation);
                CommList_Send(4, commClubMan->unk_95);
                return;
            }
        }

        sub_02032E1C(commClubMan->unk_95);
        CommList_Send(2, commClubMan->unk_95);

        if (CommClubMan_MaxPlayers() == CommInfo_CountReceived()) {
            commClubMan->connectedCnt = CommInfo_CountReceived();

            if (CommClubMan_MaxPlayers() > 2) {
                CommClubMan_SetTask(CommClubTask_ServerPrintGroupConfirm);
            } else {
                CommClubMan_DestroyList(task, commClubMan);
                CommClubMan_ServerAcceptedDual(commClubMan);
            }
        } else {
            CommClubMan_PrintAwaitingContactMsg();
            CommClubMan_SetTask(CommClubTask_ServerWaitMsgThenPoll);
        }
    } else if (v1 != 0xffffffff) {
        if (commClubMan->commType == COMM_TYPE_CONTEST) {
            for (v2 = 1; v2 < CommClubMan_MaxPlayers(); v2++) {
                if (CommSys_IsPlayerConnected(v2)) {
                    CommInfo_InitPlayer(v2);
                    CommList_Send(5, v2);
                }
            }

            CommClubMan_PrintAwaitingContactMsg();
            CommClubMan_SetTask(CommClubTask_ServerWaitMsgThenPoll);
        } else {
            CommInfo_InitPlayer(commClubMan->unk_95);
            CommList_Send(3, commClubMan->unk_95);
            CommClubMan_PrintAwaitingContactMsg();
            CommClubMan_SetTask(CommClubTask_ServerWaitMsgThenPoll);
        }
    }
}

static void CommClubTask_DifferentRegulation(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            CommClubMan_DestroyList(task, commClubMan);
            CommClubMan_Disconnect();
            sCommClubMan->retCode = COMM_CLUB_RET_4;
        }
    }
}

// Player disconnected notification, waits for button
static void CommClubTask_ServerPlayerLeft(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            CommClubMan_PrintAwaitingContactMsg();
            CommClubMan_SetTask(CommClubTask_ServerWaitMsgThenPoll);
        }
    }
}

// Waits for msg finish, returns to ServerPollPlayers
static void CommClubTask_ServerWaitMsgThenPoll(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        CommClubMan_SetTask(CommClubTask_ServerPollPlayers);
    }
}

// Prints group-confirm prompt for multi/contest
static void CommClubTask_ServerPrintGroupConfirm(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;
    int sGroupConfirmMsg[] = {
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00032,
        pl_msg_00000353_00032,
        pl_msg_00000353_00032,
        pl_msg_00000353_00033,
        pl_msg_00000353_00034,
        pl_msg_00000353_00035,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000
    };

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);
    CommClubMan_PrintMessage(sGroupConfirmMsg[commClubMan->commType], 0);
    CommClubMan_SetTask(CommClubTask_ServerGroupConfirmWaitPrint);
}

// Waits for print, closes entry, shows yes/no
static void CommClubTask_ServerGroupConfirmWaitPrint(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    CommManager_SetWirelessEntry(0);
    ListMenu_ProcessInput(commClubMan->unk_5C);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        commClubMan->unk_60 = Menu_MakeYesNoChoice(sCommClubMan->fieldSystem->bgConfig, &sYesNoWindowTemplate, 1024 - (18 + 12) - 9, 11, 4);
        CommClubMan_SetTask(CommClubTask_ServerGroupConfirmInput);
    }
}

// Processes yes/no for group confirmation, starts sync
static void CommClubTask_ServerGroupConfirmInput(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;
    int v2;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);
    ListMenu_ProcessInput(commClubMan->unk_5C);

    v1 = Menu_ProcessInputAndHandleExit(commClubMan->unk_60, 4);

    if (CommSys_CheckError() || (CommSys_ConnectedCount() != commClubMan->connectedCnt)) {
        if (v1 == 0xffffffff) {
            Menu_DestroyForExit(commClubMan->unk_60, 4);
        }

        commClubMan->unk_60 = NULL;
        CommClubMan_SetTask(CommClubTask_ServerGroupDropout);
    } else if (v1 == 0) {
        if (commClubMan->commType == COMM_TYPE_CONTEST) {
            for (v2 = 1; v2 < CommSys_ConnectedCount(); v2++) {
                if (!CommSys_IsPlayerConnected(v2)) {
                    CommClubMan_SetTask(CommClubTask_ServerGroupDropout);
                    return;
                }
            }
        }

        CommTiming_StartSync(10);
        CommClubMan_SetTask(CommClubTask_ServerSyncWait);
    } else if (v1 != 0xffffffff) {
        CommManager_SetWirelessEntry(1);
        CommClubMan_Disconnect();
        CommClubMan_DestroyList(task, commClubMan);
        sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
    }
}

// Member dropped: prints cancel msg, disconnects
static void CommClubTask_ServerGroupDropout(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    int v1[] = {
        pl_msg_00000353_00000, // There appears to be a problem...
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00054, // A member has dropped out.
        pl_msg_00000353_00055, // Record mixing with tthis group will be canceled.
        pl_msg_00000353_00056, // A Contest involving this group will be canceled.
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000
    };

    CommClubMan_DestroyList(task, commClubMan);

    if (sCommClubMan->commType == COMM_TYPE_CONTEST) {
        // The {contest type} with these members will be canceled.
        CommClubMan_PrintMessage(pl_msg_00000353_00118 + sCommClubMan->unk_92, 0);
    } else {
        CommClubMan_PrintMessage(v1[commClubMan->commType], 0);
    }

    FieldCommManager_EndBattleNoSync();
    SysTask_Start(CommClubTask_ServerDropoutWaitButton, commClubMan, 0);
}

// Waits A/B after dropout message, sets RET_CANCEL
static void CommClubTask_ServerDropoutWaitButton(SysTask *task, void *param1)
{
    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
            SysTask_Done(task);
        }
    }
}

// Host pressed B: asks "Cancel the group?"
static void CommClubTask_ServerAskCancelGroup(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;
    int v2[] = {
        pl_msg_00000353_00000, // There appears to be an error...
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00036, // Cancel a Multi Battle involving this group?
        pl_msg_00000353_00036,
        pl_msg_00000353_00036,
        pl_msg_00000353_00037, // Cancel the cooking session?
        pl_msg_00000353_00038, // Cancel mixing records with this group?
        pl_msg_00000353_00039, // Cancel holding a Contest with this group?
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000,
        pl_msg_00000353_00000
    };

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    if (CommInfo_CountReceived() > 1) {
        CommClubMan_PrintMessage(v2[commClubMan->commType], 0);
        CommClubMan_SetTask(CommClubTask_ServerCancelPromptWaitPrint);
    } else {
        CommClubMan_Disconnect();
        CommClubMan_DestroyList(task, commClubMan);
        sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
    }
}

// Waits for cancel prompt print, shows yes/no
static void CommClubTask_ServerCancelPromptWaitPrint(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);
    ListMenu_ProcessInput(commClubMan->unk_5C);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        commClubMan->unk_60 = Menu_MakeYesNoChoice(sCommClubMan->fieldSystem->bgConfig, &sYesNoWindowTemplate, 1024 - (18 + 12) - 9, 11, 4);
        CommClubMan_SetTask(CommClubTask_ServerCancelPromptInput);
    }
}

// Processes yes/no for cancellation
static void CommClubTask_ServerCancelPromptInput(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);

    ListMenu_ProcessInput(commClubMan->unk_5C);
    v1 = Menu_ProcessInputAndHandleExit(commClubMan->unk_60, 4);

    if (CommSys_CheckError()) {
        v1 = 0xfffffffe;
    }

    if (v1 == 0) {
        CommClubMan_Disconnect();
        CommClubMan_DestroyList(task, commClubMan);
        sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
    } else if (v1 != 0xffffffff) {
        CommClubMan_PrintAwaitingContactMsg();
        CommClubMan_SetTask(CommClubTask_ServerWaitMsgThenPoll);
    }
}

// Waits for CommTiming sync state 10 after group confirmed
static void CommClubTask_ServerSyncWait(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    if (CommSys_CheckError() || (CommSys_ConnectedCount() != commClubMan->connectedCnt)) {
        CommClubMan_SetTask(CommClubTask_ServerGroupDropout);
    } else {
        CommClubMan_UpdatePlayerCountDisplay(commClubMan);
        ListMenu_ProcessInput(commClubMan->unk_5C);

        if (CommTiming_IsSyncState(10) && CommTool_IsInitialized()) {
            CommClubMan_DestroyList(task, commClubMan);
            CommClubMan_FinishSuccess(commClubMan);
        }
    }
}

// Waits for contact msg print, shows yes/no
static void CommClubTask_ServerContactPrompt(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;
    u32 v1 = 0xffffffff;

    CommClubMan_UpdatePlayerCountDisplay(commClubMan);
    ListMenu_ProcessInput(commClubMan->unk_5C);

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        commClubMan->unk_60 = Menu_MakeYesNoChoice(sCommClubMan->fieldSystem->bgConfig, &sYesNoWindowTemplate, 1024 - (18 + 12) - 9, 11, 4);
        CommClubMan_SetTask(CommClubTask_ServerRespondToContact);
    }
}

static void CommClubMan_DestroyList(SysTask *task, CommClubManager *param1)
{
    Window_FillTilemap(&sCommClubMan->msgWindow, 15);
    Window_EraseStandardFrame(&param1->unk_20, 0);
    StringList_Free(param1->unk_64);
    ListMenu_Free(param1->unk_5C, NULL, NULL);
    Window_ClearAndCopyToVRAM(&param1->unk_20);
    Window_Remove(&param1->unk_20);

    if (param1->unk_97) {
        Window_EraseStandardFrame(&sCommClubMan->unk_30, 0);
        Window_ClearAndCopyToVRAM(&sCommClubMan->unk_30);
        Window_Remove(&sCommClubMan->unk_30);
        param1->unk_97 = 0;
    }

    if (task) {
        SysTask_Done(task);
    }
}

// Host accepted 2P: prints "Replied OK", starts sync
static void CommClubMan_ServerAcceptedDual(CommClubManager *commClubMan)
{
    StringTemplate_SetPlayerName(commClubMan->strTempMsg, 1, CommInfo_TrainerInfo(1));
    CommClubMan_PrintMessage(pl_msg_00000353_00057, 1); // Reeplied "OK" to {}

    SysTask_Start(CommClubMan_Run, commClubMan, 0);
    CommClubMan_SetTask(CommClubTask_WaitAcceptMsgThenSync);
}

// Client accepted by host: prints "Replied OK"
static void CommClubMan_ClientAccepted(CommClubManager *commClubMan)
{
    sub_020339AC(commClubMan->connectIndex, sCommClubMan->unk_7C);
    StringTemplate_SetPlayerName(commClubMan->strTempMsg, 1, sCommClubMan->unk_7C);
    CommClubMan_PrintMessage(pl_msg_00000353_00004, 1); // Replied, "OK"

    commClubMan->connectedCnt = CommSys_ConnectedCount();

    SysTask_Start(CommClubMan_Run, commClubMan, 0);
    CommClubMan_SetTask(CommClubTask_WaitAcceptMsgThenSync);

    sCommClubMan->msgDelay = (30 * 3);
}

// Waits for accept msg, starts sync(10)
static void CommClubTask_WaitAcceptMsgThenSync(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if ((CommSys_CurNetId() == 0) && (CommSys_ConnectedCount() != commClubMan->connectedCnt)) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
    } else if (CommClub_CheckCommError() || CommSys_CheckError()) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
    } else if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        CommTiming_StartSync(10);
        CommClubMan_SetTask(CommClubTask_WaitForGroup);
    }
}

static void CommClubTask_WaitForGroup(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if ((CommSys_CurNetId() == 0) && (CommSys_ConnectedCount() != commClubMan->connectedCnt)) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
    } else if (CommClub_CheckCommError() || CommSys_CheckError()) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
        return;
    } else if (!CommSys_IsPlayerConnected(0)) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
        return;
    } else if (CommList_Get(0, 5) != -1) {
        CommClubMan_SetTask(CommClubTask_ExitGuestRoom);
        return;
    } else if (CommTiming_IsSyncState(10)) {
        if (CommTool_IsInitialized()) {
            if (!FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
                Text_RemovePrinter(sCommClubMan->printMsgIndex);
            }

            commClubMan->connectedCnt = CommSys_ConnectedCount();
            CommClubMan_FinishSuccess(commClubMan);
            SysTask_Done(task);
            return;
        }
    }

    if (sCommClubMan->msgDelay != 0) {
        sCommClubMan->msgDelay--;

        if (sCommClubMan->msgDelay == 0) {
            int sWaitingForGroupMsg[] = {
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00058, // Multi Battle! Waiting for the rest of the group.
                pl_msg_00000353_00058,
                pl_msg_00000353_00058,
                pl_msg_00000353_00059, // Waiting for the rest of the group.
                pl_msg_00000353_00060, // Record Center! Waiting for the rest of the group.
                pl_msg_00000353_00061, // Contest! Waiting for the rest of the group.
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000,
                pl_msg_00000353_00000
            };

            if (sCommClubMan->commType == COMM_TYPE_CONTEST) {
                //{contest type} Contest! Awaiting other members!
                CommClubMan_PrintMessage(pl_msg_00000353_00113 + sCommClubMan->unk_92, 0);
            } else {
                CommClubMan_PrintMessage(sWaitingForGroupMsg[commClubMan->commType], 0);
            }
        }
    }
}

static void CommClubTask_ExitGuestRoom(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        CommClubMan_PrintMessage(pl_msg_00000353_00000, 0);
        CommClubMan_SetTask(CommClubTask_ExitGuestRoomEnd);
    }
}

static void CommClubTask_ExitGuestRoomEnd(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            SysTask_Done(task);
            CommClubMan_Disconnect();
            sCommClubMan->retCode = COMM_CLUB_RET_CANCEL;
        }
    }
}

static void CommClubMan_PlayerRefused(CommClubManager *commClubMan)
{
    sub_020339AC(commClubMan->connectIndex, sCommClubMan->unk_7C);
    StringTemplate_SetPlayerName(commClubMan->strTempMsg, 1, sCommClubMan->unk_7C);

    CommClubMan_PrintMessage(pl_msg_00000353_00005, 1); //{Player Name} refused...

    FieldCommManager_ReconnectBattleClient();
    SysTask_Start(CommClubTask_ClientRefusedWaitMsg, commClubMan, 0);
}

// Waits for refused msg print, reconnects client
static void CommClubTask_ClientRefusedWaitMsg(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        FieldCommManager_ReconnectBattleClient();
        CommClubMan_PrintChooseJoinMsg(commClubMan);
        SysTask_Done(task);
    }
}

// Reconnects client, clears all player info
static void CommClubMan_ReconnectAndClearPlayers(CommClubManager *commClubMan)
{
    FieldCommManager_ReconnectBattleClient();

    for (int netId = 0; netId < (7 + 1); netId++) {
        CommInfo_InitPlayer(netId);
    }

    SysTask_Start(CommClubTask_WaitMsgThenRejoinList, commClubMan, 0);
}

// Waits for msg + button, returns to ChooseJoinMsg
static void CommClubTask_WaitMsgThenRejoinList(SysTask *task, void *param1)
{
    CommClubManager *commClubMan = (CommClubManager *)param1;

    if (FieldMessage_FinishedPrinting(sCommClubMan->printMsgIndex)) {
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            CommClubMan_PrintChooseJoinMsg(commClubMan);
            SysTask_Done(task);
        }
    }
}

// Final: sets retCode=RET_2, manages Cast open/seal, sends player info
static void CommClubMan_FinishSuccess(CommClubManager *commClubMan)
{
    CommManager_SetWirelessEntry(0);
    sCommClubMan->retCode = COMM_CLUB_RET_2;
    CommManager_SetErrorHandling(1, 1);
    CommInfo_SendPlayerInfo();
    sub_02033EA8(1);
}

int CommClubMan_MinPlayers(void)
{
    return CommType_MinPlayers(sCommClubMan->commType);
}

static int CommClubMan_MaxPlayers(void)
{
    return CommType_MaxPlayers(sCommClubMan->commType);
}

static int CommClubMan_Regulation(void)
{
    return sCommClubMan->unk_92 + (sCommClubMan->unk_93 << 4);
}

void CommClub_StartBattleClient(FieldSystem *fieldSystem, int commType, int param2, int param3)
{
    CommClubMan_Init(fieldSystem);
    sCommClubMan->commType = commType;

    if (commType == COMM_TYPE_MIX_BATTLE || commType == COMM_TYPE_MULTI_BATTLE_1) {
        fieldSystem->battleRegulation = NULL;
    }

    sCommClubMan->unk_92 = param2;
    sCommClubMan->unk_93 = param3;

    CommClubMan_StartBattleClient(sCommClubMan);
}

void CommClub_PrintChooseJoinMsg(void)
{
    CommClubMan_PrintChooseJoinMsg(sCommClubMan);
}

u32 CommClub_CheckWindowOpenClient(void)
{
    u32 ret = sCommClubMan->retCode;

    if (COMM_CLUB_RET_0 != ret) {
        CommClubMan_Delete();
    }

    return ret;
}

void CommClub_StartBattleServer(FieldSystem *fieldSystem, int commType, int param2, int param3)
{
    CommClubMan_Init(fieldSystem);
    sCommClubMan->commType = commType;

    if (commType == COMM_TYPE_MIX_BATTLE || commType == COMM_TYPE_MULTI_BATTLE_1) {
        fieldSystem->battleRegulation = NULL;
    }

    sCommClubMan->unk_92 = param2;
    sCommClubMan->unk_93 = param3;

    CommClubMan_StartBattleServer(sCommClubMan);
}

// Entry: routes to SilentHostAwait (OWMP) or retail await UI
void CommClub_BeginHostAwait(void)
{
    CommClubMan_StartRetailHostAwait(sCommClubMan);
}

u32 CommClub_CheckWindowOpenServer(void)
{
    u32 ret = sCommClubMan->retCode;

    if (COMM_CLUB_RET_0 != ret) {
        CommClubMan_Delete();
    }

    return ret;
}

// Stub, always returns FALSE
BOOL CommClub_CheckCommError(void)
{
    return 0;
}
