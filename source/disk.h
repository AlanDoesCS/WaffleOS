//
// Created by Alan on 12/07/2024.
//

#ifndef DISK_H
#define DISK_H

#include "types.h"

#define SECTOR_SIZE 512

// From: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data?redirectedfrom=MSDN

typedef struct {
    uint16_t Reserved1 : 1;
    uint16_t Retired3 : 1;
    uint16_t ResponseIncomplete : 1;
    uint16_t Retired2 : 3;
    uint16_t FixedDevice : 1;
    uint16_t RemovableMedia : 1;
    uint16_t Retired1 : 7;
    uint16_t DeviceType : 1;
} GeneralConfigurationBits;

typedef struct {
    uint16_t FeatureSupported : 1;
    uint16_t Reserved : 15;
} TrustedComputingBits;

typedef struct {
    uint8_t  CurrentLongPhysicalSectorAlignment : 2;
    uint8_t  ReservedByte49 : 6;
    uint8_t  DmaSupported : 1;
    uint8_t  LbaSupported : 1;
    uint8_t  IordyDisable : 1;
    uint8_t  IordySupported : 1;
    uint8_t  Reserved1 : 1;
    uint8_t  StandybyTimerSupport : 1;
    uint8_t  Reserved2 : 2;
    uint16_t ReservedWord50;
} CapabilitiesBits;

typedef struct {
    uint16_t ZonedCapabilities : 2;
    uint16_t NonVolatileWriteCache : 1;
    uint16_t ExtendedUserAddressableSectorsSupported : 1;
    uint16_t DeviceEncryptsAllUserData : 1;
    uint16_t ReadZeroAfterTrimSupported : 1;
    uint16_t Optional28BitCommandsSupported : 1;
    uint16_t IEEE1667 : 1;
    uint16_t DownloadMicrocodeDmaSupported : 1;
    uint16_t SetMaxSetPasswordUnlockDmaSupported : 1;
    uint16_t WriteBufferDmaSupported : 1;
    uint16_t ReadBufferDmaSupported : 1;
    uint16_t DeviceConfigIdentifySetDmaSupported : 1;
    uint16_t LPSAERCSupported : 1;
    uint16_t DeterministicReadAfterTrimSupported : 1;
    uint16_t CFastSpecSupported : 1;
} AdditionalSupportedBits;

typedef struct {
    uint16_t Reserved0 : 1;
    uint16_t SataGen1 : 1;
    uint16_t SataGen2 : 1;
    uint16_t SataGen3 : 1;
    uint16_t Reserved1 : 4;
    uint16_t NCQ : 1;
    uint16_t HIPM : 1;
    uint16_t PhyEvents : 1;
    uint16_t NcqUnload : 1;
    uint16_t NcqPriority : 1;
    uint16_t HostAutoPS : 1;
    uint16_t DeviceAutoPS : 1;
    uint16_t ReadLogDMA : 1;
    uint16_t Reserved2 : 1;
    uint16_t CurrentSpeed : 3;
    uint16_t NcqStreaming : 1;
    uint16_t NcqQueueMgmt : 1;
    uint16_t NcqReceiveSend : 1;
    uint16_t DEVSLPtoReducedPwrState : 1;
    uint16_t Reserved3 : 8;
} SerialAtaCapabilitiesBits;

typedef struct {
    uint16_t Reserved0 : 1;
    uint16_t NonZeroOffsets : 1;
    uint16_t DmaSetupAutoActivate : 1;
    uint16_t DIPM : 1;
    uint16_t InOrderData : 1;
    uint16_t HardwareFeatureControl : 1;
    uint16_t SoftwareSettingsPreservation : 1;
    uint16_t NCQAutosense : 1;
    uint16_t DEVSLP : 1;
    uint16_t HybridInformation : 1;
    uint16_t Reserved1 : 6;
} SerialAtaFeaturesSupportedBits;

typedef struct {
    uint16_t Reserved0 : 1;
    uint16_t NonZeroOffsets : 1;
    uint16_t DmaSetupAutoActivate : 1;
    uint16_t DIPM : 1;
    uint16_t InOrderData : 1;
    uint16_t HardwareFeatureControl : 1;
    uint16_t SoftwareSettingsPreservation : 1;
    uint16_t DeviceAutoPS : 1;
    uint16_t DEVSLP : 1;
    uint16_t HybridInformation : 1;
    uint16_t Reserved1 : 6;
} SerialAtaFeaturesEnabledBits;

