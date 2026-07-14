#ifndef POKEPLATINUM_OV7_02249960_H
#define POKEPLATINUM_OV7_02249960_H

#include "field/field_system_decl.h"

void CommClub_StartBattleClient(FieldSystem *fieldSystem, int param1, int param2, int param3);
void CommClub_PrintChooseJoinMsg(void);
u32 CommClub_CheckWindowOpenClient(void);
void CommClub_StartBattleServer(FieldSystem *fieldSystem, int param1, int param2, int param3);
void CommClub_BeginHostAwait(void);
u32 CommClub_CheckWindowOpenServer(void);
BOOL CommClub_CheckCommError(void);

#endif // POKEPLATINUM_OV7_02249960_H
