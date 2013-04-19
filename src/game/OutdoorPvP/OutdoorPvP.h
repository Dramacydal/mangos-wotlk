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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef OUTDOOR_PVP_H
#define OUTDOOR_PVP_H

#include "Common.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include "OutdoorPvPMgr.h"

class WorldPacket;
class WorldObject;
class Player;
class GameObject;
class Unit;
class Creature;

enum CapturePointArtKits
{
    CAPTURE_ARTKIT_ALLIANCE = 2,
    CAPTURE_ARTKIT_HORDE    = 1,
    CAPTURE_ARTKIT_NEUTRAL  = 21
};

enum CapturePointAnimations
{
    CAPTURE_ANIM_ALLIANCE   = 1,
    CAPTURE_ANIM_HORDE      = 0,
    CAPTURE_ANIM_NEUTRAL    = 2
};

typedef std::map<ObjectGuid /*playerGuid*/, bool /*isMainZone*/> GuidZoneMap;

class OutdoorPvP
{
    friend class OutdoorPvPMgr;

    public:
        OutdoorPvP(uint32 _id) : m_id(_id), m_isBattleField(false) {}
        virtual ~OutdoorPvP() {}

        // called when the zone is initialized
        virtual void FillInitialWorldStates(WorldPacket& /*data*/, uint32& /*count*/) {}

        // Process Capture event
        virtual bool HandleEvent(uint32 /*eventId*/, GameObject* /*go*/, Player* pInvoker = NULL, uint32 spellId = 0) { return false; }

        // handle capture objective complete
        virtual void HandleObjectiveComplete(uint32 /*eventId*/, std::list<Player*> /*players*/, Team /*team*/) {}

        // Called when a creature is created
        virtual void HandleCreatureCreate(Creature* /*creature*/) {}

        // Called when a gameobject is created or removed
        virtual void HandleGameObjectCreate(GameObject* /*go*/);
        virtual void HandleGameObjectRemove(GameObject* /*go*/);

        // Called on creature death
        virtual void HandleCreatureDeath(Creature* /*creature*/) {}

        // called when a player uses a gameobject related to outdoor pvp events
        virtual bool HandleGameObjectUse(Player* /*player*/, GameObject* /*go*/) { return false; }

        // called when a player triggers an areatrigger
        virtual bool HandleAreaTrigger(Player* /*player*/, uint32 /*triggerId*/) { return false; }

        // called when a player drops a flag
        virtual bool HandleDropFlag(Player* /*player*/, uint32 /*spellId*/) { return false; }

        // update - called by the OutdoorPvPMgr
        virtual void Update(uint32 /*diff*/) {}

        // Handle player kill
        void HandlePlayerKill(Player* killer, Unit* victim);

        uint32 GetId() const { return m_id; }
        bool IsBattleField() const { return m_isBattleField; }

        virtual bool IsMember(ObjectGuid guid) { return true; }

        virtual bool InitOutdoorPvPArea() { return true; }

        // send world state update to all players present in map
        void SendUpdateWorldStateForMap(uint32 field, uint32 value, Map* map);

    protected:

        // Player related stuff
        virtual void HandlePlayerEnterZone(Player* /*player*/, bool /*isMainZone*/);
        virtual void HandlePlayerLeaveZone(Player* /*player*/, bool /*isMainZone*/);
        virtual void HandlePlayerEnterArea(Player* /*pPlayer*/, uint32 /*uiAreaId*/, bool /*isMainZone*/) { };
        virtual void HandlePlayerLeaveArea(Player* /*pPlayer*/, uint32 /*uiAreaId*/, bool /*isMainZone*/) { };

        // remove world states
        virtual void SendRemoveWorldStates(Player* /*player*/) {}

        // handle npc/player kill
        virtual void HandlePlayerKillInsideArea(Player* /*killer*/, Unit* /*victim*/) {}

        // send world state update to all players present
        void SendUpdateWorldState(uint32 field, uint32 value);

        // applies buff to a team inside the specific zone
        void BuffTeam(Team team, uint32 spellId, bool remove = false, bool onlyMembers = true, uint32 area = 0);

        // get banner artkit based on controlling team
        uint32 GetBannerArtKit(Team team, uint32 artKitAlliance = CAPTURE_ARTKIT_ALLIANCE, uint32 artKitHorde = CAPTURE_ARTKIT_HORDE, uint32 artKitNeutral = CAPTURE_ARTKIT_NEUTRAL);

        // set banner visual
        void SetBannerVisual(const WorldObject* objRef, ObjectGuid goGuid, uint32 artKit, uint32 animId);
        void SetBannerVisual(GameObject* go, uint32 artKit, uint32 animId);

        // Handle gameobject spawn / despawn
        void RespawnGO(const WorldObject* objRef, ObjectGuid goGuid, bool respawn);

        // store the players inside the area
        GuidZoneMap m_zonePlayers;

        // outdoor pvp type id
        uint32 m_id;

        bool m_isBattleField;
};

#endif
