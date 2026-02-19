import struct
import io

TYPE_NONE           =  0
TYPE_BOOLEAN        =  1
TYPE_INT32          =  2
TYPE_FLOAT          =  3
TYPE_STRING         =  4
TYPE_WIDE_STRING    =  5
TYPE_BYTE_ARRAY     =  6
TYPE_UINT32         =  7
TYPE_KEYED_ARCHIVE  =  8
TYPE_INT64          =  9
TYPE_UINT64         = 10
TYPE_VECTOR2        = 11
TYPE_VECTOR3        = 12
TYPE_VECTOR4        = 13
TYPE_MATRIX2        = 14
TYPE_MATRIX3        = 15
TYPE_MATRIX4        = 16
TYPE_COLOR          = 17
TYPE_FASTNAME       = 18
TYPE_AABBOX3        = 19
TYPE_FILEPATH       = 20
TYPE_FLOAT64        = 21
TYPE_INT8           = 22
TYPE_UINT8          = 23
TYPE_INT16          = 24
TYPE_UINT16         = 25
TYPE_RECT           = 26
TYPE_VARIANT_VECTOR = 27
TYPE_QUATERNION     = 28
TYPE_TRANSFORM      = 29
TYPE_AABBOX2        = 30
TYPES_COUNT         = 31

TYPENAME_UNKNOWN        = "unknown"
TYPENAME_BOOLEAN        = "bool"
TYPENAME_INT8           = "int8"
TYPENAME_UINT8          = "uint8"
TYPENAME_INT16          = "int16"
TYPENAME_UINT16         = "uint16"
TYPENAME_INT32          = "int32"
TYPENAME_UINT32         = "uint32"
TYPENAME_INT64          = "int64"
TYPENAME_UINT64         = "uint64"
TYPENAME_FLOAT          = "float"
TYPENAME_FLOAT64        = "float64"
TYPENAME_STRING         = "string"
TYPENAME_WIDESTRING     = "wideString"
TYPENAME_BYTE_ARRAY     = "byteArray"
TYPENAME_KEYED_ARCHIVE  = "keyedArchive"
TYPENAME_VECTOR2        = "Vector2"
TYPENAME_VECTOR3        = "Vector3"
TYPENAME_VECTOR4        = "Vector4"
TYPENAME_MATRIX2        = "Matrix2"
TYPENAME_MATRIX3        = "Matrix3"
TYPENAME_MATRIX4        = "Matrix4"
TYPENAME_COLOR          = "Color"
TYPENAME_FASTNAME       = "FastName"
TYPENAME_AABBOX3        = "AABBox3"
TYPENAME_FILEPATH       = "FilePath"
TYPENAME_RECT           = "Rect"
TYPENAME_VARIANT_VECTOR = "variantVector"
TYPENAME_QUATERNION     = "Quaternion"
TYPENAME_TRANSFORM      = "Transform"
TYPENAME_AABBOX2        = "AABBox2"

