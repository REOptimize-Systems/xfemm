/* Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of 
 * Mechatronics GmbH (LCM)
 *
 * The source code in this file is heavily derived from
 * FEMM by David Meeker <dmeeker@ieee.org>.
 * For more information on FEMM see http://www.femm.info
 * This modified version is not endorsed in any way by the original
 * authors of FEMM.
 *
 * License:
 * This software is subject to the Aladdin Free Public Licence
 * version 8, November 18, 1999.
 * The full license text is available in the file LICENSE.txt supplied
 * along with the source code.
 */

#ifndef LUAMAGNETICSCOMMANDS_H
#define LUAMAGNETICSCOMMANDS_H

class lua_State;

namespace femm {
class LuaInstance;
}

namespace femmcli
{

/**
 * LuaMagneticsCommands registers the lua commands related to magnetics.
 * The Lua magnetics command set is described in sections 3.3 and 3.4 of the FEMM manual.
 *
 */
class LuaMagneticsCommands
{
public:
    /**
     * @brief Register the common command set with the given LuaInstance
     * @param li a LuaInstance
     */
    static void registerCommands(femm::LuaInstance &li );

protected:

    static int luaAddArc(lua_State *L);
    static int luaAddBHPoint(lua_State *L);
    static int luaAddBoundaryProp(lua_State *L);
    static int luaAddCircuitProp(lua_State *L);
    static int luaAddContourPoint(lua_State *L);
    static int luaAddBlocklabel(lua_State *L);
    static int luaAddLine(lua_State *L);
    static int luaAddMatProp(lua_State *L);
    static int luaAddNode(lua_State *L);
    static int luaAddPointProp(lua_State *L);
    static int luaAnalyze(lua_State *L);
    static int luaAttachDefault(lua_State *L);
    static int luaAttachOuterSpace(lua_State *L);
    static int luaBendContourLine(lua_State *L);
    static int luaBlockIntegral(lua_State *L);
    static int luaClearBHPoints(lua_State *L);
    static int luaClearBlock(lua_State *L);
    static int luaClearContourPoint(lua_State *L);
    static int luaClearSelected(lua_State *L);
    static int luaCopyRotate(lua_State *L);
    static int luaCopyTranslate(lua_State *L);
    static int luaCreateMesh(lua_State *L);
    static int luaCreateRadius(lua_State *L);
    static int luaDefineOuterSpace(lua_State *L);
    static int luaDeleteBoundaryProperty(lua_State *L);
    static int luaDeleteCircuitProperty(lua_State *L);
    static int luaDeleteSelectedArcSegments(lua_State *L);
    static int luaDeleteSelectedBlockLabels(lua_State *L);
    static int luaDeleteSelected(lua_State *L);
    static int luaDeleteSelectedNodes(lua_State *L);
    static int luaDeleteSelectedSegments(lua_State *L);
    static int luaDeleteMaterial(lua_State *L);
    static int luaDeletePointProperty(lua_State *L);
    static int luaDetachDefault(lua_State *L);
    static int luaDetachOuterSpace(lua_State *L);
    static int luaExitPost(lua_State *L);
    static int luaExitPre(lua_State *L);
    static int luaGetBoundingBox(lua_State *L);
    static int luaGetCircuitProperties(lua_State *L);
    static int luaGetElement(lua_State *L);
    static int luaGetMaterialFromLib(lua_State *L);
    static int luaGetMeshNode(lua_State *L);
    static int luaGetPointVals(lua_State *L);
    static int luaGetProblemInfo(lua_State *L);
    static int luaGetTitle(lua_State *L);
    static int luaBGradient(lua_State *L);
    static int luaGroupSelectBlock(lua_State *L);
    static int luaLineIntegral(lua_State *L);
    static int luaMirrorCopy(lua_State *L);
    static int luaModifyBoundaryProp(lua_State *L);
    static int luaModifyCircuitProperty(lua_State *L);
    static int luaModifyMaterialProp(lua_State *L);
    static int luaModifyPointProp(lua_State *L);
    static int luaMoveRotate(lua_State *L);
    static int luaMoveTranslate(lua_State *L);
    static int luaNewDocument(lua_State *L);
    static int luaNumElements(lua_State *L);
    static int luaNumNodes(lua_State *L);
    static int luaProbDef(lua_State *L);
    static int luaPurgeMesh(lua_State *L);
    static int luaReloadNOP(lua_State *L);
    static int luaLoadSolution(lua_State *L);
    static int luaSaveDocument(lua_State *L);
    static int luaScaleMove(lua_State *L);
    static int luaSelectArcsegment(lua_State *L);
    static int luaSelectOutputBlocklabel(lua_State *L);
    static int luaSelectWithinCircle(lua_State *L);
    static int luaSelectGroup(lua_State *L);
    static int luaSelectBlocklabel(lua_State *L);
    static int luaAddContourPointFromNode(lua_State *L);
    static int luaSelectnode(lua_State *L);
    static int luaSelectWithinRectangle(lua_State *L);
    static int luaSelectSegment(lua_State *L);
    static int luaSetArcsegmentProp(lua_State *L);
    static int luaSetBlocklabelProp(lua_State *L);
    static int luaSetEditMode(lua_State *L);
    static int luaSetFocus(lua_State *L);
    static int luaSetGroup(lua_State *L);
    static int luaSetNodeProp(lua_State *L);
    static int luaSetPrevious(lua_State *L);
    static int luaSetSegmentProp(lua_State *L);
};

} /* namespace FemmLua*/

#endif /* LUAMAGNETICSCOMMANDS_H */
// vi:expandtab:tabstop=4 shiftwidth=4:

