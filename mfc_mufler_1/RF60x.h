#include <windows.h>
#include <stdio.h>
#include "FTD2XX.H"

//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Structures                                                           */
/************************************************************************/

typedef struct _RF60x_HELLO_ANSWER_
{
	BYTE	bDeviceType;
	BYTE	bDeviceModificaton;
	WORD	wDeviceSerial;
	WORD	wDeviceMaxDistance;
	WORD	wDeviceRange;
} RF60xHELLOANSWER, *LPRF60xHELLOANSWER;

#pragma pack(push, 1)
typedef struct _RF60x_STREAM_VALUE_
{
	WORD	wMeasure;
	BYTE	bStatus;
} RF60xSTREAMVALUE, *LPRF60xSTREAMVALUE;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _RF60x_UDP_VALUE_
{
	WORD	wMeasure;
	BYTE	bStatus;
} RF60xUDPVALUE, *LPRF60xUDPVALUE;

typedef struct _RF60x_UDP_PACKET_
{
	RF60xUDPVALUE	rf60xValArray[168];
	WORD	wDeviceSerial;
	WORD	wDeviceBaseDistance;
	WORD	wDeviceMeasureRange;
	BYTE	bPackCount;
	BYTE	bPacketControlSumm;
} RF60xUDPPACKET, *LPRF60xUDPPACKET;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _RF60xB_UDP_VALUE_
{
	WORD	wMeasure;
	WORD	wExposition;
	BYTE	bStatus;
} RF60xBUDPVALUE, *LPRF60xBUDPVALUE;

typedef struct _RF60xB_UDP_PACKET_
{
	RF60xBUDPVALUE	rf60xBValArray[100];
	WORD	wReserved;
	WORD	wReserved2;
	WORD	wDeviceSerial;
	WORD	wDeviceBaseDistance;
	WORD	wDeviceMeasureRange;
	BYTE	bPackCount;
	BYTE	bPacketControlSumm;
} RF60xBUDPPACKET, *LPRF60xBUDPPACKET;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _RF60xHS_MEASURE_BLOCK_
{
	DWORD	dwMeasure;
	BYTE	bStatus;
} RF60xHSMEASUREBLOCK, *LPRF60xHSMEASUREBLOCK;

typedef struct _RF60xHS_MEASURE_PACKET_
{
	RF60xHSMEASUREBLOCK	rfbValues[100];
	WORD	wBlocksCount;
	WORD	wDeviceSerial;
	WORD	wDeviceBaseDistance;
	WORD	wDeviceMeasureRange;
	BYTE	bPacketCount;
	BYTE	bPacketControlSumm;
	WORD	wServiceInfo;
	
} RF60xHSMEASUREPACKET, *LPRF60xHSMEASUREPACKET;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* Definitions                                                          */
/************************************************************************/

///////////////////////////////////////////////////////// Address / Length ////////////////
#define	RF60x_PARAMETER_POWER_STATE						((0x00) | ((0x01) << 8))
#define	RF60x_PARAMETER_ANALOG_OUT						((0x01) | ((0x01) << 8))
#define	RF60x_PARAMETER_SAMPLE_AND_SYNC					((0x02) | ((0x01) << 8))
#define	RF60x_PARAMETER_NETWORK_ADDRESS					((0x03) | ((0x01) << 8))
#define	RF60x_PARAMETER_BAUDRATE						((0x04) | ((0x01) << 8))
#define	RF60x_PARAMETER_LASER_BRIGHT					((0x05) | ((0x01) << 8))
#define	RF60x_PARAMETER_AVERAGE_COUNT					((0x06) | ((0x01) << 8))
#define	RF60x_PARAMETER_SAMPLING_PERIOD					((0x08) | ((0x02) << 8))
#define RF60x_PARAMETER_ACCUMULATION_TIME				((0x0A) | ((0x02) << 8))
#define	RF60x_PARAMETER_BEGIN_ANALOG_RANGE				((0x0C) | ((0x02) << 8))
#define	RF60x_PARAMETER_END_ANALOG_RANGE				((0x0E) | ((0x02) << 8))
#define	RF60x_PARAMETER_RESULT_DELAY_TIME				((0x10) | ((0x01) << 8))
#define	RF60x_PARAMETER_ZERO_POINT_VALUE				((0x17) | ((0x02) << 8))
#define RF60x_PARAMETER_CAN_SPEED						((0x20) | ((0x01) << 8))
#define RF60x_PARAMETER_CAN_STANDARD_ID					((0x22) | ((0x02) << 8))
#define RF60x_PARAMETER_CAN_EXTENDED_ID					((0x24) | ((0x04) << 8))
#define RF60x_PARAMETER_CAN_ID							((0x28) | ((0x01) << 8))

//////////////////////////////////////////////////////////////////////////
#define	RF60x_STREAM_STATUS_MSR							(0x01)
#define	RF60x_STREAM_STATUS_ERROR						(0x02)

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/************************************************************************/
/* RS-232 functions                                                     */
/************************************************************************/

BOOL	RF60x_OpenPort(			
			LPCSTR					lpPort_Name, 
			DWORD					dwSpeed, 
			HANDLE *				lpHandle 
			);

BOOL	RF60x_ClosePort( 
			HANDLE					hCOM 
			);

BOOL	RF60x_GetTimeouts( 
			HANDLE					hCOM,
			LPDWORD					lpdwReadIntervalTimeout,
			LPDWORD					lpdwReadTotalTimeoutMultiplier,
			LPDWORD					lpdwReadTotalTimeoutConstant,
			LPDWORD					lpdwWriteTotalTimeoutMultiplier, 
			LPDWORD					lpdwWriteTotalTimeoutConstant
			);

