#include "FemmProblem.h"

#include "femmconstants.h"

#include <ctgmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>

femm::FemmProblem::~FemmProblem()
{
}

bool femm::FemmProblem::saveFEMFile(std::string &filename) const
{
    if ( filetype == FileType::Unknown )
    {
        // can't save if file type is not set
        std::cerr << "Cannot save file because file type is unknown!\n";
        return false;
    }
    std::ofstream fem (filename);
    if ( fem.fail() )
    {
        // open failed
        std::cerr << "Opening file " << filename << " for writing failed!\n";
        return false;
    }

    writeProblemDescription(fem);
    return true;
}

void femm::FemmProblem::writeProblemDescription(std::ostream &output) const
{
    // set floating point precision once for the whole stream
    output << std::setprecision(17);
    // when filling to a width, adjust to the left
    output.setf(std::ios::left);
    // width and fill-direction are only used to emulate the output of femm42 as close as possible.

    // ***** write properties ****
    if (filetype == FileType::MagneticsFile)
        output << "[Format]      =  4.0\n";
    else
        output << "[Format]      =  1\n";
    if (filetype != FileType::ElectrostaticsFile)
    {
        output.width(12);
        output << "[Frequency]" << "  =  " << Frequency << "\n";
    }
    output.width(12);
    output << "[Precision]" << "  =  " << Precision << "\n";
    output.width(12);
    output << "[MinAngle]" << "  =  " << MinAngle << "\n";
    output.width(12);
    output << "[Depth]" << "  =  " << Depth << "\n";
    output << "[LengthUnits] =  ";
    switch (LengthUnits) {
    case femm::LengthMillimeters:
        output << "millimeters\n";
        break;
    case femm::LengthCentimeters:
        output << "centimeters\n";
        break;
    case femm::LengthMeters:
        output << "meters\n";
        break;
    case femm::LengthMils:
        output << "mils\n";
        break;
    case femm::LengthMicrometers:
        output << "microns\n";
        break;
    case femm::LengthInches:
        output << "inches\n";
        break;
    }

    if (ProblemType == femm::PLANAR)
    {
        output << "[ProblemType] =  planar\n";
    } else {
        output << "[ProblemType] =  axisymmetric\n";
        if ( extRo != 0 && extRi != 0)
        {
            output.width(12);
            output << "[extZo]" << "  =  " << extZo << "\n";
            output.width(12);
            output << "[extRo]" << "  =  " << extRo << "\n";
            output.width(12);
            output << "[extRi]" << "  =  " << extRi << "\n";
        }
    }
    if (Coords == femm::CART)
    {
        output << "[Coordinates] =  cartesian\n";
    } else {
        output << "[Coordinates] =  polar\n";
    }

    if (filetype == FileType::MagneticsFile)
    {
        output.width(12);
        output << "[ACSolver]" << "  =  " << ACSolver <<"\n";
    }
    if (!PrevSoln.empty())
    {
        output.width(12);
        output << "[PrevSoln]" << "  = \"" << PrevSoln <<"\"\n";
    }

    std::string commentString (comment);
    // escape line-breaks
    size_t pos = commentString.find('\n');
    while (pos != std::string::npos)
        commentString.replace(pos,1,"\\n");
    output.width(12);
    output << "[Comment]" << "  =  \"" << commentString <<"\"\n";

    // ***** write data ****
    output.width(12);
    output << "[PointProps]" << "  =  " << nodeproplist.size() <<"\n";
    for( const auto &prop: nodeproplist)
    {
        prop->toStream(output);
    }

    output.width(12);
    output << "[BdryProps]" << "  = " << lineproplist.size() <<"\n";
    for( const auto &prop: lineproplist)
    {
        prop->toStream(output);
    }

    output.width(12);
    output << "[BlockProps]" << "  = " << blockproplist.size() <<"\n";
    for( const auto &prop: blockproplist)
    {
        prop->toStream(output);
    }

    std::string circuitHeader;
    if (filetype == FileType::MagneticsFile)
    {
        circuitHeader =  "[CircuitProps]";
    } else {
        circuitHeader =  "[ConductorProps]";
    }

    output.width(12);
    output << circuitHeader << "  = " << circproplist.size() <<"\n";
    for( const auto &prop: circproplist)
    {
        prop->toStream(output);
    }

    // the solver does not even read this, but write it anyways...
    output << "[NumPoints] = " << nodelist.size() <<"\n";
    for (const auto &node: nodelist)
    {
        output << node->x
               << "\t" << node->y
               << "\t" << node->BoundaryMarker+1
               << "\t" << node->InGroup;

        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << node->InConductor+1;
        }
        output << "\n";
    }
    // the solver does not even read this, but write it anyways...
    output << "[NumSegments] = " << linelist.size() <<"\n";
    for (const auto &line: linelist)
    {
        // n0 n1 MaxSideLength BoundaryMarker Hidden Group [InConductor]
        output << line->n0 << "\t" << line->n1;
        if (line->MaxSideLength < 0)
        {
            output << "\t-1";
        } else {
            output << "\t" << line->MaxSideLength;
        }

        output << "\t" << line->BoundaryMarker+1
            << "\t" << (int)line->Hidden
            << "\t" << line->InGroup;

        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << line->InConductor+1;
        }
        output << "\n";
    }
    // the solver does not even read this, but write it anyways...
    output << "[NumArcSegments] = " << arclist.size() <<"\n";
    for (const auto &arc: arclist)
    {
        // n0 n1 ArcLength MaxSideLength BoundaryMarker Hidden Group [InConductor]
        output << arc->n0 << "\t" << arc->n1
            << "\t" << arc->ArcLength << "\t" << arc->MaxSideLength
            << "\t" << arc->BoundaryMarker+1
            << "\t" << (int)arc->Hidden
            << "\t" << arc->InGroup;


        if ( filetype == FileType::HeatFlowFile ||
             filetype == FileType::ElectrostaticsFile )
        {
            output << "\t" << arc->InConductor+1;
        }
        output << "\n";
    }
    // write out list of holes;
    int numHoles=0;
    for (const auto &label: labellist)
    {
        // count holes
        if(!label->hasBlockType())
            numHoles++;
    }
    output << "[NumHoles] = " << numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(!label->hasBlockType())
            output << label->x << " " << label->y << " " << label->InGroup << "\n";
    }

    output << "[NumBlockLabels] = " << labellist.size()-numHoles << "\n";
    for (const auto &label: labellist)
    {
        if(label->hasBlockType())
        {
            label->toStream(output);
        }
    }
}

