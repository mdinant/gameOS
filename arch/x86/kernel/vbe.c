#include <system.h>

#define VBE_FAR(x) 								unsigned short x[2]

// lekker bij elkaar
#define VBE_INFO_ADDR							0x2000
#define VBE_MODE_INFO_ADDR						0x3000
#define REALPTR									(unsigned int)
#define COMMAND_GET_SUPERVGA_INFORMATION    	0x4F00
#define COMMAND_GET_SUPERVGA_MODE_INFORMATION	0x4F01
#define COMMAND_SET_SUPERVGA_MODE				0x4F02

enum VESARet {
	FunctionSupported,
	FunctionNotSupported,
	FunctionCallSuccessful,
	FunctionCallFailed
};


typedef struct {
	unsigned char VESASignature[4];
	unsigned short VESAVersion;
	unsigned long OEMStringPtr;
	unsigned char Capabilities[4];
	unsigned long VideoModesPtr;
	unsigned short TotalMemory;
	unsigned short OemSoftwareRev;
	unsigned long OemVendorNamePtr;
	unsigned long OemProductNamePtr;
	unsigned long OemProductRevPtr;
	unsigned char Reserved[222];
	unsigned char OemData[256];
}__attribute__ ((packed)) vbeControllerInfo;

typedef struct {
	unsigned short ModeAttributes;
	unsigned char WinAAttributes;
	unsigned char WinBAttributes;
	unsigned short WinGranularity;
	unsigned short WinSize;
	unsigned short WinASegment;
	unsigned short WinBSegment;
	unsigned long WinFuncPtr;
	unsigned short BytesPerScanLine;
	unsigned short XResolution;
	unsigned short YResolution;
	unsigned char XCharSize;
	unsigned char YCharSize;
	unsigned char NumberOfPlanes;
	unsigned char BitsPerPixel;
	unsigned char NumberOfBanks;
	unsigned char MemoryModel;
	unsigned char BankSize;
	unsigned char NumberOfImagePages;
	unsigned char Reserved_page;
	unsigned char RedMaskSize;
	unsigned char RedMaskPos;
	unsigned char GreenMaskSize;
	unsigned char GreenMaskPos;
	unsigned char BlueMaskSize;
	unsigned char BlueMaskPos;
	unsigned char ReservedMaskSize;
	unsigned char ReservedMaskPos;
	unsigned char DirectColorModeInfo;
	unsigned long PhysBasePtr;
	unsigned long OffScreenMemOffset;
	unsigned short OffScreenMemSize;
	unsigned char Reserved[206];
}__attribute__ ((packed)) vbeModeInfo;


vbeControllerInfo * ctrl = (vbeControllerInfo *) VBE_INFO_ADDR;
vbeModeInfo * inf = (vbeModeInfo *) VBE_MODE_INFO_ADDR;

void showVesaInfo();
void printVbeControllerInfo(vbeControllerInfo * ctrl);
bool isModeInList(unsigned short modeNum, vbeControllerInfo * ctrl);
void printModesWithBBP(unsigned short bbp, vbeControllerInfo * ctrl);

enum VESARet VESAFunc(regs32_t * regs);
enum VESARet getSuperVGAInformation(vbeControllerInfo *ctrl);
enum VESARet getSuperVGAModeInformation(unsigned short modeNum);
enum VESARet setVESAMode(unsigned short modeNum, bool flat);
bool demoMode(unsigned short modeNum);

bool isModeInList(unsigned short modeNum, vbeControllerInfo * ctrl) {
	unsigned short * mode = (unsigned short*) segToFlatAddr(
			ctrl->VideoModesPtr);
	while (*mode != 0xFFFF) {
		if (*mode == modeNum) {
			return TRUE;
		}
		mode += 2;
	}
	return FALSE;
}

enum VESARet setVESAMode(unsigned short modeNum, bool flat) {
	regs32_t regs;
	regs.eax = COMMAND_SET_SUPERVGA_MODE;
	regs.ebx = modeNum;

