//#include "stdafx.h"
#include "pch.h"
#include "CalMap_Distortion_Impl.h"
//#include "ImageFactory.h"
//#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCalMap_Distortion_Impl::CCalMap_Distortion_Impl(void)
:m_bLoadMatrixFail(FALSE)
{
}


CCalMap_Distortion_Impl::~CCalMap_Distortion_Impl(void)
{
}

BOOL CCalMap_Distortion_Impl::IsLoadSuccess() const
{
	return !m_bLoadMatrixFail;
}

BOOL CCalMap_Distortion_Impl::Load(LPCTSTR szFilePath, long nPredictionSizeX, long nPredictionSizeY)
{
	// Calibration 임시 버퍼를 초기화
	m_mtxRefX.Free();
	m_mtxRefY.Free();

	// Matrix 파일 로드
	if(!LoadMatrixFile(szFilePath, m_mtxRef))
	{
		m_bLoadMatrixFail = TRUE;
		return FALSE;
	}

	// 로드 성공
	m_bLoadMatrixFail = FALSE;

	CreateImageMap(nPredictionSizeX, nPredictionSizeY);
	return TRUE;
}

BOOL CCalMap_Distortion_Impl::Save(LPCTSTR szFilePath) const
{
	try
	{
		CFile file(szFilePath, CFile::modeCreate|CFile::modeWrite);
		CArchive ar(&file, CArchive::store);

		//m_mtxRef.Column();
		//m_mtxRef.Row();
		//m_mtxRef.GetBuffer();
		//ar << m_mtxRef;
		ar << m_mtxRef.RowSize() << m_mtxRef.ColumnSize();

		double **pSrc = m_mtxRef.Buffer2D();

		for (long j = 0; j < m_mtxRef.RowSize(); j++)
            for (long i = 0; i < m_mtxRef.ColumnSize(); i++)
				ar << pSrc[j][i];
		
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL CCalMap_Distortion_Impl::SetTransform(long nPointNum, const IPVM::Point_32f_C2* i_pSrcPoint, const IPVM::Point_32f_C2* i_pDstPoint)
{
	for(long nPoint = 0 ; nPoint < nPointNum ; nPoint++)
	{
		const IPVM::Point_32f_C2 &ptInput = i_pSrcPoint[nPoint];
		const IPVM::Point_32f_C2 &ptOutput = i_pDstPoint[nPoint];

		const float dx = ptInput.m_x - ptOutput.m_x;
		const float m_y = ptInput.m_y - ptOutput.m_y;

		long nTemp = dx * dx + m_y * m_y;
		/*if(nTemp > 300)
		{
			return FALSE;
		}*/
	}

	// 이제 변환행렬 A 를 구해보자!!!
	// 일단은 4 차 기준으로 해 보자..

	// 1차 : 3
	// 1차(Bilinear) : 4
	// 2차 : 6
	// 3차 : 10
	// 4차 : 15

	const long nRowCount = 10;

	// A * IN = OUT

	// IN : 입력 포인트의 N 차 확장
	// OUT : 출력 포인트

	IPVM::Matrix matrixIN(nRowCount, nPointNum);
	IPVM::Matrix matrixOUT(2, nPointNum);

	double **ppIN = matrixIN.Buffer2D();
    double** ppOUT = matrixOUT.Buffer2D();

	for(long nPoint = 0 ; nPoint < nPointNum ; nPoint++)
	{
		const IPVM::Point_32f_C2 &ptInput = i_pSrcPoint[nPoint];
		const IPVM::Point_32f_C2 &ptOutput = i_pDstPoint[nPoint];

		long nRow = 0;

		//ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x * ptInput.m_x * ptInput.m_x;
		//ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x * ptInput.m_x * ptInput.m_y;
		//ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x * ptInput.m_y * ptInput.m_y;
		//ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_y * ptInput.m_y * ptInput.m_y;
		//ppIN[nRow++][nPoint] = ptInput.m_y * ptInput.m_y * ptInput.m_y * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x * ptInput.m_x;
		ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_y * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_y * ptInput.m_y * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_x;
		ppIN[nRow++][nPoint] = ptInput.m_x * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_y * ptInput.m_y;
		ppIN[nRow++][nPoint] = ptInput.m_x;
		ppIN[nRow++][nPoint] = ptInput.m_y;
		ppIN[nRow++][nPoint] = 1.f;

		ppOUT[0][nPoint] = ptOutput.m_x;
		ppOUT[1][nPoint] = ptOutput.m_y;
	}

	m_mtxRef = matrixOUT*~matrixIN*!(matrixIN*~matrixIN);	// ~ : Transpose
															// ! : Invert

	m_bLoadMatrixFail = FALSE;

	// 변환 행렬 검증...
	IPVM::Matrix matrixTempIN(nRowCount, 1);
	IPVM::Matrix matrixTempOUT;

	double** ppTempIN = matrixTempIN.Buffer2D();

	float fError = 0.f;

	for(long nPoint = 0 ; nPoint < nPointNum ; nPoint++)
	{
		const IPVM::Point_32f_C2 &ptInput = i_pSrcPoint[nPoint];

		long nRow = 0;

		//ppTempIN[nRow++][0] = ptInput.fx * ptInput.fx * ptInput.fx * ptInput.fx;
		//ppTempIN[nRow++][0] = ptInput.fx * ptInput.fx * ptInput.fx * ptInput.fy;
		//ppTempIN[nRow++][0] = ptInput.fx * ptInput.fx * ptInput.fy * ptInput.fy;
		//ppTempIN[nRow++][0] = ptInput.fx * ptInput.fy * ptInput.fy * ptInput.fy;
		//ppTempIN[nRow++][0] = ptInput.m_y * ptInput.m_y * ptInput.m_y * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_x * ptInput.m_x * ptInput.m_x;
		ppTempIN[nRow++][0] = ptInput.m_x * ptInput.m_x * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_x * ptInput.m_y * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_y * ptInput.m_y * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_x * ptInput.m_x;
		ppTempIN[nRow++][0] = ptInput.m_x * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_y * ptInput.m_y;
		ppTempIN[nRow++][0] = ptInput.m_x;
		ppTempIN[nRow++][0] = ptInput.m_y;
		ppTempIN[nRow++][0] = 1.f;

		matrixTempOUT = m_mtxRef * matrixTempIN;

		const float fNewX = matrixTempOUT[0][0];
		const float fNewY = matrixTempOUT[1][0];

		const IPVM::Point_32f_C2 &ptOutput = i_pDstPoint[nPoint];
		const float fdx = fNewX - ptOutput.m_x;
		const float fdy = fNewY - ptOutput.m_y;

		const float fd = sqrt(fdx * fdx + fdy * fdy);
	}

	return TRUE;
}

BOOL CCalMap_Distortion_Impl::Modify(const IPVM::Image_8u_C1 &i_Image, CPoint i_ptStart, IPVM::Image_8u_C1 &o_Image)
{
	i_ptStart.x = max(i_ptStart.x, 0);
	i_ptStart.y = max(i_ptStart.y, 0);
	CreateImageMap(i_ptStart.x + i_Image.GetSizeX(), i_ptStart.y + i_Image.GetSizeY());

#ifdef __USE_CUDA__
	return Modify_New(i_Image, i_ptStart, o_Image);
#else
	return Modify_Old(i_Image, i_ptStart, o_Image);
#endif
}
//
//BOOL CCalMap_Distortion_Impl::Modify(const IV_dataImage_8u_C3 &i_Image, CPoint i_ptStart, IV_dataImage_8u_C3 &o_Image)
//{
//	if(m_bLoadMatrixFail)
//	{
//		CImageFactory::CreateClone(&i_Image, &o_Image);
//		return FALSE;
//	}
//
//	const long nImageSizeX = i_Image.GetSizeX();
//	const long nImageSizeY = i_Image.GetSizeY();
//
//	CImageFactory::Create(nImageSizeX, nImageSizeY, &o_Image);
//
//	IPVM::Image_8u_C1 ImageCh_Inp[3];
//	IPVM::Image_8u_C1 ImageCh_Out[3];
//	for(long nCh=0; nCh<3; nCh++)
//	{
//		CImageFactory::Create(nImageSizeX, nImageSizeY, &ImageCh_Inp[nCh]);
//		CImageFactory::Create(nImageSizeX, nImageSizeY, &ImageCh_Out[nCh]);
//	}
//
//	IntekPlus::VisionMadang::ImageProcessing::SplitRGB(	i_Image,
//														(CRect)i_Image,
//														ImageCh_Inp[0],
//														ImageCh_Inp[1],
//														ImageCh_Inp[2]);
//
//	for(long nCh=0; nCh<3; nCh++)
//	{
//		Modify(ImageCh_Inp[nCh], i_ptStart, ImageCh_Out[nCh]);
//	}
//
//	IntekPlus::VisionMadang::ImageProcessing::CombineRGB(	ImageCh_Out[0],
//															ImageCh_Out[1],
//															ImageCh_Out[2],
//															(CRect)o_Image,
//															o_Image);
//
//	return TRUE;
//}

BOOL CCalMap_Distortion_Impl::Modify(const IPVM::Point_32f_C2& i_ptPoint, IPVM::Point_32f_C2& o_ptPoint)
{
	if(m_bLoadMatrixFail) return FALSE;

	// 이제 변환행렬 A 를 구해보자!!!
	// 일단은 4 차 기준으로 해 보자..

	// 1차 : 3
	// 1차(Bilinear) : 4
	// 2차 : 6
	// 3차 : 10
	// 4차 : 15

	const long nRowCount = 10;

	IPVM::Matrix matrixTempIN(nRowCount, 1);
	IPVM::Matrix matrixTempOUT;

	double** ppTempIN = matrixTempIN.Buffer2D();

	float fError = 0.f;
	long nRow = 0;

	//ppTempIN[nRow++][0] = i_ptPoint.fx * i_ptPoint.fx * i_ptPoint.fx * i_ptPoint.fx;
	//ppTempIN[nRow++][0] = i_ptPoint.fx * i_ptPoint.fx * i_ptPoint.fx * i_ptPoint.fy;
	//ppTempIN[nRow++][0] = i_ptPoint.fx * i_ptPoint.fx * i_ptPoint.fy * i_ptPoint.fy;
	//ppTempIN[nRow++][0] = i_ptPoint.fx * i_ptPoint.fy * i_ptPoint.fy * i_ptPoint.fy;
	//ppTempIN[nRow++][0] = i_ptPoint.fy * i_ptPoint.fy * i_ptPoint.fy * i_ptPoint.fy;
	ppTempIN[nRow++][0] = i_ptPoint.m_x * i_ptPoint.m_x * i_ptPoint.m_x;
	ppTempIN[nRow++][0] = i_ptPoint.m_x * i_ptPoint.m_x * i_ptPoint.m_y;
	ppTempIN[nRow++][0] = i_ptPoint.m_x * i_ptPoint.m_y * i_ptPoint.m_y;
	ppTempIN[nRow++][0] = i_ptPoint.m_y * i_ptPoint.m_y * i_ptPoint.m_y;
	ppTempIN[nRow++][0] = i_ptPoint.m_x * i_ptPoint.m_x;
	ppTempIN[nRow++][0] = i_ptPoint.m_x * i_ptPoint.m_y;
	ppTempIN[nRow++][0] = i_ptPoint.m_y * i_ptPoint.m_y;
	ppTempIN[nRow++][0] = i_ptPoint.m_x;
	ppTempIN[nRow++][0] = i_ptPoint.m_y;
	ppTempIN[nRow++][0] = 1.f;

	matrixTempOUT = m_mtxRef * matrixTempIN;

	o_ptPoint.m_x = matrixTempOUT[0][0];
	o_ptPoint.m_y = matrixTempOUT[1][0];

	return TRUE;
}

CCalMap_Distortion_Impl& CCalMap_Distortion_Impl::operator = (const CCalMap_Distortion_Impl& Object)
{
	m_mtxRef			= Object.m_mtxRef;
	m_bLoadMatrixFail	= Object.m_bLoadMatrixFail;

	//CImageFactory::CreateClone(&Object.m_mtxRefX, &m_mtxRefX);
	//CImageFactory::CreateClone(&Object.m_mtxRefY, &m_mtxRefY);

	//m_mtxRefX.SaveBmp(_T("D:\\temp.bmp"));
	//m_mtxRefY.LoadFrom(_T("D:\\temp.bmp"));
	//long SizeX = i_ImageGray.GetSizeX();
	//long SizeY = i_ImageGray.GetSizeY();
	////IPVM::Image_8u_C1 binImage(nImageSizeX, nImageSizeY);

	////.SaveBmp(_T("D:\\temp.bmp"));
	////o_ImageGray.LoadFrom(_T("D:\\temp.bmp"));

	//o_ImageGray.Create(SizeX,SizeY);
	return *this;
}

BOOL CCalMap_Distortion_Impl::LoadMatrixFile(LPCTSTR szPathName, IPVM::Matrix &matrix)
{
	try
	{
		CFile	file;
		if(!file.Open(szPathName, CFile::modeRead))
		{
			return FALSE;
		}

		CArchive ar(&file, CArchive::load);
			
		//ar >> matrix;

		long row, column;

		ar >> row >> column;
		matrix.Resize(row, column);

		double** pDst = matrix.Buffer2D();

		for(long j = 0 ; j < row ; j++)
			for(long i = 0 ; i < column ; i++)
				ar >> pDst[j][i];

		 
		ar.Close();
		file.Close();

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CCalMap_Distortion_Impl::Modify_Old(const IPVM::Image_8u_C1 &i_ImageGray, CPoint i_ptStart, IPVM::Image_8u_C1 &o_ImageGray)
{
	if(m_bLoadMatrixFail)
	{
		//CImageFactory::CreateClone(&i_ImageGray, &o_ImageGray);
		return FALSE;
	}
	long SizeX = i_ImageGray.GetSizeX();
	long SizeY = i_ImageGray.GetSizeY();
	//IPVM::Image_8u_C1 binImage(nImageSizeX, nImageSizeY);

	//.SaveBmp(_T("D:\\temp.bmp"));
	//o_ImageGray.LoadFrom(_T("D:\\temp.bmp"));

	o_ImageGray.Create(SizeX,SizeY);
	IPVM::ImageProcessing::Fill((IPVM::Rect)o_ImageGray, 0, o_ImageGray);

	IPVM::Image_32f_C1 &ImgMtxRefX = m_mtxRefX;
	IPVM::Image_32f_C1 &ImgMtxRefY = m_mtxRefY;

	const long nImageWidthBytes = o_ImageGray.GetWidthBytes();
	const long nImageX = o_ImageGray.GetSizeX();
	const long nImageY = o_ImageGray.GetSizeY();

	long nMtxStep = ImgMtxRefX.GetWidthBytes() / sizeof(float);

	BYTE*	pbyDstFirst	= o_ImageGray.GetMem();
	float*	pfX			= ImgMtxRefX.GetMem();
	float*	pfY			= ImgMtxRefY.GetMem();

	if(i_ptStart == CPoint(0,0))
	{
		for(long y = 0 ; y < nImageY ; ++y)
		{
			BYTE *pbyDst = pbyDstFirst + y * nImageWidthBytes;
			long nOff = y * nMtxStep;

			for(long x = 0 ; x < nImageX ; x++, nOff++)
			{
				pbyDst[x] = (BYTE)(i_ImageGray.GetSubpixValue(pfX[nOff], pfY[nOff])+0.5f);
			}
		}
	}
	else
	{
		for(long y = 0 ; y < nImageY ; ++y)
		{
			BYTE *pbyDst = o_ImageGray.GetMem(0, y);
			float* pfX = ImgMtxRefX.GetMem(i_ptStart.x, y + i_ptStart.y);
			float* pfY = ImgMtxRefY.GetMem(i_ptStart.x, y + i_ptStart.y);

			for(long x = 0 ; x < nImageX ; ++x)
			{
				float fX = pfX[x] - i_ptStart.x;
				float fY = pfY[x] - i_ptStart.y;

				pbyDst[x] = (BYTE)(i_ImageGray.GetSubpixValue(fX, fY)+0.5f);
			}
		}
	}

	//DebugLog::Add(_T("왜곡보상"), &o_ImageGray);

	return TRUE;
}

__forceinline BYTE getSubPixel_HYUNMIN(const BYTE* pBuffer, const long nStepX, const float fX, const float fY)
{
	// SubPixeling (linear)
	long x1 = (long)fX;
	long y1 = (long)fY;

	float h1 = static_cast<float>(fY - y1);
	float w1 = static_cast<float>(fX - x1);

	const BYTE*	pPtrSrc = pBuffer + y1 * nStepX + x1;

	return (BYTE)((1.f-w1)*(1.f-h1)*pPtrSrc[0] + w1*(1.f-h1)*pPtrSrc[1] + w1*h1*pPtrSrc[nStepX + 1] + (1.f-w1)*h1*pPtrSrc[nStepX] + 0.5f);
}

BOOL CCalMap_Distortion_Impl::Modify_New(const IPVM::Image_8u_C1 &i_ImageGray, CPoint i_ptStart, IPVM::Image_8u_C1 &o_ImageGray)
{
	if(m_bLoadMatrixFail)
	{
		//CImageFactory::CreateClone(&i_ImageGray, &o_ImageGray);
		return FALSE;
	}

	IPVM::Image_32f_C1 &ImgMtxRefX = m_mtxRefX;
	IPVM::Image_32f_C1 &ImgMtxRefY = m_mtxRefY;

	if(i_ImageGray.GetSizeX() > ImgMtxRefX.GetSizeX() || i_ImageGray.GetSizeY() > ImgMtxRefX.GetSizeY())
	{
		//CImageFactory::CreateClone(&i_ImageGray, &o_ImageGray);
		return FALSE;
	}
	//i_ImageGray.SaveBmp(_T("D:\\temp.bmp"));
	//o_ImageGray.LoadFrom(_T("D:\\temp.bmp"));

	long SizeX = i_ImageGray.GetSizeX();
	long SizeY = i_ImageGray.GetSizeY();
	//IPVM::Image_8u_C1 binImage(nImageSizeX, nImageSizeY);

	//.SaveBmp(_T("D:\\temp.bmp"));
	//o_ImageGray.LoadFrom(_T("D:\\temp.bmp"));

	o_ImageGray.Create(SizeX,SizeY);
//	CImageFactory::Create(i_ImageGray.GetSizeX(), i_ImageGray.GetSizeY(), &o_ImageGray);

	const long nImageWidthBytes = o_ImageGray.GetWidthBytes();
	const long nImageX = o_ImageGray.GetSizeX();
	const long nImageY = o_ImageGray.GetSizeY();

	long nMtxStep = ImgMtxRefX.GetWidthBytes() / sizeof(float);

	BYTE*	pbyDstFirst	= o_ImageGray.GetMem();
	float*	pfX			= ImgMtxRefX.GetMem();
	float*	pfY			= ImgMtxRefY.GetMem();

	const BYTE* pbySrc = i_ImageGray.GetMem();
	const long nSrcWidthBytes = i_ImageGray.GetWidthBytes();

	const float fLastX = (float)nImageX - 1.f;
	const float fLastY = (float)nImageY - 1.f;

	if(i_ptStart == CPoint(0,0))
	{
		#pragma omp parallel for schedule(guided, 50)
		for(long y = 0 ; y < nImageY ; ++y)
		{
			BYTE *pbyDst = pbyDstFirst + y * nImageWidthBytes;
			float *pfXTemp = pfX + y * nMtxStep;
			float *pfYTemp = pfY + y * nMtxStep;

			for(long x = 0 ; x < nImageX ; x++)
			{
				if(pfXTemp[x] >= 0.f && pfXTemp[x] < fLastX && pfYTemp[x] >= 0.f && pfYTemp[x] < fLastY)
				{
					pbyDst[x] = getSubPixel_HYUNMIN(pbySrc, nSrcWidthBytes, pfXTemp[x], pfYTemp[x]);
				}
				else
				{
					pbyDst[x] = 0;
				}
			}
		}
	}
	else
	{
		const float fLastSrcX = (float)i_ImageGray.GetSizeX() - 1.f;
		const float fLastSrcY = (float)i_ImageGray.GetSizeY() - 1.f;

		#pragma omp parallel for schedule(guided, 50)
		for(long y = 0 ; y < nImageY ; ++y)
		{
			BYTE *pbyDst = o_ImageGray.GetMem(0, y);
			float* pfX = ImgMtxRefX.GetMem(i_ptStart.x, y + i_ptStart.y);
			float* pfY = ImgMtxRefY.GetMem(i_ptStart.x, y + i_ptStart.y);

			for(long x = 0 ; x < nImageX ; ++x)
			{
				float fX = pfX[x] - i_ptStart.x;
				float fY = pfY[x] - i_ptStart.y;

				if(fX >= 0.f && fY >= 0.f && fX < fLastSrcX && fY < fLastSrcY)
				{
					pbyDst[x] = getSubPixel_HYUNMIN(pbySrc, nSrcWidthBytes, fX, fY);
				}
				else
				{
					pbyDst[x] = 0;
				}
			}
		}
	}

//	DebugLog::Add(_T("왜곡보상"), &o_ImageGray);

	return TRUE;
}

void CCalMap_Distortion_Impl::CreateImageMap(long nSizeX, long nSizeY)
{
	//----------------------------------------------------------------------------------------------------------
	// 왜곡 이미지가 새로 만들어 지므로 변경도중 다른 Thread에서 해당 함수 호출시 문제가 발생할 수 있다.
	// 그런 이유로 왜곡 이미지 데이터를 만들기 위한 부분에 SignleLock을 걸어놨다.
	//
	// 동일한 이미지 Size로 해당 함수를 여러 Thread로 호출하면
	// 첫 수행시 해당 함수에서 다른 Thread가 Lock이 걸리므로 Modify함수 호출도중
	// 왜곡 이미지의 주소가 바뀌는 일은 없어서 메모리 문제가 없다
	//
	// 허나 사실 아래의 내용은 무결한 코드는 아니다.
	// CreateImageMap을 여러 Thread에서 호출하는데 각각 요구하는 이미지 SizeX, SizeY가
	// 달라지면 Modify함수 호출도중 Image의 주소가 바뀌어서 문제가 발생할 수 있다.
	//
	// 지금까지 그런 경우는 없었고 그렇게 사용할 일도 없겠지만 주의해서 사용해야 한다. (2015/09/02)
	//----------------------------------------------------------------------------------------------------------
	CSingleLock lock(&m_csDataLock, TRUE);

	if(m_bLoadMatrixFail) return;

	long nMapSizeX = max(m_mtxRefX.GetSizeX(), nSizeX);
	long nMapSizeY = max(m_mtxRefX.GetSizeY(), nSizeY);

	if(	nMapSizeX == m_mtxRefX.GetSizeX() &&
		nMapSizeY == m_mtxRefX.GetSizeY())
	{
		// 다시 만들 필요없음
		return;
	}

	//CImageFactory::Create(nMapSizeX, nMapSizeY, &m_mtxRefX);
	//CImageFactory::Create(nMapSizeX, nMapSizeY, &m_mtxRefY);
	m_mtxRefX.Create(nMapSizeX, nMapSizeY);
	m_mtxRefY.Create(nMapSizeX, nMapSizeY);

	long nImageWidthBytes = m_mtxRefX.GetWidthBytes() / sizeof(float);
	
	double** ppMatrix = m_mtxRef.Buffer2D();

	const double *pRow0 = ppMatrix[0];
	const double *pRow1 = ppMatrix[1];

//#pragma omp parallel for schedule(guided, 50)
	for(long y = 0 ; y < nMapSizeY ; ++y)
	{
		float* pfNewX = m_mtxRefX.GetMem(0, y);
		float* pfNewY = m_mtxRefY.GetMem(0, y);

		for(long x = 0 ; x < nMapSizeX ; ++x)
		{
			float fRealX	= pRow0[0] * x * x * x
							+ pRow0[1] * x * x * y
							+ pRow0[2] * x * y * y
							+ pRow0[3] * y * y * y
							+ pRow0[4] * x * x
							+ pRow0[5] * x * y
							+ pRow0[6] * y * y
							+ pRow0[7] * x
							+ pRow0[8] * y
							+ pRow0[9];

			float fRealY	= pRow1[0] * x * x * x
							+ pRow1[1] * x * x * y
							+ pRow1[2] * x * y * y
							+ pRow1[3] * y * y * y
							+ pRow1[4] * x * x
							+ pRow1[5] * x * y
							+ pRow1[6] * y * y
							+ pRow1[7] * x
							+ pRow1[8] * y
							+ pRow1[9];

			pfNewX[x] = fRealX;
			pfNewY[x] = fRealY;
		}
	}
}
