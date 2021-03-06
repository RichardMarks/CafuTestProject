/*
=================================================================================
This file is part of Cafu, the open-source game engine and graphics engine
for multiplayer, cross-platform, real-time 3D action.
Copyright (C) 2002-2013 Carsten Fuchs Software.

Cafu is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Cafu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cafu. If not, see <http://www.gnu.org/licenses/>.

For support and more information about Cafu, visit us at <http://www.cafu.de>.
=================================================================================
*/

#ifndef CAFU_BASE_ENTITY_HPP_INCLUDED
#define CAFU_BASE_ENTITY_HPP_INCLUDED

#include "../../GameEntity.hpp"
#include "ClipSys/ClipModel.hpp"

#include <map>

#if defined(_WIN32) && _MSC_VER<1600
#include "pstdint.h"            // Paul Hsieh's portable implementation of the stdint.h header.
#else
#include <stdint.h>
#endif


class PhysicsWorldT;
struct lua_State;
namespace cf { namespace ClipSys { class CollisionModelT; } }
namespace cf { namespace TypeSys { class TypeInfoManT; } }
namespace cf { namespace TypeSys { class CreateParamsT; } }


namespace GAME_NAME
{
    class EntityCreateParamsT;
    class ApproxBaseT;


    /// The TypeInfoTs of all BaseEntityT derived classes must register with this TypeInfoManT instance.
    cf::TypeSys::TypeInfoManT& GetBaseEntTIM();


    // This structure describes each entitys state and is transmitted from the server to the clients over the network.
    struct EntityStateT
    {
        VectorT        Velocity;                // Velocity of this entity.
        char           StateOfExistance;        // For entity defined state machines, e.g. "specator, dead, alive, ...".
        char           Flags;                   // Entity defined flags.
        char           PlayerName[64];          // If it is a human player, this is its name. Usually unused otherwise.
     // ArrayT<char>   PlayerName;
        char           ModelIndex;              // An arbitrary (game defined) number that describes the BODY model of this entity (that OTHERS see).
        char           ModelSequNr;             // The sequence number of the body model.
        float          ModelFrameNr;            // The frame number of the current sequence of the body model.

        char           Health;                  // Health.
        char           Armor;                   // Armor.
        unsigned long  HaveItems;               // Bit field, entity can carry 32 different items.
        unsigned long  HaveWeapons;             // Bit field, entity can carry 32 different weapons.
        char           ActiveWeaponSlot;        // Index into HaveWeapons, HaveAmmoInWeapons, and for determining the weapon model index.
        char           ActiveWeaponSequNr;      // The weapon sequ. that WE see (the LOCAL clients VIEW weapon model). Could (and should in the future) also be used for other clients PLAYER weapon model, but currently is not.
        float          ActiveWeaponFrameNr;     // Respectively, this is the frame number of the current weapon sequence.
        unsigned short HaveAmmo[16];            // Entity can carry 16 different types of ammo (weapon independent). This is the amount of each.
        unsigned char  HaveAmmoInWeapons[32];   // Entity can carry ammo in each of the 32 weapons. This is the amount of each.

        EntityStateT(const VectorT& Velocity_,
                     char StateOfExistance_, char Flags_, char ModelIndex_, char ModelSequNr_, float ModelFrameNr_,
                     char Health_, char Armor_, unsigned long HaveItems_, unsigned long HaveWeapons_,
                     char ActiveWeaponSlot_, char ActiveWeaponSequNr_, float ActiveWeaponFrameNr_);
    };


    // This class describes "base entities", the most central component in game<-->engine communication.
    class BaseEntityT : public GameEntityI
    {
        public:

        const unsigned long ID;             // The unique ID of this entity.
        const std::string   Name;           ///< The unique instance name of this entity, normally equal to and obtained from Properties["name"]. It's explicitly kept (possibly redundantly to Properties["name"]), because with it: a) there is no confusion when "name" is not found in the Properties list, b) its const-ness is more obvious and easier to guarantee (e.g. the ScriptStateT class relies on the name never changing throughout entity lifetime, because script objects are addressed and found by name, not by instance pointer (light userdata)!), and c) access like   MyEnt->Name   is much easier to write than using Properties.find().
        const std::map<std::string, std::string> Properties;    ///< The properties of this entities from the map file.
        const unsigned long WorldFileIndex; // The index of this entity into the array in the world file, -1 if no such information exists. See [1].
        unsigned long       ParentID;       // The 'ID' of the entity that created us.
     // ID[]                ChildrenIDs;    // The entities that we have created (e.g. the rockets that a human player fired).

