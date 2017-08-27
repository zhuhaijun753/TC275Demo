/**
 * \file IfxVadc_Adc.h
 * \brief VADC ADC details
 * \ingroup IfxLld_Vadc
 *
 * \version iLLD_1_0_0_8_0
 * \copyright Copyright (c) 2013 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * \defgroup IfxLld_Vadc_Adc_Usage How to use the VADC ADC Interface driver?
 * \ingroup IfxLld_Vadc
 *
 * VADC comprises of independent analog channels with Analog/Digital converters to convert analog input to discrete digital output.
 *
 * In the following sections it will be described, how to integrate the driver into the application framework.
 *
 * \section IfxLld_Vadc_Adc_Preparation Preparation
 * \subsection IfxLld_Vadc_Adc_Include Include Files
 *
 * Include following header file into your C code:
 * \code
 *
 * #include <Vadc/Adc/IfxVadc_Adc.h>
 *
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_Variables Variables
 * \code
 *
 * // VADC handle
 * IfxVadc_Adc vadc;
 * IfxVadc_Adc_Group adcGroup;
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_ModuleInitialisation Module Initialisation
 * The module initialisation can be done in the same function:
 * \code
 *     // create configuration
 *     IfxVadc_Adc_Config adcConfig;
 *     IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
 *
 *     // initialize module
 *     // IfxVadc_Adc vadc; // declared globally
 *     IfxVadc_Adc_initModule(&vadc, &adcConfig);
 * \endcode
 *
 *
 * \subsection IfxLld_Vadc_Adc_GroupInitialisation Group Initialisation
 * The group initialisation can be done in the same function:
 * \code
 *     // create group config
 *     IfxVadc_Adc_GroupConfig adcGroupConfig;
 *     IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 *     // change group (default is GroupId_0, change to GroupId_3)
 *     adcGroupConfig.groupId = IfxVadc_GroupId_3;
 *
 *     // IMPORTANT: usually we use the same group as master!
 *     adcGroupConfig.master = adcGroupConfig.groupId;
 *
 *     // enable all arbiter request sources
 *     adcGroupConfig.arbiter.requestSlotQueueEnabled          = TRUE; // enable Queue mode
 *     adcGroupConfig.arbiter.requestSlotScanEnabled           = TRUE; // enable Scan mode
 *     adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE; // enable Background scan
 *
 *     // enable all gates in "always" mode (no edge detection)
 *     adcGroupConfig.queueRequest.triggerConfig.gatingMode          = IfxVadc_GatingMode_always;
 *     adcGroupConfig.scanRequest.triggerConfig.gatingMode           = IfxVadc_GatingMode_always;
 *     adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 *     // enable auto scan
 *     adcGroupConfig.scanRequest.autoscanEnabled = TRUE;
 *     adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
 *
 *     // initialize the group
 *     IfxVadc_Adc_initGroup(&adcGroup, &adcGroupConfig);
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_QueuedTransfers Queued Transfers
 * Now, VADC is initialised. Here,Three channels are used for queued transfers
 * \code
 *     // IMPORTANT: for deterministic results we have to disable the queue gate
 *     // while filling the queue, otherwise results could be output in the wrong order
 *        IfxVadc_GatingMode savedGate = IfxVadc_getQueueSlotGatingMode(adcGroup.group);
 *        IfxVadc_GatingSource gatingSource=IfxVadc_getQueueSlotGatingSource(adcGroup.group);
 *
 *        IfxVadc_setQueueSlotGatingConfig(adcGroup.group, gatingSource, IfxVadc_GatingMode_disabled );
 *     // create channel config
 *     IfxVadc_Adc_ChannelConfig adcChannelConfig[3];
 *     IfxVadc_Adc_Channel adcChannel[3];
 *
 *     for(int chnIx=0; chnIx<3; ++chnIx) {
 *         IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &adcGroup);
 *
 *         adcChannelConfig[chnIx].channelId = (IfxVadc_ChannelId)(chnIx);
 *         adcChannelConfig[chnIx].resultRegister = IfxVadc_ChannelResult_1; // use result register #1 for all channels
 *
 *         // initialize the channel
 *         IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);
 *
 *         // Add channel to queue with refill enabled
 *         IfxVadc_Adc_addToQueue(&adcChannel[chnIx], IFXVADC_QUEUE_REFILL);
 *
 *         // restore previous gate config
 *
 *         IfxVadc_setQueueSlotGatingConfig(adcGroup.group, gatingSource, savedGate );
 *
 *         // start the Queue
 *         IfxVadc_Adc_startQueue(&adcGroup); // just for the case that somebody copy&pastes the code - the queue has already been started in previous test
 *
 *         // get 10 results for all 3 channels and store in temporary buffer
 *         // (the usage of a buffer is required, since the print statements used by the checks take more time than the conversions)
 *         Ifx_VADC_RES resultTrace[3*10];
 *         for(int i=0; i<3*10; ++i)
 *         {
 *             unsigned chnIx = i % 3;
 *
 *             // wait for valid result
 *             Ifx_VADC_RES conversionResult;
 *             do {
 *                 conversionResult = IfxVadc_Adc_getResult(&adcChannel[chnIx]);
 *             } while( !conversionResult.B.VF );
 *
 *             // store result
 *             resultTrace[i] = conversionResult;
 *         }
 *
 *         // stop the queue
 *         IfxVadc_Adc_clearQueue(&adcGroup);
 *
 *         // check results in buffer
 *  // ...
 *     }
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_AutoScan Auto Scan
 * Autoscan of 5 channels
 * \code
 *     // create group config
 *     IfxVadc_Adc_GroupConfig adcGroupConfig;
 *     IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 *     // change group (default is GroupId_0, change to GroupId_3)
 *     adcGroupConfig.groupId = IfxVadc_GroupId_3;
 *
 *     // IMPORTANT: usually we use the same group as master!
 *     adcGroupConfig.master = adcGroupConfig.groupId;
 *
 *     // enable gate in "always" mode (no edge detection)
 *     adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 *     // enable auto scan
 *     adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;
 *     adcGroupConfig.scanRequest.autoscanEnabled = TRUE;
 *
 *     // initialize the group
 *     //IfxVadc_Adc_Group adcGroup; // no need to create a new one
 *     IfxVadc_Adc_initGroup(&adcGroup, &adcGroupConfig);
 *
 *     {
 *         // create channel config
 *         IfxVadc_Adc_ChannelConfig adcChannelConfig[5];
 *         IfxVadc_Adc_Channel adcChannel[5];
 *
 *         for(int chnIx=0; chnIx<5; ++chnIx) {
 *             IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &adcGroup);
 *
 *             adcChannelConfig[chnIx].channelId = (IfxVadc_ChannelId)(chnIx);
 *             adcChannelConfig[chnIx].resultRegister = (IfxVadc_ChannelResult)(chnIx); // use dedicated result register
 *
 *             // initialize the channel
 *             IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);
 *
 *             // add to scan
 *             unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
 *             unsigned mask = channels;
 *             IfxVadc_Adc_setScan(&adcGroup, channels, mask);
 *         }
 *
 *         // start scan
 *         IfxVadc_Adc_startScan(&adcGroup);
 *
 *         // check results
 *         for(int chnIx=0; chnIx<5; ++chnIx) {
 *             unsigned group = adcChannel[chnIx].group->groupId;
 *             unsigned channel = adcChannel[chnIx].channel;
 *
 *             // wait for valid result
 *             Ifx_VADC_RES conversionResult;
 *             do {
 *                 conversionResult = IfxVadc_Adc_getResult(&adcChannel[chnIx]);
 *             } while( !conversionResult.B.VF );
 *
 *
 *         }
 *     }
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_BackGroundScan Background Scan
 * Background Scan of 2 channels
 *
 * \code
 *     // create group config
 *     IfxVadc_Adc_GroupConfig adcGroupConfig;
 *     IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 *     // change group (default is GroupId_0, change to GroupId_3)
 *     adcGroupConfig.groupId = IfxVadc_GroupId_3;
 *
 *     // IMPORTANT: usually we use the same group as master!
 *     adcGroupConfig.master = adcGroupConfig.groupId;
 *
 *     // enable background scan
 *     adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled  = TRUE;
 *     adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
 *
 *     // enable gate in "always" mode (no edge detection)
 *     adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 *     // create channel config
 *     IfxVadc_Adc_ChannelConfig adcChannelConfig[2];
 *     IfxVadc_Adc_Channel adcChannel[2];
 *
 *     for(int chnIx=0; chnIx<2; ++chnIx)
 *     {
 *         IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &adcGroup);
 *
 *         adcChannelConfig[chnIx].channelId = (IfxVadc_ChannelId)(chnIx + 5);
 *         adcChannelConfig[chnIx].resultRegister = (IfxVadc_ChannelResult)(5 + chnIx); // use register #5 and 6 for results
 *         adcChannelConfig[chnIx].backgroundChannel = TRUE;
 *
 *         // initialize the channel
 *         IfxVadc_Adc_initChannel(&adcChannel[chnIx], &adcChannelConfig[chnIx]);
 *
 *         // add to background scan
 *         unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
 *         unsigned mask = channels;
 *         IfxVadc_Adc_setBackgroundScan(&vadc, &adcGroup, channels, mask);
 *     }
 *
 *     // start autoscan
 *     IfxVadc_Adc_startBackgroundScan(&vadc);
 *
 *     // check results
 *     for(int chnIx=0; chnIx<2; ++chnIx)
 *     {
 *         unsigned group = adcChannel[chnIx].group->groupId;
 *         unsigned channel = adcChannel[chnIx].channel;
 *
 *         // wait for valid result
 *         Ifx_VADC_RES conversionResult;
 *         do
 *         {
 *             conversionResult = IfxVadc_Adc_getResult(&adcChannel[chnIx]);
 *         } while( !conversionResult.B.VF );
 *
 *         // check with expected value
 *  // ...
 *     }
 * \endcode
 *
 * \subsection IfxLld_Vadc_Adc_EmuxConfiguration External Multiplexer Configuration
 * External Configuration of 3 channels at channel 3
 *
 * \code
 *
 *       IfxVadc_Adc_EmuxControl emuxConfig;
 *
 *         IfxVadc_Adc_initExternalMultiplexerModeConfig(&emuxConfig,vadc);
 *
 *         emuxConfig.groupId           = IfxVadc_GroupId_1;
 *         emuxConfig.channels          = (uint8)IfxVadc_ChannelId_3;
 *         emuxConfig.startChannel   = IfxVadc_EmuxSelectValue_2; // it will take 0 to 2 external channel
 *
 *         emuxConfig.sampleTimeControl = IfxVadc_EmuxSampleTimeControl_always;
 *         emuxConfig.mode              = IfxVadc_ExternalMultiplexerMode_steady;
 *
 *         IfxVadc_Adc_EmuxPinConfig pinsConfig ={
 *  .pins={ &IfxVadc_EMUX00_P02_6_OUT,
 *          &IfxVadc_EMUX01_P02_7_OUT,
 *          &IfxVadc_EMUX02_P02_8_OUT},
 *
 *
 *         .outputMode = IfxPort_OutputMode_pushPull,
 *         .padDriver  = IfxPort_PadDriver_cmosAutomotiveSpeed1
 *  };
 *         emuxConfig.emuxOutPinConfig = pinsConfig;
 *
 *         IfxVadc_Adc_initExternalMultiplexerMode(vadc, &emuxConfig);
 *
 *         IfxVadc_setEmuxGroupResolution(&vadc->G[emuxConfig.groupId], 0, IfxVadc_ChannelResolution_12bit);
 *         IfxVadc_setEmuxGroupSampletime(&vadc->G[emuxConfig.groupId], 0, 50000, 1.0e-6);
 *
 * \endcode
 *
 * \defgroup IfxLld_Vadc_Adc Interface Driver
 * \ingroup IfxLld_Vadc
 * \defgroup IfxLld_Vadc_Adc_DataStructures Data Structures
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Module Module Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Group Group Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Channel Channel Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Background_Autoscan Background Autoscan Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_ChannelScan Channel Scan Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Queue Queue Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Clock Clock Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Interrupt Interrupt Functions
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Variables Variables
 * \ingroup IfxLld_Vadc_Adc
 * \defgroup IfxLld_Vadc_Adc_Emux Emux Functions
 * \ingroup IfxLld_Vadc_Adc
 */

