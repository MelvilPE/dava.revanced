#ifndef __DAVAENGINE_NMATERIAL_NAMES_H__
#define __DAVAENGINE_NMATERIAL_NAMES_H__

#include "Base/FastName.h"

namespace DAVA
{
class NMaterialName
{
public:
    static const FastName DECAL_ALPHABLEND;
    static const FastName DECAL_ALPHABLEND_CULLFACE;
    static const FastName PIXELLIT_SPECULARMAP_ALPHATEST;
    static const FastName TEXTURED_ALPHABLEND;
    static const FastName TEXTURED_ALPHABLEND_CULLFACE;
    static const FastName DECAL_ALPHATEST;
    static const FastName PIXELLIT_SPECULARMAP_OPAQUE;
    static const FastName TEXTURED_ALPHATEST;
    static const FastName TEXTURED_VERTEXCOLOR_ALPHATEST;
    static const FastName TEXTURED_VERTEXCOLOR_ALPHABLEND;
    static const FastName DECAL_OPAQUE;
    static const FastName TEXTURED_OPAQUE;
    static const FastName TEXTURED_OPAQUE_NOCULL;
    static const FastName TEXTURED_VERTEXCOLOR_OPAQUE;
    static const FastName DETAIL_ALPHABLEND;
    static const FastName SHADOWRECT;
    static const FastName TILE_MASK;
    static const FastName TILE_MASK_DEBUG;
    static const FastName DETAIL_ALPHATEST;
    static const FastName SHADOW_VOLUME;
    static const FastName VERTEXCOLOR_ALPHABLEND;
    static const FastName VERTEXCOLOR_ALPHABLEND_NODEPTHTEST;
    static const FastName VERTEXCOLOR_ALPHABLEND_TEXTURED;
    static const FastName DETAIL_OPAQUE;
    static const FastName SILHOUETTE;
    static const FastName VERTEXCOLOR_FRAMEBLEND_ALPHABLEND;
    static const FastName SKYOBJECT;
    static const FastName VERTEXCOLOR_FRAMEBLEND_OPAQUE;
    static const FastName PIXELLIT_ALPHATEST;
    static const FastName SPEEDTREE_ALPHATEST;
    static const FastName SPEEDTREE_ALPHABLEND;
    static const FastName SPEEDTREE_ALPHABLEND_ALPHATEST;
    static const FastName SPEEDTREE_OPAQUE;
    static const FastName SPHERICLIT_SPEEDTREE_ALPHATEST;
    static const FastName SPHERICLIT_SPEEDTREE_ALPHABLEND;
    static const FastName SPHERICLIT_SPEEDTREE_ALPHABLEND_ALPHATEST;
    static const FastName SPHERICLIT_TEXTURED_OPAQUE;
    static const FastName SPHERICLIT_TEXTURED_ALPHATEST;
    static const FastName SPHERICLIT_TEXTURED_ALPHABLEND;
    static const FastName SPHERICLIT_TEXTURED_VERTEXCOLOR_OPAQUE;
    static const FastName SPHERICLIT_TEXTURED_VERTEXCOLOR_ALPHATEST;
    static const FastName SPHERICLIT_TEXTURED_VERTEXCOLOR_ALPHABLEND;
    static const FastName VERTEXCOLOR_OPAQUE;
    static const FastName VERTEXCOLOR_OPAQUE_NODEPTHTEST;
    static const FastName PIXELLIT_OPAQUE;
    static const FastName TEXTURE_LIGHTMAP_ALPHABLEND;
    static const FastName VERTEXLIT_ALPHATEST;
    static const FastName PIXELLIT_SPECULAR_ALPHATEST;
    static const FastName TEXTURE_LIGHTMAP_ALPHATEST;
    static const FastName VERTEXLIT_OPAQUE;
    static const FastName PIXELLIT_SPECULAR_OPAQUE;
    static const FastName TEXTURE_LIGHTMAP_OPAQUE;
    static const FastName GRASS;

    static const FastName PARTICLES;
    static const FastName PARTICLES_FRAMEBLEND;