        cf::GameSys::GameWorldI*            GameWorld;      ///< Pointer to the game world implementation.
        const cf::ClipSys::CollisionModelT* CollisionModel; ///< The collision model of this entity, NULL for none.
        cf::ClipSys::ClipModelT             ClipModel;      ///< The clip model of this entity. Note that clip models can take NULL collision model pointers, so that the ClipModel instance is always non-NULL and available.

        // [1]  I have been trying hard to save this variable, because 'ID' seems to handle the same purpose.
        // This would mean to overload 'ID' with multiple purposes though, and that doesn't seem right to me:
        // a) Care must be taken that 'ID's are always unique (never get re-used), but this should be the case anyway.
        // b) Worse is the implied order assumption (both on server and on *CLIENT* side):
        //    In order to conclude from the 'ID' to the (no-longer-present) 'MapFileID', assumptions must hold that are hard and dangerous to enforce.
        //    For example, 'ID==MapFileID' must hold for all entities that have map file information associated with them.
        // c) Even if all that worked, it would e.g. be impossible to duplicate such an entity.
        // d) Moreover, it isn't worth the effort: 'MapFileID' causes network load only once, during the baseline message for this entity.
        //    That doesn't happen all that often, so I rather keep it than introducing a lot of complicated dependencies and problems.


        /// The destructor.
        virtual ~BaseEntityT();

        /// The implementation calls DoSerialize(), that derived classes override to add their own data.
        void Serialize(cf::Network::OutStreamT& Stream) const /*final override*/;

        /// The implementation calls DoDeserialize(), that derived classes override to read their own data.
        /// It also calls ProcessEvent() (overridden by derived classes) for any received events.
        void Deserialize(cf::Network::InStreamT& Stream, bool IsIniting) /*final override*/;

        // Implement GameEntityI base class methods.
        virtual void NotifyLeaveMap() { }
        virtual unsigned long GetID() const { return ID; }
        virtual std::string GetName() const { return Name; }
        virtual unsigned long GetWorldFileIndex() const { return WorldFileIndex; }
        virtual cf::GameSys::GameWorldI* GetGameWorld() const { return GameWorld; }
        virtual const Vector3dT& GetOrigin() const { return m_Origin; }
        virtual const BoundingBox3dT& GetDimensions() const { return m_Dimensions; }
        virtual void GetCameraOrientation(unsigned short& h, unsigned short& p, unsigned short& b) const { h=m_Heading; p=m_Pitch; b=m_Bank; }
        virtual void GetBodyOrientation(unsigned short& h, unsigned short& p, unsigned short& b) const { h=m_Heading; p=m_Pitch; b=m_Bank; }

        /// Returns the heading of this entity.
        unsigned short GetHeading() const { return m_Heading; }


        // Some convenience functions for reading the Properties.
        // Actually, the Properties should be replaced by a PropDictT class that has these functions as its methods...
        float       GetProp(const std::string& Key, float       Default=0.0f) const;
        double      GetProp(const std::string& Key, double      Default=0.0) const;
        int         GetProp(const std::string& Key, int         Default=0) const;
        std::string GetProp(const std::string& Key, std::string Default="") const;
        Vector3fT   GetProp(const std::string& Key, Vector3fT   Default=Vector3fT()) const;
        Vector3dT   GetProp(const std::string& Key, Vector3dT   Default=Vector3dT()) const;


        /// This SERVER-SIDE function is used to notify this entity that it was touched by another entity.
        /// 'Entity' is the entity that touches this one, and is usually the entity from which the call is made.
        /// Calls are only made from within other entities 'Think()' functions.
        /// (Unfortunately, this function cannot be declared as "protected": see "C++ FAQs, 2nd edition" by Cline, Lomow, Girou, pages 249f.)
        virtual void NotifyTouchedBy(BaseEntityT* Entity);

