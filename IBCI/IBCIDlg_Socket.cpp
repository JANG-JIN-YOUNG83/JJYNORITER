// 1. 미리 컴파일된 헤더
#include "framework.h"
#include "pch.h"
// 2. 주 헤더
#include "IBCI.h"
#include "IBCIDlg.h"
#include "IBCIDlg_InnerDefine.h"
#include "afxdialogex.h"
// 3. 보조 헤더
#include "AlarmCommSocket.h"
#include "../SharedModule/VisionHostShare/TCPMSG_I2V.h"
#include "../SharedModule/VisionHostShare/TCPMSG_V2I.h"
#include "../SharedModule/VisionHostShare/ParameterVision.h"
#include "../SharedModule/VisionHostShare/ParameterSystem.h"
#include "../SharedModule/VisionHostShare/V2I_VisionRecipe.h"
#include "../VisionModule/Base/ParaCollector.h"
#include "../Inspection/Accessor.h"
#include "Path.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

afx_msg LRESULT CIBCIDlg::OnReceiveSocket(WPARAM wParam, LPARAM lParam)
{
    const auto msgInfo = IPVM::MessageSocket::ParseMsg(wParam, lParam);

    const long messageIndex = msgInfo.m_messageIndex;
    const long messageLength = msgInfo.m_messageLength;
    const void* message = msgInfo.m_message;

    switch ((TCPMSG_I2V)messageIndex)
    {
        //case TCPMSG_I2V::LOT_INFO:
        //    ReceiveSocket_LotInfo(message, messageLength);
        //    break;
        case TCPMSG_I2V::SPEC_CURRENT_DATA:
            ReceiveSocket_SpecCurrentData(message, messageLength);
            break;
        case TCPMSG_I2V::LOT_CHANGE:
            ReceiveSocket_LotChange(message, messageLength);
            break;
        case TCPMSG_I2V::DATA_RESET:
            //ReceiveSocket_DataReset();
            break;
        //case TCPMSG_I2V::VISION_ALIVE:
        //    break; // 처리 내용 없음
        //case TCPMSG_I2V::NEW_RECEIPE:
        //    ReceiveSocket_NewRecipe(message, messageLength);
        //    break;
        case TCPMSG_I2V::RECEIPE_CHANGE:
            ReceiveSocket_RecipeChange(message, messageLength);
            break;
        //case TCPMSG_I2V::SPEC_NGMARK_INFO:
        //    ReceiveSocket_SpecNGMarkInfo(message, messageLength);
        //    break;
        //case TCPMSG_I2V::SPEC_CHANGE:
        //    ReceiveSocket_SpecChange(message, messageLength);
        //    break;
        case TCPMSG_I2V::ALARM_RESET:
            //  ReceiveSocket_AlarmReset(message, messageLength);
            break;
        //case TCPMSG_I2V::MARKING_ON_OFF:
        //    ReceiveSocket_MarkingOnOff(message, messageLength);
        //    break;
        case TCPMSG_I2V::BAR_TYPE_CAMERA_INIT:
            //  ReceiveSocket_BarTypeCameraInit(message, messageLength);
            break;
        //case TCPMSG_I2V::EXTRA_SPEC_DATA:
        //    ReceiveSocket_ExtraSpecData(message, messageLength);
        //    break;
        //case TCPMSG_I2V::TEST_GRAB_START:
        //    ReceiveSocket_TestGrabStart();
        //    break;
        case TCPMSG_I2V::VISION_RESET:
            ReceiveSocket_VisionReset();
            break;
        //case TCPMSG_I2V::VISION_ERROR_PROOFING:
        //    ReceiveSocket_VisionErrorProofing(message, messageLength);
        //    break;
        case TCPMSG_I2V::SYNCHRONIZE_TIME:
            //  ReceiveSocket_SynchronizeTime(message, messageLength);
            break;
        //case TCPMSG_I2V::TEST_SURFACE_START:
        //    ReceiveSocket_TestSimulSurface();
        //    break;
        //case TCPMSG_I2V::LOGIN_INIT:
        //    ReceiveSocket_LoginInit(message, messageLength);
        //    break;
        //case TCPMSG_I2V::LOGIN_ACCESS_SEND:
        //    ReceiveSocket_LogAccess(message, messageLength);
        //    break;
        //case TCPMSG_I2V::SEND_SHAREDIMAGEQUEUE_DATA:
        //    ReceiveSocket_ShareImageQueueSave(message, messageLength);
        //    break;
        //case TCPMSG_I2V::LOGIN_RESULT:
        //    ReceiveSocket_LogResult(message, messageLength);
        //    break;
        //case TCPMSG_I2V::REQEUST_PROGRAM_BACKUP:
        //    ReceiveSocket_ProgramBackup(message, messageLength);
        //    break;
        //case TCPMSG_I2V::SIMULATION_INSPECT:
        //    ReceiveSocket_SimulationInspect(message, messageLength);
        //    break;
        //case TCPMSG_I2V::REQEUST_VISION_INI:
        //    ReceiveSocket_VisionIni(message, messageLength);
        //    break;
        //case TCPMSG_I2V::SEND_SAMEIOCHECK:
        //    ReceiveSocket_SameIOCheck(message, messageLength);
        //    break;
        //case TCPMSG_I2V::REPLY_VISION_ALARM:
        //    ReceiveSocket_ReplyVisionAlarm(message, messageLength);
        //    break;
        //case TCPMSG_I2V::IO_MESSAGE_MAP:
        //    ReceiveSocket_IO(message, messageLength);
        //    break;
        case TCPMSG_I2V::REQEUST_JSONDATA:
            ReceiveSocket_RequestJSONData(message, messageLength);
            break;
        default:
            if (true)
            {
                m_loggerTcp.Log(0, _T("Unknown Message Error [%d]"), messageIndex);

                CString strMessageIndex;
                strMessageIndex.Format(_T("%d"), messageIndex);

                m_pClientSocket->SendAlarm_Log(600 /*임시*/, Share::ALARM_LEVEL::red, _T(""),
                    _T("Unknown Message ") + strMessageIndex, _T("Unknown Message Error Occured ") + strMessageIndex,
                    _T(""));
            }
            break;
    }
    return 0;
}