	if (flat) {
		regs.ebx |= 1 << 14;
	}

	return VESAFunc(&regs);
}

enum VESARet VESAFunc(regs32_t * regs) {
	int32_beta(0x10, regs);

	if (regs->eax != 0x4F) {
		return FunctionNotSupported;
	}

	return FunctionCallSuccessful;
}

enum VESARet getSuperVGAInformation(vbeControllerInfo * ctrl) {
	regs32_t regs;

	memcpy(ctrl, "VBE2", 4);
	memset(&regs, 0, sizeof(regs32_t));

	regs.eax = COMMAND_GET_SUPERVGA_INFORMATION;
	regs.edi = VBE_INFO_ADDR;
	regs.es = 0;

	return VESAFunc(&regs);

}
enum VESARet getSuperVGAModeInformation(unsigned short modeNum) {
	regs32_t regs;
	memset(&regs, 0, sizeof(regs32_t));

	regs.eax = COMMAND_GET_SUPERVGA_MODE_INFORMATION;
	regs.ecx = modeNum;
	regs.edi = VBE_MODE_INFO_ADDR;
	regs.es = 0;

	return VESAFunc(&regs);

}
void printModesWithBBP(unsigned short bbp, vbeControllerInfo * ctrl) {
	printf("modes with bbp %u:\n", bbp);
	unsigned short * mode = (unsigned short*) segToFlatAddr(
			ctrl->VideoModesPtr);
	while (*mode != 0xFFFF) {
		char hex[8];
		hex_to_char(*mode, hex);
		enum VESARet ret = getSuperVGAModeInformation(*mode);

		switch (ret) {
		case FunctionNotSupported: {
			printf("Function Not Supported for mode: %s\n", hex);
			break;
		}
		case FunctionCallFailed: {
			printf("Function Call Failed for mode: %s\n", hex);
			break;
		}
		default: {
			// do something
			if (inf->BitsPerPixel == bbp) {
				hex_to_char(*mode, hex);
				printf("%4s  ", hex);

				if (CHECK_BIT(inf->ModeAttributes, 0) == 0) {
					printf("- NOT SUPPORTED IN HARWARE - ");
				}
				printf("%4u %4u  ", inf->XResolution, inf->YResolution);
				if (CHECK_BIT(inf->ModeAttributes, 7)) {
					printf("%3s ", "lfb");
				}
				if (CHECK_BIT(inf->ModeAttributes, 6)) {
					printf("%4s\t", "only");
				} else {
					printf("%3s\t", "win");
				}
				printf("BPS: %7u\t", inf->BytesPerScanLine);
				printf("NoIP: %2u\t", inf->NumberOfImagePages);
				//hex_to_char(inf->PhysBasePtr, hex);
				//printf("physPtr: 0x%8s", hex);
				hex_to_char(inf->OffScreenMemSize, hex);
				printf("OMSize: 0x%s", hex);
				//printf("OMSize: %u", inf->OffScreenMemSize);

				printf("\n");
			}
			break;
		}
		}
		mode += 2;
	}
}

void printVbeControllerInfo(vbeControllerInfo * ctrl) {
	char hex[8];
	printf("VESASignature: %s\n", ctrl->VESASignature);
	printf("VESAVersion: %u (major), %u (minor)\n", ctrl->VESAVersion >> 8,
			ctrl->VESAVersion & 0xFF);
	printf("OEMString: %s\n", (char*) segToFlatAddr(ctrl->OEMStringPtr));

	if (CHECK_BIT(ctrl->Capabilities[0], 7)) {

		printf("Capabilities: DAC width is switchable\n");
	} else {
		printf(
				"Capabilities: DAC is fixed width, with 6-bits per primary color\n");

	}
	//printf("capability: %u\n", ctrl->Capabilities[0]);

//			if (((ctrl->Capabilities[0] >> 0) & 0x01) == 1) {
//				printf(
//						"Capabilities: DAC is fixed width, with 6-bits per primary color\n");
//			} else
//				printf("Capabilities: DAC width is switchable\n");

	printf("Video Modes Available:\n");
	unsigned short * mode = (unsigned short*) segToFlatAddr(
			ctrl->VideoModesPtr);
	while (*mode != 0xFFFF) {
		hex_to_char(*mode, hex);
		printf(" | 0x%s", hex);
		mode += sizeof(uint16_t);
	}
	printf("\n");
	printf("TotalMemory (dec): %u\n", ((long) (ctrl->TotalMemory)) * 0xFFFF);

}

