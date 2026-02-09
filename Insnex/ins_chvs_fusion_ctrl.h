#pragma once
#include "ins_chvs_fusion_def.h"
#include "ins_chvs_ctrl.hpp"

class FusionMatArray;
class INS_CHVS_FUSION_DLL_API InsCHVS_StorageFusionImage
{
	friend class InsCHVSFusion;
private:
	std::shared_ptr<FusionMatArray> fusionMatArray = nullptr;

	InsCHVS_StorageFusionImage() = delete;
	InsCHVS_StorageFusionImage(std::shared_ptr<FusionMatArray> arrray);
public:
	~InsCHVS_StorageFusionImage();

	/// <summary>
	/// If false, it means that there is no fused image in the result
	/// </summary>
	/// <returns>Is there any result</returns>
	bool isEmpty();

	/// <summary>
	/// Get the fusion image of the specified type
	/// </summary>
	/// <param name="type">Fusion type</param>
	/// <returns>Information related to the image</returns>
	InsCHVS_FusionImage at(int type);
	InsCHVS_FusionImage at(const InsCHVS_FusionType type) { return at((int)type); }
	InsCHVS_FusionImage operator[](int type) { return at(type); }
};

class FusionMediator;
class INS_CHVS_FUSION_DLL_API InsCHVSFusion
{
protected:
	FusionMediator* mediator = nullptr;

	InsCHVSFusion();
	~InsCHVSFusion();

public:

	/// <summary>
	/// Set whether to enable or disable
	/// </summary>
	/// <param name="enable_status">Enable state</param>
	/// <returns>Return value</returns>
	INS_Result InsCHVS_Set_PMSFusion_Enable(IN InsCHVS_FuncEnable enable_status);
	INS_Result InsCHVS_Get_PMSFusion_Enable(OUT InsCHVS_FuncEnable* enable_status_ptr);

	/// <summary>
	/// Set the relevant parameters for fusion
	/// </summary>
	/// <param name="param">Related parameters of fusion</param>
	/// <returns>Return value</returns>
	INS_Result InsCHVS_Set_PMSFusion_Param(IN InsCHVS_PMSFusionParam* param);
	INS_Result InsCHVS_Get_PMSFusion_Param(OUT InsCHVS_PMSFusionParam* param);

	/// <summary>
	/// Pop out the fusion result
	/// (InsCHVS_StorageFusionImage)It is used to store the results of fusion, and users can directly operate the memory area pointed to by the image data
	/// After popping out, the SDK no longer stores results internally, and users must release them themselves
	/// This will transfer the right to life and management to the user
	///	Automatically released at the end of its lifecycle
	/// </summary>
	/// <returns></returns>
	InsCHVS_StorageFusionImage pop();

	/// <summary>
	/// Get the configuration of the local computer
	/// </summary>
	/// <param name="cpuName">Name of CPU</param>
	/// <param name="gpuName">Name of GPU List</param>
	/// <param name="gpuCount">The number of GPU lists</param>
	/// <returns>Return value</returns>
	static INS_Result InsCHVS_Get_Computer_Info(OUT char* cpuName, OUT char** gpuName, OUT int* gpuCount);


	/// <summary>
	/// Set the image size of the fusion image
	/// This function generally does not need to be called
	/// The fusion class will automatically handle width, height, and step size internally
	///	But if handling errors, you need to manually modify the size of the image before starting the run
	/// </summary>
	/// <param name="width">width</param>
	/// <param name="height">height</param>
	/// <param name="stride">The number of bytes occupied by a row of image data. Usually equal to width.</param>
	/// <returns>Return valu</returns>
	INS_Result InsCHVS_Set_PMSFusion_ImageSize(IN unsigned int width, IN unsigned int height, IN unsigned int stride);


	/// <summary>
	/// Set the order of fusion images. This parameter is not saved by the camera and needs to be set separately each time.
	/// </summary>
	/// <param name="led_mode">Select the light source mode that needs to be set</param>
	/// <param name="imageOrder">The order of input corresponding to the current mode should correspond to the number of strobe channels. The minimum quantity is 3.
	///		eg.InsCHVS_LED_TriggerMode::LED_SeparateTimedFlashingBrightness->{1,2,1};InsCHVS_LED_TriggerMode::LED_QuadrupleIndependentBrightness->{4,3,2,1}</param>
	/// <param name="size">Size of order pointer</param>
	/// <returns></returns>
	INS_Result InsCHVS_Set_PMSFusion_Order(IN const InsCHVS_LED_TriggerMode led_mode, IN const int* imageOrder, IN const int size);
	INS_Result InsCHVS_Get_PMSFusion_Order(IN const InsCHVS_LED_TriggerMode led_mode, OUT int* imageOrder, OUT int* size);

	/// <summary>
	/// Set the fusion parameters, included ths fusion parameters and the order of fusion images
	/// </summary>
	/// <param name="filePath">File path</param>
	/// <returns></returns>
	INS_Result InsCHVS_Save_PMSFusion_ParamFile(IN const char* filePath);
	INS_Result InsCHVS_Load_PMSFusion_ParamFile(IN const char* filePath);
};

class InsPMSLCamera_p;
class INS_CHVS_FUSION_DLL_API InsPMSLCamera : public InsCHVSCamera, public InsCHVSFusion
{
public:
    InsPMSLCamera();
	~InsPMSLCamera();

	INS_Result InsCHVS_Cmd_Start() override;
	INS_Result InsCHVS_Cmd_Stop() override;

	INS_Result InsCHVS_AcquireImage(InsCHVS_Buffer* originalBuffer);
	INS_Result InsCHVS_AcquireImage(InsCHVS_Buffer* originalBuffer, unsigned int nMsec);


	INS_Result InsCHVS_RegisterCallback(unsigned int uEventType, RunHookFnPtr fEventFunc, void* pContext) override;
	INS_Result InsCHVS_UnRegisterCallback(unsigned int uEventType) override;

	INS_Result InsCHVS_DestroyModel();
private:
	InsPMSLCamera_p* pPMSL = nullptr;
};
