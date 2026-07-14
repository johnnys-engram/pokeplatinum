#include "comm_tool.h"

// Retail wireless helper shared by CommSys users.  It provides small
// fixed-size command helpers for timing barriers, Union/Comm Club list stamps,
// and a 70-byte temporary data exchange used by link apps.

#include <nitro.h>
#include <string.h>

#include "constants/heap.h"

#include "communication_system.h"
#include "heap.h"

typedef struct {
    u8 tag;
    u8 value;
} CommListEntry;

#define COMM_TOOL_TEMP_DATA_SIZE 70

typedef struct {
    CommListEntry listEntries[MAX_CONNECTED_PLAYERS];
    u8 syncNumbers[MAX_CONNECTED_PLAYERS];
    u8 tempData[MAX_CONNECTED_PLAYERS][COMM_TOOL_TEMP_DATA_SIZE + 2];
    u8 hasReceivedTempData[MAX_CONNECTED_PLAYERS];
    u8 syncState;
    u8 pendingSyncNumber;
    u8 syncPending;
} CommTool;

static CommTool *sCommTool = NULL;

void CommTool_Init(enum HeapID heapID)
{
    if (!sCommTool) {
        sCommTool = Heap_Alloc(heapID, sizeof(CommTool));
        MI_CpuFill8(sCommTool, 0, sizeof(CommTool));
    }

    for (int netId = 0; netId < MAX_CONNECTED_PLAYERS; netId++) {
        sCommTool->syncNumbers[netId] = 0xff;
    }

    sCommTool->syncState = 0xff;
    sCommTool->pendingSyncNumber = 0xff;
    sCommTool->syncPending = 0;
}

void CommTool_Delete(void)
{
    Heap_Free(sCommTool);
    sCommTool = NULL;
}

BOOL CommTool_IsInitialized(void)
{
    if (sCommTool) {
        return 1;
    }

    return 0;
}

void CommTool_CmdSyncRequest(int netId, int param1, void *_buff, void *param3)
{
    u8 *buff = _buff;
    u8 syncNumber = buff[0];
    u8 v2[2];
    int otherNetId;

    if (CommSys_CurNetId() == 0) {
        v2[0] = netId;
        v2[1] = syncNumber;
        CommSys_SendDataFixedSizeServer(18, &v2);

        sCommTool->syncNumbers[netId] = syncNumber;

        for (otherNetId = 0; otherNetId < MAX_CONNECTED_PLAYERS; otherNetId++) {
            if (CommSys_IsPlayerConnected(otherNetId)) {
                if (syncNumber != sCommTool->syncNumbers[otherNetId]) {
                    return;
                }
            }
        }

        CommSys_SendDataFixedSizeServer(17, &syncNumber);
    }
}

void CommTool_CmdSyncAck(int netId, int param1, void *param2, void *param3)
{
    u8 *v0 = param2;
    sCommTool->syncNumbers[v0[0]] = v0[1];
}

void CommTool_CmdSyncComplete(int netId, int param1, void *param2, void *param3)
{
    u8 *v0 = param2;
    u8 v1 = v0[0];

    sCommTool->syncState = v1;
}

// Queue a retail timing barrier.  The next CommTool_SendQueuedSync call sends
// command 16; the parent echoes progress with command 18 and broadcasts command
// 17 when all connected players have reached the same sync number.
void CommTiming_StartSync(u8 syncNumber)
{
    sCommTool->pendingSyncNumber = syncNumber;
    sCommTool->syncPending = TRUE;
}

void CommTool_SendQueuedSync(void)
{
    if (sCommTool) {
        if (sCommTool->syncPending) {
            if (CommSys_SendDataFixedSize(16, &sCommTool->pendingSyncNumber)) {
                sCommTool->syncPending = 0;
            }
        }
    }
}

// Returns TRUE once every connected machine has requested the same sync number.
BOOL CommTiming_IsSyncState(u8 syncState)
{
    if (sCommTool == NULL) {
        return TRUE;
    }

    if (sCommTool->syncState == syncState) {
        return TRUE;
    }

    return FALSE;
}

int CommTiming_GetSyncNumber(int netId)
{
    return sCommTool->syncNumbers[netId];
}

void CommList_Recv(int netId, int param1, void *param2, void *param3)
{
    CommListEntry *v0 = param2;

    sCommTool->listEntries[netId].tag = v0->tag;
    sCommTool->listEntries[netId].value = v0->value;
}

int CommList_Size(void)
{
    return sizeof(CommListEntry);
}

// Broadcast a two-byte Comm Club list stamp.  Retail callers use this to mark
// accept/refuse/rules results in the shared connection list.
void CommList_Send(u8 param0, u8 param1)
{
    CommListEntry v0;

    v0.tag = param0;
    v0.value = param1;

    CommSys_SendDataFixedSize(19, &v0);
}

// Read a list stamp from a shared slot.  Returns the value when the slot
// contains the requested tag, otherwise -1.
int CommList_Get(int param0, u8 param1)
{
    if (!sCommTool) {
        return -1;
    }

    if (sCommTool->listEntries[param0].tag == param1) {
        return sCommTool->listEntries[param0].value;
    }

    return -1;
}

// Clear the retail shared list slots before another Union/Comm Club selection.
void CommList_Refresh(void)
{
    int v0;

    for (v0 = 0; v0 < (7 + 1); v0++) {
        MI_CpuFill8(&sCommTool->listEntries[v0], 0, sizeof(CommListEntry));
    }
}

void CommTool_ClearReceivedTempDataAllPlayers(void)
{
    for (int i = 0; i < MAX_CONNECTED_PLAYERS; i++) {
        sCommTool->hasReceivedTempData[i] = 0;
    }
}

// Send one fixed-size temporary data block through command 20.
BOOL CommTool_SendTempData(int param0, const void *param1)
{
    if (sCommTool) {
        MI_CpuCopy8(param1, sCommTool->tempData[param0], COMM_TOOL_TEMP_DATA_SIZE);
        CommSys_SendDataFixedSize(20, sCommTool->tempData[param0]);
        return 1;
    }

    return 0;
}

// Return a received temporary data block for netId, or NULL until command 20 arrives.
const void *CommTool_GetReceivedTempData(int netId)
{
    if (sCommTool->hasReceivedTempData[netId]) {
        return &sCommTool->tempData[netId];
    }

    return NULL;
}

void CommTool_CmdRecvTempData(int netId, int param1, void *param2, void *param3)
{
    sCommTool->hasReceivedTempData[netId] = TRUE;
    MI_CpuCopy8(param2, sCommTool->tempData[netId], COMM_TOOL_TEMP_DATA_SIZE);
}

int CommTool_TempDataSize(void)
{
    return COMM_TOOL_TEMP_DATA_SIZE;
}