typedef struct {
    uint16_t SmartCommands : 1;
    uint16_t SecurityMode : 1;
    uint16_t RemovableMediaFeature : 1;
    uint16_t PowerManagement : 1;
    uint16_t Reserved1 : 1;
    uint16_t WriteCache : 1;
    uint16_t LookAhead : 1;
    uint16_t ReleaseInterrupt : 1;
    uint16_t ServiceInterrupt : 1;
    uint16_t DeviceReset : 1;
    uint16_t HostProtectedArea : 1;
    uint16_t Obsolete1 : 1;
    uint16_t WriteBuffer : 1;
    uint16_t ReadBuffer : 1;
    uint16_t Nop : 1;
    uint16_t Obsolete2 : 1;
    uint16_t DownloadMicrocode : 1;
    uint16_t DmaQueued : 1;
    uint16_t Cfa : 1;
    uint16_t AdvancedPm : 1;
    uint16_t Msn : 1;
    uint16_t PowerUpInStandby : 1;
    uint16_t ManualPowerUp : 1;
    uint16_t Reserved2 : 1;
    uint16_t SetMax : 1;
    uint16_t Acoustics : 1;
    uint16_t BigLba : 1;
    uint16_t DeviceConfigOverlay : 1;
    uint16_t FlushCache : 1;
    uint16_t FlushCacheExt : 1;
    uint16_t WordValid83 : 2;
    uint16_t SmartErrorLog : 1;
    uint16_t SmartSelfTest : 1;
    uint16_t MediaSerialNumber : 1;
    uint16_t MediaCardPassThrough : 1;
    uint16_t StreamingFeature : 1;
    uint16_t GpLogging : 1;
    uint16_t WriteFua : 1;
    uint16_t WriteQueuedFua : 1;
    uint16_t WWN64Bit : 1;
    uint16_t URGReadStream : 1;
    uint16_t URGWriteStream : 1;
    uint16_t ReservedForTechReport : 2;
    uint16_t IdleWithUnloadFeature : 1;
    uint16_t WordValid : 2;
} CommandSetSupportBits;

typedef struct {
    uint16_t SmartCommands : 1;
    uint16_t SecurityMode : 1;
    uint16_t RemovableMediaFeature : 1;
    uint16_t PowerManagement : 1;
    uint16_t Reserved1 : 1;
    uint16_t WriteCache : 1;
    uint16_t LookAhead : 1;
    uint16_t ReleaseInterrupt : 1;
    uint16_t ServiceInterrupt : 1;
    uint16_t DeviceReset : 1;
    uint16_t HostProtectedArea : 1;
    uint16_t Obsolete1 : 1;
    uint16_t WriteBuffer : 1;
    uint16_t ReadBuffer : 1;
    uint16_t Nop : 1;
    uint16_t Obsolete2 : 1;
    uint16_t DownloadMicrocode : 1;
    uint16_t DmaQueued : 1;
    uint16_t Cfa : 1;
    uint16_t AdvancedPm : 1;
    uint16_t Msn : 1;
    uint16_t PowerUpInStandby : 1;
    uint16_t ManualPowerUp : 1;
    uint16_t Reserved2 : 1;
    uint16_t SetMax : 1;
    uint16_t Acoustics : 1;
    uint16_t BigLba : 1;
    uint16_t DeviceConfigOverlay : 1;
    uint16_t FlushCache : 1;
    uint16_t FlushCacheExt : 1;
    uint16_t Resrved3 : 1;
    uint16_t Words119_120Valid : 1;
    uint16_t SmartErrorLog : 1;
    uint16_t SmartSelfTest : 1;
    uint16_t MediaSerialNumber : 1;
    uint16_t MediaCardPassThrough : 1;
    uint16_t StreamingFeature : 1;
    uint16_t GpLogging : 1;
    uint16_t WriteFua : 1;
    uint16_t WriteQueuedFua : 1;
    uint16_t WWN64Bit : 1;
    uint16_t URGReadStream : 1;
    uint16_t URGWriteStream : 1;
    uint16_t ReservedForTechReport : 2;
    uint16_t IdleWithUnloadFeature : 1;
    uint16_t Reserved4 : 2;
} CommandSetActiveBits;

typedef struct { // ------------------------------------------------
    uint16_t TimeRequired : 15;
    uint16_t ExtendedTimeReported : 1;
} NormalSecurityEraseUnitBits;

typedef struct {
    uint16_t TimeRequired : 15;
    uint16_t ExtendedTimeReported : 1;
} EnhancedSecurityEraseUnitBits;