void CIBCIDlg::ReceiveSocket_SpecCurrentData(const void* message, const long messageLength)
{
    m_loggerTcp.Log(0, _T("SPEC CURRENT Recv"));

    CString strModel;
    strModel.Format(_T("%s"), (BYTE*)message);

    if (strModel != g_systemParameter.m_currentModel)
    {
        m_pClientSocket->SendAlarm_Log(
            900 /*임시*/, Share::ALARM_LEVEL::red, _T(""), _T("Recipe Name"), _T("Recipe name is not same"), _T(""));
        return;
    }

    SendSpec(0);
    ReceiveSocket_JsonData(); //현재 json param data보냄
}

void CIBCIDlg::ReceiveSocket_JsonData()
{
    Share::ParameterSystem paras;
    GetJsonSystemData(paras, g_systemParameter);
    m_pClientSocket->SendChangedParameterSystem(paras); //Send Json system_info
    SendVisionParameters(); // Send json para_info
}

void CIBCIDlg::SendVisionParameters()
{
    TCPMSG::V2I::VisionRecipe recipe;
    auto& dbJob = recipe.m_db;
    Share::ParameterVision paras;
    paras.m_modelName = g_systemParameter.m_currentModel;
    Base::FUNCTION_PARACOLLECTOR collect(&paras);
    //if (1 == g_systemParameter.nPC_ID)
    Inspection::Accessor::GetRoot().LinkDataBase(true, dbJob);
    //else if (2 == g_systemParameter.nPC_ID)
    //    m_paraSurface->LinkDataBase(true, dbJob[_T("Surface")]);
    m_pClientSocket->SendChangedParameterVision(paras);
}

