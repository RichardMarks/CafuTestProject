-- Let DeathMatch maps be up to 131072 (65536*2) units big in each direction. This corresponds to a side length of 3329,23 km.
-- Note: If this gets any bigger than 65536, CaWE needs revision, especially cmap brush loading (starting digging from LoadSave_cmap.cpp)!
Mapsize={ -65536, 65536 }


-- Returns a clone of the given argument "node".
-- This is especially interesting when node is a table, because then a deep copy is made.
function clone(node)
    if type(node)~="table" then return node end

    -- node is a table, make a deep copy.
    local b={};
    table.foreach(node, function(k,v) b[k]=clone(v) end);

    return b;
end


-- This function takes a variable number of arguments, all of which must be of type "table".
-- It deep-copies all tables into one common table, and returns the combined result.
function newEntClassDef(...)
    local ResultTable={};

    for i, SubTable in ipairs{...} do
        -- Put a clone of each element of SubTable into ResultTable.
        for k, v in pairs(SubTable) do
            ResultTable[k]=clone(v);
        end
    end

    return ResultTable;
end


-- A base class with common definitions.
Common=
{
    name=
    {
        type       ="string";
        description="The name of this entity. It must be unique and non-empty.";
     -- editor_useredit=true;
        uniqueValue=true;
    };

    -- origin={ ... };
}


-- A base class for orientable entities.
Angles=
{
    angles=
    {
        type       ="string";
        description="Bank Heading Pitch (Y Z X)";
        value      ="0 0 0";
    };
}


-- This table contains the definitions of all entity classes in the game.
-- * Connection to C++ classes (CppClass key).
-- * Defines supported variables (properties, key/value pairs) by that (C++) entity class.
-- * Default values for instantiated entities.
-- * Auxiliary information for the Ca3DE World Editor CaWE.
EntityClassDefs={}


EntityClassDefs["worldspawn"]=newEntClassDef(Common,
{
    isSolid    =true;
    CppClass   ="EntWorldspawnT";
    description="The world entity.";

    message=
    {
        type       ="string";
        description="This value is shown when the map starts.";
     -- editor_useredit=true;
    };

    lightmap_patchsize=
    {
        type       ="string";
        description="LightMap PatchSize";
        value      ="200.0";
    };

    shlmap_patchsize=
    {
        type       ="string";
        description="SHL Map PatchSize";
        value      ="200.0";
    };
})


EntityClassDefs["info_generic"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntInfoGenericT";
    description="A generic entity that provides additional information at any given point in the map.\n" ..
                "It's main purpose is to mark points in the map that the map scripts can refer to.";
    size       ={ { -16, -16, -16 }, { 16, 16, 16 } };
    color      ={ 255, 255, 200 };      -- Very bright yellow.

    param1     ={ type="string"; description="A generic parameter for use by the map script."; };
    param2     ={ type="string"; description="A generic parameter for use by the map script."; };
    param3     ={ type="string"; description="A generic parameter for use by the map script."; };
})

-- Info Player Start.
-- This is not a game-specific entity, but rather an engine entity, simply because it is sensible for each game to have at least one such.
-- Moreover, CaBSP even requires for its proper operation that at least one such entity exists, and that they all are *INSIDE* the actual map!
EntityClassDefs["info_player_start"]=newEntClassDef(Common, Angles,
{
    isPoint    =true;
    CppClass   ="EntInfoPlayerStartT";
    description="A starting location for the player.";
    size       ={ { -16, -16, -36 }, { 16, 16, 36 } };      -- Rename in editor_bbox? Rename color in editor_bbox_color?
    color      ={ 0, 255, 0 };
})