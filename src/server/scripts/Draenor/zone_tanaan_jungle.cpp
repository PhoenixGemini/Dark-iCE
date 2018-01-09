/*
* Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "Player.h"
#include "Unit.h"
#include "Vehicle.h"

/// Fel Sludge - 188520
class spell_tanaan_fel_sludge : public SpellScriptLoader
{
public:
    spell_tanaan_fel_sludge() : SpellScriptLoader("spell_tanaan_fel_sludge") { }

    class spell_tanaan_fel_sludge_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_tanaan_fel_sludge_AuraScript);

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Target = GetTarget();

            if (!l_Target)
                return;

            if (p_AurEff->GetBase()->GetStackAmount() >= 10)
                l_Target->Kill(l_Target, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_tanaan_fel_sludge_AuraScript::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_tanaan_fel_sludge_AuraScript();
    }
};

enum eMaps
{
    TANAAN_JUNGLE_100_PHASE_MAP = 1464,
    DREANOR_MAP                 = 1116,
    TANAAN_JUNGLE_100_ZONE_ID   = 6723
};

// Dark Portal phasing
class PlayerScript_TanaanJungle_Phasing : public PlayerScript
{
public:
    PlayerScript_TanaanJungle_Phasing() : PlayerScript("PlayerScript_TanaanJungle_Phasing")
    {
    }

    void OnUpdateZone(Player* player, uint32 newZoneID, uint32 oldZoneID, uint32 /*newAreaID*/) override
    {
        if (player->IsInFlight())
            return;

        if (player->GetMapId() == TANAAN_JUNGLE_100_PHASE_MAP || player->GetMapId() == DREANOR_MAP)
        {
            if (newZoneID != oldZoneID && (newZoneID == TANAAN_JUNGLE_100_ZONE_ID || oldZoneID == TANAAN_JUNGLE_100_ZONE_ID))
            {
                if (newZoneID == TANAAN_JUNGLE_100_ZONE_ID && player->GetMapId() == DREANOR_MAP)
                    player->SeamlessTeleportToMap(TANAAN_JUNGLE_100_PHASE_MAP);
                else if (newZoneID != TANAAN_JUNGLE_100_ZONE_ID && player->GetMapId() == TANAAN_JUNGLE_100_PHASE_MAP)
                    player->SeamlessTeleportToMap(DREANOR_MAP);
            }
        }
    }
};

class npc_terrorfist : public CreatureScript
{
public:
    npc_terrorfist() : CreatureScript("npc_terrorfist") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_terrorfistAI(creature);
    }

    struct npc_terrorfistAI : public ScriptedAI
    {
        npc_terrorfistAI(Creature* creature) : ScriptedAI(creature) { }

        enum TerrorfistSpells
        {
            SpellSmash             = 172679,
            SpellEnrage            = 32714
        };

        void EnterCombat(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(13), [this](TaskContext context)
            {
                me->CastSpell(me, SpellSmash, false);
                context.Repeat(Seconds(13));
            });

            me->GetScheduler().Schedule(Seconds(30), [this](TaskContext context)
            {
                me->AddAura(SpellEnrage, me);
                context.Repeat(Seconds(30));
            });
        }
    };
};

class npc_vengeance : public CreatureScript
{
public:
    npc_vengeance() : CreatureScript("npc_vengeance") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_vengeanceAI(creature);
    }

    struct npc_vengeanceAI : public ScriptedAI
    {
        npc_vengeanceAI(Creature* creature) : ScriptedAI(creature) { }

        enum VengeanceSpells
        {
            SpellEnvelopingShadows = 170178,
            SpellShadowInfusion    = 170180
        };

        void EnterCombat(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(17), [this](TaskContext context)
            {
                me->CastSpell(me, SpellEnvelopingShadows, false);
                context.Repeat(Seconds(17));
            });

            me->GetScheduler().Schedule(Seconds(14), [this](TaskContext context)
            {
                me->CastSpell(me, SpellShadowInfusion, false);
                context.Repeat(Seconds(14));
            });
        }
    };
};