BOOL	RF60x_SetTimeouts( 
			HANDLE					hCOM,
			DWORD					dwReadIntervalTimeout,
			DWORD					dwReadTotalTimeoutMultiplier,
			DWORD					dwReadTotalTimeoutConstant,
			DWORD					dwWriteTotalTimeoutMultiplier, 
			DWORD					dwWriteTotalTimeoutConstant		
			);

BOOL	RF60x_HelloCmd(		
			HANDLE					hCOM, 
			BYTE					bAddress, 
			LPRF60xHELLOANSWER		lprfHelloAnswer 
			);

BOOL	RF60x_ReadParameter(		
			HANDLE					hCOM, 
			BYTE					bAddress, 
			WORD					wParameter,
			DWORD *					lpdwValue 
			);

BOOL	RF60x_WriteParameter(		
			HANDLE					hCOM, 
			BYTE					bAddress, 
			WORD					wParameter,
			DWORD					dwValue 
			);

BOOL	RF60x_ReadCustomParameter(		
			HANDLE					hCOM, 
			BYTE					bAddress, 
			BYTE					bParameterAddress,
			BYTE					bParameterSize,
			void *					lpValue 
			);

BOOL	RF60x_WriteCustomParameter(		
			HANDLE					hCOM, 
			BYTE					bAddress, 
			BYTE					bParameterAddress,
			BYTE					bParameterSize,
			void *					lpValue 
			);

BOOL	RF60x_FlushToFlash(		
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_RestoreFromFlash(		
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_LockResult(		
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_Measure(	
			HANDLE					hCOM, 
			BYTE					bAddress, 
			USHORT *				lpusValue 
			);

BOOL	RF60x_StartStream(	
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_StopStream(	
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_GetStreamMeasure(	
			HANDLE					hCOM, 
			USHORT *				lpusValue 
			);

BOOL	RF60x_GetStreamMeasures( 
			HANDLE					hCOM, 
			LPRF60xSTREAMVALUE		lpusStreamValues, 
			DWORD					dwMeasuresCount, 
			LPDWORD					lpdwReadedMeasuresCount 
			);

BOOL	RF60x_CustomCmd(	
			HANDLE					hCOM, 
			char *					pcInData, 
			DWORD					dwInSize, 
			char *					pcOutData, 
			DWORD *					pdwOutSize
			);

/************************************************************************/
/* FTDI (RS-485) functions                                              */
/************************************************************************/

BOOL	RF60x_FTDI_OpenPort(		
			LPCSTR					lpPort_Name, 
			DWORD					dwSpeed, 
			FT_HANDLE *				lpftHandle 
			);

BOOL	RF60x_FTDI_ClosePort( 
			FT_HANDLE				hftHandle 
			);

BOOL	RF60x_FTDI_HelloCmd(	
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			LPRF60xHELLOANSWER		lprfHelloAnswer 
			);

BOOL	RF60x_FTDI_ReadParameter(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			WORD					wParameter,
			DWORD *					lpdwValue 
			);

BOOL	RF60x_FTDI_WriteParameter(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			WORD					wParameter,
			DWORD					dwValue 
			);

BOOL	RF60x_FTDI_ReadCustomParameter(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			BYTE					bParameterAddress,
			BYTE					bParameterSize,
			void *					lpValue 
			);

BOOL	RF60x_FTDI_WriteCustomParameter(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			BYTE					bParameterAddress,
			BYTE					bParameterSize,
			void *					lpValue 
			);

BOOL	RF60x_FTDI_FlushToFlash(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_FTDI_RestoreFromFlash(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_FTDI_LockResult(		
			FT_HANDLE				hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_FTDI_Measure( 
			FT_HANDLE				hCOM, 
			BYTE					bAddress, 
			USHORT *				pdwData 
			);

BOOL	RF60x_FTDI_StartStream(	
			FT_HANDLE				hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_FTDI_StopStream(	
			HANDLE					hCOM, 
			BYTE					bAddress
			);

BOOL	RF60x_FTDI_GetStreamMeasure(	
			FT_HANDLE				hCOM, 
			USHORT *				lpusValue 
			);

BOOL	RF60x_FTDI_CustomCmd( 
			FT_HANDLE				hCOM, 
			char *					pcInData, 
			DWORD					dwInSize, 
			char *					pcOutData, 
			DWORD *					pdwOutSize
			);


/************************************************************************/
/* Ethernet functions		                                            */
/************************************************************************/

BOOL	RF60x_Ethernet_OpenPort(		
			HANDLE *				lphHandle 
			);

BOOL	RF60xHS_Ethernet_OpenPort(		
			HANDLE *				lphHandle 
			);

BOOL	RF60x_Ethernet_SetTimeout( 
			HANDLE					hHandle, 
			DWORD					dwTimeout 
			);

BOOL	RF60x_Ethernet_ClosePort( 
			HANDLE					hHandle 
			);

BOOL	RF603_Ethernet_GetStreamMeasure(
			HANDLE					hHandle,
			LPRF60xUDPPACKET		lprf60xUDPPacket
			);

BOOL	RF603B_Ethernet_GetStreamMeasure(
			HANDLE					hHandle,
			LPRF60xBUDPPACKET		lprf60xBUDPPacket
			);

BOOL	RF603HS_Ethernet_GetStreamMeasure(
			HANDLE					hHandle,
			LPRF60xHSMEASUREPACKET	lprf60xHSUDPPacket
			);


#ifdef __cplusplus
}
#endif      

//////////////////////////////////////////////////////////////////////////