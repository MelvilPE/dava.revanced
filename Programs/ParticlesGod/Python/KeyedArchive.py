import struct

VERSION_LEGACY = 0
VERSION_STRING_MAP = 1
VERSION_REGISTER_MAP = 2
VERSION_LATEST_VALID = VERSION_REGISTER_MAP

REGISTERED_MAP_TAG_STRING_MAP = 0

REGISTERED_MAP_TAG_TOP_ARCHIVE = 0
REGISTERED_MAP_TAG_SUB_ARCHIVE = 1
REGISTERED_MAP_TAG_EMPTY_ARCHIVE = 0xFF

KEYED_ARCHIVE_HEADER_SIGNATURE = b'KA'


def Load(inputFile, registry = None):
    import VariantType

    result = {}

    signature = inputFile.read(2)
    if signature != KEYED_ARCHIVE_HEADER_SIGNATURE:
        print("[KeyedArchive::Load] Wrong archive signature:", signature)
        return False

    version = struct.unpack("<B", inputFile.read(1))[0]
    if (version != VERSION_STRING_MAP) and (version != VERSION_REGISTER_MAP):
        print("[KeyedArchive::Load] Wrong archive version:", signature)
        return False
    
    tag = struct.unpack("<B", inputFile.read(1))[0]
    if version == VERSION_REGISTER_MAP:
        if (tag != REGISTERED_MAP_TAG_TOP_ARCHIVE) and (tag != REGISTERED_MAP_TAG_SUB_ARCHIVE) and (tag != REGISTERED_MAP_TAG_EMPTY_ARCHIVE):
            print("[KeyedArchive::Load] Wrong archive tag:", signature)
            return False

    if version == VERSION_STRING_MAP:
        itemsCount = struct.unpack("<I", inputFile.read(4))[0]

        for index in range(itemsCount):
            read = VariantType.Read(inputFile)
            if read == False:
                print("[KeyedArchive::Load] Failed to read a key")
                return False

            key = read[VariantType.TYPENAME_STRING]

            value = VariantType.Read(inputFile)
            if value == False:
                print("[KeyedArchive::Load] Failed to read a value")
                return False
            
            result[key] = value

        return result

    if version == VERSION_REGISTER_MAP:
        if tag == REGISTERED_MAP_TAG_EMPTY_ARCHIVE:
            return {}
        
        if tag == REGISTERED_MAP_TAG_TOP_ARCHIVE:
            registry, keys = {}, []
            registryCount = struct.unpack("<I", inputFile.read(4))[0]

            for index in range(registryCount):
                keyLen = struct.unpack("<H", inputFile.read(2))[0]
                key = inputFile.read(keyLen).decode('utf-8')
                keys.append(key)

            for index in range(registryCount):
                keyHash = str(struct.unpack("<I", inputFile.read(4))[0])
                registry[keyHash] = keys[index]

            itemsCount = struct.unpack("<I", inputFile.read(4))[0]
            for index in range(itemsCount):
                keyHash = str(struct.unpack("<I", inputFile.read(4))[0])
                key = registry[keyHash]

                value = VariantType.Read(inputFile, registry)
                if value == False:
                    print("[KeyedArchive::Load] Failed to read a value")
                    return False
                
                result[key] = value

            return result
        
        if tag == REGISTERED_MAP_TAG_SUB_ARCHIVE:
            itemsCount = struct.unpack("<I", inputFile.read(4))[0]
            for index in range(itemsCount):
                keyHash = str(struct.unpack("<I", inputFile.read(4))[0])
                key = registry[keyHash]
                
                value = VariantType.Read(inputFile, registry)
                if value == False:
                    print("[KeyedArchive::Load] Failed to read a value")
                    return False
                result[key] = value

            return result

    return result

def Save(outputFile, archive, registry = None):
    import VariantType

    outputFile.write(KEYED_ARCHIVE_HEADER_SIGNATURE)
    outputFile.write(struct.pack("<B", VERSION_STRING_MAP))
    outputFile.write(struct.pack("<B", REGISTERED_MAP_TAG_STRING_MAP))

    outputFile.write(struct.pack("<I", len(archive)))
    for key, value in archive.items():
        VariantType.Write(outputFile, {VariantType.TYPENAME_STRING: key})
        VariantType.Write(outputFile, value)

    return True