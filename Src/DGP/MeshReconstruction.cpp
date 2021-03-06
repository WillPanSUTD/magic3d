//#include "StdAfx.h"
#include "MeshReconstruction.h"
#include "../Dependence/PoissonReconstruction.h"

namespace MagicDGP
{
    MeshReconstruction::MeshReconstruction()
    {
    }

    MeshReconstruction::~MeshReconstruction()
    {
    }

    LightMesh3D* MeshReconstruction::ScreenPoissonReconstruction(const Point3DSet* pPC)
    {
        return MagicDependence::PoissonReconstruction::ScreenPoissonRecon(pPC);
    }
}