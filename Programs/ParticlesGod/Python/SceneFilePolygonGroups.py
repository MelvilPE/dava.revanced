import struct

def GetMaxPolygonGroupsIds(polygonsFile):
    import KeyedArchive
    import VariantType
    
    maxDataNodesId = 0
    
    signature, version, nodeCount, nodeCount1 = struct.unpack("<4sIII", polygonsFile.read(16))

    for index in range(nodeCount):
        archive = KeyedArchive.Load(polygonsFile)
        if archive == False:
            print("[SceneFilePolygonGroups::GetMaxPolygonGroupsIds] Failed to load PolygonGroup node")
            return False
        
        id = struct.unpack("<Q", archive["#id"][VariantType.TYPENAME_BYTE_ARRAY])[0]
        if id > maxDataNodesId:
            maxDataNodesId = id

    return maxDataNodesId

if __name__ == "__main__":
    GetMaxPolygonGroupsIds("Leopard1.scg")