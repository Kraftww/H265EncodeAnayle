#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"

#define ALIGN_BYTE                 (64)
#define INPUTNAMEBYTEMAX           (128)
#define LINEFEEDNUM                (16)

#define PRINT_DEBUG                (0)

#define NALHEADOFFSET              (4)
#define NALHEADDATA                (0x000001)
static const unsigned char uc8LocalNalData[NALHEADOFFSET] = {0x0, 0x0, 0x0, 0x1};

static void COMMON_H265NalFrameTypePrint(const int s32FrameFlag, const unsigned long long u64Data, const unsigned int u32FrameNum)
{
	switch(s32FrameFlag)
	{
		case HEVC_NAL_TRAIL_N:
		case HEVC_NAL_TRAIL_R:
		case HEVC_NAL_TSA_N:
		case HEVC_NAL_TSA_R:
		case HEVC_NAL_STSA_N:
		case HEVC_NAL_STSA_R:
		case HEVC_NAL_RADL_N:
		case HEVC_NAL_RADL_R:
		case HEVC_NAL_RASL_N:
		case HEVC_NAL_RASL_R:
		{
			//printf("Current frame is P Slice\n");
			break;
		}
		
		case HEVC_NAL_BLA_W_LP:
		case HEVC_NAL_BLA_W_RADL:
		case HEVC_NAL_BLA_N_LP:
		case HEVC_NAL_IDR_W_RADL:
		case HEVC_NAL_IDR_N_LP:
		case HEVC_NAL_CRA_NUT:
		{
			printf("Current frame is I Slice[s32FrameFlag = %d][u32FrameSeq = %d], offset = %llu\n", s32FrameFlag, u32FrameNum, u64Data);
			break;
		}

		case HEVC_NAL_VPS:
		{
			printf("Current frame is VPS Slice\n");
			break;
		}

		case HEVC_NAL_SPS:
		{
			printf("Current frame is SPS Slice\n");
			break;
		}

		case HEVC_NAL_PPS:
		{
			printf("Current frame is PPS Slice\n");
			break;
		}

		case HEVC_NAL_SEI_PREFIX:
		case HEVC_NAL_SEI_SUFFIX:
		{
			printf("Current frame is SEI Slice\n");
			break;
		}

		default:
		{
			printf("Unidentified frame type = %d\n", s32FrameFlag);
			break;
		}
	}
}

//帧类型分析: FrameType/FrameNum/FrameSize/PPS/SPS/GOP
static int COMMON_H265HeadAnalyze(const unsigned char  *uc8SrcData, int s32Offset)
{
	//8Byte
	unsigned long long u64State64 = 0;
	unsigned int u32FrameNum      = 0;
	unsigned int u32FrameSize     = 0;

	unsigned int u32OldPost       = 0;
	unsigned int u32NewPost       = 0;

	printf("sizeof(NALHEADDATA) = %ld\n", sizeof(NALHEADDATA));

	for(int i = 0; i < s32Offset; i++)
	{
		//一个字节一个字节写入u64State64(8Byte)
		u64State64 = (u64State64 << 8) | uc8SrcData[i];
		
		if(NALHEADDATA != (u64State64 >> (3 * 8) & 0xffffff))
		{
			//找到起始码之后
			//注释致谢: 雷霄骅 
			/*
			* 此时state64内容如下：
			*               |      Start Code    | NALU Header |
			* |------|------|------|------|------|------|------|------|
			*
			*               |  buf |  buf |  buf |  buf |  buf | buf  |
			*               | [t-5]| [t-4]| [t-3]| [t-2]| [t-1]|  [t] |
			*
			* Start Code:
			* 0x000001
			*
			* NALU Header:
			* forbidden_zero_bit: 1bit。取值0。
			* nal_unit_type: 6 bit。NALU类型。
			* nuh_layer_id: 6 bit。目前取值为0（保留以后使用）.
			* nuh_temporal_id_plus1: 3 bit。减1后为NALU时域层标识号TemporalID。
			*
			*/
			//state64右移16bit之后，state64最低字节为起始码后面的1Byte。即为NALU Header的前一个字节
			//NALU Header的前一个字节中，第1bit为forbidden_zero_bit，取值为0；
			//2-7bit为nal_unit_type；第8bit为nuh_layer_id，取值为0。

			//在这里state64右移(16+1)bit，然后相与0x3F(00111111)
			continue;
		}
		else
		{
			//Get u32FrameSize
			if(u32OldPost != u32NewPost)
			{
				u32FrameSize = u32NewPost - u32OldPost;
				//printf("u32FrameSize = %d\n", u32FrameSize);
				u32OldPost = i;
			}

			u32NewPost = i;
			
			//Get u32FrameNum
			u32FrameNum++;

			//Get Nal Unit Flag
			int s32NalUnit = 0x0;
			s32NalUnit     = (u64State64 >> (2 * 8 + 1)) & 0x3F;

			COMMON_H265NalFrameTypePrint(s32NalUnit, u64State64, u32FrameNum);
		}
	}

	printf("All the u32FrameNum is %d\n", u32FrameNum);

	return 0;
}

