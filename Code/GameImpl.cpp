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

#include "GameImpl.hpp"
#include "BaseEntity.hpp"
#include "EntityCreateParams.hpp"
#include "InfoGeneric.hpp"
#include "InfoPlayerStart.hpp"
#include "TypeSys.hpp"

#include "ConsoleCommands/Console.hpp"
#include <map>


// Intentionally use a named, non-anonymous namespace here to give AllTypeInfos[] external linkage.
// With an anonymous namespace, the compiler might optimize AllTypeInfos[] out, defeating its purpose.
#if 1
namespace GAME_NAME
{
    /*
     * The purpose of this code is to make sure that the constructors of all static TypeInfoT
     * members of all entity classes have been run and thus the TypeInfoTs all registered
     * themselves at the global type info manager (TypeInfoManT).
     *
     * Q: Why isn't that automatically the case, given that all TypeInfoTs are *static* members of
     *    their classes and supposed to be initialized before main() begins anyway?
     *
     * First, the C++ standard does not guarantee that nonlocal objects with static storage duration
     * are initialized before main() begins. Rather, their initialization can be deferred until
     * before their first use. This problem would be fixed by calling this function early in main(),
     * but as has been convincingly explained by James Kanze in [1], that is not an issue anyway:
     * Compilers just do not implement deferred initialization, mostly for backward-compatibility.
     *
     * The second and more important factor is the linker:
     * Both under Windows and Linux (and probably everywhere else), linkers include the symbols in
     * static libraries only in the executables if they resolve an unresolved external. This is
     * contrary to .obj files that are given to the linker directly [1].
     *
     * Thus, with the entity files all being part of a library, there are only two approaches to
     * make sure that all relevant units make it into the executable: Either pass the object files
     * directly and individually to the linker, or employ code like below.
     *
     * The problem with passing the individual object files is that this is difficult to implement
     * in SCons, and probably any other build system. Therefore, the only method for solving the
     * problem a reliable and portable manner that works well with any build system seems to be
     * the use of a method like below.
     *
     * [1] For more details, see the thread "Can initialization of static class members be forced
     *     before main?" that I've begun on 2008-Apr-03 in comp.lang.c++:
     *     http://groups.google.de/group/comp.lang.c++/browse_thread/thread/e264caa531ff52a9/
     *     Another very good explanation is at:
     *     http://blog.copton.net/articles/linker/index.html#linker-dependencies
     */

    const cf::TypeSys::TypeInfoT* AllTypeInfos[] = {
        &EntInfoGenericT::TypeInfo,
        &EntInfoPlayerStartT::TypeInfo,
    };
}
#endif

using namespace GAME_NAME;


// Static method that returns the singleton instance.
GameImplT& GameImplT::GetInstance()
{
    static GameImplT GI;

    return GI;
}


GameImplT::GameImplT()
    : RunningAsClient(false),
      RunningAsServer(false)
{
    //LookupTables::Initialize();

    GetBaseEntTIM().Init();
}


void GameImplT::Initialize(bool AsClient, bool AsServer, ModelManagerT& ModelMan)
{
    //m_PlayerModels.PushBack(ModelMan.GetModel("Games/DeathMatch/Models/Players/Alien/Alien.cmdl"));

    RunningAsClient=AsClient;
    RunningAsServer=AsServer;

    if (RunningAsClient)
    {
    }

    if (RunningAsServer)
    {
    }
}


void GameImplT::Release()
{
    if (RunningAsClient)
    {
    }

    if (RunningAsServer)
    {
    }
}


const cf::TypeSys::TypeInfoManT& GameImplT::GetEntityTIM() const
{
    return GetBaseEntTIM();
}


// This function is called by the server, in order to obtain a (pointer to a) 'BaseEntityT' from a map file entity.
IntrusivePtrT<GameEntityI> GameImplT::CreateGameEntityFromMapFile(const cf::TypeSys::TypeInfoT* TI, const std::map<std::string, std::string>& Properties,
    const cf::SceneGraph::GenericNodeT* RootNode, const cf::ClipSys::CollisionModelT* CollisionModel, unsigned long ID,
    unsigned long WorldFileIndex, unsigned long MapFileIndex, cf::GameSys::GameWorldI* GameWorld, const Vector3T<double>& Origin)
{
    // YES! THIS is how it SHOULD work!
    GameEntityI* NewEnt=static_cast<GameEntityI*>(TI->CreateInstance(
        EntityCreateParamsT(ID, Properties, RootNode, CollisionModel, WorldFileIndex, MapFileIndex, GameWorld, Origin)));

    assert(NewEnt!=NULL);
    assert(NewEnt->GetType()==TI);

    if (NewEnt==NULL)
    {
        Console->Warning("Could not create instance for type \"" + std::string(TI->ClassName) + "\".\n");
        TI->Print(false /*Don't print the child classes.*/);
        return NULL;
    }

    return NewEnt;
}


// This function is called by the client, in order to obtain a (pointer to a) 'BaseEntityT' for a new entity
// whose TypeNr and ID it got via a net message from the server.
IntrusivePtrT<GameEntityI> GameImplT::CreateGameEntityFromTypeNr(unsigned long TypeNr, const std::map<std::string, std::string>& Properties,
    const cf::SceneGraph::GenericNodeT* RootNode, const cf::ClipSys::CollisionModelT* CollisionModel,
    unsigned long ID, unsigned long WorldFileIndex, unsigned long MapFileIndex, cf::GameSys::GameWorldI* GameWorld)
{
    const cf::TypeSys::TypeInfoT* TI=GetBaseEntTIM().FindTypeInfoByNr(TypeNr);

    assert(TI!=NULL);
    assert(TI->TypeNr==TypeNr);
    assert(TI->CreateInstance!=NULL);

    return static_cast<GameEntityI*>(TI->CreateInstance(
        EntityCreateParamsT(ID, Properties, RootNode, CollisionModel, WorldFileIndex, MapFileIndex, GameWorld, VectorT())));
}