void CIBCIDlg::GetJsonSystemData(Share::ParameterSystem& Params, SystemParameter& SysParam)
{
    //Params.Add(_T("EXIST GRAB HARDWARE"), g_systemParameter.m_isExistGrabHardware, SysParam.m_isExistGrabHardware);
    Params.Add(_T("CF PATH"), g_systemParameter.strCfPath, SysParam.strCfPath);
    Params.Add(_T("PC ID"), g_systemParameter.nPC_ID, SysParam.nPC_ID);
    Params.Add(_T("ANODE / CATHODE"), g_systemParameter.nAnodeCathode, SysParam.nAnodeCathode);
    Params.Add(_T("SENSOR IMAGE MIRROR"), g_systemParameter.bSensorImageMirror, SysParam.bSensorImageMirror);
    Params.Add(_T("ROLL MAP X AXIS MIRROR"), g_systemParameter.bXaxisMirror, SysParam.bXaxisMirror);
    Params.Add(_T("ROLL MAP Y AXIS MIRROR"), g_systemParameter.bYaxisMirror, SysParam.bYaxisMirror);

    Params.Add(_T("PIXEL TO UM X"), g_systemParameter.m_dPixelToUmXUp, SysParam.m_dPixelToUmXUp);
    Params.Add(_T("PIXEL TO UM X DOWN"), g_systemParameter.m_dPixelToUmXDown, SysParam.m_dPixelToUmXDown);
    Params.Add(_T("PIXEL TO UM Y"), g_systemParameter.m_dPixelToUmY, SysParam.m_dPixelToUmY);

    Params.Add(_T("LAST RECIPE"), g_systemParameter.m_currentModel, SysParam.m_currentModel);

    Params.Add(_T("LANGUAGE"), g_systemParameter.nLanguage, SysParam.nLanguage);
    Params.Add(_T("EXECUTOR TYPE"), (int)g_systemParameter.executorType, (int)SysParam.executorType);
    Params.Add(_T("MACHINE TYPE"), (int)g_systemParameter.machineType, (int)SysParam.machineType);

    Params.Add(_T("USE_ISI"), g_systemParameter.bUseISI, SysParam.bUseISI);
    Params.Add(_T("CUTTING PITCH"), g_systemParameter.fCuttingPitch, SysParam.fCuttingPitch);
    Params.Add(_T("CUTTING MARGIN"), g_systemParameter.nCuttingMargin, SysParam.nCuttingMargin);

    //Params.Add(_T("CORE COUNT"), g_systemParameter.m_coreCount, SysParam.m_coreCount);

    Params.Add(_T("IMAGE QUEUE SIZE"), g_systemParameter.m_imageQueueSize, SysParam.m_imageQueueSize);
    //Params.Add(_T("IMAGE SAVE"), g_systemParameter.m_imageSaveOption, SysParam.m_imageSaveOption);
    Params.Add(_T("IMAGE SAVE TYPE"), (int)g_systemParameter.m_imageSaveType, (int)SysParam.m_imageSaveType);
    Params.Add(_T("MACHINE NUMBER"), g_systemParameter.szMachineNumber, SysParam.szMachineNumber);

    Params.Add(_T("CONNECT IP"), g_systemParameter.szConnectIP, SysParam.szConnectIP);
    Params.Add(_T("CONNECT PORT"), g_systemParameter.nConnectPort, SysParam.nConnectPort);
    Params.Add(_T("CONNECT PORT2"), g_systemParameter.nConnectPort2, SysParam.nConnectPort2);

    Params.Add(
        _T("CONNECTION TAPE INTERLOCK"), g_systemParameter.bConnectionTapeInterlock, SysParam.bConnectionTapeInterlock);

    Params.Add(_T("USE SERVER"), g_systemParameter.bUseServer, SysParam.bUseServer);
    Params.Add(_T("SERVER"), g_systemParameter.bServer, SysParam.bServer);
    Params.Add(_T("HOST IP"), g_systemParameter.strHostIP, SysParam.strHostIP);
    Params.Add(_T("PORT"), g_systemParameter.nPort, SysParam.nPort);
    Params.Add(_T("CLIENT PC ID"), g_systemParameter.nClientPCID, SysParam.nClientPCID);

    Params.Add(_T("TOP/BACK"), g_systemParameter.bDirection, SysParam.bDirection);
    Params.Add(_T("MISMATCH ANIMATION DIRECTION"), g_systemParameter.bOSDS, SysParam.bOSDS);
    Params.Add(_T("ROLLER START DIRECTION"), g_systemParameter.bRollerOS, SysParam.bRollerOS);
    Params.Add(_T("COATING ONE SIDE/TWO SIDE"), g_systemParameter.bRollerTwoSide, SysParam.bRollerTwoSide);

    Params.Add(_T("LANE 1/2"), g_systemParameter.nLaneCount, SysParam.nLaneCount);

    Params.Add(_T("DEFECT CSV SAVE"), g_systemParameter.bDefectCsvSave, SysParam.bDefectCsvSave);
    Params.Add(_T("MSA CSV SAVE"), g_systemParameter.bMSASave, SysParam.bMSASave);

    Params.Add(_T("NOT PAIR"), g_systemParameter.bNotPair, SysParam.bNotPair);
    Params.Add(_T("PATH_RESULT_DATA_CSV"), g_systemParameter.pathResultDataCsv, SysParam.pathResultDataCsv);
    Params.Add(_T("WARNING COUNT"), g_systemParameter.nWarningCnt, SysParam.nWarningCnt);
    Params.Add(_T("SPEC COUNT"), g_systemParameter.nSpecCnt, SysParam.nSpecCnt);
    Params.Add(_T("EDGE VIEW IMAGE TYPE"), g_systemParameter.nEdgeViewImageType, SysParam.nEdgeViewImageType);
    Params.Add(_T("EDGE VIEW SAMPLING"), g_systemParameter.bEdgeViewSampling, SysParam.bEdgeViewSampling);
    Params.Add(_T("SAMPLING COUNT"), g_systemParameter.nSamplingCnt, SysParam.nSamplingCnt);
}