typedef struct {
    uint16_t LogicalSectorsPerPhysicalSector : 4;
    uint16_t Reserved0 : 8;
    uint16_t LogicalSectorLongerThan256Words : 1;
    uint16_t MultipleLogicalSectorsPerPhysicalSector : 1;
    uint16_t Reserved1 : 2;
} PhysicalLogicalSectorSizeBits;

typedef struct {
    uint16_t ReservedForDrqTechnicalReport : 1;
    uint16_t WriteReadVerify : 1;
    uint16_t WriteUncorrectableExt : 1;
    uint16_t ReadWriteLogDmaExt : 1;
    uint16_t DownloadMicrocodeMode3 : 1;
    uint16_t FreefallControl : 1;
    uint16_t SenseDataReporting : 1;
    uint16_t ExtendedPowerConditions : 1;
    uint16_t Reserved0 : 6;
    uint16_t WordValid : 2;
} CommandSetSupportExtBits;

typedef struct {
    uint16_t ReservedForDrqTechnicalReport : 1;
    uint16_t WriteReadVerify : 1;
    uint16_t WriteUncorrectableExt : 1;
    uint16_t ReadWriteLogDmaExt : 1;
    uint16_t DownloadMicrocodeMode3 : 1;
    uint16_t FreefallControl : 1;
    uint16_t SenseDataReporting : 1;
    uint16_t ExtendedPowerConditions : 1;
    uint16_t Reserved0 : 6;
    uint16_t Reserved1 : 2;
} CommandSetActiveExtBits;

typedef struct {
    uint16_t SecuritySupported : 1;
    uint16_t SecurityEnabled : 1;
    uint16_t SecurityLocked : 1;
    uint16_t SecurityFrozen : 1;
    uint16_t SecurityCountExpired : 1;
    uint16_t EnhancedSecurityEraseSupported : 1;
    uint16_t Reserved0 : 2;
    uint16_t SecurityLevel : 1;
    uint16_t Reserved1 : 7;
} SecurityStatusBits;

typedef struct {
    uint16_t MaximumCurrentInMA : 12;
    uint16_t CfaPowerMode1Disabled : 1;
    uint16_t CfaPowerMode1Required : 1;
    uint16_t Reserved0 : 1;
    uint16_t Word160Supported : 1;
} CfaPowerMode1Bits;

typedef struct {
    uint16_t SupportsTrim : 1;
    uint16_t Reserved0 : 15;
} DataSetManagementFeatureBits;

typedef struct {
    uint16_t Supported : 1;
    uint16_t Reserved0 : 1;
    uint16_t WriteSameSuported : 1;
    uint16_t ErrorRecoveryControlSupported : 1;
    uint16_t FeatureControlSuported : 1;
    uint16_t DataTablesSuported : 1;
    uint16_t Reserved1 : 6;
    uint16_t VendorSpecific : 4;
} SCTCommandTransportBits;

typedef struct {
    uint16_t AlignmentOfLogicalWithinPhysical : 14;
    uint16_t Word209Supported : 1;
    uint16_t Reserved0 : 1;
} BlockAlignmentBits;

typedef struct {
    uint16_t NVCachePowerModeEnabled : 1;
    uint16_t Reserved0 : 3;
    uint16_t NVCacheFeatureSetEnabled : 1;
    uint16_t Reserved1 : 3;
    uint16_t NVCachePowerModeVersion : 4;
    uint16_t NVCacheFeatureSetVersion : 4;
} NVCacheCapabilitiesBits;

typedef struct {
    uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
    uint8_t Reserved;
} NVCacheOptionsBits;

typedef struct {
    uint16_t MajorVersion : 12;
    uint16_t TransportType : 4;
} TransportMajorVersionBits;

typedef struct {
    uint16_t GeneralConfiguration;
    uint16_t NumCylinders;
    uint16_t NumHeads;
    uint16_t NumSectorsPerTrack;
    uint8_t SerialNumber[20];
    uint8_t FirmwareRevision[8];
    uint8_t ModelNumber[40];
    uint16_t Capabilities[2];
    uint32_t UserAddressableSectors;
    uint32_t total_sectors_lba48[2];     // 64 bit value
    uint16_t MultiWordDMASupport;        // in case DMA support is added later on
    uint16_t MultiWordDMAActive;
    uint16_t AdvancedPIOModes;
} ATA_IDENTIFY_DEVICE_DATA;

uint8_t read_status(void);
void select_device(uint8_t device);
void init_disk(void);
void wait_not_busy(void);
void wait_ready(void);
int read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* buffer);
int write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t* buffer);
int identify_device(ATA_IDENTIFY_DEVICE_DATA* device_info);

#endif //DISK_H
