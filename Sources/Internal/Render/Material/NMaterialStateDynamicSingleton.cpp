#include "NMaterialStateDynamicSingleton.h"

namespace DAVA
{
NMaterialStateDynamicSingleton* NMaterialStateDynamicSingleton::GetInstance()
{
    static NMaterialStateDynamicSingleton instance;
    return &instance;
}

NMaterialStateDynamicSingleton::NMaterialStateDynamicSingleton()
{
    archive = new DAVA::KeyedArchive();
}

NMaterialStateDynamicSingleton::~NMaterialStateDynamicSingleton()
{
    DAVA::SafeRelease(archive);
}

KeyedArchive* NMaterialStateDynamicSingleton::GetArchive()
{
    return archive;
}

bool NMaterialStateDynamicSingleton::TryUpdateArchive(KeyedArchive* param)
{
    if (!param->IsKeyExists("flags") ||
        !param->IsKeyExists("presets") ||
        !param->IsKeyExists("properties") ||
        !param->IsKeyExists("textures"))
    {
        Logger::Warning("[NMaterialStateDynamicSingleton::TryUpdateArchive] Missing flags / presets / properties / textures");
        return false;
    }

    DAVA::SafeRelease(archive);
    archive = new KeyedArchive(*param);
    return true;
}

String NMaterialStateDynamicSingleton::GetDefaultArchiveYaml()
{
    ScopedPtr<KeyedArchive> defaultArchive(new KeyedArchive());

    Vector<String> flagsArray = {
        "BLENDING",
        "VERTEX_FOG",
        "FOG_LINEAR",
        "FOG_HALFSPACE",
        "FOG_HALFSPACE_LINEAR",
        "FOG_ATMOSPHERE",
        "FOG_ATMOSPHERE_NO_ATTENUATION",
        "FOG_ATMOSPHERE_NO_SCATTERING",
        "TEXTURE0_SHIFT_ENABLED",
        "ENABLE_FOG",
        "ENABLE_HIGH_QUALITY_FOG",
        "TEXTURE0_SHIFT_ENABLED",
        "TEXTURE0_ANIMATION_SHIFT",
        "WAVE_ANIMATION",
        "FAST_NORMALIZATION",
        "TILED_DECAL_MASK",
        "TILE_DECAL_ROTATION",
        "TILED_DECAL_TRANSFORM",
        "TILED_DECAL_ANIMATED_EMISSION",
        "TILED_DECAL_BLEND_NORMAL",
        "TILED_DECAL_OVERRIDE_ROUGHNESS_METALLIC",
        "ALBEDO_TRANSFORM",
        "FLATCOLOR",
        "FLATALBEDO",
        "DISTANCE_ATTENUATION",
        "SPECULAR",
        "SEPARATE_NORMALMAPS",
        "VERTEX_COLOR",
        "SPEED_TREE_OBJECT",
        "SPEEDTREE_JOINT_TRANSFORM",
        "SPEEDTREE_JOINT_LENGTHWISE_TRANSFORM",
        "SPHERICAL_LIT",
        "TANGENT_SPACE_WATER_REFLECTIONS",
        "DEBUG_UNITY_Z_NORMAL",
        "DEBUG_Z_NORMAL_SCALE",
        "DEBUG_NORMAL_ROTATION",
        "HARD_SKINNING",
        "SOFT_SKINNING",
        "FLOWMAP_SKY",
        "PARTICLES_MASK",
        "PARTICLES_FLOWMAP",
        "PARTICLES_FLOWMAP_ANIMATION",
        "PARTICLES_PERSPECTIVE_MAPPING",
        "PARTICLES_THREE_POINT_GRADIENT",
        "PARTICLES_NOISE",
        "PARTICLES_FRESNEL_TO_ALPHA",
        "PARTICLES_ALPHA_REMAP",
        "PARTICLES_VERTEX_ANIMATION",
        "PARTICLES_VERTEX_ANIMATION_MASK",
        "MATERIAL_VIEW_LIGHTMAP_ONLY",
        "MATERIAL_VIEW_TEXTURE_ONLY",
        "SETUP_LIGHTMAP",
        "VIEW_ALBEDO",
        "VIEW_NORMAL",
        "VIEW_NORMAL_FINAL",
        "VIEW_ROUGHNESS",
        "VIEW_METALLIC",
        "VIEW_AMBIENTOCCLUSION",
        "VIEW_AMBIENT",
        "VIEW_DIFFUSE",
        "VIEW_SPECULAR",
        "ALPHA_EROSION",
        "SOFT_PARTICLES",
        "FRAME_BLEND",
        "FORCE_2D_MODE",
        "LANDSCAPE_USE_INSTANCING",
        "LANDSCAPE_LOD_MORPHING",
        "LANDSCAPE_MORPHING_COLOR",
        "HEIGHTMAP_FLOAT_TEXTURE",
        "ALPHATEST",
        "ALPHATESTVALUE",
        "ALPHASTEPVALUE",
        "ILLUMINATION_USED",
        "ILLUMINATION_SHADOW_CASTER",
        "ILLUMINATION_SHADOW_RECEIVER",
        "TEST_OCCLUSION",
        "FORCED_SHADOW_DIRECTION",
        "HEATMAP",
        "GEO_DECAL",
        "GEO_DECAL_SPECULAR",
        "VERTEX_DISTORTION",
        "NORMAL_DETAIL",
        "LANDSCAPE_USE_RELAXMAP",
        "LANDSCAPE_HEIGHT_BLEND",
        "LANDSCAPE_SEPARATE_LIGHTMAP_CHANNEL",
        "LANDSCAPE_HAS_METALLIC_AND_EMISSION",
        "LANDSCAPE_SCALED_TILES_NON_PBR",
        "ALLOW_POINT_LIGHTS",
        "FORCE_POINT_LIGHTS_ENABLED",
        "ENVIRONMENT_MAPPING",
        "ENVIRONMENT_MAPPING_NORMALMAP",
        "BLEND_BY_ANGLE",
        "SHADOW_RECEIVER",
        "USE_SHADOW_MAP",
        "DEBUG_SHADOW_CASCADES",
        "SHADOW_CASCADES_COUNT",
        "SHADOW_CASCADES_BLEND",
        "SHADOW_PCF",
        "DECAL_ALBEDO",
        "DECAL_NORMAL",
        "DECAL_TREAD",
        "FADE_OUT_WITH_TIME",
        "DEPTH_TARGET_IS_FRAMEBUFFER",
        "DEPTH_PREPASS_ENABLED",
        "VEGETATION_BEND",
        "VEGETATION_LIT",
        "PUSH_TO_NEAR_PLANE_HACK",
        "DISTANCE_FADE_OUT",
        "POINT_LIGHTS_OVERRIDE_SHADOW",
        "DIFFUSE_IN_LIGHTMAP",
        "AMBIENT_ATTENUATION_BOX",
        "PASS_NAME",
        "DECAL_TEXTURE_COUNT",
        "VIEW_MODE_OVERDRAW_HEAT",
        "WATER_RIPPLES_ENABLED",
        "WATER_RIPPLES_ALLOWED",
        "INSTANCED_CHAIN",
        "DIRT_COVERAGE",
        "WETNESS_MULTILEVEL",
        "WETNESS_SIMPLIFIED",
        "COLORBLIND_MODE",
        "DEBUG_UNLIT",
        "HIGHLIGHT_COLOR",
        "HIGHLIGHT_WAVE_ANIM",
        "DECAL_VERTICAL_FADE",
        "DECAL_BACK_SIDE_FADE",
        "WIND_ANIMATION",
        "SHADER_ULTRA",
        "MATERIAL_DECAL",
        "MATERIAL_DETAIL",
        "MATERIAL_LIGHTMAP",
        "PBR_DECAL",
        "PBR_DETAIL",
        "PBR_LIGHTMAP",
        "IGNORE_GLOBAL_FLAT_COLOR",
        "IGNORE_LIGHTMAP_ADJUSTMENT",
        "IGNORE_BASE_COLOR_PBR_TINT",
        "IGNORE_ROUGHNESS_PBR_TINT",
        "FLORA_LOD_TRANSITION_NEAR",
        "FLORA_LOD_TRANSITION_FAR",
        "FLORA_BILLBOARD",
        "FLORA_AMBIENT_ANIMATION",
        "FLORA_WIND_ANIMATION",
        "FLORA_WAVE_ANIMATION",
        "FLORA_PBR_LIGHTING",
        "FLORA_NORMAL_MAP",
        "FLORA_EDGE_MAP",
        "FLORA_FAKE_SHADOW",
        "FLORA_LAYING",
        "EMISSIVE_COLOR",
        "TILED_DECAL_EMISSIVE_COLOR",
        "TILED_DECAL_NOISE_SPREADING",
        "TILED_DECAL_SPATIAL_SPREADING",
        "EMISSIVE_ALBEDO",
        "TILED_DECAL_EMISSIVE_ALBEDO",
        "TILED_DECAL_ANIM_MASK",
        "WATER_RENDER_OBJECT",
        "WATER_TESSELLATION",
        "WATER_DEFORMATION",
        "BILLBOARD_FACE_MAIN_CAMERA",
        "VERTEX_VERTICAL_OFFSET",
        "VERTEX_DISPLACEMENT",
        "GLOBAL_TINT",
        "GLOBAL_PBR_TINT"
    };

    Vector<VariantType> flags;
    for (auto flagName : flagsArray)
    {
        VariantType flag;
        flag.SetString(flagName);
        flags.push_back(flag);
    }

    defaultArchive->SetVariantVector("flags", flags);

    Vector<String> presetsArray = {
        "Additive",
        "ReflectionMask",
        "Multiplicative",
        "ForceAfterOpaqueLayer",
        "DisableShadow",
        "ForceAlphaTestLayer",
        "AlphaTest",
        "ForegroundEffect",
        "HighlightAnimation",
        "LodTransition",
        "DisableShadowTransition",
        "Opaque",
        "Clamp",
        "LightMap",
        "NoDepthTest",
        "ForceVegetationLayer",
        "AlphaBlendAdditive",
        "Debug",
        "WaterDeformation",
        "AlphaMask",
        "ForceDrawDepthPrepass",
        "DisableDepthWrite",
        "DebugColorUnlit",
        "DisableShadowRuntimeOnly",
        "AlphaBlend",
        "SimpleColor",
        "BlendWithConstAlpha",
        "ForceAfterDynamicDecalsLayer",
        "DynamicDecalLayer",
        "FlowMap",
        "ForceTranclucentLayer",
        "VertexColor",
        "CullFrontDepthGreater"
    };

    Vector<VariantType> presets;
    for (auto presetName : presetsArray)
    {
        VariantType preset;
        preset.SetString(presetName);
        presets.push_back(preset);
    }

    defaultArchive->SetVariantVector("presets", presets);

    Vector<String> texturesArray = {
        "albedo",
        "normalmap",
        "specularmap",
        "detail",
        "lightmap",
        "pbrLightmap",
        "decal",
        "cubemap",
        "heightmap",
        "tangentSpace",
        "decalmask",
        "decaltexture",
        "decalTexture1",
        "decalTexture2",
        "decalTexture3",
        "mask",
        "flowmap",
        "noiseTex",
        "alphaRemapTex",
        "dynamicReflection",
        "dynamicRefraction",
        "heatMap",
        "particlesRT",
        "envReflectionMask",
        "dirtNormalMap",
        "dirtHeightMap",
        "wetnessBoundaryMap",
        "vertexAnimationTex",
        "dynamicShadowMap",
        "dynamicDepthPrepass",
        "blueNoise64x64",
        "perlinNoise",
        "specularBRDF",
        "specularReflectionMap",
        "diffuseIrradianceMap",
        "dynamicTextureSrc0",
        "vegetationColorMap",
        "vegetationDensityMap",
        "baseColorMap",
        "baseNormalMap",
        "baseRMMap",
        "miscMap",
        "maskMap",
        "roughnessAOMap",
        "pbrAlbedoRoughnessMap",
        "pbrLandscapeNormalMap",
        "pbrLandscapeLightmap",
        "tileAlbedoHeightArray",
        "tileNormalArray",
        "tileRoughnessAOArray",
        "tileRoughnessMetallicArray",
        "tileAOEmissionArray",
        "decalColorMap",
        "decalNormalMap",
        "decalRMMap",
        "pbrDecalColorRoughnessMap",
        "pbrDecalLightmap",
        "pbrDetailColorMap",
        "pbrDetailNormalMap",
        "pbrDetailRoughnessAOMap",
        "floraColorMap",
        "floraPbrColorMap",
        "floraLandscapeNormalMap",
        "floraLightmap",
        "floraEdgeMap",
        "floraFakeShadow"
    };

    Vector<VariantType> textures;
    for (auto textureName : texturesArray)
    {
        VariantType texture;
        texture.SetString(textureName);
        textures.push_back(texture);
    }

    defaultArchive->SetVariantVector("textures", textures);

    Vector<PropertyDefinition> defs = {
        { "lightPosition0", 2, 1, VariantType(Vector3(0.0f, 0.0f, 0.0f)) },
        { "ambientColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "diffuseColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "specularColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "materialLightAmbientColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "materialLightDiffuseColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "materialLightSpecularColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "lightIntensity0", 0, 1, VariantType(0.5f) },
        { "materialSpecularShininess", 0, 1, VariantType(0.5f) },
        { "fogLimit", 0, 1, VariantType(1.0f) },
        { "fogColor", 2, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "fogDensity", 0, 1, VariantType(0.005f) },
        { "fogStart", 0, 1, VariantType(0.0f) },
        { "fogEnd", 0, 1, VariantType(500.0f) },
        { "fogHalfspaceDensity", 0, 1, VariantType(0.005f) },
        { "fogHalfspaceHeight", 0, 1, VariantType(50.0f) },
        { "fogHalfspaceFalloff", 0, 1, VariantType(0.005f) },
        { "fogHalfspaceLimit", 0, 1, VariantType(1.0f) },
        { "fogAtmosphereColorSun", 2, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "fogAtmosphereColorSky", 2, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "fogAtmosphereScattering", 0, 1, VariantType(1.0f) },
        { "fogAtmosphereDistance", 0, 1, VariantType(500.0f) },
        { "flatColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "texture0Shift", 1, 1, VariantType(Vector2(0.0f, 0.0f)) },
        { "uvOffset", 1, 1, VariantType(Vector2(0.0f, 0.0f)) },
        { "uvScale", 1, 1, VariantType(Vector2(0.0f, 0.0f)) },
        { "lightmapSize", 0, 1, VariantType(1.0f) },
        { "decalTileCoordScale", 1, 1, VariantType(Vector2(0.0f, 0.0f)) },
        { "decalTileColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "detailTileCoordScale", 1, 1, VariantType(Vector2(0.0f, 0.0f)) },
        { "shadowColor", 3, 1, VariantType(Color(1.0f, 0.0f, 0.0f, 1.0f)) },
        { "forcedShadowDirection", 2, 1, VariantType(Vector3(0.0f, 0.0f, 0.0f)) },
        { "waterColor", 3, 1, VariantType(Color(0.0f, 0.0f, 0.0f, 1.0f)) }
    };

    Vector<VariantType> properties = DefaultArchiveProperties(defs);
    defaultArchive->SetVariantVector("properties", properties);
    return defaultArchive->SaveToYamlString();
}

void NMaterialStateDynamicSingleton::SetDefaultArchiveYaml()
{
    archive->LoadFromYamlString(GetDefaultArchiveYaml());
}

Vector<VariantType> NMaterialStateDynamicSingleton::DefaultArchiveProperties(Vector<PropertyDefinition> defs)
{
    Vector<VariantType> result;

    for (const auto& def : defs)
    {
        VariantType prop;
        ScopedPtr<KeyedArchive> arch(new KeyedArchive());
        arch->SetString("name", def.name);
        arch->SetUInt32("type", def.type);
        arch->SetUInt32("size", def.size);
        arch->SetVariant("defaultValue", def.defaultValue);
        prop.SetKeyedArchive(arch);
        result.push_back(prop);
    }

    return result;
}

} // namespace DAVA