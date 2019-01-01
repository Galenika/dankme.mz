#pragma once

#include "IAppSystem.hpp"
#include "IViewRender.hpp"

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define MAXSTUDIOSKINS		32

// These are given to FindMaterial to reference the texture groups that Show up on the 
#define TEXTURE_GROUP_LIGHTMAP						        "Lightmaps"
#define TEXTURE_GROUP_WORLD							          "World textures"
#define TEXTURE_GROUP_MODEL							          "Model textures"
#define TEXTURE_GROUP_VGUI							          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						        "Particle textures"
#define TEXTURE_GROUP_DECAL							          "Decal textures"
#define TEXTURE_GROUP_SKYBOX						          "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				      "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							          "Other textures"
#define TEXTURE_GROUP_PRECACHED						        "Precached"
#define TEXTURE_GROUP_CUBE_MAP						        "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					      "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					        "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			    "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			  "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			  "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					      "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					        "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					      "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				      "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			  "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					      "Morph Targets"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
struct MaterialSystem_Config_t;
class VMatrix;
class matrix3x4_t;
class ITexture;
struct MaterialSystemHWID_t;
class KeyValues;
class IShader;
class IVertexTexture;
class IMorph;
class ICallQueue;
struct MorphWeight_t;
class IFileList;
struct VertexStreamSpec_t;
struct ShaderStencilState_t;
struct MeshInstanceData_t;
class IClientMaterialSystem;
class CPaintMaterial;
class IPaintMapDataManager;
class IPaintMapTextureManager;
class GPUMemoryStats;
struct AspectRatioInfo_t;
struct CascadedShadowMappingState_t;

class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;

class LightDesc_t;
class MaterialMatrixMode_t;
class MaterialCullMode_t;
class MaterialHeightClipMode_t;
class MaterialFogMode_t;
class MaterialIndexFormat_t;
class OcclusionQueryObjectHandle_t;
class MaterialPrimitiveType_t;
class FlashlightState_t;
class MorphFormat_t;
class Rect_t;
class DeformationBase_t;
class ColorCorrectionHandle_t;
class MaterialNonInteractiveMode_t;

enum StencilOperation_t
{
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
	STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};

enum StencilComparisonFunction_t
{
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};

