#pragma once

#include <memory>
#include <stdint.h>

#ifndef INSCHVS_IMAGE
typedef struct _INS_IMAGE
{
	unsigned char* data;
	unsigned int   width;
	unsigned int   height;
	unsigned int   stride;
	unsigned int   channels;
	unsigned int   depth;
}InsCHVS_Image;
#define INSCHVS_IMAGE
#endif // !INSCHVS_IMAGE


typedef enum _INS_IMAGE_TYPE {
    Albedo = 0,      
    Derivate,
    DerivateAbs ,
    Gradient1, 
    Gradient2,  
    Gradient3,   
    GradientMix ,
    HeightMap
}InsCHVS_FusionType;

typedef struct _INS_FUSIOIN_IMAGE
{
    InsCHVS_FusionType Type;
	InsCHVS_Image ImageInfo;
    long long Milliseconds;
}InsCHVS_FusionImage;



typedef struct _INS_FUSION_PARAM 
{
    int IfOutAlbedo = 1;            // enable calculate Albedo image
    int IfOutDerivate = 1;          // enable calculate Derived image
    int IfOutDerivateAbs = 1;       // enable calculate DerivateAbs image
    int IfOutGradient1 = 1;         // enable calculate NormalX image
    int IfOutGradient2 = 1;         // enable calculate NormalY image
    int IfOutGradient3 = 1;         // enable calculate NormalX image
    int IfOutGradientMix = 0;       // enable calculate NormalMix image
    int IfOutHeightMap = 0;         // enable calculate HeightMap image   
    //
    int SmoothKernalSize = 1;   // Control the smoothness of these two Derivate and DerivateAbs images
    //                       
    //伪3d高度图的迭代次数，控制图像的精细程度，设的越大，耗时越多，结果越抽象
    //The number of iterations for the height map to control the level of image refinement
    //The larger, the more time-consuming, and the more abstract it is
    int MaxIteration = 10;
    //
    int B_albedo = 0;                   // albedo's b(equivalent to a reference plane)
    double K_albedo = 1.0;              // albedo's k(equivalent to controlling contrast)
    int B_derivate = 0;                 // derivate's b, Suggestion 128
    double K_derivate = 1.0;            // derivate's k, Suggestion 128
    int B_derivateAbs = 0;              // derivateabs's b, Suggestion 0
    double K_derivateAbs = 1.0;         // derivateabs's k
    int B_gradient1 = 0;                // gradient1's b, Suggestion 128
    double K_gradient1 = 1.0;           // gradient1's k
    int B_gradient2 = 0;                // gradient2's b, Suggestion 128
    double K_gradient2 = 1.0;           // gradient2's k
    int B_gradient3 = 0;                // gradient3's b
    double K_gradient3 = 1.0;           // gradient3's k
    int B_gradientMix = 0;              // meaningless, do not participate in the calculation; mix three gradient images
    double K_gradientMix = 1.0;         // meaningless, do not participate in the calculation; mix three gradient images
    int B_heightMap = 0;                // heightmap's b
    double K_heightMap = 1.0;           // heightmap's k

    bool enableCUDA = false;            // Using NIVIDIA graphics card fusion
    int gpuID = 0;                      // Set the selected GPU ID

    /* 此部分参数，在CUDA模式下不参与运算 */
    /* This parameter does not participate in calculations in CUDA mode */
    // 采样率（0~1）
    // sampling rate (0~1)
    double SamplingRate = 1.0;
    // 是否启用快速算法，启用后无法使用分段行数、分段列数功能
    // enable the fast algorithm enabled. Once enabled, the segmented row and column functions cannot be used
    bool enableQuickMode = false;
    // 分段行数计算使能
    // enable segmented row count calculation
    bool enablePartRows = false;
    // 分段行数
    // number of segmented rows
    int partRows = 1000;
    // 分段列数计算使能
    // enable segmented column count calculation
    bool enablePartCols = false;
    // 分段列数
    // number of segmented columns
    int partCols = 1000;
    // 是否启用算法矫正
    // whether to enable algorithm correction
    int IfOpenIlluminationCorrector = 0;
}InsCHVS_PMSFusionParam;


#ifdef INS_CHVS_FUSION_DLL_EXPORT
#define INS_CHVS_FUSION_DLL_API  __declspec(dllexport)
#else
#define INS_CHVS_FUSION_DLL_API __declspec(dllimport)
#endif
