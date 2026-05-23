#pragma once

#include "FBXExportUtils.h"

namespace DAVA
{
class NMaterial;

namespace FBXExporterDetails
{
// NMaterial* is non-const because NMaterial's getter methods are not const.
FbxSurfaceMaterial* ExportMaterial(NMaterial* material, FbxScene* fbxScene);
void ClearMaterialExportCache();

}; //ns FBXExporterDetails
}; //ns DAVA