enum CompiledVtfFlags
{
    TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
    TEXTUREFLAGS_TRILINEAR = 0x00000002,
    TEXTUREFLAGS_CLAMPS = 0x00000004,
    TEXTUREFLAGS_CLAMPT = 0x00000008,
    TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
    TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
    TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
    TEXTUREFLAGS_NORMAL = 0x00000080,
    TEXTUREFLAGS_NOMIP = 0x00000100,
    TEXTUREFLAGS_NOLOD = 0x00000200,
    TEXTUREFLAGS_ALL_MIPS = 0x00000400,
    TEXTUREFLAGS_PROCEDURAL = 0x00000800,
    TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
    TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
    TEXTUREFLAGS_ENVMAP = 0x00004000,
    TEXTUREFLAGS_RENDERTARGET = 0x00008000,
    TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
    TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
    TEXTUREFLAGS_SINGLECOPY = 0x00040000,
    TEXTUREFLAGS_PRE_SRGB = 0x00080000,
    TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
    TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
    TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
    TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
    TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
    TEXTUREFLAGS_CLAMPU = 0x02000000,
    TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
    TEXTUREFLAGS_SSBUMP = 0x08000000,
    TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
    TEXTUREFLAGS_BORDER = 0x20000000,
    TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
    TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum StandardLightmap_t
{
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
    IMaterial	*material;
    int			lightmapPageID;
};

enum MaterialThreadMode_t
{
    MATERIAL_SINGLE_THREADED,
    MATERIAL_QUEUED_SINGLE_THREADED,
    MATERIAL_QUEUED_THREADED
};

enum MaterialContextType_t
{
    MATERIAL_HARDWARE_CONTEXT,
    MATERIAL_QUEUED_CONTEXT,
    MATERIAL_NULL_CONTEXT
};

enum
{
    MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialTextureInfo_t
{
    int iExcludeInformation;
};

struct ApplicationPerformanceCountersInfo_t
{
    float msMain;
    float msMST;
    float msGPU;
    float msFlip;
    float msTotal;
};

struct ApplicationInstantCountersInfo_t
{
    uint32_t m_nCpuActivityMask;
    uint32_t m_nDeferredWordsAllocated;
};
struct MaterialAdapterInfo_t
{
    char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
    unsigned int m_VendorID;
    unsigned int m_DeviceID;
    unsigned int m_SubSysID;
    unsigned int m_Revision;
    int m_nDXSupportLevel;			// This is the *preferred* dx support level
    int m_nMinDXSupportLevel;
    int m_nMaxDXSupportLevel;
    unsigned int m_nDriverVersionHigh;
    unsigned int m_nDriverVersionLow;
};

struct MaterialVideoMode_t
{
    int m_Width;			// if width and height are 0 and you select 
    int m_Height;			// windowed mode, it'll use the window size
    ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
    int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};
enum HDRType_t
{
    HDR_TYPE_NONE,
    HDR_TYPE_INTEGER,
    HDR_TYPE_FLOAT,
};

enum RestoreChangeFlags_t
{
    MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
    MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};

enum RenderTargetSizeMode_t
{
    RT_SIZE_NO_CHANGE = 0,
    RT_SIZE_DEFAULT = 1,
    RT_SIZE_PICMIP = 2,
    RT_SIZE_HDR = 3,
    RT_SIZE_FULL_FRAME_BUFFER = 4,
    RT_SIZE_OFFSCREEN = 5,
    RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
};

enum MaterialRenderTargetDepth_t
{
    MATERIAL_RT_DEPTH_SHARED = 0x0,
    MATERIAL_RT_DEPTH_SEPARATE = 0x1,
    MATERIAL_RT_DEPTH_NONE = 0x2,
    MATERIAL_RT_DEPTH_ONLY = 0x3,
};

typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);
typedef void(*EndFrameCleanupFunc_t)(void);
typedef bool(*EndFramePriorToNextContextFunc_t)(void);
typedef void(*OnLevelShutdownFunc_t)(void *pUserData);

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

class IMatRenderContext : public IRefCounted
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;
	virtual void				Flush(bool flushHardware = false) = 0;
	virtual void				BindLocalCubemap(ITexture *pTexture) = 0;
	virtual void				SetRenderTarget(ITexture *pTexture) = 0;
	virtual ITexture *			GetRenderTarget(void) = 0;
	virtual void				GetRenderTargetDimensions(int &width, int &height) const = 0;
	virtual void				Bind(IMaterial *material, void *proxyData = 0) = 0;
	virtual void				BindLightmapPage(int lightmapPageID) = 0;
	virtual void				DepthRange(float zNear, float zFar) = 0;
	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) = 0;
	virtual void				SetAmbientLight(float r, float g, float b) = 0;
	virtual void				SetLight(int lightNum, const LightDesc_t& desc) = 0;
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;
	virtual void				CopyRenderTargetToTexture(ITexture *pTexture) = 0;
	virtual void				SetFrameBufferCopyTexture(ITexture *pTexture, int textureIndex = 0) = 0;
	virtual ITexture		   *GetFrameBufferCopyTexture(int textureIndex) = 0;
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const& matrix) = 0;
	virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrix(VMatrix const& matrix) = 0;
	virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix *matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t *matrix) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;
	virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
	virtual void				SetHeightClipZ(float z) = 0;
	virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual MaterialFogMode_t	GetFogMode(void) = 0;
	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const* rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const* rgb) = 0;
	virtual void				GetFogColor(unsigned char *rgb) = 0;
	virtual void				SetNumBoneWeights(int numBones) = 0;
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char *pTextureBudgetGroup, IMaterial * pMaterial = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;
	virtual IMesh* GetDynamicMesh(bool buffered = true, IMesh* pVertexOverride = 0, IMesh* pIndexOverride = 0, IMaterial *pAutoBind = 0) = 0;
	virtual IVertexBuffer *CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char *pTextureBudgetGroup) = 0;
	virtual IIndexBuffer *CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char *pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer *) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer *) = 0;
	virtual IVertexBuffer *GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer *GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;
	virtual void BindVertexBuffer(int streamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0;
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;
	virtual void	SyncToken(const char *pToken) = 0;
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;
	virtual void SetFlashlightMode(bool bEnable) = 0;
	virtual void SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture) = 0;
	virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix &worldToView) = 0;
	virtual bool GetFlashlightMode() const = 0;
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void Unused3() {}
	virtual IMorph *CreateMorph(MorphFormat_t format, const char *pDebugName) = 0;
	virtual void DestroyMorph(IMorph *pMorph) = 0;
	virtual void BindMorph(IMorph *pMorph) = 0;
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;
	virtual void Unused4() {};
	virtual void Unused5() {};
	virtual void Unused6() {};
	virtual void Unused7() {};
	virtual void Unused8() {};
	virtual void ReadPixelsAndStretch(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) = 0;
	virtual void GetWindowSize(int &width, int &height) const = 0;
	virtual void DrawScreenSpaceRectangle(IMaterial *pMaterial, int destx, int desty, int width, int height, float src_texture_x0, float src_texture_y0, float src_texture_x1, float src_texture_y1, int src_texture_width, int src_texture_height, void *pClientRenderable = NULL, int nXDice = 1, int nYDice = 1) = 0;
	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;
	virtual void PushRenderTargetAndViewport() = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture) = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PopRenderTargetAndViewport(void) = 0;
	virtual void BindLightmapTexture(ITexture *pLightmapTexture) = 0;
	virtual void CopyRenderTargetToTextureEx(ITexture *pTexture, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture *pTexture, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const &value) = 0;
	virtual void SetStencilEnable(bool onoff) = 0;
	virtual void SetStencilFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilZFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilPassOperation(StencilOperation_t op) = 0;
	virtual void SetStencilCompareFunction(StencilComparisonFunction_t cmpfn) = 0;
	virtual void SetStencilReferenceValue(int ref) = 0;
	virtual void SetStencilTestMask(uint32 msk) = 0;
	virtual void SetStencilWriteMask(uint32 msk) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) = 0;
	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture *pTexture) = 0;
	virtual void PushCustomClipPlane(const float *pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0;
	virtual void GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices) = 0;
	virtual int GetMaxVerticesToRender(IMaterial *pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial *pMaterial1, IMaterial *pMaterial2, int lightMapID1, int lightMapID2) = 0;
	virtual IMesh *GetFlexMesh() = 0;
	virtual void SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture) = 0;
	virtual ITexture *GetLocalCubemap() = 0;
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;
	virtual bool EnableClipping(bool bEnable) = 0;
	virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ) = 0;
	virtual void BeginPIXEvent(unsigned long color, const char *szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char *szName) = 0;
	virtual void BeginBatch(IMesh* pIndices) = 0;
	virtual void BindBatch(IMesh* pVertices, IMaterial *pAutoBind = NULL) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;
	virtual ICallQueue *GetCallQueue() = 0;
	virtual void GetWorldSpaceCameraPosition(Vector *pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector *pVecForward, Vector *pVecRight, Vector *pVecUp) = 0;
	virtual void				ResetToneMappingScale(float monoscale) = 0;
	virtual void				SetGoalToneMappingScale(float monoscale) = 0;
	virtual void				TurnOnToneMapping() = 0;
	virtual void				SetToneMappingScaleLinear(const Vector &scale) = 0;
	virtual Vector				GetToneMappingScaleLinear(void) = 0;
	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;
	virtual void				PerformFullScreenStencilOperation(void) = 0;
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;
	virtual void				PushDeformation(DeformationBase_t const *Deformation) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;
	virtual bool				GetMorphAccumulatorTexCoord(Vector2D *pTexCoord, IMorph *pMorph, int nVertex) = 0;
	virtual IMesh*				GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true, IMesh* pVertexOverride = 0,	IMesh* pIndexOverride = 0, IMaterial *pAutoBind = 0) = 0;
	virtual void				FogMaxDensity(float flMaxDensity) = 0;
	virtual IMaterial *GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void *GetCurrentProxy() = 0;
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char *pName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char *pLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;
	virtual void SetNonInteractivePacifierTexture(ITexture *pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture *pTexture, MaterialNonInteractiveMode_t mode) = 0;
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	virtual void *			LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void *pData) = 0;
	template< class E > E*  LockRenderDataTyped(int nCount, const E* pSrcData = NULL);
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0;
	virtual bool			IsRenderData(const void *pData) const = 0;
	virtual void			PrintfVA(char *fmt, va_list vargs) = 0;
	virtual void			Printf(const char *fmt, ...) = 0;
	virtual float			Knob(char *knobname, float *setvalue = NULL) = 0;
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;
	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;
};

