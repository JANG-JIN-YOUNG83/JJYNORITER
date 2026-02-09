#pragma once

#include "Gadget/SimpleWorkerThread.h"
#include "../Grab/GrabExecutor.h"

class CISICellAllDefect;
class CInspectionCell;

enum class COLOR : long
{
	TOP_SILINDER_UP = 0,
	TOP_SILINDER_DOWN = 1,
};

class ViewCellImageUpdater : public IPVM::SimpleWorkerThread
{
public:
	ViewCellImageUpdater() = delete;
	ViewCellImageUpdater(const ViewCellImageUpdater &rhs) = delete;
	ViewCellImageUpdater(MachineType machineType, IPVM::ImageView &imageViewBright, IPVM::ImageView &imageViewDark, IPVM::ImageView &imageViewBright2, IPVM::ImageView &imageViewDark2);
	virtual ~ViewCellImageUpdater();

	ViewCellImageUpdater &operator=(const ViewCellImageUpdater &rhs) = delete;

public:
    void Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark, long CellId, CISICellAllDefect& ISICellAllDefect);
	void Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark, long CellId);
	void Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark);
    void Push();
    void SetGrayMode(long nType);
    void SetInspMode(long nType, BOOL bInlineCheck, BOOL bBatch);
    void SetInlineUpdateMode(long nType);
    void SetInspectionCell(CInspectionCell *CellInfo);
    long m_nGrayType;
    long m_nModeInspection;
    BOOL m_bInlineCheck;
    BOOL m_bBatchCheck;
	long m_nInlineMode;
    CInspectionCell* m_pInspectionCell;
    BOOL m_bSurfaceInspCheck;
    void SetSurfaceInspCheck(BOOL bCheck);
    void SetImageRoiClear();

protected:
	virtual bool OnTrigger() override;

private:
	IPVM::ImageView& m_imageViewBright;
	IPVM::ImageView& m_imageViewDark;
	IPVM::ImageView& m_imageViewBright2;
	IPVM::ImageView& m_imageViewDark2;

	CCriticalSection m_cs;

	long m_cellId;
    CISICellAllDefect* m_pISICellAllDefect;

	IPVM::TimeCheck *m_timeCheck;
	IPVM::Image_8u_C1* m_srcImageBright;
	IPVM::Image_8u_C1 *m_srcImageDark;

	IPVM::Image_8u_C1 *m_resizedFullImage;
	IPVM::Image_8u_C1 *m_resizedHalfImage;

	BOOL GetHorLineEquations(const std::vector<std::vector<IPVM::Point_32f_C2>>& vecHorLinePoints, std::vector<IPVM::LineEq_32f>& vecHorLineEquations);

	void SurfaceInsp(); 
    //void SurfaceInsp(CInspectionCell CellInfo);

	MachineType m_machineType;

	void UpdateImageView(IPVM::Image_8u_C1& imageBright, IPVM::Image_8u_C1& imageDark, IPVM::ImageView &imageViewBright, IPVM::ImageView &imageViewDark);
	void UpdateImageView_Batch(IPVM::Image_8u_C1& imageBright, IPVM::Image_8u_C1& imageDark, IPVM::ImageView& imageViewBright, IPVM::ImageView& imageViewDark);
};