static int COMMON_StringPrint(FILE *pFd, const int s32ByteSize)
{
	int s32Ret                   = 0;
	int i                        = 0;
	int s32MallocMemSize = _ALIGN(s32ByteSize, ALIGN_BYTE);
	unsigned char *pbuf      = NULL;

	//Step 1 : memory init
	pbuf = malloc(s32MallocMemSize);
	printf("malloc sample size = %d[0x%x]\n", \
		s32ByteSize, \
		s32MallocMemSize);

	//Step 2 : read data
	//匹配Nal heak strstr
	fread(pbuf, 1, s32MallocMemSize, pFd);
	
	//获取每帧长度
	//printf("Get pbuf = %s\n", pbuf);

	//Step 3 : print review
	//打印预览
	#if PRINT_DEBUG
	for(i = 0; i < s32MallocMemSize; i++)
	{
		if(0 == i % LINEFEEDNUM)
		{
			printf("[*]\n");
		}

		int c8Print = 0x0;
		memcpy(&c8Print, pbuf + i, 1);
		//printf("pbuf = %d\n", pbuf + i);
		printf("0x%2x ", c8Print);
	}
	#endif //PRINT_DEBUG

	COMMON_H265HeadAnalyze(pbuf, s32MallocMemSize);

	//Step 5 : free memory
	free(pbuf);
	pbuf = NULL;

	return s32Ret;
}

static void COMMON_UsagePrint()
{
	printf("Please put into : H265AnlyPrint */*.H265\n");
}

int main(int argc, char *argv[])
{
        //条件判断
        //getopt_long
        //printf("Hello World\n");

	FILE *pFd                               = NULL;
	int s32FileSize                         = 0;
	char c8LocalFileName[INPUTNAMEBYTEMAX]  = {0x0};

	if(NULL != argv \
		&& 2 <= argc)
	{
		if(NULL != &argv[1])
		{
			//printf("argv[0] = %s\n", argv[0]);
			printf("argv[1] = %s\n", argv[1]);
			//printf("argv[2] = %s\n", argv[2]);
		}

		if(128 > strlen(argv[1]))
		{
			memcpy(c8LocalFileName, argv[1], INPUTNAMEBYTEMAX);
			printf("c8LocalFileName = %s\n", c8LocalFileName);
			c8LocalFileName[strlen(c8LocalFileName)] = '\0';
		}
		else
		{
			printf("The input file of name is outsize the Max[%d]\n", INPUTNAMEBYTEMAX);
			return -1;
		}
	}
	else
	{
		COMMON_UsagePrint();
		printf("exit\n");
		return -1;
	}

	//判断文件是否存在
	if(0 != access(c8LocalFileName, F_OK))
	{
		printf("c8LocalFileName is no exist\n");
		return -1;
	}

	pFd = fopen(c8LocalFileName, "rb");
	if(NULL == pFd)
	{
		perror("fopen is error\n");
		return -1;
	}
	
	fseek(pFd, 0, SEEK_END);
	//存在越界的风险
	s32FileSize = ftell(pFd);
	printf("[%s]-[%d] s32FileSize = %d\n" ,__FUNCTION__, __LINE__, s32FileSize);
	fseek(pFd, 0, SEEK_SET);

	COMMON_StringPrint(pFd, s32FileSize);

	fclose(pFd);

	return 0;
}

