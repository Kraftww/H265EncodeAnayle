#ifndef COMMON_H265ANALUPRINT_INC
#define COMMON_H265ANALUPRINT_INC

#include <stdio.h>

#define _ALIGN(n, _align64) (((n) + (_align64 - 1)) & ~(_align64 - 1))

/**
 * Table 7-3: NAL unit type codes
 */
 
//目前作分析暂使用这些参数, 来自于ffmpeg hevc.h
typedef enum HEVCNALUnitType 
{
	HEVC_NAL_TRAIL_N	= 0,
	HEVC_NAL_TRAIL_R	= 1,
	HEVC_NAL_TSA_N		= 2,
	HEVC_NAL_TSA_R		= 3,
	HEVC_NAL_STSA_N 	= 4,
	HEVC_NAL_STSA_R 	= 5,
	HEVC_NAL_RADL_N 	= 6,
	HEVC_NAL_RADL_R 	= 7,
	HEVC_NAL_RASL_N 	= 8,
	HEVC_NAL_RASL_R 	= 9,
	HEVC_NAL_BLA_W_LP	= 16,
	HEVC_NAL_BLA_W_RADL = 17,
	HEVC_NAL_BLA_N_LP	= 18,
	HEVC_NAL_IDR_W_RADL = 19,
	HEVC_NAL_IDR_N_LP	= 20,
	HEVC_NAL_CRA_NUT	= 21,
	HEVC_NAL_VPS		= 32,
	HEVC_NAL_SPS		= 33,
	HEVC_NAL_PPS		= 34,
	HEVC_NAL_AUD		= 35,
	HEVC_NAL_EOS_NUT	= 36,
	HEVC_NAL_EOB_NUT	= 37,
	HEVC_NAL_FD_NUT 	= 38,
	HEVC_NAL_SEI_PREFIX = 39,
	HEVC_NAL_SEI_SUFFIX = 40,
}__HEVCNALUnitType_COMMON_E;
	
typedef	enum HEVCSliceType
{
	HEVC_SLICE_B = 0,
	HEVC_SLICE_P = 1,
	HEVC_SLICE_I = 2,
}__HEVCSliceType_COMMON_E;



#endif //COMMON_H265ANALUPRINT_INC



