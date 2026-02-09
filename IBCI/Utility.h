#pragma once

class UtilityImage
{
public:
	static bool CreateBrightDarkImage(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright, IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger = nullptr);
    static bool CreateBrightDarkImage_sapera(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright, IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger = nullptr);
    static bool CreateBrightDarkImage_MSA(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright,
        IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger = nullptr);
    //jjy
    static bool CreateGradientImage_insnex(const IPVM::Image_8u_C1& Topsource, const IPVM::Image_8u_C1& bottomource,
        IPVM::Image_8u_C1& o_imageGradient, IPVM::Image_8u_C1& o_imageABS, bool bExactly,
        IPVM::LoggerInterface* logger = nullptr);
   
    
    static void SaveImage(IPVM::Image_8u_C1, CString path);

};