void showVesaInfo() {
	long long tlonglong;
	printf("sizeof long long: %u\n", sizeof(tlonglong));
	printf("GET_SUPERVGA_INFORMATION... ");
	anykey();
	enum VESARet ret = getSuperVGAInformation(ctrl);

	switch (ret) {
	case FunctionNotSupported: {
		printf("Function Not Supported\n");
		anykey();
		return;
	}
	case FunctionCallFailed: {
		printf("Function Call Failed\n");
		anykey();
		return;
	}
	default: {
		printf("FunctionCallSuccessful\n");
		anykey();
	}
	}

	printVbeControllerInfo(ctrl);
//    unsigned short modeNum = 0x178;
//    if (isModeInList(modeNum, ctrl)) {
//    	printf("ModeNum 0x178 is in list");
//    } else {
//    	printf("ModeNum 0x178 is NOT in list");
//    }
	printf("Press ENTER for list modes with bbp 15");
	anykey();
	printModesWithBBP(15, ctrl);
	printf("Press ENTER for list modes with bbp 16");
	anykey();
	printModesWithBBP(16, ctrl);
	printf("Press ENTER for list modes with bbp 24");
	anykey();
	printModesWithBBP(24, ctrl);
	printf("Press ENTER for list modes with bbp 32");
	anykey();
	printModesWithBBP(32, ctrl);

}

bool demoMode(unsigned short modeNum) {

	char hex[8];
	hex_to_char(modeNum, hex);

	enum VESARet ret;
	ret = getSuperVGAModeInformation(modeNum);
	switch (ret) {
	case FunctionNotSupported: {
		printf("getSuperVGAModeInformation Not Supported for mode: %s\n", hex);
		return FALSE;
	}
	case FunctionCallFailed: {
		printf("getSuperVGAModeInformation Failed for mode: %s\n", hex);
		return FALSE;
	}
	default:
		hex_to_char(inf->PhysBasePtr, hex);
		printf("press ENTER to set mode %4u %4u %2u lfb at %8s...",
				inf->XResolution, inf->YResolution, inf->BitsPerPixel, hex);
		anykey();
		break;
	}

	ret = setVESAMode(modeNum, TRUE);

	switch (ret) {
	case FunctionNotSupported: {
		printf("SetVESAMode Not Supported for mode: %s\n", hex);
		return FALSE;
	}
	case FunctionCallFailed: {
		printf("SetVESAMode Failed for mode: %s\n", hex);
		return FALSE;
	}
	default: {
		// now in video mode :)
		//	anykey();

		size_t bufSize = inf->BytesPerScanLine * inf->YResolution;

		unsigned char *bBuffer = (unsigned char *)(inf->PhysBasePtr + bufSize);

		unsigned char * ptr = (unsigned char *) inf->PhysBasePtr;

		// do a color

		unsigned char color[] = {255, 100,50,0};
		//unsigned char iColor = 200;
		int c;
		//size_t size = 1;

		for (c = 0; c < bufSize - 4; c+=4) {


			bBuffer[c] = color[0];
			bBuffer[c+1] = color[1];
			bBuffer[c+2] = color[2];
			bBuffer[c+3] = color[3];
		}


		//memset(bBuffer, 200, bufSize);
		//memcpy(ptr, bBuffer, bufSize);


		memcpy_SSE2(ptr, bBuffer, bufSize);

		//memset(ptr, 128, 2000);
		anykey();
	}
		break;

	}

	return TRUE;
}

bool init_vbe() {


	showVesaInfo();

	//demoMode(0x146);

	return TRUE;

}



