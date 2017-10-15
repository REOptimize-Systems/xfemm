/* Copyright 2017 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
 * Contributions by Johannes Zarl-Zierl were funded by Linz Center of
 * Mechatronics GmbH (LCM)
 * Copyright 1998-2016 David Meeker <dmeeker@ieee.org>
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

#include "LuaHeatflowCommands.h"
#include "LuaCommonCommands.h"

#include "CHPointVals.h"
#include "hpproc.h"
#include "hsolver.h"
#include "femmconstants.h"
#include "femmenums.h"
#include "FemmState.h"
#include "LuaInstance.h"

#include <lua.h>

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef DEBUG_FEMMLUA
#define debug std::cerr
#else
#define debug while(false) std::cerr
#endif

using namespace femm;
using std::swap;

void femmcli::LuaHeatflowCommands::registerCommands(LuaInstance &li)
{
    li.addFunction("hi_add_arc", LuaCommonCommands::luaAddArc);
    li.addFunction("hi_addarc", LuaCommonCommands::luaAddArc);
    li.addFunction("hi_add_block_label", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("hi_addblocklabel", LuaCommonCommands::luaAddBlocklabel);
    li.addFunction("hi_add_bound_prop", luaAddBoundaryProperty);
    li.addFunction("hi_addboundprop", luaAddBoundaryProperty);
    li.addFunction("hi_add_conductor_prop", luaAddConductorProperty);
    li.addFunction("hi_addconductorprop", luaAddConductorProperty);
    li.addFunction("hi_add_material", luaAddMaterialProperty);
    li.addFunction("hi_addmaterial", luaAddMaterialProperty);
    li.addFunction("hi_add_node", LuaCommonCommands::luaAddNode);
    li.addFunction("hi_addnode", LuaCommonCommands::luaAddNode);
    li.addFunction("hi_add_point_prop", luaAddPointProperty);
    li.addFunction("hi_addpointprop", luaAddPointProperty);
    li.addFunction("hi_add_segment", LuaCommonCommands::luaAddLine);
    li.addFunction("hi_addsegment", LuaCommonCommands::luaAddLine);
    //li.addFunction("hi_add_tk_point", luaAddtkpoint);
    //li.addFunction("hi_addtkpoint", luaAddtkpoint);
    //li.addFunction("hi_analyse", luaAnalyze);
    //li.addFunction("hi_analyze", luaAnalyze);
    li.addFunction("hi_attach_default", LuaCommonCommands::luaAttachDefault);
    li.addFunction("hi_attachdefault", LuaCommonCommands::luaAttachDefault);
    li.addFunction("hi_attach_outer_space", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("hi_attachouterspace", LuaCommonCommands::luaAttachOuterSpace);
    li.addFunction("hi_clear_selected", LuaCommonCommands::luaClearSelected);
    li.addFunction("hi_clearselected", LuaCommonCommands::luaClearSelected);
    //li.addFunction("hi_clear_tk_points", luaCleartkpoints);
    //li.addFunction("hi_cleartkpoints", luaCleartkpoints);
    li.addFunction("hi_close", LuaCommonCommands::luaExitPre);
    li.addFunction("hi_copy_rotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("hi_copyrotate", LuaCommonCommands::luaCopyRotate);
    li.addFunction("hi_copy_translate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("hi_copytranslate", LuaCommonCommands::luaCopyTranslate);
    li.addFunction("hi_create_mesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("hi_createmesh", LuaCommonCommands::luaCreateMesh);
    li.addFunction("hi_create_radius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("hi_createradius", LuaCommonCommands::luaCreateRadius);
    li.addFunction("hi_define_outer_space", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("hi_defineouterspace", LuaCommonCommands::luaDefineOuterSpace);
    li.addFunction("hi_delete_bound_prop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("hi_deleteboundprop", LuaCommonCommands::luaDeleteBoundaryProperty);
    li.addFunction("hi_delete_conductor", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("hi_deleteconductor", LuaCommonCommands::luaDeleteCircuitProperty);
    li.addFunction("hi_delete_material", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("hi_deletematerial", LuaCommonCommands::luaDeleteMaterial);
    li.addFunction("hi_delete_point_prop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("hi_deletepointprop", LuaCommonCommands::luaDeletePointProperty);
    li.addFunction("hi_delete_selected_arcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("hi_deleteselectedarcsegments", LuaCommonCommands::luaDeleteSelectedArcSegments);
    li.addFunction("hi_delete_selected_labels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("hi_deleteselectedlabels", LuaCommonCommands::luaDeleteSelectedBlockLabels);
    li.addFunction("hi_delete_selected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("hi_deleteselected", LuaCommonCommands::luaDeleteSelected);
    li.addFunction("hi_delete_selected_nodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("hi_deleteselectednodes", LuaCommonCommands::luaDeleteSelectedNodes);
    li.addFunction("hi_delete_selected_segments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("hi_deleteselectedsegments", LuaCommonCommands::luaDeleteSelectedSegments);
    li.addFunction("hi_detach_default", LuaCommonCommands::luaDetachDefault);
    li.addFunction("hi_detachdefault", LuaCommonCommands::luaDetachDefault);
    li.addFunction("hi_detach_outer_space", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("hi_detachouterspace", LuaCommonCommands::luaDetachOuterSpace);
    li.addFunction("hi_getboundingbox", LuaCommonCommands::luaGetBoundingBox);
    //li.addFunction("hi_get_material", luaGetMaterialFromLib);
    //li.addFunction("hi_getmaterial", luaGetMaterialFromLib);
    li.addFunction("hi_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("hi_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("hi_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("hi_grid_snap", LuaInstance::luaNOP);
    li.addFunction("hi_gridsnap", LuaInstance::luaNOP);
    li.addFunction("hi_hide_grid", LuaInstance::luaNOP);
    li.addFunction("hi_hidegrid", LuaInstance::luaNOP);
    li.addFunction("hi_load_solution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("hi_loadsolution", LuaCommonCommands::luaLoadSolution);
    li.addFunction("hi_maximize", LuaInstance::luaNOP);
    li.addFunction("hi_minimize", LuaInstance::luaNOP);
    li.addFunction("hi_mirror", LuaCommonCommands::luaMirrorCopy);
    //li.addFunction("hi_modify_bound_prop", luaModifyBoundaryProperty);
    //li.addFunction("hi_modifyboundprop", luaModifyBoundaryProperty);
    //li.addFunction("hi_modify_conductor_prop", luaModifyConductorProperty);
    //li.addFunction("hi_modifyconductorprop", luaModifyConductorProperty);
    //li.addFunction("hi_modify_material", luaModifyMaterialProperty);
    //li.addFunction("hi_modifymaterial", luaModifyMaterialProperty);
    //li.addFunction("hi_modify_point_prop", luaModifyPointProperty);
    //li.addFunction("hi_modifypointprop", luaModifyPointProperty);
    li.addFunction("hi_move_rotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("hi_moverotate", LuaCommonCommands::luaMoveRotate);
    li.addFunction("hi_move_translate", LuaCommonCommands::luaMoveTranslate);
    li.addFunction("hi_movetranslate", LuaCommonCommands::luaMoveTranslate);
    //li.addFunction("hi_new_document", luaNewdocument);
    //li.addFunction("hi_newdocument", luaNewdocument);
    //li.addFunction("hi_prob_def", luaProblemDefinition);
    //li.addFunction("hi_probdef", luaProblemDefinition);
    li.addFunction("hi_purge_mesh", LuaCommonCommands::luaPurgeMesh);
    li.addFunction("hi_purgemesh", LuaCommonCommands::luaPurgeMesh);
    li.addFunction("hi_read_dxf", LuaInstance::luaNOP);
    li.addFunction("hi_readdxf", LuaInstance::luaNOP);
    li.addFunction("hi_refresh_view", LuaInstance::luaNOP);
    li.addFunction("hi_refreshview", LuaInstance::luaNOP);
    li.addFunction("hi_resize", LuaInstance::luaNOP);
    li.addFunction("hi_restore", LuaInstance::luaNOP);
    li.addFunction("hi_save_as", LuaCommonCommands::luaSaveDocument);
    li.addFunction("hi_saveas", LuaCommonCommands::luaSaveDocument);
    li.addFunction("hi_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("hi_savebitmap", LuaInstance::luaNOP);
    li.addFunction("hi_save_dxf", LuaInstance::luaNOP);
    li.addFunction("hi_savedxf", LuaInstance::luaNOP);
    li.addFunction("hi_save_metafile", LuaInstance::luaNOP);
    li.addFunction("hi_savemetafile", LuaInstance::luaNOP);
    li.addFunction("hi_scale", LuaCommonCommands::luaScaleMove);
    li.addFunction("hi_select_arcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("hi_selectarcsegment", LuaCommonCommands::luaSelectArcsegment);
    li.addFunction("hi_select_circle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("hi_selectcircle", LuaCommonCommands::luaSelectWithinCircle);
    li.addFunction("hi_select_group", LuaCommonCommands::luaSelectGroup);
    li.addFunction("hi_selectgroup", LuaCommonCommands::luaSelectGroup);
    li.addFunction("hi_select_label", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("hi_selectlabel", LuaCommonCommands::luaSelectBlocklabel);
    li.addFunction("hi_select_node", LuaCommonCommands::luaSelectNode);
    li.addFunction("hi_selectnode", LuaCommonCommands::luaSelectNode);
    li.addFunction("hi_select_rectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("hi_selectrectangle", LuaCommonCommands::luaSelectWithinRectangle);
    li.addFunction("hi_select_segment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("hi_selectsegment", LuaCommonCommands::luaSelectSegment);
    li.addFunction("hi_set_block_prop", LuaCommonCommands::luaSetBlocklabelProperty);
    li.addFunction("hi_setblockprop", LuaCommonCommands::luaSetBlocklabelProperty);
    li.addFunction("hi_set_edit_mode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("hi_seteditmode", LuaCommonCommands::luaSetEditMode);
    li.addFunction("hi_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("hi_setfocus", LuaCommonCommands::luaSetFocus);
    li.addFunction("hi_set_grid", LuaInstance::luaNOP);
    li.addFunction("hi_setgrid", LuaInstance::luaNOP);
    li.addFunction("hi_set_group", LuaCommonCommands::luaSetGroup);
    li.addFunction("hi_setgroup", LuaCommonCommands::luaSetGroup);
    //li.addFunction("hi_set_node_prop", luaSetNodeProperty);
    //li.addFunction("hi_setnodeprop", luaSetNodeProperty);
    //li.addFunction("hi_set_segment_prop", luaSetSegmentProperty);
    //li.addFunction("hi_setsegmentprop", luaSetSegmentProperty);
    li.addFunction("hi_show_grid", LuaInstance::luaNOP);
    li.addFunction("hi_showgrid", LuaInstance::luaNOP);
    li.addFunction("hi_show_mesh", LuaInstance::luaNOP);
    li.addFunction("hi_showmesh", LuaInstance::luaNOP);
    li.addFunction("hi_show_names", LuaInstance::luaNOP);
    li.addFunction("hi_shownames", LuaInstance::luaNOP);
    li.addFunction("hi_zoom_in", LuaInstance::luaNOP);
    li.addFunction("hi_zoomin", LuaInstance::luaNOP);
    li.addFunction("hi_zoom", LuaInstance::luaNOP);
    li.addFunction("hi_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("hi_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("hi_zoom_out", LuaInstance::luaNOP);
    li.addFunction("hi_zoomout", LuaInstance::luaNOP);

    li.addFunction("ho_add_contour", LuaCommonCommands::luaAddContourPoint);
    li.addFunction("ho_addcontour", LuaCommonCommands::luaAddContourPoint);
    li.addFunction("ho_bend_contour", LuaCommonCommands::luaBendContourLine);
    li.addFunction("ho_bendcontour", LuaCommonCommands::luaBendContourLine);
    //li.addFunction("ho_block_integral", luaBlockintegral);
    //li.addFunction("ho_blockintegral", luaBlockintegral);
    li.addFunction("ho_clear_block", LuaCommonCommands::luaClearBlockSelection);
    li.addFunction("ho_clearblock", LuaCommonCommands::luaClearBlockSelection);
    li.addFunction("ho_clear_contour", LuaCommonCommands::luaClearContourPoint);
    li.addFunction("ho_clearcontour", LuaCommonCommands::luaClearContourPoint);
    li.addFunction("ho_close", LuaCommonCommands::luaExitPost);
    //li.addFunction("ho_get_conductor_properties", luaGetConductorProperties);
    //li.addFunction("ho_getconductorproperties", luaGetConductorProperties);
    li.addFunction("ho_get_element", LuaCommonCommands::luaGetElement);
    li.addFunction("ho_getelement", LuaCommonCommands::luaGetElement);
    li.addFunction("ho_get_node", LuaCommonCommands::luaGetMeshNode);
    li.addFunction("ho_getnode", LuaCommonCommands::luaGetMeshNode);
    //li.addFunction("ho_get_point_values", luaGetPointValues);
    //li.addFunction("ho_getpointvalues", luaGetPointValues);
    li.addFunction("ho_get_problem_info", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("ho_getprobleminfo", LuaCommonCommands::luaGetProblemInfo);
    li.addFunction("ho_get_title", LuaCommonCommands::luaGetTitle);
    li.addFunction("ho_gettitle", LuaCommonCommands::luaGetTitle);
    li.addFunction("ho_grid_snap", LuaInstance::luaNOP);
    li.addFunction("ho_gridsnap", LuaInstance::luaNOP);
    li.addFunction("ho_group_select_block", LuaCommonCommands::luaGroupSelectBlock);
    li.addFunction("ho_groupselectblock", LuaCommonCommands::luaGroupSelectBlock);
    li.addFunction("ho_hide_contour_plot", LuaInstance::luaNOP);
    li.addFunction("ho_hidecontourplot", LuaInstance::luaNOP);
    li.addFunction("ho_hide_density_plot", LuaInstance::luaNOP);
    li.addFunction("ho_hidedensityplot", LuaInstance::luaNOP);
    li.addFunction("ho_hide_grid", LuaInstance::luaNOP);
    li.addFunction("ho_hidegrid", LuaInstance::luaNOP);
    li.addFunction("ho_hide_mesh", LuaInstance::luaNOP);
    li.addFunction("ho_hidemesh", LuaInstance::luaNOP);
    li.addFunction("ho_hide_points", LuaInstance::luaNOP);
    li.addFunction("ho_hidepoints", LuaInstance::luaNOP);
    //li.addFunction("ho_line_integral", luaLineIntegral);
    //li.addFunction("ho_lineintegral", luaLineIntegral);
    li.addFunction("ho_make_plot", LuaInstance::luaNOP);
    li.addFunction("ho_makeplot", LuaInstance::luaNOP);
    li.addFunction("ho_maximize", LuaInstance::luaNOP);
    li.addFunction("ho_minimize", LuaInstance::luaNOP);
    li.addFunction("ho_num_elements", LuaCommonCommands::luaNumElements);
    li.addFunction("ho_numelements", LuaCommonCommands::luaNumElements);
    li.addFunction("ho_num_nodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("ho_numnodes", LuaCommonCommands::luaNumNodes);
    li.addFunction("ho_refresh_view", LuaInstance::luaNOP);
    li.addFunction("ho_refreshview", LuaInstance::luaNOP);
    li.addFunction("ho_reload", LuaCommonCommands::luaLoadSolution);
    li.addFunction("ho_resize", LuaInstance::luaNOP);
    li.addFunction("ho_restore", LuaInstance::luaNOP);
    li.addFunction("ho_save_bitmap", LuaInstance::luaNOP);
    li.addFunction("ho_savebitmap", LuaInstance::luaNOP);
    li.addFunction("ho_save_metafile", LuaInstance::luaNOP);
    li.addFunction("ho_savemetafile", LuaInstance::luaNOP);
    li.addFunction("ho_select_block", LuaCommonCommands::luaSelectOutputBlocklabel);
    li.addFunction("ho_selectblock", LuaCommonCommands::luaSelectOutputBlocklabel);
    //li.addFunction("ho_select_conductor", luaSelectConductor);
    //li.addFunction("ho_selectconductor", luaSelectConductor);
    li.addFunction("ho_select_point", LuaCommonCommands::luaAddContourPointFromNode);
    li.addFunction("ho_selectpoint", LuaCommonCommands::luaAddContourPointFromNode);
    li.addFunction("ho_set_edit_mode", LuaInstance::luaNOP);
    li.addFunction("ho_seteditmode", LuaInstance::luaNOP);
    li.addFunction("ho_set_focus", LuaCommonCommands::luaSetFocus);
    li.addFunction("ho_setfocus", LuaCommonCommands::luaSetFocus);
    li.addFunction("ho_set_grid", LuaInstance::luaNOP);
    li.addFunction("ho_setgrid", LuaInstance::luaNOP);
    li.addFunction("ho_show_contour_plot", LuaInstance::luaNOP);
    li.addFunction("ho_showcontourplot", LuaInstance::luaNOP);
    li.addFunction("ho_show_density_plot", LuaInstance::luaNOP);
    li.addFunction("ho_showdensityplot", LuaInstance::luaNOP);
    li.addFunction("ho_show_grid", LuaInstance::luaNOP);
    li.addFunction("ho_showgrid", LuaInstance::luaNOP);
    li.addFunction("ho_show_mesh", LuaInstance::luaNOP);
    li.addFunction("ho_showmesh", LuaInstance::luaNOP);
    li.addFunction("ho_show_names", LuaInstance::luaNOP);
    li.addFunction("ho_shownames", LuaInstance::luaNOP);
    li.addFunction("ho_show_points", LuaInstance::luaNOP);
    li.addFunction("ho_showpoints", LuaInstance::luaNOP);
    li.addFunction("ho_show_vector_plot", LuaInstance::luaNOP);
    li.addFunction("ho_showvectorplot", LuaInstance::luaNOP);
    //li.addFunction("ho_smooth", luaSmoothing); // -> FIXME; implement for others as well
    li.addFunction("ho_zoom_in", LuaInstance::luaNOP);
    li.addFunction("ho_zoomin", LuaInstance::luaNOP);
    li.addFunction("ho_zoom", LuaInstance::luaNOP);
    li.addFunction("ho_zoom_natural", LuaInstance::luaNOP);
    li.addFunction("ho_zoomnatural", LuaInstance::luaNOP);
    li.addFunction("ho_zoom_out", LuaInstance::luaNOP);
    li.addFunction("ho_zoomout", LuaInstance::luaNOP);
}

/**
 * @brief Add a new boundary property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaHF
 *
 * \internal
 * ### Implements:
 * - \lua{hi_addboundprop("boundpropname", BdryFormat, Tset, qs, Tinf, h, beta)}
 *
 * ### FEMM sources:
 * - \femm42{femm/HDRAWLUA.cpp,lua_addboundprop()}
 * \endinternal
 */