void femm::FemmProblem::updateLabelsFromIndex()
{
    // block labels
    for (auto &label: labellist)
    {
        if (label->hasBlockType())
            label->BlockTypeName = blockproplist[label->BlockType]->BlockName;
        if (label->isInCircuit())
            label->InCircuitName = circproplist[label->InCircuit]->CircName;
    }
    // points
    for (auto &node: nodelist)
    {
        if ( node->hasBoundaryMarker())
            node->BoundaryMarkerName = nodeproplist[node->BoundaryMarker]->PointName;
        if ( node->isInConductor())
            node->InConductorName = circproplist[node->InConductor]->CircName;
    }
    // segments
    for (auto &segm: linelist)
    {
        if (segm->hasBoundaryMarker())
            segm->BoundaryMarkerName = lineproplist[segm->BoundaryMarker]->BdryName;
        if (segm->isInConductor())
            segm->InConductorName = circproplist[segm->InConductor]->CircName;
    }
    // arc segments
    for (auto &asegm: arclist)
    {
        if (asegm->hasBoundaryMarker())
            asegm->BoundaryMarkerName = lineproplist[asegm->BoundaryMarker]->BdryName;
        if (asegm->isInConductor())
            asegm->InConductorName = circproplist[asegm->InConductor]->CircName;
    }
}

void femm::FemmProblem::updateBlockMap()
{
    int idx=0;
    blockMap.clear();
    for (const auto &bprop: blockproplist)
    {
        blockMap[bprop->BlockName] = idx++;
    }
}

void femm::FemmProblem::updateCircuitMap()
{
    int idx=0;
    circuitMap.clear();
    for (const auto &circuit: circproplist)
    {
        circuitMap[circuit->CircName] = idx++;
    }
}

void femm::FemmProblem::updateLineMap()
{
    int idx=0;
    lineMap.clear();
    for (const auto &bprop: lineproplist)
    {
        lineMap[bprop->BdryName] = idx++;
    }
}

void femm::FemmProblem::updateNodeMap()
{
    int idx=0;
    nodeMap.clear();
    for (const auto &pprop: nodeproplist)
    {
        nodeMap[pprop->PointName] = idx++;
    }
}

void femm::FemmProblem::invalidateMesh()
{
    meshnodes.clear();
    meshelems.clear();
}

bool femm::FemmProblem::isMeshed() const
{
    return !(meshelems.empty() && meshnodes.empty());
}

bool femm::FemmProblem::addNode(double x, double y, double d)
{
    // create an appropriate node and call addNode on it
    std::unique_ptr<CNode> node = std::make_unique<CNode>(x,y);
    return addNode(std::move(node), d);
}

bool femm::FemmProblem::addNode(std::unique_ptr<femm::CNode> &&node, double d)
{
    CComplex c,a0,a1,a2;
    double R;
    double x = node->x;
    double y = node->y;

    // test to see if ``too close'' to existing node...
    for (int i=0; i<(int)nodelist.size(); i++)
        if(nodelist[i]->GetDistance(x,y)<d) return false;

    // can't put a node on top of a block label; do same sort of test.
    for (int i=0;i<(int)labellist.size();i++)
        if(labellist[i]->GetDistance(x,y)<d) return false;

    // if all is OK, add point in to the node list...
    nodelist.push_back(std::move(node));

    // test to see if node is on an existing line; if so,
    // break into two lines;

    for(int i=0, k=(int)linelist.size(); i<k; i++)
    {
        if (fabs(shortestDistanceFromSegment(x,y,i))<d)
        {
            std::unique_ptr<CSegment> segm;
            segm = std::make_unique<CSegment>(*linelist[i]);
            linelist[i]->n1=nodelist.size()-1;
            segm->n0=nodelist.size()-1;
            linelist.push_back(std::move(segm));
        }
    }

    // test to see if node is on an existing arc; if so,
    // break into two arcs;
    for(int i=0, k=(int)arclist.size(); i<k; i++)
    {
        if (shortestDistanceFromArc(CComplex(x,y),*arclist[i])<d)
        {
            a0.Set(nodelist[arclist[i]->n0]->x,nodelist[arclist[i]->n0]->y);
            a1.Set(nodelist[arclist[i]->n1]->x,nodelist[arclist[i]->n1]->y);
            a2.Set(x,y);
            getCircle(*arclist[i],c,R);

            std::unique_ptr<CArcSegment> asegm;
            asegm = std::make_unique<CArcSegment>(*arclist[i]);
            arclist[i]->n1 = nodelist.size()-1;
            arclist[i]->ArcLength = arg((a2-c)/(a0-c))*180./PI;
            asegm->n0 = nodelist.size()-1;
            asegm->ArcLength = arg((a1-c)/(a2-c))*180./PI;
            arclist.push_back(std::move(asegm));
        }
    }
    return true;
}



