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

#ifndef CAFU_GAME_IMPLEMENTATION_HPP_INCLUDED
#define CAFU_GAME_IMPLEMENTATION_HPP_INCLUDED

#include "../../Game.hpp"
#include "Templates/Array.hpp"

#ifndef GAME_NAME
#error Macro GAME_NAME must be defined!
#endif


class CafuModelT;
class PhysicsWorldT;
class SoundI;


namespace GAME_NAME
{
    //class CarriedWeaponT;


    /// This class provides the "DeathMatch" implementation of the GameI interface.
    class GameImplT : public cf::GameSys::GameI
    {
        public:

        // Implement the methods of the GameI interface.
        void Initialize(bool AsClient, bool AsServer, ModelManagerT& ModelMan);
        void Release();
        const cf::TypeSys::TypeInfoManT& GetEntityTIM() const;

        IntrusivePtrT<GameEntityI> CreateGameEntityFromMapFile(const cf::TypeSys::TypeInfoT* TI, const std::map<std::string, std::string>& Properties, const cf::SceneGraph::GenericNodeT* RootNode, const cf::ClipSys::CollisionModelT* CollisionModel, unsigned long ID, unsigned long WorldFileIndex, unsigned long MapFileIndex, cf::GameSys::GameWorldI* GameWorld, const Vector3T<double>& Origin);
        IntrusivePtrT<GameEntityI> CreateGameEntityFromTypeNr(unsigned long TypeNr, const std::map<std::string, std::string>& Properties, const cf::SceneGraph::GenericNodeT* RootNode, const cf::ClipSys::CollisionModelT* CollisionModel, unsigned long ID, unsigned long WorldFileIndex, unsigned long MapFileIndex, cf::GameSys::GameWorldI* GameWorld);


        // Additional methods.

        /// Maps a player model index to a player model instance.
        /// Used to obtain a player model from a player entities State.ModelIndex member.
        //const CafuModelT* GetPlayerModel(unsigned int ModelIndex) const;

        /// This function returns a pointer to the CarriedWeaponT instance for the desired ActiveWeaponSlot.
        /// When no such class exists, it returns a pointer to an empty dummy implementation
        /// (but for the convenience of the caller, it never returns NULL or an invalid pointer).
        //const CarriedWeaponT* GetCarriedWeapon(unsigned int ActiveWeaponSlot) const;

        /// Returns the singleton instance of this class.
        static GameImplT& GetInstance();


        private:

        /// The constructor is private because this is a singleton class.
        GameImplT();

        bool                      RunningAsClient;
        bool                      RunningAsServer;

        //ArrayT<const CafuModelT*> m_PlayerModels;   ///< The player models available in this game.
        //ArrayT<CarriedWeaponT*>   m_CarriedWeapons; ///< The set of carry-able weapons in this game.
        //ArrayT<SoundI*>           m_PreCacheSounds; ///< Array of all precached sounds.
    };
}

#endif