def Read(inputFile, registry=None):
    import KeyedArchive

    varType = struct.unpack("<B", inputFile.read(1))[0]
    data = None

    if varType == TYPE_STRING:
        if registry is not None:
            keyHash = str(struct.unpack("<I", inputFile.read(4))[0])
            data = {TYPENAME_STRING: registry[keyHash]}
        else:
            stringLen = struct.unpack("<I", inputFile.read(4))[0]
            string = inputFile.read(stringLen).decode('utf-8')
            data = {TYPENAME_STRING: string}

    elif varType == TYPE_BOOLEAN:
        data = {TYPENAME_BOOLEAN: bool(struct.unpack("<B", inputFile.read(1))[0])}

    elif varType == TYPE_INT32:
        data = {TYPENAME_INT32: struct.unpack('<i', inputFile.read(4))[0]}

    elif varType == TYPE_FLOAT:
        data = {TYPENAME_FLOAT: struct.unpack('<f', inputFile.read(4))[0]}

    elif varType == TYPE_BYTE_ARRAY:
        sizeOfArray = struct.unpack("<I", inputFile.read(4))[0]
        data = {TYPENAME_BYTE_ARRAY: inputFile.read(sizeOfArray)}

    elif varType == TYPE_UINT32:
        data = {TYPENAME_UINT32: struct.unpack("<I", inputFile.read(4))[0]}

    elif varType == TYPE_KEYED_ARCHIVE:
        sizeArchive = struct.unpack("<I", inputFile.read(4))[0]
        data = {TYPENAME_KEYED_ARCHIVE: KeyedArchive.Load(inputFile, registry)}

    elif varType == TYPE_INT64:
        data = {TYPENAME_INT64: struct.unpack("<q", inputFile.read(8))[0]}

    elif varType == TYPE_UINT64:
        data = {TYPENAME_UINT64: struct.unpack("<Q", inputFile.read(8))[0]}

    elif varType == TYPE_VECTOR2:
        x, y = struct.unpack("<ff", inputFile.read(8))
        data = {TYPENAME_VECTOR2: {"x": x, "y": y}}

    elif varType == TYPE_VECTOR3:
        x, y, z = struct.unpack("<fff", inputFile.read(12))
        data = {TYPENAME_VECTOR3: {"x": x, "y": y, "z": z}}

    elif varType == TYPE_VECTOR4:
        x, y, z, w = struct.unpack("<ffff", inputFile.read(16))
        data = {TYPENAME_VECTOR4: {"x": x, "y": y, "z": z, "w": w}}

    elif varType == TYPE_MATRIX2:
        values = struct.unpack("<ffff", inputFile.read(16))
        data = {TYPENAME_MATRIX2: {
            "m00": values[0], "m01": values[1],
            "m10": values[2], "m11": values[3]
        }}

    elif varType == TYPE_MATRIX3:
        values = struct.unpack("<fffffffff", inputFile.read(36))
        data = {TYPENAME_MATRIX3: {
            "m00": values[0], "m01": values[1], "m02": values[2],
            "m10": values[3], "m11": values[4], "m12": values[5],
            "m20": values[6], "m21": values[7], "m22": values[8]
        }}

    elif varType == TYPE_MATRIX4:
        values = struct.unpack("<" + "f"*16, inputFile.read(64))
        data = {TYPENAME_MATRIX4: {
            "m00": values[0],  "m01": values[1],  "m02": values[2],  "m03": values[3],
            "m10": values[4],  "m11": values[5],  "m12": values[6],  "m13": values[7],
            "m20": values[8],  "m21": values[9],  "m22": values[10], "m23": values[11],
            "m30": values[12], "m31": values[13], "m32": values[14], "m33": values[15]
        }}

    elif varType == TYPE_COLOR:
        r, g, b = struct.unpack("<fff", inputFile.read(12))
        data = {TYPENAME_COLOR: {"r": r, "g": g, "b": b}}

    elif varType == TYPE_FASTNAME:
        if registry is not None:
            keyHash = str(struct.unpack("<I", inputFile.read(4))[0])
            data = {TYPENAME_FASTNAME: registry[keyHash]}
        else:
            stringLen = struct.unpack("<I", inputFile.read(4))[0]
            string = inputFile.read(stringLen).decode('utf-8')
            data = {TYPENAME_FASTNAME: string}

    elif varType == TYPE_AABBOX3:
        min_x, min_y, min_z, max_x, max_y, max_z = struct.unpack("<ffffff", inputFile.read(24))
        data = {TYPENAME_AABBOX3: {
            "min": {"x": min_x, "y": min_y, "z": min_z},
            "max": {"x": max_x, "y": max_y, "z": max_z}
        }}

    elif varType == TYPE_FLOAT64:
        data = {TYPENAME_FLOAT64: struct.unpack("<d", inputFile.read(8))[0]}

    elif varType == TYPE_INT8:
        data = {TYPENAME_INT8: struct.unpack("<b", inputFile.read(1))[0]}

    elif varType == TYPE_UINT8:
        data = {TYPENAME_UINT8: struct.unpack("<B", inputFile.read(1))[0]}

    elif varType == TYPE_INT16:
        data = {TYPENAME_INT16: struct.unpack("<h", inputFile.read(2))[0]}

    elif varType == TYPE_UINT16:
        data = {TYPENAME_UINT16: struct.unpack("<H", inputFile.read(2))[0]}

    elif varType == TYPE_RECT:
        x, y, w, h = struct.unpack("<ffff", inputFile.read(16))
        data = {TYPENAME_RECT: {"x": x, "y": y, "w": w, "h": h}}

    elif varType == TYPE_VARIANT_VECTOR:
        dataStored = []
        itemsCount = struct.unpack("<I", inputFile.read(4))[0]
        for index in range(itemsCount):
            item = Read(inputFile, registry)
            dataStored.append(item)
        data = {TYPENAME_VARIANT_VECTOR: dataStored}

    elif varType == TYPE_QUATERNION:
        x, y, z, w = struct.unpack("<ffff", inputFile.read(16))
        data = {TYPENAME_QUATERNION: {"x": x, "y": y, "z": z, "w": w}}

    elif varType == TYPE_TRANSFORM:
        px, py, pz, qx, qy, qz, qw, sx, sy, sz = struct.unpack("<ffffffffff", inputFile.read(40))
        data = {TYPENAME_TRANSFORM: {
            "position": {"x": px, "y": py, "z": pz},
            "rotation": {"x": qx, "y": qy, "z": qz, "w": qw},
            "scale": {"x": sx, "y": sy, "z": sz}
        }}

    elif varType == TYPE_AABBOX2:
        x0, y0, x1, y1 = struct.unpack("<ffff", inputFile.read(16))
        data = {TYPENAME_AABBOX2: {"x0": x0, "y0": y0, "x1": x1, "y1": y1}}

    else:
        print("[VariantType::Read] Reading wrong variant type found:", [varType, inputFile.tell()])
        return False

    return data