class npc_deathtalon : public CreatureScript
{
public:
    npc_deathtalon() : CreatureScript("npc_deathtalon") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_deathtalonAI(creature);
    }

    struct npc_deathtalonAI : public ScriptedAI
    {
        npc_deathtalonAI(Creature* creature) : ScriptedAI(creature) { }

        enum DeathtalonSpells
        {
            SpellHeadbutt          = 158546,
            SpellQuills            = 158564,
            SpellPierce            = 158560,
            SpellScreech           = 167367
        };

        void EnterCombat(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(40), [this](TaskContext context)
            {
                me->CastSpell(me, SpellHeadbutt, false);
                context.Repeat(Seconds(40));
            });

            me->GetScheduler().Schedule(Seconds(20), [this](TaskContext context)
            {
                me->CastSpell(me, SpellQuills, false);
                context.Repeat(Seconds(60));
            });

            me->GetScheduler().Schedule(Seconds(15), [this](TaskContext context)
            {
                me->CastSpell(me, SpellPierce, false);
                context.Repeat(Seconds(30));
            });

            me->GetScheduler().Schedule(Seconds(5), [this](TaskContext context)
            {
                me->CastSpell(me, SpellScreech, false);
                context.Repeat(Seconds(90), Seconds(100));
            });
        }
    };
};

class at_deathtalon_screech : public AreaTriggerEntityScript
{
public:
    at_deathtalon_screech() : AreaTriggerEntityScript("at_deathtalon_screech") { }

    struct at_deathtalon_screechAI : AreaTriggerAI
    {
        at_deathtalon_screechAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

        enum eSpells
        {
            SPELL_SCREECH_DAMAGE = 158558
        };

        void OnUnitEnter(Unit* unit) override
        {
            if (Unit* caster = at->GetCaster())
                if (caster != unit)
                    caster->CastSpell(unit, SPELL_SCREECH_DAMAGE, true);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const override
    {
        return new at_deathtalon_screechAI(areatrigger);
    }
};

/// Quills - 158564
class spell_deathtalon_quills : public SpellScriptLoader
{
public:
    spell_deathtalon_quills() : SpellScriptLoader("spell_deathtalon_quills") { }

    class spell_deathtalon_quills_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_deathtalon_quills_AuraScript);

        enum
        {
            SPELL_QUILLS_DAMAGE = 158567
        };

        void OnTick(AuraEffect const* /*aurEff*/)
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(caster, SPELL_QUILLS_DAMAGE, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathtalon_quills_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_deathtalon_quills_AuraScript();
    }
};

class npc_doomroller : public CreatureScript
{
public:
    npc_doomroller() : CreatureScript("npc_doomroller") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_doomrollerAI(creature);
    }

    enum eNpcs
    {
        NPC_DRIVER      = 94807,
        NPC_PASSENGER   = 95062
    };

    struct npc_doomrollerAI : public ScriptedAI
    {
        npc_doomrollerAI(Creature* creature) : ScriptedAI(creature) { }

        enum DoomrollerSpells
        {
            SpellSiegeNova = 188579,
        };

        void Reset() override
        {
            me->setFaction(14);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->GetMotionMaster()->MovePoint(1, 4092.80f, -760.21f, 2.89f, false);

            if (Vehicle* meVehicle = me->GetVehicleKit())
            {
                meVehicle->AddPassenger(NPC_PASSENGER, 0);
                meVehicle->AddPassenger(NPC_PASSENGER, 2);
                meVehicle->AddPassenger(NPC_PASSENGER, 3);
                meVehicle->AddPassenger(NPC_PASSENGER, 5);
                meVehicle->AddPassenger(NPC_DRIVER, 7);
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type == POINT_MOTION_TYPE && point == 1)
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(40), [this](TaskContext context)
            {
                me->CastSpell(me, SpellSiegeNova, false);
                context.Repeat(Seconds(40));
            });
        }
    };
};

void AddSC_tanaan_jungle()
{
    new spell_tanaan_fel_sludge();

    new PlayerScript_TanaanJungle_Phasing();

    new npc_terrorfist();
    new npc_vengeance();
    new npc_doomroller();

    new npc_deathtalon();
    new at_deathtalon_screech();
    new spell_deathtalon_quills();
}
