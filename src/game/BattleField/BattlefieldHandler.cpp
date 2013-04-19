/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "WorldSession.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "BattleField/BattleField.h"

// This send to player to invite him to teleport to battlefield
// Param1: battlefield id
// Param2: zone id of battlefield (4197 for wg)
// Param3: time for player to accept in seconds
void WorldSession::SendBfInvitePlayerToWar(uint32 uiBattlefieldId, uint32 uiZoneId, uint32 uiTimeToAccept)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_ENTRY_INVITE, 12);
    data << uint32(uiBattlefieldId);                        // 1 for Wintergrasp
    data << uint32(uiZoneId);
    data << uint32(time(NULL) + uiTimeToAccept);

    SendPacket(&data);
}

// This is send to invite player to join the queue when he is in battlefield zone and it is about to start or by battlemaster
// Param1: battlefield id
void WorldSession::SendBfInvitePlayerToQueue(uint32 uiBattlefieldId)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_QUEUE_INVITE, 5);

    data << uint32(uiBattlefieldId);                        // 1 for Wintergrasp
    data << uint8(1);                                       // warmup ? used ?

    SendPacket(&data);
}

// This packet is in response to inform player that he joins queue
// Param1: battlefield id
// Param2: zone id of battlefield (4197 for wg)
// Param3: if players is able to queue
// Param4: if battlefield is full
void WorldSession::SendBfQueueInviteResponse(uint32 uiBattlefieldId, uint32 uiZoneId, bool bCanQueue, bool bFull)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_QUEUE_REQUEST_RESPONSE, 11);
    data << uint32(uiBattlefieldId);
    data << uint32(uiZoneId);
    data << uint8(bCanQueue ? 1 : 0);   // Accepted     // 0 you cannot queue wg    // 1 you are queued     
    data << uint8(bFull ? 0 : 1);       // Logging In   // 0 wg full                // 1 queue for upcoming
    data << uint8(1);                   // Warmup
    SendPacket(&data);
}

// This is called when player accepts invitation to battlefield
// Param1: battlefield id
void WorldSession::SendBfEntered(uint32 uiBattlefieldId)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_ENTERING, 7);
    data << uint32(uiBattlefieldId);
    data << uint8(1);                                       // unk
    data << uint8(1);                                       // unk
    data << uint8(_player->isAFK() ? 1 : 0);                // Clear AFK
    SendPacket(&data);
}

void WorldSession::SendBfLeaveMessage(uint32 uiBattlefieldId, BattlefieldLeaveReason reason)
{
    WorldPacket data(SMSG_BATTLEFIELD_MANAGER_EJECTED, 7);
    data << uint32(uiBattlefieldId);
    data << uint8(reason);          //byte Reason
    data << uint8(2);               //byte BattleStatus
    data << uint8(0);               //bool Relocated
    SendPacket(&data);
}

// Send by client when he click on accept for queue
void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recv_data)
{
    uint32 uiBattlefieldId;
    uint8 bAccepted;

    recv_data >> uiBattlefieldId >> bAccepted;
    DEBUG_LOG("HandleQueueInviteResponse: uiBattlefieldId:%u v:%u", uiBattlefieldId, bAccepted);

    if (BattleField* bf = sOutdoorPvPMgr.GetBattlefieldById(uiBattlefieldId))
        bf->OnPlayerInviteResponse(_player, bAccepted);
}

// Send by client when he clicks accept or denies invitation
void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recv_data)
{
    uint32 uiBattlefieldId;
    uint8 bAccepted;

    recv_data >> uiBattlefieldId >> bAccepted;
    DEBUG_LOG("HandleBattlefieldInviteResponse: uiBattlefieldId: %u bAccepted:%u", uiBattlefieldId, bAccepted);

    if (BattleField* bf = sOutdoorPvPMgr.GetBattlefieldById(uiBattlefieldId))
        bf->OnPlayerPortResponse(_player, bAccepted);
}

void WorldSession::HandleBfExitRequest(WorldPacket& recv_data)
{
    uint32 uiBattlefieldId;
    recv_data >> uiBattlefieldId;

    DEBUG_LOG("HandleBfExitRequest: uiBattlefieldId: %u", uiBattlefieldId);

    if (BattleField* bf = sOutdoorPvPMgr.GetBattlefieldById(uiBattlefieldId))
        bf->OnPlayerQueueExitRequest(GetPlayer());
}