    static const FastName DEBUG_DRAW_OPAQUE;
    static const FastName DEBUG_DRAW_ALPHABLEND;
    static const FastName DEBUG_DRAW_WIREFRAME;
    static const FastName DEBUG_DRAW_PARTICLES;
    static const FastName DEBUG_DRAW_PARTICLES_NO_DEPTH;

    static const FastName WATER_ALL_QUALITIES;

    static const FastName WATER_PER_PIXEL_REAL_REFLECTIONS;
    static const FastName WATER_PER_PIXEL_CUBEMAP_ALPHABLEND;
    static const FastName WATER_PER_VERTEX_CUBEMAP_DECAL;

    static const FastName NORMALIZED_BLINN_PHONG_PER_PIXEL_OPAQUE;
    static const FastName NORMALIZED_BLINN_PHONG_PER_PIXEL_FAST_OPAQUE;
    static const FastName NORMALIZED_BLINN_PHONG_PER_VERTEX_OPAQUE;

    static const FastName BLINN_PHONG;
    static const FastName DECAL;
    static const FastName DETAIL;
    static const FastName DETAIL_Q;
    static const FastName SPHERICAL_LIT_Q;
    static const FastName VERTEXCOLOR;
    static const FastName TEXTURED;
    static const FastName TEXTURE_LIGHTMAP;
    static const FastName SPEEDTREE;
    static const FastName SPEEDTREE_SPHERICAL_LIT_Q;
};

class NMaterialTextureName
{
public:
    static const FastName TEXTURE_ALBEDO;
    static const FastName TEXTURE_NORMAL;
    static const FastName TEXTURE_SPECULAR;
    static const FastName TEXTURE_DETAIL;
    static const FastName TEXTURE_LIGHTMAP;
    static const FastName TEXTURE_PBR_LIGHTMAP;
    static const FastName TEXTURE_DECAL;
    static const FastName TEXTURE_CUBEMAP;
    static const FastName TEXTURE_HEIGHTMAP;
    static const FastName TEXTURE_TANGENTSPACE;
    static const FastName TEXTURE_DECALMASK;
    static const FastName TEXTURE_DECALTEXTURE;
    static const FastName TEXTURE_TILED_DECAL_1;
    static const FastName TEXTURE_TILED_DECAL_2;
    static const FastName TEXTURE_TILED_DECAL_3;
    static const FastName TEXTURE_MASK;
    static const FastName TEXTURE_FLOW;
    static const FastName TEXTURE_NOISE;
    static const FastName TEXTURE_ALPHA_REMAP;
    static const FastName TEXTURE_DYNAMIC_REFLECTION;
    static const FastName TEXTURE_DYNAMIC_REFRACTION;
    static const FastName TEXTURE_PARTICLES_HEATMAP;
    static const FastName TEXTURE_PARTICLES_RT;
    static const FastName TEXTURE_ENV_REFLECTION_MASK;
    static const FastName TEXTURE_DIRT_NORMAL;
    static const FastName TEXTURE_DIRT_HEIGHTMAP;
    static const FastName TEXTURE_WETNESS_BOUNDARY;
    static const FastName TEXTURE_VERTEX_ANIMATION_TEXTURE;
    static const FastName TEXTURE_DYNAMIC_SHADOW_MAP;
    static const FastName TEXTURE_DYNAMIC_DEPTH_PREPASS;
    static const FastName TEXTURE_DYNAMIC_BLUE_NOISE;
    static const FastName TEXTURE_DYNAMIC_PERLIN_NOISE;
    static const FastName TEXTURE_DYNAMIC_SPECULAR_BRDF;
    static const FastName TEXTURE_DYNAMIC_SPECULAR_REFLECTION_MAP;
    static const FastName TEXTURE_DYNAMIC_DIFFUSE_IRRADIANCE_MAP;
    static const FastName TEXTURE_DYNAMIC_SRC_0;
    static const FastName TEXTURE_VEGETATION_COLOR_MAP;
    static const FastName TEXTURE_VEGETATION_DENSITY_MAP;
    static const FastName TEXTURE_PBR_BASE_COLOR_MAP;
    static const FastName TEXTURE_PBR_BASE_NORMAL_MAP;
    static const FastName TEXTURE_PBR_BASE_RM_MAP;
    static const FastName TEXTURE_PBR_MISC_MAP;
    static const FastName TEXTURE_PBR_MASK_MAP;
    static const FastName TEXTURE_PBR_ROUGHNESS_AO_MAP;
    static const FastName TEXTURE_PBR_LANDSCAPE_ALBEDO_ROUGHNESS_MAP;
    static const FastName TEXTURE_PBR_LANDSCAPE_NORMAL_MAP;
    static const FastName TEXTURE_PBR_LANDSCAPE_LIGHTMAP;
    static const FastName TEXTURE_PBR_LANDSCAPE_TILE_ALBEDO_HEIGHT_ARRAY;
    static const FastName TEXTURE_PBR_LANDSCAPE_TILE_NORMAL_ARRAY;
    static const FastName TEXTURE_PBR_LANDSCAPE_TILE_ROUGHNESS_AO_ARRAY;
    static const FastName TEXTURE_PBR_LANDSCAPE_TILE_ROUGHNESS_METALLIC_ARRAY;
    static const FastName TEXTURE_PBR_LANDSCAPE_TILE_AO_EMISSION_ARRAY;
    static const FastName TEXTURE_PBR_DECAL_COLOR_MAP;
    static const FastName TEXTURE_PBR_DECAL_NORMAL_MAP;
    static const FastName TEXTURE_PBR_DECAL_RM_MAP;
    static const FastName TEXTURE_PBR_DECAL_COLOR_ROUGHNESS_MAP;
    static const FastName TEXTURE_PBR_DECAL_LIGHTMAP;
    static const FastName TEXTURE_PBR_DETAIL_COLOR_MAP;
    static const FastName TEXTURE_PBR_DETAIL_NORMAL_MAP;
    static const FastName TEXTURE_PBR_DETAIL_ROUGHNESS_AO_MAP;
    static const FastName TEXTURE_FLORA_COLOR_MAP;
    static const FastName TEXTURE_FLORA_PBR_COLOR_MAP;
    static const FastName TEXTURE_FLORA_LANDSCAPE_NORMAL_MAP;
    static const FastName TEXTURE_FLORA_LIGHTMAP;
    static const FastName TEXTURE_FLORA_EDGE_MAP;
    static const FastName TEXTURE_FLORA_FAKE_SHADOW;

