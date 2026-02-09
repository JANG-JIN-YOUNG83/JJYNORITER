#ifndef __DATASTRUCTS_HEADER_FILE__
#define __DATASTRUCTS_HEADER_FILE__

typedef unsigned short  UINT16;
typedef unsigned int    UINT32;


struct  SBlobPartInfo{
	long  lArea;
	short nLeft;
	short nRight;
	short nTop;
	short nBottom;
};

struct SRun{ 
	short	nColStart;
	short	nColEnd;
	long	lLabel;
	long	lEqTableIdx;
};

//struct SSRun{ 
//	short	nColStart;
//	short	nColEnd;
//	long	lPermLabel;
//};

/*
struct SObjInfo{
	long  lArea;
	long  lEqLabel;
	long  lCenX;
	long  lCenY;

	short nLeft;
	short nRight;
	short nTop;
	short nBottom;
} SObjInfo;*/

#endif