void CIBCIDlg::ReceiveSocket_VisionReset()
{
    m_loggerTcp.Log(0, _T("Vision Reset Recv"));

    if (m_bInline == TRUE)
    {
        OnInlineStop();

        Sleep(10);

        OnInlineStart();
    }
}

void CIBCIDlg::ReceiveSocket_RequestJSONData(const void* message, const long messageLength)
{
    m_loggerTcp.Log(0, _T("RequestJSONData Recv"));
    CString strModel;
    strModel.Format(_T("%s"), (BYTE*)message);
    m_bReceiveRequestData = TRUE;
}

void CIBCIDlg::ReceiveSocket_RecipeChange(const void* message, const long messageLength)
{
    m_loggerTcp.Log(0, _T("RECIPE CHANGE Recv"));

    CString strModel;
    strModel.Format(_T("%s"), (BYTE*)message);

    if (g_systemParameter.m_currentModel == strModel)
    {
        m_loggerTcp.Log(0, _T("Change Recipe And Current Recipe Equal"));
    }
    else
    {
        CFileFind find;

        CString strRecipePath;

        strRecipePath.Format(_T("%s\\%s.job"), Path::GetRecipeFolder(), strModel);

        if (!find.FindFile(strRecipePath))
        {
            m_pClientSocket->SendAlarm_Log(
                900 /*임시*/, Share::ALARM_LEVEL::red, _T(""), _T("Recipe Name"), _T("Recipe Not Exist"), _T(""));
        }
        else
        {
            RecipeLoad(strModel);
        }
    }
}

void CIBCIDlg::ReceiveSocket_LotChange(const void* message, const long messageLength)
{
    m_loggerTcp.Log(0, _T("LOT CHANGE Recv"));

    LotClear();
}