bool femm::FemmProblem::addArcSegment(femm::CArcSegment &asegm, double tol)
{
    // don't add if line is degenerate
    if (asegm.n0==asegm.n1)
        return false;

    // don't add if the arc is already in the list;
    for(int i=0; i<(int)arclist.size(); i++){
        if ((arclist[i]->n0==asegm.n0) && (arclist[i]->n1==asegm.n1) &&
                (fabs(arclist[i]->ArcLength-asegm.ArcLength)<1.e-02)) return false;
        // arcs are ``the same'' if start and end points are the same, and if
        // the arc lengths are relatively close (but a lot farther than
        // machine precision...
    }

    // add proposed arc to the linelist
    asegm.IsSelected = 0;

    CComplex p[2];
    std::vector < CComplex > newnodes;
    // check to see if there are intersections
    for(int i=0; i<(int)linelist.size(); i++)
    {
        int j = getLineArcIntersection(*linelist[i],asegm,p);
        if (j>0)
            for(int k=0; k<j; k++)
                newnodes.push_back(p[k]);
    }
    for (int i=0; i<(int)arclist.size(); i++)
    {
        int j = getArcArcIntersection(asegm,*arclist[i],p);
        if (j>0)
            for(int k=0; k<j; k++)
                newnodes.push_back(p[k]);
    }

    // add nodes at intersections
    double t;
    if (tol==0)
    {
        if (nodelist.size()<2) t=1.e-08;
        else{
            CComplex p0,p1;
            p0 = nodelist[0]->CC();
            p1 = p0;
            for (int i=1; i<(int)nodelist.size(); i++)
            {
                if(nodelist[i]->x<p0.re) p0.re = nodelist[i]->x;
                if(nodelist[i]->x>p1.re) p1.re = nodelist[i]->x;
                if(nodelist[i]->y<p0.im) p0.im = nodelist[i]->y;
                if(nodelist[i]->y>p1.im) p1.im = nodelist[i]->y;
            }
            t = abs(p1-p0)*CLOSE_ENOUGH;
        }
    }
    else t = tol;

    for (int i=0; i<(int)newnodes.size(); i++)
        addNode(newnodes[i].re,newnodes[i].im,t);

    // add proposed arc segment;
    arclist.push_back(std::make_unique<CArcSegment>(asegm));

    // check to see if proposed arc passes through other points;
    // if so, delete arc and create arcs that link intermediate points;
    // does this by recursive use of AddArcSegment;

    unselectAll();
    CComplex c;
    double R;
    getCircle(asegm,c,R);

    double dmin = tol;
    if (tol==0)
        dmin = fabs(R*PI*asegm.ArcLength/180.)*1.e-05;

    int k = (int)arclist.size()-1;
    for(int i=0; i<(int)nodelist.size(); i++)
    {
        if( (i!=asegm.n0) && (i!=asegm.n1) )
        {
            double d=shortestDistanceFromArc(CComplex(nodelist[i]->x,nodelist[i]->y),*arclist[k]);

            //	MsgBox("d=%g dmin=%g",d,dmin);
            // what is the purpose of this test?
            //	if (abs(nodelist[i]->CC()-nodelist[asegm.n0]->CC())<2.*dmin) d=2.*dmin;
            //	if (abs(nodelist[i]->CC()-nodelist[asegm.n1]->CC())<2.*dmin) d=2.*dmin;


            if (d<dmin){

                CComplex a0,a1,a2;
                a0.Set(nodelist[asegm.n0]->x,nodelist[asegm.n0]->y);
                a1.Set(nodelist[asegm.n1]->x,nodelist[asegm.n1]->y);
                a2.Set(nodelist[i]->x,nodelist[i]->y);
                arclist[k]->ToggleSelect();
                deleteSelectedArcSegments();

                CArcSegment newarc = asegm;
                newarc.n1 = i;
                newarc.ArcLength = arg((a2-c)/(a0-c))*180./PI;
                addArcSegment(newarc,dmin);

                newarc = asegm;
                newarc.n0 = i;
                newarc.ArcLength = arg((a1-c)/(a2-c))*180./PI;
                addArcSegment(newarc,dmin);

                i = nodelist.size();
            }
        }
    }

    return true;
}