#ifndef IFXVADC_ADC_H
#define IFXVADC_ADC_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Vadc/Std/IfxVadc.h"
#include "_Utilities/Ifx_Assert.h"

/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/

typedef uint8 IfxVadc_Adc_SYNCTR_STSEL;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Vadc_Adc_DataStructures
 * \{ */
/** \brief VADC handle data structure
 */
typedef struct
{
    Ifx_VADC *vadc;     /**< \brief Specifies the pointer to the VADC module registers */
} IfxVadc_Adc;

/** \brief Gating/Trigger configuration structure
 */
typedef struct
{
    IfxVadc_GatingSource  gatingSource;      /**< \brief Specifies used gate input for group */
    IfxVadc_TriggerSource triggerSource;     /**< \brief Specifies used Trigger input for group */
    IfxVadc_GatingMode    gatingMode;        /**< \brief Specifies gating mode. High level, Low Level  or Gating disabled */
    IfxVadc_TriggerMode   triggerMode;       /**< \brief Specifies trigger mode. Rising, falling any edge leads to an trigger event */
} IfxVadc_Adc_GatingTriggerConfig;

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_DataStructures
 * \{ */
/** \brief Arbiter configuration structure.
 */
typedef struct
{
    IfxVadc_ArbitrationRounds arbiterRoundLength;                   /**< \brief Specifies arbiter round length. */
    boolean                   requestSlotQueueEnabled;              /**< \brief request queue if enabled. */
    boolean                   requestSlotScanEnabled;               /**< \brief request scan if enabled. */
    boolean                   requestSlotBackgroundScanEnabled;     /**< \brief request background scan if enabled. */
} IfxVadc_Adc_ArbiterConfig;

/** \brief Background scan mode configuration structure.
 */
typedef struct
{
    boolean                         autoBackgroundScanEnabled;     /**< \brief background autoscan functionality enable or disable. */
    IfxVadc_Adc_GatingTriggerConfig triggerConfig;                 /**< \brief trigger and gating configuration. */
    IfxVadc_RequestSlotPriority     requestSlotPrio;               /**< \brief priority of used background scan request slot. */
    IfxVadc_RequestSlotStartMode    requestSlotStartMode;          /**< \brief start mode for request background scan source. */
} IfxVadc_Adc_BackgroundScanConfig;

/** \brief Input class configuration structure
 */
typedef struct
{
    float32                   sampleTime;     /**< \brief Specifies the requested sample time for input class */
    IfxVadc_ChannelResolution resolution;     /**< \brief Specifies the conversion Mode 8,10,12Bit or 10bit fast compare */
} IfxVadc_Adc_ClassConfig;

/** \brief Group handle data structure
 */
typedef struct
{
    IfxVadc_Adc     module;      /**< \brief The VADC handle structure */
    Ifx_VADC_G     *group;       /**< \brief Pointer to the group registers */
    IfxVadc_GroupId groupId;     /**< \brief Specifies the group index */
} IfxVadc_Adc_Group;

/** \brief Queue configuration structure
 */
typedef struct
{
    boolean                         flushQueueAfterInit;      /**< \brief Specifies if the queue is flushed after configuration */
    IfxVadc_Adc_GatingTriggerConfig triggerConfig;            /**< \brief trigger and gating configuration. */
    IfxVadc_RequestSlotPriority     requestSlotPrio;          /**< \brief priority of used  queue request slot. */
    IfxVadc_RequestSlotStartMode    requestSlotStartMode;     /**< \brief start mode for request queue source. */
} IfxVadc_Adc_QueueConfig;

/** \brief Scan mode configuration structure.
 */
typedef struct
{
    boolean                         autoscanEnabled;          /**< \brief Specifies autoscan functionality. */
    IfxVadc_Adc_GatingTriggerConfig triggerConfig;            /**< \brief Specifies trigger and gating configuration */
    IfxVadc_RequestSlotPriority     requestSlotPrio;          /**< \brief priority of used  scan request slot. */
    IfxVadc_RequestSlotStartMode    requestSlotStartMode;     /**< \brief start mode for request scan source. */
} IfxVadc_Adc_ScanConfig;

/** \} */

typedef struct
{
    IfxVadc_Emux_Out  *pins[3];        /**< \brief Emux Pins configuration */
    IfxPort_OutputMode outputMode;     /**< \brief the pin output mode which should be configured */
    IfxPort_PadDriver  padDriver;      /**< \brief Pad driver */
} IfxVadc_Adc_EmuxPinConfig;

/** \addtogroup IfxLld_Vadc_Adc_DataStructures
 * \{ */
/** \brief Channel handle data structure
 */
typedef struct
{
    IfxVadc_ChannelId        channel;       /**< \brief Specifies the channel index */
    IfxVadc_ChannelResult    resultreg;     /**< \brief Specifies allocated result register */
    const IfxVadc_Adc_Group *group;         /**< \brief Specifies the group of the channel */
} IfxVadc_Adc_Channel;

/** \brief Channel configuration structure
 */
typedef struct
{
    boolean                   globalResultUsage;       /**< \brief Specifies storage in global result register */
    boolean                   synchonize;              /**< \brief Specifies synchronized conversion channel */
    boolean                   backgroundChannel;       /**< \brief Specifies channel is used as background channel */
    boolean                   rightAlignedStorage;     /**< \brief Specifies result is right aligned */
    Ifx_Priority              resultPriority;          /**< \brief Interrupt priority of the result trigger interrupt, if 0 the interrupt is disable */
    Ifx_Priority              channelPriority;         /**< \brief Interrupt priority of the channel trigger interrupt, if 0 the interrupt is disable */
    IfxSrc_Tos                resultServProvider;      /**< \brief Interrupt service provider for the result trigger interrupt */
    IfxSrc_Tos                channelServProvider;     /**< \brief Interrupt service provider for the channel trigger interrupt */
    IfxVadc_SrcNr             resultSrcNr;             /**< \brief Service node of the result trigger */
    IfxVadc_SrcNr             channelSrcNr;            /**< \brief Service node of the channel trigger */
    IfxVadc_ChannelId         channelId;               /**< \brief Specifies the channel index */
    IfxVadc_InputClasses      inputClass;              /**< \brief Specifies input class selection */
    IfxVadc_ChannelReference  reference;               /**< \brief Specifies Reference selection */
    IfxVadc_ChannelResult     resultRegister;          /**< \brief Specifies Result register selection */
    IfxVadc_BoundarySelection lowerBoundary;           /**< \brief Specifies lower boundary selection */
    IfxVadc_BoundarySelection upperBoundary;           /**< \brief Specifies upper boundary selection */
    IfxVadc_BoundaryExtension boundaryMode;            /**< \brief Specifies Standard mode of fast compare mode */
    IfxVadc_LimitCheck        limitCheck;              /**< \brief Specifies boundary band selection upper/lower */
    const IfxVadc_Adc_Group  *group;                   /**< \brief Specifies pointer to the IfxVadc_Adc_Group group handle */
} IfxVadc_Adc_ChannelConfig;

/** \brief VADC module configuration structure
 */
typedef struct
{
    Ifx_VADC               *vadc;                                                         /**< \brief Specifies the pointer to the VADC module registers */
    IfxVadc_Adc_ClassConfig globalInputClass[IFXVADC_NUM_GLOBAL_INPUTCLASSES];            /**< \brief Specifies the global conversion settings one and two */
    float32                 digitalFrequency;                                             /**< \brief Specifies digital ADC Frequency */
    float32                 analogFrequency;                                              /**< \brief Specifies analog ADC Frequency */
    float32                 moduleFrequency;                                              /**< \brief module Frequency in Hz. */
    boolean                 startupCalibration;                                           /**< \brief Can be enabled to execute a startup calibration (disabled by default).
                                                                                           * Note that this option will also enable all converter groups.
                                                                                           * If this isn't desired, don't use this option, but execute IfxVadc_Adc_startupCalibration() after all ADC groups have been initialized. */
    IfxVadc_LowSupplyVoltageSelect supplyVoltage;                                         /**< \brief Select Low Power Supply Voltage */
} IfxVadc_Adc_Config;

/** \brief Emux Control Structure
 */
typedef struct
{
    Ifx_VADC                       *vadc;                      /**< \brief pointer to Module Configuration */
    IfxVadc_ExternalMultiplexerMode mode;                      /**< \brief Specifies the External Multiplexer mode */
    IfxVadc_EmuxSelectValue         startChannel;              /**< \brief specifies the external channel start value(EMUX[x:0])
                                                                * x- specifies external channel number */
    IfxVadc_EmuxCodingScheme        code;                      /**< \brief specifes binary/gray code */
    IfxVadc_EmuxSampleTimeControl   sampleTimeControl;         /**< \brief specifies when to use sample time control */
    IfxVadc_GroupId                 groupId;                   /**< \brief specifies groupId */
    uint8                           channels;                  /**< \brief specifies channel number */
    IfxVadc_EmuxInterface           emuxInterface;             /**< \brief specifies the Emux interface */
    IfxVadc_Adc_EmuxPinConfig       emuxOutPinConfig;          /**< \brief configure the emux output pin */
    IfxVadc_ChannelSelectionStyle   channelSelectionStyle;     /**< \brief External Multiplexer Channel Selection Style */
} IfxVadc_Adc_EmuxControl;

/** \brief Group configuration structure
 */
typedef struct
{
    const IfxVadc_Adc               *module;                                   /**< \brief Specifies pointer to the IfxVadc_Adc module handle */
    IfxVadc_GroupId                  groupId;                                  /**< \brief Specifies the group/kernel id */
    IfxVadc_GroupId                  master;                                   /**< \brief Specifies the master group. If master is different from groupId, then the group is configured as slave. */
    IfxVadc_Adc_ClassConfig          inputClass[IFXVADC_NUM_INPUTCLASSES];     /**< \brief Specifies conversion settings one and two */
    IfxVadc_Adc_ScanConfig           scanRequest;                              /**< \brief Specifies scan mode configuration */
    IfxVadc_Adc_QueueConfig          queueRequest;                             /**< \brief Specifies queued mode configuration */
    IfxVadc_Adc_BackgroundScanConfig backgroundScanRequest;                    /**< \brief Specifies back ground scan configuration */
    boolean                          disablePostCalibration;                   /**< \brief Specifies if calibration after conversion (post calibration) should be disabled */
    IfxVadc_Adc_ArbiterConfig        arbiter;                                  /**< \brief Arbiter configuration structure. */
} IfxVadc_Adc_GroupConfig;

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Module
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Reset the VADC module
 * \param vadc pointer to the VADC module
 * \return None
 *
 * Example Usage :\ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_deInitModule(IfxVadc_Adc *vadc);

/** \brief Get the current VADC configuration (e.g. VADC frequency)
 * \param vadc pointer to the VADC module
 * \param config pointer to the VADC module configuration
 * \return None
 *
 * Example Usage :\ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_getModuleConfig(IfxVadc_Adc *vadc, IfxVadc_Adc_Config *config);

/** \brief Get conversion result based on the Request Source. (Function does not care about the alignment)
 * value = raw * gain + offset
 * \param group pointer to the VADC group
 * \param channel channel number
 * \param sourceType type of request source
 * \return scaled Conversion result
 *
 * \code
 * // create configuration
 * IfxVadc_Adc_Config adcConfig;
 * IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
 *
 * // initialize module
 * IfxVadc_Adc vadc;
 * IfxVadc_Adc_initModule(&vadc, &adcConfig);
 *
 * // create group config
 * IfxVadc_Adc_GroupConfig adcGroupConfig;
 * IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 * // change group (default is GroupId0, change to GroupId2)
 * adcGroupConfig.groupId = IfxVadc_GroupId2;
 * adcGroupConfig.master = adcGroupConfig.groupId;
 *
 * // enable gate in "always" mode (no edge detection)
 * adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 * // enable auto scan
 * adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;
 * adcGroupConfig.scanRequest.autoscanEnabled = TRUE;
 *
 * // initialize the group
 * IfxVadc_Adc_Group adcGroup;
 * IfxVadc_Adc_initGroup(&adcGroup, &adcGroupConfig);
 *
 * // create channel config
 * IfxVadc_Adc_ChannelConfig adcChannelConfig;
 * IfxVadc_Adc_initChannelConfig(&adcChannelConfig, &adcGroup);
 *
 * // change channel (default is ChannelId0, change to ChannelId2)
 * adcChannelConfig.channelId = IfxVadc_ChannelId2;
 *
 * // initialize the channel
 * IfxVadc_Adc_Channel adcChannel;
 * IfxVadc_Adc_initChannel(&adcChannel, &adcChannelConfig);
 *
 * uint32 channels = (1 << 2); // enable channel #2
 * uint32 mask = (1 << 7) | (1 << 2); // modify the selection for channel #7 and #2; channel #7 will be disabled
 *
 * // configure wait for read mode
 * IfxVadc_Adc_configureWaitForReadMode(&adcChannel, TRUE);
 *
 * // configure scan
 * IfxVadc_Adc_setScan(&adcGroup, channels, mask);
 *
 * // start the scan
 * IfxVadc_Adc_startScan(&adcGroup);
 *
 * // wait for valid result
 * Ifx_VADC_RES resultChannel;
 * do {
 * resultChannel = IfxVadc_Adc_getResultBasedOnRequestSource(&adcGroup, IfxVadc_ChannelId2, IfxVadc_RequestSource_scan);
 * } while( !resultChannel.B.VF );
 * \endcode
 *
 */
IFX_INLINE Ifx_VADC_RES IfxVadc_Adc_getResultBasedOnRequestSource(IfxVadc_Adc_Group *group, IfxVadc_ChannelId channel, IfxVadc_RequestSource sourceType);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Disable VADC Module
 * \param vadc Pointer to VADC Module
 * \return None
 */
IFX_EXTERN void IfxVadc_Adc_disableModule(Ifx_VADC *vadc);

/** \brief Initialise the VADC to run with the expected frequency and calibration
 * \param vadc pointer to the VADC handle
 * \param config pointer to the VADC configuration
 * \return IfxVadc_Status
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN IfxVadc_Status IfxVadc_Adc_initModule(IfxVadc_Adc *vadc, const IfxVadc_Adc_Config *config);

/** \brief Initialise buffer with default VADC configuration
 * \param config pointer to the VADC module configuration
 * \param vadc pointer to the VADC
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_initModuleConfig(IfxVadc_Adc_Config *config, Ifx_VADC *vadc);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Group
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Gets the current group register set
 * \param group Group handle data structure
 * \return Group register set
 *
 * Ifx_VADC* vadc = &MODULE_VADC;
 * Ifx_VADC_G* group = &MODULE_VADC.G[0]; // for group 0
 *
 * uint32 channels = (1 << 5) | (1 << 2); // enable channel #5 and #2
 * uint32 mask = (1 << 7) | (1 << 5) | (1 << 2); // modify the selection for channel #7, #5 and #2; channel #7 will be disabled
 *
 * //confiure wait for read mode for global result register
 * IfxVadc_configureWaitForReadMode(group,IfxVadc_ChannelResult0, TRUE);
 * IfxVadc_configureWaitForReadMode(group,IfxVadc_ChannelResult1, TRUE);
 *
 * // configure scan
 * IfxVadc_setScan(group, channels, mask);
 *
 * // enable auto scan
 * IfxVadc_setAutoScan(group, TRUE);
 *
 * // start the scan
 * IfxVadc_startScan(group);
 *
 * // wait for conversion to finish
 *
 * // fetch the 2 results of conversion for group 0
 * Ifx_VADC_RES results[10];
 * result = IfxVadc_getGroupResult(group, results, 0, 2);
 *
 */
IFX_INLINE Ifx_VADC_G *IfxVadc_Adc_getGroupRegsFromGroup(const IfxVadc_Adc_Group *group);

/** \brief Get conversion result for the group
 * \param group pointer to the VADC group
 * \param results pointer to scaled conversion results
 * \param resultOffset offset for the first result
 * \param numResults number of results
 * \return None
 *
 * \code
 * // create configuration
 * IfxVadc_Adc_Config adcConfig;
 * IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
 *
 * // initialize module
 * IfxVadc_Adc vadc;
 * IfxVadc_Adc_initModule(&vadc, &adcConfig);
 *
 * // create group config
 * IfxVadc_Adc_GroupConfig adcGroupConfig;
 * IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 * // change group (default is GroupId0, change to GroupId2)
 * adcGroupConfig.groupId = IfxVadc_GroupId2;
 * adcGroupConfig.master = adcGroupConfig.groupId;
 *
 * // enable gate in "always" mode (no edge detection)
 * adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 * // enable auto scan
 * adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;
 * adcGroupConfig.scanRequest.autoscanEnabled = TRUE;
 *
 * // initialize the group
 * IfxVadc_Adc_Group adcGroup;
 * IfxVadc_Adc_initGroup(&adcGroup, &adcGroupConfig);
 *
 * // create channel config
 * IfxVadc_Adc_ChannelConfig adcChannelConfig2;
 * IfxVadc_Adc_initChannelConfig(&adcChannelConfig2, &adcGroup);
 *
 * // change channel (default is ChannelId0, change to ChannelId2)
 * adcChannelConfig2.channelId = IfxVadc_ChannelId2;
 *
 * // initialize the channel
 * IfxVadc_Adc_Channel adcChannel2;
 * IfxVadc_Adc_initChannel(&adcChannel2, &adcChannelConfig2);
 *
 * // create channel config
 * IfxVadc_Adc_ChannelConfig adcChannelConfig5;
 * IfxVadc_Adc_initChannelConfig(&adcChannelConfig5, &adcGroup);
 *
 * // change channel (default is ChannelId0, change to ChannelId5)
 * adcChannelConfig5.channelId = IfxVadc_ChannelId5;
 * // change channel result register (default is ChannelResult0, change to ChannelResult1)
 * adcChannelConfig5.resultRegister = IfxVadc_ChannelResult1;
 *
 * // initialize the channel
 * IfxVadc_Adc_Channel adcChannel5;
 * IfxVadc_Adc_initChannel(&adcChannel5, &adcChannelConfig5);
 *
 * uint32 channels = (1 << 5) | (1 << 2); // enable channel #5 and #2
 * uint32 mask = (1 << 7) | (1 << 5) | (1 << 2); // modify the selection for channel #7, #5 and #2; channel #7 will be disabled
 *
 * // configure wait for read mode
 * IfxVadc_Adc_configureWaitForReadMode(&adcChannel2, TRUE);
 * IfxVadc_Adc_configureWaitForReadMode(&adcChannel5, TRUE);
 *
 * // configure scan
 * IfxVadc_Adc_setScan(&adcGroup, channels, mask);
 *
 * // start the scan
 * IfxVadc_Adc_startScan(&adcGroup);
 *
 * // wait for conversion to finish
 * IfxVadc_Status scanStatus;
 * do
 * {
 *    scanStatus = IfxVadc_Adc_getScanStatus(&adcGroup);
 * } while(scanStatus==IfxVadc_Status_ChannelsStillPending);
 *
 * // fetch the 2 results of conversion for group 0
 * Ifx_VADC_RES results[10];
 * IfxVadc_Adc_getGroupResult(&adcGroup, results, 0, 2);
 * \endcode
 *
 */
IFX_INLINE void IfxVadc_Adc_getGroupResult(IfxVadc_Adc_Group *group, Ifx_VADC_RES *results, uint32 resultOffset, uint32 numResults);

/** \brief Gets the current group module register address
 * \param group Group handle data structure
 * \return Group module register base address
 */
IFX_INLINE Ifx_VADC *IfxVadc_Adc_getVadcFromGroup(const IfxVadc_Adc_Group *group);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Reset the VADC group
 * \param group pointer to the VADC group
 * \return None
 *
 * Example Usage :\ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_deInitGroup(IfxVadc_Adc_Group *group);

/** \brief Get the current group configuration (e.g. vadc frequency)
 * \param group pointer to the VADC group
 * \param config pointer to the VADC group configuration
 * \return None
 *
 * Example Usage :\ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_getGroupConfig(IfxVadc_Adc_Group *group, IfxVadc_Adc_GroupConfig *config);

/** \brief Initialise the VADC group (also autoscan and queue modes) Slave Groups must initialize first.
 * \param group pointer to the VADC group
 * \param config pointer to the VADC group configuration
 * \return IfxVadc_Status
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN IfxVadc_Status IfxVadc_Adc_initGroup(IfxVadc_Adc_Group *group, const IfxVadc_Adc_GroupConfig *config);

/** \brief Initialise buffer with default VADC configuration
 * \param config pointer to the VADC group configuration
 * \param vadc pointer to the VADC module
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_initGroupConfig(IfxVadc_Adc_GroupConfig *config, IfxVadc_Adc *vadc);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Channel
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief pointer to the VADC channel
 * \param channel pointer to the VADC channel
 * \param waitForRead wait for read mode enabled/disable
 * \return None
 *
 * For coding example see: \ref IfxVadc_Adc_getGroupResult
 *
 */
IFX_INLINE void IfxVadc_Adc_configureWaitForReadMode(IfxVadc_Adc_Channel *channel, boolean waitForRead);

/** \brief Get conversion result (Function does not care about the alignment)
 * \param channel pointer to the VADC channel
 * \return scaled Conversion result
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE Ifx_VADC_RES IfxVadc_Adc_getResult(IfxVadc_Adc_Channel *channel);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Get the current channel configuration (e.g. sample settings)
 * \param channel pointer to the VADC channel
 * \param config pointer to the VADC channel configuration
 * \return None
 *
 * Example Usage :\ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_getChannelConfig(IfxVadc_Adc_Channel *channel, IfxVadc_Adc_ChannelConfig *config);

/** \brief get the channel Conversion Time
 * \param channel Channel
 * \param conversionMode specifies Compatible mode(Standard Conversion mode).
 * \return channel conversion time in sec
 */
IFX_EXTERN float32 IfxVadc_Adc_getChannelConversionTime(IfxVadc_Adc_Channel *channel, IfxVadc_ConversionType conversionMode);

/** \brief Initialise one channel with given configuration
 * \param channel pointer to the VADC channel
 * \param config pointer to the VADC channel configuration
 * \return IfxVadc_Status
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN IfxVadc_Status IfxVadc_Adc_initChannel(IfxVadc_Adc_Channel *channel, const IfxVadc_Adc_ChannelConfig *config);

/** \brief Initialise buffer with default channel configuration
 * \param config pointer to the VADC channel configuration
 * \param group pointer to the VADC group
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_EXTERN void IfxVadc_Adc_initChannelConfig(IfxVadc_Adc_ChannelConfig *config, const IfxVadc_Adc_Group *group);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Background_Autoscan
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief access function to enable/disable  wait for read mode for global result register
 * \param vadc pointer to the VADC module
 * \param waitForRead wait for read mode enabled/disabled
 * \return None
 *
 * For coding example see: \ref IfxVadc_Adc_getGlobalResult
 *
 */
IFX_INLINE void IfxVadc_Adc_configureWaitForReadModeForGlobalResultRegister(IfxVadc_Adc *vadc, boolean waitForRead);

/** \brief Gives the background scan status for a group
 * \param vadc pointer to the VADC module
 * \return IfxVadc_Status
 */
IFX_INLINE IfxVadc_Status IfxVadc_Adc_getBackgroundScanStatus(IfxVadc_Adc *vadc);

/** \brief returns result stored in global result register
 * \param vadc pointer to the VADC module
 * \return global result register
 *
 * \code
 * // create configuration
 * IfxVadc_Adc_Config adcConfig;
 * IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
 *
 * // initialize module
 * IfxVadc_Adc vadc;
 * IfxVadc_Adc_initModule(&vadc, &adcConfig);
 *
 * // create group config
 * IfxVadc_Adc_GroupConfig adcGroupConfig;
 * IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &vadc);
 *
 * // change group (default is GroupId_0, change to GroupId_3)
 * adcGroupConfig.groupId = IfxVadc_GroupId_3;
 *
 * // IMPORTANT: usually we use the same group as master!
 * adcGroupConfig.master = adcGroupConfig.groupId;
 *
 * // enable background scan
 * adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled  = TRUE;
 * adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
 *
 * // enable gate in "always" mode (no edge detection)
 * adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
 *
 * // initialize the group
 * IfxVadc_Adc_Group adcGroup;
 * IfxVadc_Adc_initGroup(&adcGroup, &adcGroupConfig);
 *
 * // create channel config
 * IfxVadc_Adc_ChannelConfig adcChannelConfig2;
 * IfxVadc_Adc_initChannelConfig(&adcChannelConfig2, &adcGroup);
 * adcChannelConfig2.backgroundChannel   = TRUE;
 * adcChannelConfig2.globalResultUsage   = TRUE;
 *
 * // change channel (default is ChannelId0, change to ChannelId2)
 * adcChannelConfig2.channelId = IfxVadc_ChannelId2;
 *
 * // initialize the channel
 * IfxVadc_Adc_Channel adcChannel2;
 * IfxVadc_Adc_initChannel(&adcChannel2, &adcChannelConfig2);
 *
 * // create channel config
 * IfxVadc_Adc_ChannelConfig adcChannelConfig5;
 * IfxVadc_Adc_initChannelConfig(&adcChannelConfig5, &adcGroup);
 * adcChannelConfig5.backgroundChannel   = TRUE;
 * adcChannelConfig5.globalResultUsage   = TRUE;
 *
 * // change channel (default is ChannelId0, change to ChannelId5)
 * adcChannelConfig5.channelId = IfxVadc_ChannelId5;
 *
 *
 * // initialize the channel
 * IfxVadc_Adc_Channel adcChannel5;
 * IfxVadc_Adc_initChannel(&adcChannel5, &adcChannelConfig5);
 *
 * uint32 channels = (1 << 5) | (1 << 2); // enable channel #5 and #2
 * uint32 mask = (1 << 7) | (1 << 5) | (1 << 2); // modify the selection for channel #7, #5 and #2; channel #7 will be disabled
 *
 * //configure wait for read mode
 * IfxVadc_Adc_configureWaitForReadModeForGlobalResultRegister(&vadc, TRUE);
 *
 * // configure background scan
 * IfxVadc_Adc_setBackgroundScan(&vadc, &adcGroupConfig, channels, mask);
 *
 * // start the background scan
 * IfxVadc_Adc_startBackgroundScan(&vadc);
 *
 * // wait for valid result for channel 2
 * Ifx_VADC_GLOBRES resultChannel2;
 * do {
 * resultChannel2 = IfxVadc_Adc_getGlobalResult(&vadc);
 * } while( !resultChannel2.B.VF );
 *
 * // wait for valid result for channel 5
 * Ifx_VADC_GLOBRES resultChannel5;
 * do {
 *     resultChannel5 = IfxVadc_Adc_getGlobalResult(&vadc);
 * } while( !resultChannel5.B.VF );
 * \endcode
 *
 */
IFX_INLINE Ifx_VADC_GLOBRES IfxVadc_Adc_getGlobalResult(IfxVadc_Adc *vadc);

/** \brief configures a background scan
 * \param vadc pointer to the VADC module
 * \param group pointer to the VADC group
 * \param channels specifies the channels which should be enabled/disabled
 * \param mask specifies the channels which should be modified
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_setBackgroundScan(IfxVadc_Adc *vadc, IfxVadc_Adc_Group *group, uint32 channels, uint32 mask);

/** \brief Starts a background scan
 * \param vadc pointer to the VADC module
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_startBackgroundScan(IfxVadc_Adc *vadc);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_ChannelScan
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Gives the scan status for a group.
 * \param group pointer to the VADC group
 * \return IfxVadc_Status
 *
 * For coding example see: \ref IfxVadc_Adc_getResultBasedOnRequestSource
 *
 */
IFX_INLINE IfxVadc_Status IfxVadc_Adc_getScanStatus(IfxVadc_Adc_Group *group);

/** \brief Configures an autoscan.
 * \param group pointer to the VADC group
 * \param channels specifies the channels which should be enabled/disabled
 * \param mask specifies the channels which should be modified
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_setScan(IfxVadc_Adc_Group *group, uint32 channels, uint32 mask);

/** \brief Starts an autoscan on the specified group
 * \param group pointer to the VADC group
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_startScan(IfxVadc_Adc_Group *group);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Queue
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Add an entry to the queue of a group for the specified channel with the following options set:
 *  refill
 *  source interrupt enable/disable
 * external trigger control
 * \param channel pointer to the VADC channel
 * \param options options for channel
 * \return None
 *
 * For coding example see: \ref IfxVadc_Adc_getResult
 *
 */
IFX_INLINE void IfxVadc_Adc_addToQueue(IfxVadc_Adc_Channel *channel, uint32 options);

/** \brief Flush the contents of the queue of a group
 * \param group pointer to the VADC group
 * \return None
 *
 * For coding example see: \ref IfxVadc_Adc_getResult
 *
 */
IFX_INLINE void IfxVadc_Adc_clearQueue(IfxVadc_Adc_Group *group);

/** \brief Gives the status of the Queue of a group by returning non zero value if the Queue is full
 * \param group pointer to the VADC group
 * \return Queue status
 *
 * For coding example see: \ref IfxVadc_Adc_getResult
 *
 */
IFX_INLINE IfxVadc_Status IfxVadc_Adc_getQueueStatus(IfxVadc_Adc_Group *group);

/** \brief Starts a queue of a group by generating a trigger event through software
 * \param group pointer to the VADC group
 * \return None
 *
 * For coding example see: \ref IfxLld_Vadc_Adc_Usage
 *
 */
IFX_INLINE void IfxVadc_Adc_startQueue(IfxVadc_Adc_Group *group);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Emux
 * \{ */

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief initialise default configuration for external multiplexer
 * \param emuxConfig speciifies EMUX configuration
 * \param vadc pointer to VADC module space
 * \return None
 */
IFX_EXTERN void IfxVadc_Adc_initExternalMultiplexerModeConfig(IfxVadc_Adc_EmuxControl *emuxConfig, Ifx_VADC *vadc);

/** \brief initalise external multiplexer.
 * \param vadc Pointer to VADC Module space
 * \param emuxControl speciifies EMUX configuration
 * \return None
 */
IFX_EXTERN void IfxVadc_Adc_initExternalMultiplexerMode(Ifx_VADC *vadc, const IfxVadc_Adc_EmuxControl *emuxControl);

/** \} */

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/

IFX_INLINE void IfxVadc_Adc_addToQueue(IfxVadc_Adc_Channel *channel, uint32 options)
{
    IfxVadc_addToQueue(channel->group->group, channel->channel, options);
}


IFX_INLINE void IfxVadc_Adc_clearQueue(IfxVadc_Adc_Group *group)
{
    IfxVadc_clearQueue(group->group, TRUE);
}


IFX_INLINE void IfxVadc_Adc_configureWaitForReadMode(IfxVadc_Adc_Channel *channel, boolean waitForRead)
{
    IfxVadc_configureWaitForReadMode(channel->group->group, channel->resultreg, waitForRead);
}


IFX_INLINE void IfxVadc_Adc_configureWaitForReadModeForGlobalResultRegister(IfxVadc_Adc *vadc, boolean waitForRead)
{
    IfxVadc_configureWaitForReadModeForGlobalResultRegister(vadc->vadc, waitForRead);
}


IFX_INLINE void IfxVadc_Adc_deInitModule(IfxVadc_Adc *vadc)
{
    Ifx_VADC *vadcSFR = vadc->vadc;

    IfxVadc_resetKernel(vadcSFR);
}


IFX_INLINE IfxVadc_Status IfxVadc_Adc_getBackgroundScanStatus(IfxVadc_Adc *vadc)
{
    return IfxVadc_getBackgroundScanStatus(vadc->vadc);
}


IFX_INLINE Ifx_VADC_GLOBRES IfxVadc_Adc_getGlobalResult(IfxVadc_Adc *vadc)
{
    return IfxVadc_getGlobalResult(vadc->vadc);
}


IFX_INLINE Ifx_VADC_G *IfxVadc_Adc_getGroupRegsFromGroup(const IfxVadc_Adc_Group *group)
{
    return group->group;
}


IFX_INLINE void IfxVadc_Adc_getGroupResult(IfxVadc_Adc_Group *group, Ifx_VADC_RES *results, uint32 resultOffset, uint32 numResults)
{
    IfxVadc_getGroupResult(group->group, results, resultOffset, numResults);
}


IFX_INLINE void IfxVadc_Adc_getModuleConfig(IfxVadc_Adc *vadc, IfxVadc_Adc_Config *config)
{
    config->vadc                           = vadc->vadc;
    config->analogFrequency                = IfxVadc_getAdcAnalogFrequency(vadc->vadc);
    config->digitalFrequency               = IfxVadc_getAdcDigitalFrequency(vadc->vadc);
    config->globalInputClass[0].resolution = IfxVadc_getGlobalResolution(vadc->vadc, 0);
    config->globalInputClass[1].resolution = IfxVadc_getGlobalResolution(vadc->vadc, 1);
    config->globalInputClass[0].sampleTime = IfxVadc_getGlobalSampleTime(vadc->vadc, 0, config->analogFrequency);
    config->globalInputClass[1].sampleTime = IfxVadc_getGlobalSampleTime(vadc->vadc, 1, config->analogFrequency);
    config->moduleFrequency                = IfxVadc_getAdcModuleFrequency();
    config->startupCalibration             = IfxVadc_getStartupCalibration(vadc->vadc);
}


IFX_INLINE IfxVadc_Status IfxVadc_Adc_getQueueStatus(IfxVadc_Adc_Group *group)
{
    return IfxVadc_getQueueStatus(group->group);
}


IFX_INLINE Ifx_VADC_RES IfxVadc_Adc_getResult(IfxVadc_Adc_Channel *channel)
{
    return IfxVadc_getResult(channel->group->group, channel->resultreg);
}


IFX_INLINE Ifx_VADC_RES IfxVadc_Adc_getResultBasedOnRequestSource(IfxVadc_Adc_Group *group, IfxVadc_ChannelId channel, IfxVadc_RequestSource sourceType)
{
    return IfxVadc_getResultBasedOnRequestSource(group->module.vadc, group->group, channel, sourceType);
}


IFX_INLINE IfxVadc_Status IfxVadc_Adc_getScanStatus(IfxVadc_Adc_Group *group)
{
    return IfxVadc_getScanStatus(group->group);
}


IFX_INLINE Ifx_VADC *IfxVadc_Adc_getVadcFromGroup(const IfxVadc_Adc_Group *group)
{
    return group->module.vadc;
}


IFX_INLINE void IfxVadc_Adc_setBackgroundScan(IfxVadc_Adc *vadc, IfxVadc_Adc_Group *group, uint32 channels, uint32 mask)
{
    IfxVadc_setBackgroundScan(vadc->vadc, group->groupId, channels, mask);
}


IFX_INLINE void IfxVadc_Adc_setScan(IfxVadc_Adc_Group *group, uint32 channels, uint32 mask)
{
    IfxVadc_setScan(group->group, channels, mask);
}


IFX_INLINE void IfxVadc_Adc_startBackgroundScan(IfxVadc_Adc *vadc)
{
    IfxVadc_startBackgroundScan(vadc->vadc);
}


IFX_INLINE void IfxVadc_Adc_startQueue(IfxVadc_Adc_Group *group)
{
    IfxVadc_startQueue(group->group);
}


IFX_INLINE void IfxVadc_Adc_startScan(IfxVadc_Adc_Group *group)
{
    IfxVadc_startScan(group->group);
}


#endif /* IFXVADC_ADC_H */