        /// This SERVER-SIDE method is called whenever another entity walked into one of our trigger volumes (trigger brushes).
        /// It is called once per frame as long as the other entity stays within our trigger volume.
        ///
        /// @param Activator   The entity that walked into our trigger volume and thus caused the call of this method.
        ///
        /// Note that most entity classes just do nothing in their implementation of this method, usually because they never have trigger brushes anyway.
        /// Entity classes that are very likely to provide an implementation though are EntTriggerT (of course!) and the items like weapons,
        /// so they can e.g. detect being picked up.
        /// Calls to this method normally come from within the Activator->Think() method, because it are the activators themselves that detect
        /// that they entered a trigger volume (it are not the entities with the triggers that detect that something entered their volume).
        /// (Unfortunately, this function cannot be declared as "protected": see "C++ FAQs, 2nd edition" by Cline, Lomow, Girou, pages 249f.)
        virtual void OnTrigger(BaseEntityT* Activator);

        /// This SERVER-SIDE method is called whenever another entity pushes us.
        /// Whenever an entity moves, it may want to push other entities that are touching it.
        /// Examples are lifts that give the objects on it a ride, or players that push barrels or crates.
        /// As the pusher itself may be pushed (e.g. when a player pushes a barrel that pushes a barrel or when a lift pushes a crate
        /// that pushes the player on top of it), there can be an entire chain of pushers.
        ///
        /// @param Pushers
        ///   The chain of pushers that are pushing this entity. Usually, there is only one element in this list,
        ///   but in theory, e.g. when a train pushes a waggon that pushes a waggon that pushes a waggon..., the list can be
        ///   arbitrarily long.
        ///   The topmost pusher, i.e. Pushers[i] with i==Pushers.Size()-1, is our immediate pusher, Pushers[i-1] is the pusher of our pusher,
        ///   and so forth. Pushers[0] is the source pusher of the entire chain.
        ///   Note that the Pushers list must always be kept acyclic, i.e. each entity must occur in the list at most once,
        ///   or otherwise there is momentum to infinite recursion. For example, the last waggon of as train must not also be a pusher of the train;
        ///   when a T-shaped object pushes a U-shaped object sideways (with its vertical bar in the center of the U), the U must not also push the T, etc.
        ///
        /// @param PushVector
        ///   This vector describes the desired and required direction and length of the push.
        virtual void OnPush(ArrayT<BaseEntityT*>& Pushers, const Vector3dT& PushVector);

        /// This SERVER-SIDE function is used to have this entity take damage.
        /// 'Entity' is the entitiy that causes the damage (i.e., who fired a shot). It is usually the entity from which the call is made.
        /// 'Amount' is the amount of damage that was caused, and is usually subtracted from this entitys health.
        /// 'ImpactDir' is the direction from which we were hit.
        /// Calls are only made from within other entities 'Think()' functions.
        /// (Unfortunately, this function cannot be declared as "protected": see "C++ FAQs, 2nd edition" by Cline, Lomow, Girou, pages 249f.)
        virtual void TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir);

        /// This SERVER-SIDE function is used for posting an event of the given type.
        /// The event is automatically sent from the entity instance on the server to the entity instances
        /// on the clients, and causes a matching call to ProcessEvent() there.
        /// The meaning of the event type is specific to the concrete entity class.
        /// Note that events are fully predictable: they work well even in the presence of client prediction.
        void PostEvent(unsigned int EventType) { m_EventsCount[EventType]++; }

        // Implement SERVER-SIDE GameEntityI base class methods.
        virtual void ProcessConfigString(const void* ConfigData, const char* ConfigString) /*override*/;
        virtual void Think(float FrameTime, unsigned long ServerFrameNr) /*override*/;


        /// This CLIENT-SIDE function is called to process events on the client.
        /// Events that have been posted via PostEvent() on the server (or in client prediction) are eventually
        /// received in Deserialize(), which automatically calls this method.
        /// In the call, Deserialize() indicates the number of new events since the last call (at least one,
        /// or it wouldn't call ProcessEvent() at all). This way, each event can be processed exactly once.
        ///
        /// TODO: Move into private section below "DoDeserialize()" ??
        virtual void ProcessEvent(unsigned int EventType, unsigned int NumEvents);