    static bool IsRuntimeTexture(const FastName& texture);
};

class NMaterialParamName
{
public:
    static const FastName PARAM_LIGHT_POSITION0;
    static const FastName PARAM_PROP_AMBIENT_COLOR;
    static const FastName PARAM_PROP_DIFFUSE_COLOR;
    static const FastName PARAM_PROP_SPECULAR_COLOR;
    static const FastName PARAM_LIGHT_AMBIENT_COLOR;
    static const FastName PARAM_LIGHT_DIFFUSE_COLOR;
    static const FastName PARAM_LIGHT_SPECULAR_COLOR;
    static const FastName PARAM_LIGHT_INTENSITY0;
    static const FastName PARAM_MATERIAL_SPECULAR_SHININESS;
    static const FastName PARAM_FOG_LIMIT;
    static const FastName PARAM_FOG_COLOR;
    static const FastName PARAM_FOG_DENSITY;
    static const FastName PARAM_FOG_START;
    static const FastName PARAM_FOG_END;
    static const FastName PARAM_FOG_ATMOSPHERE_COLOR_SUN;
    static const FastName PARAM_FOG_ATMOSPHERE_COLOR_SKY;
    static const FastName PARAM_FOG_ATMOSPHERE_SCATTERING;
    static const FastName PARAM_FOG_ATMOSPHERE_DISTANCE;
    static const FastName PARAM_FOG_HALFSPACE_HEIGHT;
    static const FastName PARAM_FOG_HALFSPACE_DENSITY;
    static const FastName PARAM_FOG_HALFSPACE_FALLOFF;
    static const FastName PARAM_FOG_HALFSPACE_LIMIT;
    static const FastName PARAM_FLAT_COLOR;
    static const FastName PARAM_TEXTURE0_SHIFT;
    static const FastName PARAM_UV_OFFSET;
    static const FastName PARAM_UV_SCALE;
    static const FastName PARAM_LIGHTMAP_SIZE;
    static const FastName PARAM_DECAL_TILE_SCALE;
    static const FastName PARAM_DECAL_TILE_COLOR;
    static const FastName PARAM_DETAIL_TILE_SCALE;
    static const FastName PARAM_RCP_SCREEN_SIZE;
    static const FastName PARAM_SCREEN_OFFSET;
    static const FastName PARAM_ALPHATEST_THRESHOLD;
    static const FastName PARAM_LANDSCAPE_TEXTURE_TILING;
    static const FastName WATER_CLEAR_COLOR;
    static const FastName DEPRECATED_SHADOW_COLOR_PARAM;
    static const FastName DEPRECATED_LANDSCAPE_TEXTURE_0_TILING;
    static const FastName PARAM_TREE_LEAF_COLOR_MUL;
    static const FastName FORCED_SHADOW_DIRECTION_PARAM;
    static const FastName PARAM_SPECULAR_SCALE;
    static const FastName PARAM_PARTICLES_GRADIENT_COLOR_FOR_WHITE;
    static const FastName PARAM_PARTICLES_GRADIENT_COLOR_FOR_BLACK;
    static const FastName PARAM_PARTICLES_GRADIENT_COLOR_FOR_MIDDLE;
    static const FastName PARAM_PARTICLES_GRADIENT_MIDDLE_POINT;
};

class NMaterialPresetName
{
public:
    static const FastName ADDITIVE;
    static const FastName REFLECTION_MASK;
    static const FastName MULTIPLICATIVE;
    static const FastName FORCE_AFTER_OPAQUE_LAYER;
    static const FastName DISABLE_SHADOW;
    static const FastName FORCE_ALPHA_TEST_LAYER;
    static const FastName ALPHA_TEST;
    static const FastName FOREGROUND_EFFECT;
    static const FastName HIGHLIGHT_ANIMATION;
    static const FastName LOD_TRANSITION;
    static const FastName DISABLE_SHADOW_TRANSITION;
    static const FastName PRESET_OPAQUE;
    static const FastName CLAMP;
    static const FastName LIGHTMAP;
    static const FastName NO_DEPTH_TEST;
    static const FastName FORCE_VEGETATION_LAYER;
    static const FastName ALPHA_BLEND_ADDITIVE;
    static const FastName DEBUG;
    static const FastName WATER_DEFORMATION;
    static const FastName ALPHA_MASK;
    static const FastName FORCE_DRAW_DEPTH_PREPASS;
    static const FastName DISABLE_DEPTH_WRITE;
    static const FastName DEBUG_COLOR_UNLIT;
    static const FastName DISABLE_SHADOW_RUNTIME_ONLY;
    static const FastName ALPHA_BLEND;
    static const FastName SIMPLE_COLOR;
    static const FastName BLEND_WITH_CONST_ALPHA;
    static const FastName FORCE_AFTER_DYNAMIC_DECALS_LAYER;
    static const FastName DYNAMIC_DECAL_LAYER;
    static const FastName FLOW_MAP;
    static const FastName FORCE_TRANCLUCENT_LAYER;
    static const FastName VERTEX_COLOR;
    static const FastName CULL_FRONT_DEPTH_GREATER;