def Write(outputFile, data, registry=None):
    import struct
    import KeyedArchive

    typename = next(iter(data))
    value = data[typename]

    if typename == TYPENAME_STRING:
        outputFile.write(struct.pack("<B", TYPE_STRING))
        if registry is not None:
            keyHash = int(value)
            outputFile.write(struct.pack("<I", keyHash))
        else:
            encoded = value.encode("utf-8")
            outputFile.write(struct.pack("<I", len(encoded)))
            outputFile.write(encoded)

    elif typename == TYPENAME_BOOLEAN:
        outputFile.write(struct.pack("<B", TYPE_BOOLEAN))
        outputFile.write(struct.pack("<B", 1 if value else 0))

    elif typename == TYPENAME_INT32:
        outputFile.write(struct.pack("<B", TYPE_INT32))
        outputFile.write(struct.pack("<i", value))

    elif typename == TYPENAME_FLOAT:
        outputFile.write(struct.pack("<B", TYPE_FLOAT))
        outputFile.write(struct.pack("<f", value))

    elif typename == TYPENAME_BYTE_ARRAY:
        outputFile.write(struct.pack("<B", TYPE_BYTE_ARRAY))
        outputFile.write(struct.pack("<I", len(value)))
        outputFile.write(value)

    elif typename == TYPENAME_UINT32:
        outputFile.write(struct.pack("<B", TYPE_UINT32))
        outputFile.write(struct.pack("<I", value))

    elif typename == TYPENAME_KEYED_ARCHIVE:
        archiveBuffer = io.BytesIO()
        wrote = KeyedArchive.Save(archiveBuffer, value, registry)
        if wrote == False:
            print("[VariantType::Write] Failed to write variant type keyed archive!")
            return False

        outputFile.write(struct.pack("<B", TYPE_KEYED_ARCHIVE))
        outputFile.write(struct.pack("<I", len(archiveBuffer.getvalue())))
        outputFile.write(archiveBuffer.getvalue())

    elif typename == TYPENAME_INT64:
        outputFile.write(struct.pack("<B", TYPE_INT64))
        outputFile.write(struct.pack("<q", value))

    elif typename == TYPENAME_UINT64:
        outputFile.write(struct.pack("<B", TYPE_UINT64))
        outputFile.write(struct.pack("<Q", value))

    elif typename == TYPENAME_VECTOR2:
        outputFile.write(struct.pack("<B", TYPE_VECTOR2))
        outputFile.write(struct.pack("<ff", value["x"], value["y"]))

    elif typename == TYPENAME_VECTOR3:
        outputFile.write(struct.pack("<B", TYPE_VECTOR3))
        outputFile.write(struct.pack("<fff", value["x"], value["y"], value["z"]))

    elif typename == TYPENAME_VECTOR4:
        outputFile.write(struct.pack("<B", TYPE_VECTOR4))
        outputFile.write(struct.pack("<ffff", value["x"], value["y"], value["z"], value["w"]))

    elif typename == TYPENAME_MATRIX2:
        outputFile.write(struct.pack("<B", TYPE_MATRIX2))
        outputFile.write(struct.pack(
            "<ffff",
            value["m00"], value["m01"],
            value["m10"], value["m11"]
        ))

    elif typename == TYPENAME_MATRIX3:
        outputFile.write(struct.pack("<B", TYPE_MATRIX3))
        outputFile.write(struct.pack(
            "<fffffffff",
            value["m00"], value["m01"], value["m02"],
            value["m10"], value["m11"], value["m12"],
            value["m20"], value["m21"], value["m22"]
        ))

    elif typename == TYPENAME_MATRIX4:
        outputFile.write(struct.pack("<B", TYPE_MATRIX4))
        outputFile.write(struct.pack(
            "<" + "f"*16,
            value["m00"], value["m01"], value["m02"], value["m03"],
            value["m10"], value["m11"], value["m12"], value["m13"],
            value["m20"], value["m21"], value["m22"], value["m23"],
            value["m30"], value["m31"], value["m32"], value["m33"]
        ))

    elif typename == TYPENAME_COLOR:
        outputFile.write(struct.pack("<B", TYPE_COLOR))
        outputFile.write(struct.pack("<fff", value["r"], value["g"], value["b"]))

    elif typename == TYPENAME_FASTNAME:
        outputFile.write(struct.pack("<B", TYPE_FASTNAME))
        if registry is not None:
            keyHash = int(value)
            outputFile.write(struct.pack("<I", keyHash))
        else:
            encoded = value.encode("utf-8")
            outputFile.write(struct.pack("<I", len(encoded)))
            outputFile.write(encoded)

    elif typename == TYPENAME_AABBOX3:
        outputFile.write(struct.pack("<B", TYPE_AABBOX3))
        outputFile.write(struct.pack(
            "<ffffff",
            value["min"]["x"], value["min"]["y"], value["min"]["z"],
            value["max"]["x"], value["max"]["y"], value["max"]["z"]
        ))

    elif typename == TYPENAME_FLOAT64:
        outputFile.write(struct.pack("<B", TYPE_FLOAT64))
        outputFile.write(struct.pack("<d", value))

    elif typename == TYPENAME_INT8:
        outputFile.write(struct.pack("<B", TYPE_INT8))
        outputFile.write(struct.pack("<b", value))

    elif typename == TYPENAME_UINT8:
        outputFile.write(struct.pack("<B", TYPE_UINT8))
        outputFile.write(struct.pack("<B", value))

    elif typename == TYPENAME_INT16:
        outputFile.write(struct.pack("<B", TYPE_INT16))
        outputFile.write(struct.pack("<h", value))

    elif typename == TYPENAME_UINT16:
        outputFile.write(struct.pack("<B", TYPE_UINT16))
        outputFile.write(struct.pack("<H", value))

    elif typename == TYPENAME_RECT:
        outputFile.write(struct.pack("<B", TYPE_RECT))
        outputFile.write(struct.pack("<ffff", value["x"], value["y"], value["w"], value["h"]))

    elif typename == TYPENAME_VARIANT_VECTOR:
        outputFile.write(struct.pack("<B", TYPE_VARIANT_VECTOR))
        outputFile.write(struct.pack("<I", len(value)))
        for item in value:
            Write(outputFile, item, registry)

    elif typename == TYPENAME_QUATERNION:
        outputFile.write(struct.pack("<B", TYPE_QUATERNION))
        outputFile.write(struct.pack("<ffff", value["x"], value["y"], value["z"], value["w"]))

    elif typename == TYPENAME_TRANSFORM:
        outputFile.write(struct.pack("<B", TYPE_TRANSFORM))
        outputFile.write(struct.pack(
            "<ffffffffff",
            value["position"]["x"], value["position"]["y"], value["position"]["z"],
            value["rotation"]["x"], value["rotation"]["y"], value["rotation"]["z"], value["rotation"]["w"],
            value["scale"]["x"], value["scale"]["y"], value["scale"]["z"]
        ))

    elif typename == TYPENAME_AABBOX2:
        outputFile.write(struct.pack("<B", TYPE_AABBOX2))
        outputFile.write(struct.pack("<ffff", value["x0"], value["y0"], value["x1"], value["y1"]))

    else:
        print(f"[VariantType::Write] Writing wrong variant type found:: {typename}")
        return False
    
    return True