        // Implement CLIENT-SIDE GameEntityI base class methods.
        virtual bool GetLightSourceInfo(unsigned long& DiffuseColor, unsigned long& SpecularColor, VectorT& Position, float& Radius, bool& CastsShadows) const /*override*/;
        virtual void Draw(bool FirstPersonView, float LodDist) const /*override*/;
        virtual void Interpolate(float FrameTime) /*final override*/;
        virtual void PostDraw(float FrameTime, bool FirstPersonView) /*override*/;


        /// Returns the proper type info for this entity.
        virtual const cf::TypeSys::TypeInfoT* GetType() const;
        static void* CreateInstance(const cf::TypeSys::CreateParamsT& Params);
        static const cf::TypeSys::TypeInfoT TypeInfo;     ///< The type info object for (objects/instances of) this class.


        // Methods provided to be called from the map/entity Lua scripts.
        static int GetName(lua_State* L);
        static int GetOrigin(lua_State* L);
        static int SetOrigin(lua_State* L);


        protected:

        /// Protected constructor such that only concrete entities can call this for creating a BaseEntityT, but nobody else.
        /// Concrete entities are created in the GameI::CreateGameEntityFromMapFile() method for the server-side,
        /// and in the GameI::CreateGameEntityFromTypeNr() method for the client-side.
        BaseEntityT(const EntityCreateParamsT& Params, const BoundingBox3dT& Dimensions, const unsigned int NUM_EVENT_TYPES);

        /// Concrete entities call this method in their constructors in order to have us automatically interpolate
        /// the value that has been specified with the Interp instance.
        /// Interpolation occurs on the client only: it is advanced in the video frames between server updates.
        /// Each update received from the server (applied in DoDeserialize()) automatically updates the reference value
        /// for the interpolation in the next video frames.
        void Register(ApproxBaseT* Interp);

        Vector3dT      m_Origin;        ///< World coordinate of (the eye of) this entity.
        BoundingBox3dT m_Dimensions;    ///< The bounding box of this entity (relative to the origin).
        unsigned short m_Heading;       ///< Heading (north is along the ??-axis).
        unsigned short m_Pitch;         ///< Pitch (for looking up/down).
        unsigned short m_Bank;          ///< Bank (e.g. used when dead and lying on the side).


        private:

        BaseEntityT(const BaseEntityT&);        ///< Use of the Copy    Constructor is not allowed.
        void operator = (const BaseEntityT&);   ///< Use of the Assignment Operator is not allowed.

        /// Derived classes override this method in order to write their state into the given stream.
        /// The method itself is automatically called from Serialize(), see Serialize() for more details.
        ///
        /// (This follows the "Non-Virtual Interface Idiom" as described by Scott Meyers in
        /// "Effective C++, 3rd Edition", item 35 ("Consider alternatives to virtual functions.").)
        virtual void DoSerialize(cf::Network::OutStreamT& Stream) const { }

        /// Derived classes override this method in order to read their state from the given stream.
        /// The method itself is automatically called from Deserialize(), see Deserialize() for more details.
        ///
        /// (This follows the "Non-Virtual Interface Idiom" as described by Scott Meyers in
        /// "Effective C++, 3rd Edition", item 35 ("Consider alternatives to virtual functions.").)
        virtual void DoDeserialize(cf::Network::InStreamT& Stream) { }

        ArrayT<uint32_t>     m_EventsCount;     ///< A counter for each event type for the number of its occurrences. Serialized (and deserialized) normally along with the entity state.
        ArrayT<uint32_t>     m_EventsRef;       ///< A reference counter for each event type for the number of processed occurrences. Never serialized (or deserialized), never reset, strictly growing.

        ArrayT<ApproxBaseT*> m_Interpolators;   ///< The interpolators for advancing values in the client video frames between server updates. The values that are interpolated are specified by calls to Register() by the derived entity class.
    };
}

#endif