    static bool IsRuntimePreset(const FastName& preset);
};

class NMaterialFlagName
{
public:
    static const FastName FLAG_BLENDING;
    static const FastName FLAG_VERTEXFOG;
    static const FastName FLAG_FOG_LINEAR;
    static const FastName FLAG_FOG_HALFSPACE;
    static const FastName FLAG_FOG_HALFSPACE_LINEAR;
    static const FastName FLAG_FOG_ATMOSPHERE;
    static const FastName FLAG_FOG_ATMOSPHERE_NO_ATTENUATION;
    static const FastName FLAG_FOG_ATMOSPHERE_NO_SCATTERING;
    static const FastName FLAG_TEXTURESHIFT;
    static const FastName FLAG_ENABLE_FOG;
    static const FastName FLAG_ENABLE_HIGH_QUALITY_FOG;
    static const FastName FLAG_TEXTURE_SHIFT;
    static const FastName FLAG_TEXTURE0_ANIMATION_SHIFT;
    static const FastName FLAG_WAVE_ANIMATION;
    static const FastName FLAG_FAST_NORMALIZATION;
    static const FastName FLAG_TILED_DECAL_MASK;
    static const FastName FLAG_TILED_DECAL_ROTATION;
    static const FastName FLAG_TILED_DECAL_TRANSFORM;
    static const FastName FLAG_TILED_DECAL_ANIMATED_EMISSION;
    static const FastName FLAG_TILED_DECAL_BLEND_NORMAL;
    static const FastName FLAG_TILED_DECAL_OVERRIDE_ROUGHNESS_METALLIC;
    static const FastName FLAG_ALBEDO_TRANSFORM;
    static const FastName FLAG_FLATCOLOR;
    static const FastName FLAG_FLAT_ALBEDO;
    static const FastName FLAG_DISTANCE_ATTENUATION;
    static const FastName FLAG_SPECULAR;
    static const FastName FLAG_SEPARATE_NORMALMAPS;
    static const FastName FLAG_VERTEX_COLOR;
    static const FastName FLAG_SPEED_TREE_OBJECT;
    static const FastName FLAG_SPEEDTREE_JOINT_TRANSFORM;
    static const FastName FLAG_SPEEDTREE_JOINT_LENGTHWISE_TRANSFORM;
    static const FastName FLAG_SPHERICAL_LIT;
    static const FastName FLAG_TANGENT_SPACE_WATER_REFLECTIONS;
    static const FastName FLAG_DEBUG_UNITY_Z_NORMAL;
    static const FastName FLAG_DEBUG_Z_NORMAL_SCALE;
    static const FastName FLAG_DEBUG_NORMAL_ROTATION;
    static const FastName FLAG_HARD_SKINNING;
    static const FastName FLAG_SOFT_SKINNING;
    static const FastName FLAG_FLOWMAP_SKY;
    static const FastName FLAG_PARTICLES_MASK;
    static const FastName FLAG_PARTICLES_FLOWMAP;
    static const FastName FLAG_PARTICLES_FLOWMAP_ANIMATION;
    static const FastName FLAG_PARTICLES_PERSPECTIVE_MAPPING;
    static const FastName FLAG_PARTICLES_THREE_POINT_GRADIENT;
    static const FastName FLAG_PARTICLES_NOISE;
    static const FastName FLAG_PARTICLES_FRESNEL_TO_ALPHA;
    static const FastName FLAG_PARTICLES_ALPHA_REMAP;
    static const FastName FLAG_PARTICLES_VERTEX_ANIMATION;
    static const FastName FLAG_PARTICLES_VERTEX_ANIMATION_MASK;
    static const FastName FLAG_LIGHTMAPONLY;
    static const FastName FLAG_TEXTUREONLY;
    static const FastName FLAG_SETUPLIGHTMAP;
    static const FastName FLAG_VIEWALBEDO;
    static const FastName FLAG_VIEWNORMAL;
    static const FastName FLAG_VIEWNORMAL_FINAL;
    static const FastName FLAG_VIEWROUGHNESS;
    static const FastName FLAG_VIEWMETALLIC;
    static const FastName FLAG_VIEWAMBIENT_OCCLUSION;
    static const FastName FLAG_VIEWAMBIENT;
    static const FastName FLAG_VIEWDIFFUSE;
    static const FastName FLAG_VIEWSPECULAR;
    static const FastName FLAG_ALPHA_EROSION;
    static const FastName FLAG_SOFT_PARTICLES;
    static const FastName FLAG_FRAME_BLEND;
    static const FastName FLAG_FORCE_2D_MODE;
    static const FastName FLAG_LANDSCAPE_USE_INSTANCING;
    static const FastName FLAG_LANDSCAPE_LOD_MORPHING;
    static const FastName FLAG_LANDSCAPE_MORPHING_COLOR;
    static const FastName FLAG_HEIGHTMAP_FLOAT_TEXTURE;
    static const FastName FLAG_ALPHA_TEST;
    static const FastName FLAG_ALPHA_TEST_VALUE;
    static const FastName FLAG_ALPHA_STEP_VALUE;
    static const FastName FLAG_ILLUMINATION_USED;
    static const FastName FLAG_ILLUMINATION_SHADOW_CASTER;
    static const FastName FLAG_ILLUMINATION_SHADOW_RECEIVER;
    static const FastName FLAG_TEST_OCCLUSION;
    static const FastName FLAG_FORCED_SHADOW_DIRECTION;
    static const FastName FLAG_PARTICLES_DEBUG_SHOW_HEATMAP;
    static const FastName FLAG_GEO_DECAL;
    static const FastName FLAG_GEO_DECAL_SPECULAR;
    static const FastName FLAG_VERTEX_DISTORTION_DEPRECATED;
    static const FastName FLAG_NORMAL_DETAIL;
    static const FastName FLAG_LANDSCAPE_USE_RELAXMAP;
    static const FastName FLAG_LANDSCAPE_HEIGHT_BLEND;
    static const FastName FLAG_LANDSCAPE_SEPARATE_LIGHTMAP_CHANNEL;
    static const FastName FLAG_LANDSCAPE_HAS_METALLIC_AND_EMISSION;
    static const FastName FLAG_LANDSCAPE_SCALED_TILES_NON_PBR;
    static const FastName FLAG_ALLOW_POINT_LIGHTS;
    static const FastName FLAG_FORCE_POINT_LIGHTS_ENABLED;
    static const FastName FLAG_ENVIRONMENT_MAPPING;
    static const FastName FLAG_ENVIRONMENT_MAPPING_NORMALMAP;
    static const FastName FLAG_BLEND_BY_ANGLE;
    static const FastName FLAG_SHADOW_RECEIVER;
    static const FastName FLAG_USE_SHADOW_MAP;
    static const FastName FLAG_DEBUG_SHADOW_CASCADES;
    static const FastName FLAG_SHADOW_CASCADES_COUNT;
    static const FastName FLAG_SHADOW_CASCADES_BLEND;
    static const FastName FLAG_SHADOW_PCF;
    static const FastName FLAG_DECAL_ALBEDO;
    static const FastName FLAG_DECAL_NORMAL;
    static const FastName FLAG_DECAL_TREAD;
    static const FastName FLAG_FADE_OUT_WITH_TIME;
    static const FastName DEPTH_TARGET_IS_FRAMEBUFFER;
    static const FastName DEPTH_PREPASS_ENABLED;
    static const FastName FLAG_VEGETATION_BEND;
    static const FastName VEGETATION_LIT;
    static const FastName FLAG_PUSH_TO_NEAR_PLANE_HACK;
    static const FastName FLAG_DISTANCE_FADE_OUT;
    static const FastName FLAG_POINT_LIGHTS_OVERRIDE_SHADOW;
    static const FastName FLAG_DIFFUSE_IN_LIGHTMAP;
    static const FastName FLAG_AMBIENT_ATTENUATION_BOX;
    static const FastName PASS_NAME_ENGINE_DEFINE;
    static const FastName FLAG_DECAL_TEXTURE_COUNT;
    static const FastName FLAG_VIEW_MODE_OVERDRAW_HEAT;
    static const FastName FLAG_WATER_RIPPLES_ENABLED;
    static const FastName FLAG_WATER_RIPPLES_ALLOWED;
    static const FastName FLAG_INSTANCED_CHAIN;
    static const FastName FLAG_DIRT_COVERAGE;
    static const FastName FLAG_WETNESS_MULTILEVEL;
    static const FastName FLAG_WETNESS_SIMPLIFIED;
    static const FastName FLAG_COLORBLIND_MODE;
    static const FastName FLAG_DEBUG_UNLIT;
    static const FastName FLAG_HIGHLIGHT_COLOR;
    static const FastName FLAG_HIGHLIGHT_WAVE_ANIM;
    static const FastName FLAG_DECAL_VERTICAL_FADE;
    static const FastName FLAG_DECAL_BACK_SIDE_FADE;
    static const FastName FLAG_WIND_ANIMATION;
    static const FastName FLAG_SHADER_ULTRA;
    static const FastName FLAG_MATERIAL_DECAL;
    static const FastName FLAG_MATERIAL_DETAIL;
    static const FastName FLAG_MATERIAL_LIGHTMAP;
    static const FastName FLAG_PBR_DECAL;
    static const FastName FLAG_PBR_DETAIL;
    static const FastName FLAG_PBR_LIGHTMAP;
    static const FastName FLAG_IGNORE_GLOBAL_FLAT_COLOR;
    static const FastName FLAG_IGNORE_LIGHTMAP_ADJUSTMENT;
    static const FastName FLAG_IGNORE_BASE_COLOR_PBR_TINT;
    static const FastName FLAG_IGNORE_ROUGHNESS_PBR_TINT;
    static const FastName FLAG_FLORA_LOD_TRANSITION_NEAR;
    static const FastName FLAG_FLORA_LOD_TRANSITION_FAR;
    static const FastName FLAG_FLORA_BILLBOARD;
    static const FastName FLAG_FLORA_AMBIENT_ANIMATION;
    static const FastName FLAG_FLORA_WIND_ANIMATION;
    static const FastName FLAG_FLORA_WAVE_ANIMATION;
    static const FastName FLAG_FLORA_PBR_LIGHTING;
    static const FastName FLAG_FLORA_NORMAL_MAP;
    static const FastName FLAG_FLORA_EDGE_MAP;
    static const FastName FLAG_FLORA_FAKE_SHADOW;
    static const FastName FLAG_FLORA_LAYING;
    static const FastName FLAG_EMISSIVE_COLOR;
    static const FastName FLAG_TILED_DECAL_EMISSIVE_COLOR;
    static const FastName FLAG_TILED_DECAL_NOISE_SPREADING;
    static const FastName FLAG_TILED_DECAL_SPATIAL_SPREADING;
    static const FastName FLAG_EMISSIVE_ALBEDO;
    static const FastName FLAG_TILED_DECAL_EMISSIVE_ALBEDO;
    static const FastName FLAG_TILED_DECAL_ANIM_MASK;
    static const FastName FLAG_WATER_RENDER_OBJECT;
    static const FastName FLAG_WATER_TESSELLATION;
    static const FastName FLAG_WATER_DEFORMATION;
    static const FastName FLAG_BILLBOARD_FACE_MAIN_CAMERA;
    static const FastName FLAG_VERTEX_VERTICAL_OFFSET;
    static const FastName FLAG_VERTEX_DISPLACEMENT;
    static const FastName FLAG_GLOBAL_TINT;
    static const FastName FLAG_GLOBAL_PBR_TINT;

    static bool IsRuntimeFlag(const FastName& flag);
};

class NMaterialSerializationKey
{
public:
    static const DAVA::String MaterialKey;
    static const DAVA::String ParentMaterialKey;
    static const DAVA::String FXName;
    static const DAVA::String PropertiesKey;
    static const DAVA::String TexturesKey;
    static const DAVA::String FlagsKey;
    static const DAVA::String QualityGroup;
    static const DAVA::String MaterialName;
    static const DAVA::String ConfigName;
    static const DAVA::String ConfigArchive;
    static const DAVA::String ConfigCount;
    static const FastName DefaultConfigName;
    static const DAVA::String CustomCullMode;
    static const DAVA::String EnabledPresets;
    static const DAVA::String ForceQuality;
};

class NMaterialQualityName
{
public:
    static const FastName QUALITY_FLAG_NAME;
    static const FastName QUALITY_GROUP_FLAG_NAME;
    static const FastName DEFAULT_QUALITY_NAME;
};
}; // namespace DAVA

#endif /* defined(__DAVAENGINE_NMATERIAL_NAMES_H__) */