class IMaterialSystem : public IAppSystem
{
public:

    virtual CreateInterfaceFn               Init(char const* pShaderAPIDLL, IMaterialProxyFactory *pMaterialProxyFactory, CreateInterfaceFn fileSystemFactory, CreateInterfaceFn cvarFactory = NULL) = 0;
    virtual void                            SetShaderAPI(char const *pShaderAPIDLL) = 0;
    virtual void                            SetAdapter(int nAdapter, int nFlags) = 0;
    virtual void                            ModInit() = 0;
    virtual void                            ModShutdown() = 0;
    virtual void                            SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
    virtual MaterialThreadMode_t            GetThreadMode() = 0;
    virtual void                            ExecuteQueued() = 0;
    virtual void                            OnDebugEvent(const char *pEvent) = 0;
    virtual IMaterialSystemHardwareConfig*  GetHardwareConfig(const char *pVersion, int *returnCode) = 0;
    virtual void                            __unknown() = 0;
    virtual bool                            UpdateConfig(bool bForceUpdate) = 0; //20
    virtual bool                            OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;
    virtual const MaterialSystem_Config_t&  GetCurrentConfigForVideoCard() const = 0;
    virtual bool                            GetRecommendedConfigurationInfo(int nDXLevel, KeyValues * pKeyValues) = 0;
    virtual int                             GetDisplayAdapterCount() const = 0;
    virtual int                             GetCurrentAdapter() const = 0;
    virtual void                            GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;
    virtual int                             GetModeCount(int adapter) const = 0;
    virtual void                            GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;
    virtual void                            AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
    virtual void                            GetDisplayMode(MaterialVideoMode_t& mode) const = 0; //30
    virtual bool                            SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;
    virtual bool                            SupportsMSAAMode(int nMSAAMode) = 0;
    virtual const MaterialSystemHWID_t&     GetVideoCardIdentifier(void) const = 0;
    virtual void                            SpewDriverInfo() const = 0;
    virtual void                            GetBackBufferDimensions(int &width, int &height) const = 0;
    virtual ImageFormat                     GetBackBufferFormat() const = 0;
    virtual const AspectRatioInfo_t&        GetAspectRatioInfo() const = 0;
    virtual bool                            SupportsHDRMode(HDRType_t nHDRModede) = 0;
    virtual bool                            AddView(void* hwnd) = 0;
    virtual void                            RemoveView(void* hwnd) = 0; //40
    virtual void                            SetView(void* hwnd) = 0;
    virtual void                            BeginFrame(float frameTime) = 0;
    virtual void                            EndFrame() = 0;
    virtual void                            Flush(bool flushHardware = false) = 0;
    virtual unsigned int                    GetCurrentFrameCount() = 0;
    virtual void                            SwapBuffers() = 0;
    virtual void                            EvictManagedResources() = 0;
    virtual void                            ReleaseResources(void) = 0;
    virtual void                            ReacquireResources(void) = 0;
    virtual void                            AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0; //50
    virtual void                            RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
    virtual void                            AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
    virtual void                            RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
    virtual void                            AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
    virtual void                            RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
    virtual void                            OnLevelShutdown() = 0;
    virtual bool                            AddOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
    virtual bool                            RemoveOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
    virtual void                            OnLevelLoadingComplete() = 0;
    virtual void                            ResetTempHWMemory(bool bExitingLevel = false) = 0; //60
    virtual void                            HandleDeviceLost() = 0;
    virtual int                             ShaderCount() const = 0;
    virtual int                             GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;
    virtual int                             ShaderFlagCount() const = 0;
    virtual const char*                     ShaderFlagName(int nIndex) const = 0;
    virtual void                            GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;
    virtual IMaterialProxyFactory*          GetMaterialProxyFactory() = 0;
    virtual void                            SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;
    virtual void                            EnableEditorMaterials() = 0;
    virtual void                            EnableGBuffers() = 0; //70
    virtual void                            SetInStubMode(bool bInStubMode) = 0;
    virtual void                            DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
    virtual void                            DebugPrintUsedTextures(void) = 0;
    virtual void                            ToggleSuppressMaterial(char const* pMaterialName) = 0;
    virtual void                            ToggleDebugMaterial(char const* pMaterialName) = 0;
    virtual bool                            UsingFastClipping(void) = 0;
    virtual int                             StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer
    virtual void                            UncacheAllMaterials() = 0;
    virtual void                            UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
    virtual void                            CacheUsedMaterials() = 0; //80
    virtual void                            ReloadTextures() = 0;
    virtual void                            ReloadMaterials(const char *pSubString = NULL) = 0;
    virtual IMaterial*                      CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues) = 0;
    virtual IMaterial*                      FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL) = 0;
	virtual void							unk0() = 0;
	virtual MaterialHandle_t                FirstMaterial() const = 0;
    virtual MaterialHandle_t                NextMaterial(MaterialHandle_t h) const = 0;
    virtual MaterialHandle_t                InvalidMaterial() const = 0;
    virtual IMaterial*                      GetMaterial(MaterialHandle_t h) const = 0;
    virtual int                             GetNumMaterials() const = 0;
    virtual ITexture*                       FindTexture(char const* pTextureName, const char *pTextureGroupName, bool complain = true) = 0;
    virtual bool                            IsTextureLoaded(char const* pTextureName) const = 0;
    virtual ITexture*                       CreateProceduralTexture(const char	*pTextureName, const char *pTextureGroupName, int w, int h, ImageFormat fmt, int nFlags) = 0;
    virtual void                            BeginRenderTargetAllocation() = 0;
    virtual void                            EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources
    virtual ITexture*                       CreateRenderTargetTexture(int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat	format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;
    virtual ITexture*                       CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
    virtual ITexture*                       CreateNamedRenderTargetTexture(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false) = 0;
    virtual ITexture*                       CreateNamedRenderTargetTextureEx2(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
	virtual void				BeginLightmapAllocation() = 0;
	virtual void				EndLightmapAllocation() = 0;
	virtual int 				AllocateLightmap(int width, int height, int offsetIntoLightmapPage[2], IMaterial *pMaterial) = 0;
	virtual int					AllocateWhiteLightmap(IMaterial *pMaterial) = 0;
	virtual void				UpdateLightmap(int lightmapPageID, int lightmapSize[2], int offsetIntoLightmapPage[2], float *pFloatImage, float *pFloatImageBump1, float *pFloatImageBump2, float *pFloatImageBump3) = 0;
	virtual int					GetNumSortIDs() = 0;
	virtual void				GetSortInfo(MaterialSystem_SortInfo_t *sortInfoArray) = 0;
	virtual void				GetLightmapPageSize(int lightmap, int *width, int *height) const = 0;
	virtual void				ResetMaterialLightmapPageInfo() = 0;
	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;
	virtual IMatRenderContext *	GetRenderContext() = 0;
	virtual bool				SupportsShadowDepthTextures(void) = 0;
	virtual void				BeginUpdateLightmaps(void) = 0;
	virtual void				EndUpdateLightmaps(void) = 0;
	virtual MaterialLock_t		Lock() = 0;
	virtual void				Unlock(MaterialLock_t) = 0;
	virtual ImageFormat			GetShadowDepthTextureFormat() = 0;
	virtual bool				SupportsFetch4(void) = 0;
	virtual IMatRenderContext *CreateRenderContext(MaterialContextType_t type) = 0;
	virtual IMatRenderContext *SetRenderContext(IMatRenderContext *) = 0;
	virtual bool				SupportsCSAAMode(int nNumSamples, int nQualityLevel) = 0;
	virtual void				RemoveModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
	virtual IMaterial *			FindProceduralMaterial(const char *pMaterialName, const char *pTextureGroupName, KeyValues *pVMTKeyValues) = 0;
	virtual ImageFormat			GetNullTextureFormat() = 0;
	virtual void				AddTextureAlias(const char *pAlias, const char *pRealName) = 0;
	virtual void				RemoveTextureAlias(const char *pAlias) = 0;
	virtual int					AllocateDynamicLightmap(int lightmapSize[2], int *pOutOffsetIntoPage, int frameID) = 0;
	virtual void				SetExcludedTextures(const char *pScriptName) = 0;
	virtual void				UpdateExcludedTextures(void) = 0;
	virtual bool				IsInFrame() const = 0;
	virtual void				CompactMemory() = 0;
	virtual void ReloadFilesInList(IFileList *pFilesToReload) = 0;
	virtual	bool				AllowThreading(bool bAllow, int nServiceThread) = 0;
	virtual IMaterial *			FindMaterialEx(char const* pMaterialName, const char *pTextureGroupName, int nContext, bool complain = true, const char *pComplainPrefix = NULL) = 0;
	virtual void				SetRenderTargetFrameBufferSizeOverrides(int nWidth, int nHeight) = 0;
	virtual void				GetRenderTargetFrameBufferDimensions(int & nWidth, int & nHeight) = 0;
	virtual char *GetDisplayDeviceName() const = 0;
};
