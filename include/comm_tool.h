#ifndef POKEPLATINUM_COMM_TOOL_H
#define POKEPLATINUM_COMM_TOOL_H

#include "constants/heap.h"

void CommTool_Init(enum HeapID heapID);
void CommTool_Delete(void);
BOOL CommTool_IsInitialized(void);
void CommTool_CmdSyncRequest(int param0, int param1, void *param2, void *param3);
void CommTool_CmdSyncAck(int param0, int param1, void *param2, void *param3);
void CommTool_CmdSyncComplete(int param0, int param1, void *param2, void *param3);
void CommTiming_StartSync(u8 syncNo);
void CommTool_SendQueuedSync(void);
BOOL CommTiming_IsSyncState(u8 syncState);
int CommTiming_GetSyncNumber(int netId);
void CommList_Recv(int param0, int param1, void *param2, void *param3);
int CommList_Size(void);
void CommList_Send(u8 param0, u8 param1);
int CommList_Get(int param0, u8 param1);
void CommList_Refresh(void);
void CommTool_ClearReceivedTempDataAllPlayers(void);
BOOL CommTool_SendTempData(int param0, const void *param1);
const void *CommTool_GetReceivedTempData(int param0);
void CommTool_CmdRecvTempData(int param0, int param1, void *param2, void *param3);
int CommTool_TempDataSize(void);

#endif // POKEPLATINUM_COMM_TOOL_H