// identical in fmesher, FPProc and HPProc
int femm::FemmProblem::closestArcSegment(double x, double y) const
{
    if(arclist.size()==0) return -1;

    double d0=shortestDistanceFromArc(CComplex(x,y),*arclist[0]);
    int idx=0;
    for(int i=0; i<(int)arclist.size(); i++)
    {
        double d1=shortestDistanceFromArc(CComplex(x,y),*arclist[i]);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

int femm::FemmProblem::closestBlockLabel(double x, double y) const
{
    if(labellist.size()==0) return -1;

    int idx=0;
    double d0=labellist[0]->GetDistance(x,y);
    for(int i=0; i<(int)labellist.size(); i++)
    {
        double d1=labellist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

// identical in fmesher, FPProc, and HPProc
int femm::FemmProblem::closestNode(double x, double y) const
{
    if(nodelist.size()==0) return -1;

    int idx=0;
    double d0=nodelist[0]->GetDistance(x,y);
    for(int i=0; i<(int)nodelist.size(); i++)
    {
        double d1=nodelist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

// identical in fmesher, hpproc
int femm::FemmProblem::closestSegment(double x, double y) const
{
    if(linelist.size()==0) return -1;

    int idx=0;
    double d0=shortestDistanceFromSegment(x,y,0);
    for(int i=0; i<(int)linelist.size(); i++)
    {
        double d1=shortestDistanceFromSegment(x,y,i);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

bool femm::FemmProblem::consistencyCheckOK() const
{
    using std::to_string;
    bool ok = true;
    // block labels
    for (const auto &label: labellist)
    {
        if (label->hasBlockType())
        {
            int idx = label->BlockType;
            if ( idx < 0 || idx >= (int)blockproplist.size() )
            {
                std::cerr << "Label has invalid block type index: " << to_string(idx)
                          << " (0 <= index < " << to_string(blockproplist.size()) << ")\n";
                ok = false;
            } else if (label->BlockTypeName != blockproplist[idx]->BlockName)
            {
                std::cerr << "Label has inconsistent block type name: " << label->BlockTypeName
                          << ", but block type " << to_string(idx)
                          << " has name " << blockproplist[idx]->BlockName << "\n";
                ok = false;
            }
        }
        if (label->isInCircuit())
        {
            int idx = label->InCircuit;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Label has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (label->InCircuitName != circproplist[idx]->CircName)
            {
                std::cerr << "Label has inconsistent circuit name: " << label->InCircuitName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // points
    for (const auto &node: nodelist)
    {
        if (node->hasBoundaryMarker())
        {
            int idx = node->BoundaryMarker;
            if ( idx < 0 || idx >= (int)nodeproplist.size() )
            {
                std::cerr << "Point has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(nodeproplist.size()) << ")\n";
                ok = false;
            } else if (node->BoundaryMarkerName != nodeproplist[idx]->PointName)
            {
                std::cerr << "Point has inconsistent boundary marker name: " << node->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << nodeproplist[idx]->PointName << "\n";
                ok = false;
            }
        }
        if (node->isInConductor())
        {
            int idx = node->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid conductor index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (node->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent conductor name: " << node->InConductorName
                          << ", but conductor " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // segments
    for (const auto &segm: linelist)
    {
        if (segm->hasBoundaryMarker())
        {
            int idx = segm->BoundaryMarker;
            if ( idx < 0 || idx >= (int)lineproplist.size() )
            {
                std::cerr << "Line has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(lineproplist.size()) << ")\n";
                ok = false;
            } else if (segm->BoundaryMarkerName != lineproplist[idx]->BdryName)
            {
                std::cerr << "Line has inconsistent boundary marker name: " << segm->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << lineproplist[idx]->BdryName << "\n";
                ok = false;
            }
        }
        if (segm->isInConductor())
        {
            int idx = segm->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (segm->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent circuit name: " << segm->InConductorName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    // arc segments
    for (const auto &asegm: arclist)
    {
        if (asegm->hasBoundaryMarker())
        {
            int idx = asegm->BoundaryMarker;
            if ( idx < 0 || idx >= (int)lineproplist.size() )
            {
                std::cerr << "Line has invalid boundary marker index: " << to_string(idx)
                          << " (0 <= index < " << to_string(lineproplist.size()) << ")\n";
                ok = false;
            } else if (asegm->BoundaryMarkerName != lineproplist[idx]->BdryName)
            {
                std::cerr << "Line has inconsistent boundary marker name: " << asegm->BoundaryMarkerName
                          << ", but boundary marker " << to_string(idx)
                          << " has name " << lineproplist[idx]->BdryName << "\n";
                ok = false;
            }
        }
        if (asegm->isInConductor())
        {
            int idx = asegm->InConductor;
            if ( idx < 0 || idx >= (int)circproplist.size() )
            {
                std::cerr << "Point has invalid circuit index: " << to_string(idx)
                          << " (0 <= index < " << to_string(circproplist.size()) << ")\n";
                ok = false;
            } else if (asegm->InConductorName != circproplist[idx]->CircName)
            {
                std::cerr << "Point has inconsistent circuit name: " << asegm->InConductorName
                          << ", but circuit " << to_string(idx)
                          << " has name " << circproplist[idx]->CircName << "\n";
                ok = false;
            }
        }
    }
    return ok;
}

bool femm::FemmProblem::canCreateRadius(int n) const
{
    // check to see if a selected point, specified by n, can be
    // converted to a radius.  To be able to be converted to a radius,
    // the point must be an element of either 2 lines, 2 arcs, or
    // 1 line and 1 arc.

    int j=0;
    for (const auto &line: linelist)
    {
        if (n==line->n0 || n==line->n1)
            j++;
    }
    for (const auto &arc: arclist)
    {
        if (n==arc->n0 || n==arc->n1)
            j++;
    }

    return (j==2);
}

bool femm::FemmProblem::createRadius(int n, double r)
{
    // replace the node indexed by n with a radius of r

    if(r<=0)
        return false;

    int nseg=0;
    int narc=0;
    int arc[2],seg[2];

    for(int k=0; k<(int)linelist.size(); k++)
        if ((linelist[k]->n0==n) || (linelist[k]->n1==n))
            seg[nseg++]=k;
    for(int k=0; k<(int)arclist.size(); k++)
        if ((arclist[k]->n0==n) || (arclist[k]->n1==n))
            arc[narc++]=k;

    if ((narc+nseg)!=2)
        return false;

    // there are three valid cases:
    switch (nseg-narc)
    {
    case 0:  // One arc and one line
    {
        CComplex c,u,p0,p1,q,p[4],v[8],i1[8],i2[8];
        double rc,b,R[4],phi;
        CArcSegment ar;

        // inherit the boundary condition from the arc so that
        // we can apply it to the newly created arc later;
        ar.InGroup       =arclist[arc[0]]->InGroup;
        ar.BoundaryMarkerName=arclist[arc[0]]->BoundaryMarkerName;

        // get the center and radius of the circle associated with the arc;
        getCircle(*arclist[arc[0]],c,rc);

        // get the locations of the endpoints of the segment;
        p0=nodelist[n]->CC();
        if(linelist[seg[0]]->n0==n)
            p1=nodelist[linelist[seg[0]]->n1]->CC();
        else
            p1=nodelist[linelist[seg[0]]->n0]->CC();

        u=(p1-p0)/abs(p1-p0);  // unit vector along the line
        q=p0 + u*Re((c-p0)/u); // closest point on line to center of circle
        u=(q-c)/abs(q-c); // unit vector from center to closest point on line;

        p[0]=q+r*u; R[0]=rc+r;
        p[1]=q-r*u; R[1]=rc+r;
        p[2]=q+r*u; R[2]=rc-r;
        p[3]=q-r*u; R[3]=rc-r;

        int j=0;
        for(int k=0;k<4;k++)
        {
            b=R[k]*R[k]-abs(p[k]-c)*abs(p[k]-c);
            if (b>=0){
                b=sqrt(b);
                v[j++]=p[k]+I*b*(p[k]-c)/abs(p[k]-c);
                v[j++]=p[k]-I*b*(p[k]-c)/abs(p[k]-c);
            }
        }

        // locations of the centerpoints that could be for the radius that
        // we are looking for are stored in v.  We now need to paw through
        // them to find the one solution that we are after.
        u=(p1-p0)/abs(p1-p0);  // unit vector along the line
        int m=0;
        for(int k=0;k<j;k++)
        {
            i1[m]=p0 +u*Re((v[k]-p0)/u); // intersection with the line
            i2[m]=c + rc*(v[k]-c)/abs(v[k]-c); // intersection with the arc;
            v[m]=v[k];

            // add this one to the list of possibly valid solutions if
            // both of the intersection points actually lie on the arc
            if ( shortestDistanceFromArc(i2[m],*arclist[arc[0]])<(r/10000.) &&
                 shortestDistanceFromSegment(Re(i1[m]),Im(i1[m]),seg[0])<(r/10000.)
                 && abs(i1[m]-i2[m])>(r/10000.))
            {
                m++;
                if (m==2) break;
            }

        }

        if (m==0) return false;

        // But there are also special cases where there could be two answers.
        // We then pick the solution that has the center point closest to the point to be removed.
        if(m>1)
        {
            if (abs(v[0]-p0)<abs(v[1]-p0)) j=0;
            else j=1;
        }
        else j=0;	// The index of the winning case is in j....

        updateUndo();
        addNode(Re(i1[j]),Im(i1[j]),r/10000.);
        addNode(Re(i2[j]),Im(i2[j]),r/10000.);
        unselectAll();

        // delete the node that is to be replace by a radius;
        n=closestNode(Re(p0),Im(p0));
        nodelist[n]->IsSelected=true;
        deleteSelectedNodes();

        // compute the angle spanned by the new arc;
        phi=arg((i2[j]-v[j])/(i1[j]-v[j]));
        if (phi<0)
        {
            c=i2[j]; i2[j]=i1[j]; i1[j]=c;
            phi=fabs(phi);
        }

        // add in the new radius;
        ar.n0=closestNode(Re(i1[j]),Im(i1[j]));
        ar.n1=closestNode(Re(i2[j]),Im(i2[j]));
        ar.ArcLength=phi/DEG;
        addArcSegment(ar);

        return true;
    }
    case 2:  // Two lines
    {
        CComplex p0,p1,p2;
        double phi,len;
        CArcSegment ar;

        if (linelist[seg[0]]->n0==n) p1=nodelist[linelist[seg[0]]->n1]->CC();
        else p1=nodelist[linelist[seg[0]]->n0]->CC();

        if (linelist[seg[1]]->n0==n) p2=nodelist[linelist[seg[1]]->n1]->CC();
        else p2=nodelist[linelist[seg[1]]->n0]->CC();

        p0=nodelist[n]->CC();

        // get the angle between the lines
        phi=arg((p2-p0)/(p1-p0));

        // check to see if this case is degenerate
        if (fabs(phi)>(179.*DEG)) return false;

        // check to see if the points are in the wrong order
        // and fix it if they are.
        if (phi<0){
            p0=p1; p1=p2; p2=p0; p0=nodelist[n]->CC();
            std::swap(seg[0],seg[1]);
            phi=fabs(phi);
        }

        len = r/tan(phi/2.); // distance from p0 to the tangency point;

        // catch the case where the desired radius is too big to fit;
        if ((abs(p1-p0)<len) || (abs(p2-p0)<len)) return false;

        // compute the locations of the tangency points;
        p1=len*(p1-p0)/abs(p1-p0)+p0;
        p2=len*(p2-p0)/abs(p2-p0)+p0;

        // inherit the boundary condition from one of the segments
        // so that we can apply it to the newly created arc later;
        ar.BoundaryMarkerName=linelist[seg[0]]->BoundaryMarkerName;
        ar.InGroup       =linelist[seg[0]]->InGroup;

        // add new nodes at ends of radius
        updateUndo();
        addNode(Re(p1),Im(p1),len/10000.);
        addNode(Re(p2),Im(p2),len/10000.);
        unselectAll();

        // delete the node that is to be replace by a radius;
        n=closestNode(Re(p0),Im(p0));
        nodelist[n]->IsSelected=true;
        deleteSelectedNodes();

        // add in the new radius;
        ar.n0=closestNode(Re(p2),Im(p2));
        ar.n1=closestNode(Re(p1),Im(p1));
        ar.ArcLength=180.-phi/DEG;
        addArcSegment(ar);

        return true;
    }
    case -2: // Two arcs
    {
        CComplex c0,c1,c2,p[8],i1[8],i2[8];
        double a[8],b[8],c,d[8],x[8],r0,r1,r2,phi;
        CArcSegment ar;

        r0=r;
        getCircle(*arclist[arc[0]],c1,r1);
        getCircle(*arclist[arc[1]],c2,r2);
        c=abs(c2-c1);

        // solve for all of the different possible cases;
        a[0]=r1+r0; b[0]=r2+r0;
        a[1]=r1+r0; b[1]=r2+r0;
        a[2]=r1-r0; b[2]=r2-r0;
        a[3]=r1-r0; b[3]=r2-r0;
        a[4]=r1-r0; b[4]=r2+r0;
        a[5]=r1-r0; b[5]=r2+r0;
        a[6]=r1+r0; b[6]=r2-r0;
        a[7]=r1+r0; b[7]=r2-r0;
        for(int k=0;k<8;k++){
            x[k]=(b[k]*b[k]+c*c-a[k]*a[k])/(2.*c*c);
            d[k]=sqrt(b[k]*b[k]-x[k]*x[k]*c*c);
        }
        for(int k=0;k<8;k+=2)
        {
            // solve for the center point of the radius for each solution.
            p[k]  =((1-x[k])*c+I*d[k])*(c2-c1)/abs(c2-c1) + c1;
            p[k+1]=((1-x[k])*c-I*d[k])*(c2-c1)/abs(c2-c1) + c1;
        }

        c0=nodelist[n]->CC();

        int j=0;
        for(int k=0;k<8;k++)
        {
            i1[j]=c1 + r1*(p[k]-c1)/abs(p[k]-c1); // compute possible intersection points
            i2[j]=c2 + r2*(p[k]-c2)/abs(p[k]-c2); // with the arcs;
            p[j] =p[k];

            // add this one to the list of possibly valid solutions if
            // both of the intersection points actually lie on the arc
            if ( shortestDistanceFromArc(i1[j],*arclist[arc[0]])<(r0/10000.) &&
                 shortestDistanceFromArc(i2[j],*arclist[arc[1]])<(r0/10000.) &&
                 abs(i1[j]-i2[j])>(r0/10000.))
            {
                j++;
                if (j==2) break;
            }

        }

        // There could be no valid solutions...
        if (j==0) return false;

        // But there are also special cases where there could be two answers.
        // We then pick the solution that has the center point closest to the point to be removed.
        if(j>1)
        {
            if (abs(p[0]-c0)<abs(p[1]-c0)) j=0;
            else j=1;
        }
        else j=0;	// The index of the winning case is in j....

        // inherit the boundary condition from one of the segments
        // so that we can apply it to the newly created arc later;
        ar.BoundaryMarkerName=arclist[arc[0]]->BoundaryMarkerName;
        ar.InGroup=arclist[arc[0]]->InGroup;

        // add new nodes at ends of radius
        updateUndo();
        addNode(Re(i1[j]),Im(i1[j]),c/10000.);
        addNode(Re(i2[j]),Im(i2[j]),c/10000.);
        unselectAll();

        // delete the node that is to be replace by a radius;
        n=closestNode(Re(c0),Im(c0));
        nodelist[n]->IsSelected=true;
        deleteSelectedNodes();

        // compute the angle spanned by the new arc;
        phi=arg((i2[j]-p[j])/(i1[j]-p[j]));
        if (phi<0)
        {
            c0=i2[j]; i2[j]=i1[j]; i1[j]=c0;
            phi=fabs(phi);
        }

        // add in the new radius;
        ar.n0=closestNode(Re(i1[j]),Im(i1[j]));
        ar.n1=closestNode(Re(i2[j]),Im(i2[j]));
        ar.ArcLength=phi/DEG;
        addArcSegment(ar);

        return true;
    }
    }

    return false;
}

bool femm::FemmProblem::deleteSelectedArcSegments()
{
    size_t oldsize = arclist.size();

    if (!arclist.empty())
    {
        // remove selected elements
        arclist.erase(
                    std::remove_if(arclist.begin(),arclist.end(),
                                   [](const auto& arc){ return arc->IsSelected;} ),
                    arclist.end()
                    );
    }
    arclist.shrink_to_fit();

    return arclist.size() != oldsize;
}

bool femm::FemmProblem::deleteSelectedBlockLabels()
{
    size_t oldsize = labellist.size();

    if (!labellist.empty())
    {
        // remove selected elements
        labellist.erase(
                    std::remove_if(labellist.begin(),labellist.end(),
                                   [](const auto& label){ return label->IsSelected;} ),
                    labellist.end()
                    );
    }
    labellist.shrink_to_fit();

    return labellist.size() != oldsize;
}

bool femm::FemmProblem::deleteSelectedNodes()
{
    bool changed = false;

    if (nodelist.size() > 0)
    {
        int i=0;
        do
        {
            if(nodelist[i]->IsSelected!=0)
            {
                changed=true;
                // first remove all lines that contain the point;
                for (int j=0; j<(int)linelist.size(); j++)
                    if((linelist[j]->n0==i) || (linelist[j]->n1==i))
                        linelist[j]->ToggleSelect();
                deleteSelectedSegments();

                // remove all arcs that contain the point;
                for (int j=0; j<(int)arclist.size(); j++)
                    if((arclist[j]->n0==i) || (arclist[j]->n1==i))
                        arclist[j]->ToggleSelect();
                deleteSelectedArcSegments();

                // remove node from the nodelist...
                nodelist.erase(nodelist.begin()+i);

                // update lines to point to the new node numbering
                for (int j=0; j<(int)linelist.size(); j++)
                {
                    if (linelist[j]->n0>i) linelist[j]->n0--;
                    if (linelist[j]->n1>i) linelist[j]->n1--;
                }

                // update arcs to point to the new node numbering
                for (int j=0; j<(int)arclist.size(); j++)
                {
                    if (arclist[j]->n0>i) arclist[j]->n0--;
                    if (arclist[j]->n1>i) arclist[j]->n1--;
                }
            } else
                i++;
        } while (i<(int)nodelist.size());
    }

    nodelist.shrink_to_fit();
    return changed;
}

bool femm::FemmProblem::deleteSelectedSegments()
{
    size_t oldsize = linelist.size();

    if (!linelist.empty())
    {
        // remove selected elements
        linelist.erase(
                    std::remove_if(linelist.begin(),linelist.end(),
                                   [](const auto& segm){ return segm->IsSelected;} ),
                    linelist.end()
                    );
    }
    linelist.shrink_to_fit();

    return linelist.size() != oldsize;
}

int femm::FemmProblem::getArcArcIntersection(const femm::CArcSegment &arc0, const femm::CArcSegment &arc1, CComplex *p) const
{
    CComplex a0,a1,t,c0,c1;
    double d,l,R0,R1,z0,z1,c,tta0,tta1;
    int i=0;

    a0.Set(nodelist[arc0.n0]->x,nodelist[arc0.n0]->y);
    a1.Set(nodelist[arc1.n0]->x,nodelist[arc1.n0]->y);

    getCircle(arc1,c1,R1);
    getCircle(arc0,c0,R0);

    d=abs(c1-c0);			// distance between centers

    if ((d>R0+R1) || (d<1.e-08)) return 0;
    // directly eliminate case where there can't
    // be any crossings....

    l=sqrt((R0+R1-d)*(d+R0-R1)*(d-R0+R1)*(d+R0+R1))/(2.*d);
    c=1.+(R0/d)*(R0/d)-(R1/d)*(R1/d);
    t=(c1-c0)/d;
    tta0=arc0.ArcLength*PI/180;
    tta1=arc1.ArcLength*PI/180;

    p[i]=c0 + (c*d/2.+ I*l)*t;		// first possible intersection;
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    if(fabs(d-R0+R1)/(R0+R1)< 1.e-05)
    {
        p[i]=c0+ c*d*t/2.;
        return i;
    }

    p[i]=c0 + (c*d/2.-I*l)*t;		// second possible intersection
    z0=arg((p[i]-c0)/(a0-c0));
    z1=arg((p[i]-c1)/(a1-c1));
    if ((z0>0.) && (z0<tta0) && (z1>0.) && (z1<tta1)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;
}

bool femm::FemmProblem::getBoundingBox(double (&x)[2], double (&y)[2]) const
{
    if (nodelist.size()<2)
        return false;

    // initial estimate
    x[0]=nodelist[0]->x;  x[1]=nodelist[0]->x;
    y[0]=nodelist[0]->y;  y[1]=nodelist[0]->y;
    // expand to fit
    for (const auto &node: nodelist)
    {
        if(node->x<x[0]) x[0]=node->x;
        if(node->x>x[1]) x[1]=node->x;
        if(node->y<y[0]) y[0]=node->y;
        if(node->y>y[1]) y[1]=node->y;
    }

    for (const auto &label: labellist)
    {
        if(label->x<x[0]) x[0]=label->x;
        if(label->x>x[1]) x[1]=label->x;
        if(label->y<y[0]) y[0]=label->y;
        if(label->y>y[1]) y[1]=label->y;
    }

    // arcs can "curve" outside of the bounding box defined by their endpoints
    for (const auto &arc: arclist)
    {
        int k=(int) ceil(arc->ArcLength/arc->MaxSideLength);
        double dt = arc->ArcLength*PI/(((double) k)*180.);
        CComplex c;
        double R;
        getCircle(*arc,c,R);
        CComplex p(nodelist[arc->n0]->x, nodelist[arc->n0]->y);
        CComplex s=exp(I*dt);
        for(int j=0; j<k; j++)
        {
            p=(p-c)*s+c;
            if(p.re<x[0]) x[0]=p.re;
            if(p.re>x[1]) x[1]=p.re;
            if(p.im<y[0]) y[0]=p.im;
            if(p.im>y[1]) y[1]=p.im;
        }
    }

    return true;
}

// identical in fmesher, FPProc and HPProc
void femm::FemmProblem::getCircle(const femm::CArcSegment &arc, CComplex &c, double &R) const
{
    // construct the coordinates of the two points on the circle
    CComplex a0(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    CComplex a1(nodelist[arc.n1]->x,nodelist[arc.n1]->y);

    // calculate distance between arc endpoints
    double d = abs(a1 - a0);

    // figure out what the radius of the circle is...

    // get unit vector pointing from a0 to a1
    CComplex t = (a1 - a0) / d;

    // convert swept angle from degrees to radians
    double tta = arc.ArcLength * PI / 180.;

    // the radius is half the chord length divided by sin of
    // half the swept angle (finding the side length of a
    // triangle formed by the two points and the centre)
    R = d / (2.*sin(tta/2.));

    // center of the arc segment's circle
    c = a0 + (d/2. + I * sqrt(R*R - d*d / 4.)) * t;
}

std::string femm::FemmProblem::getTitle() const
{
    return pathName;
}

bool femm::FemmProblem::getIntersection(int n0, int n1, int segm, double *xi, double *yi) const
{
    CComplex p0,p1,q0,q1;
    double ee,x,z;

    // Check to see if the two lines have a common endpoint
    // If they do, there can be no other intersection...
    if (n0==linelist[segm]->n0) return false;
    if (n0==linelist[segm]->n1) return false;
    if (n1==linelist[segm]->n0) return false;
    if (n1==linelist[segm]->n1) return false;

    // Get a definition of "real small" based on the lengths
    // of the lines of interest;
    p0=nodelist[linelist[segm]->n0]->CC();
    p1=nodelist[linelist[segm]->n1]->CC();
    q0=nodelist[n0]->CC();
    q1=nodelist[n1]->CC();
    ee = (std::min)(abs(p1-p0),abs(q1-q0))*1.0e-8;

    // Rotate and scale the prospective line
    q0=(q0-p0)/(p1-p0);
    q1=(q1-p0)/(p1-p0);

    // Check for cases where there is obviously no intersection
    if ((Re(q0)<=0.) && (Re(q1)<=0.)) return false;
    if ((Re(q0)>=1.) && (Re(q1)>=1.)) return false;
    if ((Im(q0)<=0.) && (Im(q1)<=0.)) return false;
    if ((Im(q0)>=0.) && (Im(q1)>=0.)) return false;

    // compute intersection
    z=Im(q0)/Im(q0-q1);

    // check to see if the line segments intersect at a point sufficiently
    // far from the segment endpoints....
    x=Re((1.0 - z)*q0 + z*q1);
    if((x < ee) || (x > (1.0 - ee))) return false;

    // return resulting intersection point
    p0 = (1.0 - z)*nodelist[n0]->CC() + z*nodelist[n1]->CC();
    *xi=Re(p0);
    *yi=Im(p0);

    return true;
}

int femm::FemmProblem::getLineArcIntersection(const femm::CSegment &seg, const femm::CArcSegment &arc, CComplex *p) const
{
    CComplex p0,p1,a0,a1,t,v,c;
    double d,l,R,z,tta;
    int i=0;

    p0.Set(nodelist[seg.n0]->x,nodelist[seg.n0]->y);
    p1.Set(nodelist[seg.n1]->x,nodelist[seg.n1]->y);
    a0.Set(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    a1.Set(nodelist[arc.n1]->x,nodelist[arc.n1]->y);
    d=abs(a1-a0);			// distance between arc endpoints

    // figure out what the radius of the circle is...
    t = (a1-a0) / d;
    tta = arc.ArcLength * PI / 180.;
    R = d / ( 2. * std::sin(tta / 2.));
    c = a0 + (d/2. + I*sqrt(R*R-d*d/4.))*t; // center of the arc segment's circle...

    // figure out the distance between line and circle's center;
    d=abs(p1-p0);
    t=(p1-p0)/d;
    v=(c-p0)/t;
    if (fabs(Im(v))>R) return 0;
    l=sqrt( R*R - Im(v)*Im(v));	// Im(v) is distance between line and center...

    if ((l/R) < 1.e-05) 		// case where line is very close to a tangent;
    {
        p[i]=p0 + Re(v)*t;		// make it be a tangent.
        R=Re((p[i]-p0)/t);
        z=arg((p[i]-c)/(a0-c));
        if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;
        return i;
    }

    p[i]=p0 + (Re(v)+l)*t;		// first possible intersection;
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    p[i]=p0 + (Re(v)-l)*t;		// second possible intersection
    R=Re((p[i]-p0)/t);
    z=arg((p[i]-c)/(a0-c));
    if ((R>0) && (R<d) && (z>0.) && (z<tta)) i++;

    // returns the number of valid intersections found;
    // intersections are returned in the array p[];
    return i;
}



double femm::FemmProblem::lengthOfLine(int i) const
{
    return abs(nodelist[linelist[i]->n0]->CC()-
            nodelist[linelist[i]->n1]->CC());
}

// identical in fmesher, hpproc, fpproc
double femm::FemmProblem::shortestDistanceFromSegment(double p, double q, int segm) const
{
    double x0=nodelist[linelist[segm]->n0]->x;
    double y0=nodelist[linelist[segm]->n0]->y;
    double x1=nodelist[linelist[segm]->n1]->x;
    double y1=nodelist[linelist[segm]->n1]->y;

    double t=((p-x0)*(x1-x0) + (q-y0)*(y1-y0))/
            ((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));

    if (t>1.) t=1.;
    if (t<0.) t=0.;

    double x2=x0+t*(x1-x0);
    double y2=y0+t*(y1-y0);

    return sqrt((p-x2)*(p-x2) + (q-y2)*(q-y2));
}



// identical in fmesher, FPProc and HPProc
double femm::FemmProblem::shortestDistanceFromArc(CComplex p, const femm::CArcSegment &arc) const
{
    double R;
    CComplex c;
    getCircle(arc,c,R);

    double d=abs(p-c);
    if(d==0) return R;

    CComplex a0(nodelist[arc.n0]->x,nodelist[arc.n0]->y);
    CComplex a1(nodelist[arc.n1]->x,nodelist[arc.n1]->y);
    CComplex t=(p-c)/d;
    double l=abs(p-c-R*t);
    double z=arg(t/(a0-c))*180/PI;
    if ((z>0) && (z<arc.ArcLength)) return l;

    z=abs(p-a0);
    l=abs(p-a1);
    if(z<l) return z;
    return l;
}

void femm::FemmProblem::unselectAll()
{
    for(auto &node: nodelist) node->IsSelected = false;
    for(auto &line: linelist) line->IsSelected = false;
    for(auto &label: labellist) label->IsSelected = false;
    for(auto &arc: arclist) arc->IsSelected = false;
}

void femm::FemmProblem::undo()
{
    for(int i=0; i<(int)undolinelist.size(); i++)
        linelist[i].swap(undolinelist[i]);
    for(int i=0; i<(int)undoarclist.size(); i++)
        arclist[i].swap(undoarclist[i]);
    for(int i=0; i<(int)undolabellist.size(); i++)
        labellist[i].swap(undolabellist[i]);
    // Note(ZaJ): why not nodelist??
}

void femm::FemmProblem::undoLines()
{
    for(int i=0; i<(int)undolinelist.size(); i++)
        linelist[i].swap(undolinelist[i]);
}

void femm::FemmProblem::updateUndo()
{
    undonodelist.clear();
    undolinelist.clear();
    undoarclist.clear();
    undolabellist.clear();

    // copy each entry
    for(const auto& node: nodelist)
        undonodelist.push_back(std::make_unique<CNode>(*node));
    for(const auto& line: linelist)
        undolinelist.push_back(std::make_unique<CSegment>(*line));
    for(const auto& arc: arclist)
        undoarclist.push_back(std::make_unique<CArcSegment>(*arc));
    for(const auto& label: labellist)
        undolabellist.push_back(label->clone());
}

femm::FemmProblem::FemmProblem(FileType ftype)
    : FileFormat(-1)
    , Frequency(0.0)
    , Precision(1.e-08)
    , MinAngle(DEFAULT_MINIMUM_ANGLE)
    , Depth(1)
    , LengthUnits(LengthInches)
    , Coords(CART)
    , ProblemType(PLANAR)
    , extZo(0)
    , extRo(0)
    , extRi(0)
    , comment()
    , ACSolver(0)
    , dT(0)
    , PrevSoln()
    , DoForceMaxMeshArea(false)
    , nodelist()
    , linelist()
    , arclist()
    , labellist()
    , nodeproplist()
    , lineproplist()
    , blockproplist()
    , circproplist()
    , solved(false)
    , meshnodes()
    , meshelems()
    , pathName()
    //, solutionFile()
    , filetype(ftype)
    , blockMap()
    , circuitMap()
{}