int femmcli::LuaHeatflowCommands::luaAddBoundaryProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::unique_ptr<CHBoundaryProp> m = std::make_unique<CHBoundaryProp>();

    int n=lua_gettop(L);
    if (n>0) m->BdryName = lua_tostring(L,1);
    if(n>1) m->BdryFormat=(int) lua_todouble(L,2);
    if(n>2) m->Tset=lua_todouble(L,3);
    if(n>3) m->qs=lua_todouble(L,4);
    if(n>4) m->Tinf=lua_todouble(L,5);
    if(n>5) m->h=lua_todouble(L,6);
    if(n>6) m->beta=lua_todouble(L,7);

    doc->lineproplist.push_back(std::move(m));
    doc->updateLineMap();
    return 0;
}

/**
 * @brief Add a new circuit property with a given name.
 * @param L
 * @return 0
 * \ingroup LuaHF
 *
 * \internal
 * ### Implements:
 * - \lua{hi_addconductorprop("conductorname", Tc, qc, conductortype)}
 *
 * ### FEMM sources:
 * - \femm42{femm/HDRAWLUA.cpp,lua_addcircuitprop()}
 * \endinternal
 */
int femmcli::LuaHeatflowCommands::luaAddConductorProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::unique_ptr<CHConductor> m = std::make_unique<CHConductor>();
    int n=lua_gettop(L);

    if(n>0) m->CircName=lua_tostring(L,1);
    if(n>1) m->V=lua_todouble(L,2); // ChdrawDoc calls the Tc parameter "T", not "V"
    if(n>2) m->q=lua_todouble(L,3);
    if(n>3) m->CircType=(int) lua_todouble(L,4);


    femmState->femmDocument()->circproplist.push_back(std::move(m));
    femmState->femmDocument()->updateCircuitMap();

    return 0;
}

/**
 * @brief Add a new material property.
 * @param L
 * @return 0
 * \ingroup LuaHF
 *
 * \internal
 * ### Implements:
 * - \lua{hi_add_material("materialname", kx, ky, qv, kt)}
 *
 * ### FEMM sources:
 * - \femm42{femm/HDRAWLUA.cpp,lua_addmatprop()}
 * \endinternal
 */
int femmcli::LuaHeatflowCommands::luaAddMaterialProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());

    std::unique_ptr<CHMaterialProp> m = std::make_unique<CHMaterialProp>();
    int n=lua_gettop(L);

    if (n>0) {
        m->BlockName = lua_tostring(L,1);
    }
    if (n>1) {
          m->Kx = lua_todouble(L,2);
          m->Ky = m->Kx;
    }
    if(n>2) m->Ky = lua_todouble(L,3);
    if(n>3) m->qv = lua_todouble(L,4);
    if(n>3) m->Kt = lua_todouble(L,5);

    femmState->femmDocument()->blockproplist.push_back(std::move(m));
    femmState->femmDocument()->updateBlockMap();
    return 0;
}

/**
 * @brief Add a new point property.
 * @param L
 * @return 0
 * \ingroup LuaHF
 *
 * \internal
 * ### Implements:
 * - \lua{hi_addpointprop("pointpropname",Tp,qp)}
 *
 * ### FEMM sources:
 * - \femm42{femm/HDRAWLUA.cpp,lua_addpointprop()}
 * \endinternal
 */
int femmcli::LuaHeatflowCommands::luaAddPointProperty(lua_State *L)
{
    auto luaInstance = LuaInstance::instance(L);
    std::shared_ptr<FemmState> femmState = std::dynamic_pointer_cast<FemmState>(luaInstance->femmState());
    std::shared_ptr<FemmProblem> doc = femmState->femmDocument();

    std::unique_ptr<CHPointProp> m = std::make_unique<CHPointProp>();
    int n=lua_gettop(L);

    if (n>0) m->PointName = lua_tostring(L,1);
    if (n>1) m->V = lua_todouble(L,2); // called T in ChdrawDoc's CPointProp
    if (n>2) m->qp = lua_todouble(L,3);

    doc->nodeproplist.push_back(std::move(m));
    doc->updateNodeMap();
    return 0;
}
