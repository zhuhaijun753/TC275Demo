/**
 * \file IfxVadc_Adc.c
 * \brief VADC ADC details
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
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "IfxVadc_Adc.h"

/** \addtogroup IfxLld_Vadc_Adc_Group
 * \{ */
/******************************************************************************/
/*------------------------Inline Function Prototypes--------------------------*/
/******************************************************************************/

/** \brief Gets the master id
 * \param slave Index of the group
 * \param masterIndex master kernel index
 * \return Master Group Id
 */
IFX_INLINE IfxVadc_GroupId IfxVadc_Adc_getMasterId(IfxVadc_GroupId slave, IfxVadc_Adc_SYNCTR_STSEL masterIndex);

/** \brief Gets the current master kernel index.
 * \param slave Index of the group
 * \param master Index of the group
 * \return current master kernel index
 */
IFX_INLINE IfxVadc_Adc_SYNCTR_STSEL IfxVadc_Adc_getMasterKernelIndex(IfxVadc_GroupId slave, IfxVadc_GroupId master);

/** \} */

/** \addtogroup IfxLld_Vadc_Adc_Variables
 * \{ */

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

static const IfxVadc_Adc_SYNCTR_STSEL IfxVadc_Adc_masterIndex[IFXVADC_NUM_ADC_CAL_GROUPS][IFXVADC_NUM_ADC_CAL_GROUPS] = {
    /* 0   1   2   3   4   5   6   7 */
    {0, 1, 2, 3, 0, 0, 0, 0},   /* Grp 0 */
    {1, 0, 2, 3, 0, 0, 0, 0},   /* Grp 1 */
    {1, 2, 0, 3, 0, 0, 0, 0},   /* Grp 2 */
    {1, 2, 3, 0, 0, 0, 0, 0},   /* Grp 3 */
    {0, 0, 0, 0, 0, 1, 2, 3},   /* Grp 4 */
    {0, 0, 0, 0, 1, 0, 2, 3},   /* Grp 5 */
    {0, 0, 0, 0, 1, 2, 0, 3},   /* Grp 6 */
    {0, 0, 0, 0, 1, 2, 3, 0}    /* Grp 7 */
};

/** \} */

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/

IFX_INLINE IfxVadc_GroupId IfxVadc_Adc_getMasterId(IfxVadc_GroupId slave, IfxVadc_Adc_SYNCTR_STSEL masterIndex)
{
    uint8           i, idxOffset;
    IfxVadc_GroupId masterId = slave;

    if (masterIndex == 0)
    {
        masterId = slave;
    }
    else
    {
        idxOffset = (slave < 4) ? 0 : 4;    /* 4 is the index in the above IfxVadc_Adc_masterIndex about which symmetry is observed */

        for (i = 0; i < 4; i++)
        {
            if (IfxVadc_Adc_masterIndex[slave][i + idxOffset] == masterIndex)
            {
                return i + idxOffset;
            }
        }
    }

    return masterId;
}


IFX_INLINE IfxVadc_Adc_SYNCTR_STSEL IfxVadc_Adc_getMasterKernelIndex(IfxVadc_GroupId slave, IfxVadc_GroupId master)
{
    return IfxVadc_Adc_masterIndex[slave][master];
}


/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void IfxVadc_Adc_deInitGroup(IfxVadc_Adc_Group *group)
{
    Ifx_VADC       *vadc       = IfxVadc_Adc_getVadcFromGroup(group);
    Ifx_VADC_G     *vadcG      = IfxVadc_Adc_getGroupRegsFromGroup(group);
    /* Get group index */
    IfxVadc_GroupId groupIndex = group->groupId;

    /* Request Access to configuration registers */
    IfxVadc_enableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);
    IfxVadc_resetGroup(vadcG);
    IfxVadc_disableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);
}


void IfxVadc_Adc_disableModule(Ifx_VADC *vadc)
{
    IfxVadc_disableModule(vadc);
}


void IfxVadc_Adc_getChannelConfig(IfxVadc_Adc_Channel *channel, IfxVadc_Adc_ChannelConfig *config)
{
    Ifx_VADC_G       *vadcG        = IfxVadc_Adc_getGroupRegsFromGroup(channel->group);
    IfxVadc_ChannelId channelIndex = channel->channel;

    config->channelId = channel->channel;
    config->group     = channel->group;

    Ifx_VADC_CHCTR tempChctr;
    tempChctr.U                 = 0;
    tempChctr                   = IfxVadc_getChannelControlConfig(vadcG, channelIndex);

    config->inputClass          = tempChctr.B.ICLSEL;
    config->reference           = tempChctr.B.REFSEL;
    config->resultRegister      = tempChctr.B.RESREG;
    config->globalResultUsage   = tempChctr.B.RESTBS;
    config->lowerBoundary       = tempChctr.B.BNDSELL;
    config->upperBoundary       = tempChctr.B.BNDSELU;
    config->boundaryMode        = tempChctr.B.BNDSELX;
    config->limitCheck          = tempChctr.B.CHEVMODE;
    config->synchonize          = tempChctr.B.SYNC;
    config->rightAlignedStorage = tempChctr.B.RESPOS;

    config->backgroundChannel   = ((IfxVadc_getAssignedChannels(vadcG)).U & (1 << channelIndex)) ? FALSE : TRUE;
    uint32                 channelServiceRequestNodePtr;
    /* Get Channel index */
    IfxVadc_GroupId        groupIndex = channel->group->groupId;
    channelServiceRequestNodePtr = ((IfxVadc_getChannelServiceRequestNodePointer0(vadcG)).U >> (channel->channel * 4)) & 0xF;
    volatile Ifx_SRC_SRCR *src        = IfxVadc_getSrcAddress(groupIndex, channelServiceRequestNodePtr);

    if (src->B.SRE == 1)
    {
        config->channelSrcNr        = channelServiceRequestNodePtr;
        config->channelPriority     = src->B.SRPN;
        config->channelServProvider = src->B.TOS;
    }
    else
    {
        config->channelSrcNr        = 0;
        config->channelPriority     = 0;
        config->channelServProvider = 0;
    }

    uint32 resultServiceRequestNodePtr;

    if (config->resultRegister < IfxVadc_ChannelResult_8)
    {
        resultServiceRequestNodePtr = ((IfxVadc_getChannelResultServiceRequestNodePointer0(vadcG)).U >> (channel->channel * 4)) & 0xF;
    }
    else
    {
        resultServiceRequestNodePtr = ((IfxVadc_getChannelResultServiceRequestNodePointer1(vadcG)).U >> ((channel->channel - IfxVadc_ChannelResult_8) * 4)) & 0xF;
    }

    src = IfxVadc_getSrcAddress(groupIndex, resultServiceRequestNodePtr);

    if (src->B.SRE == 1)
    {
        config->resultSrcNr        = resultServiceRequestNodePtr;
        config->resultPriority     = src->B.SRPN;
        config->resultServProvider = src->B.TOS;
    }
    else
    {
        config->resultSrcNr        = 0;
        config->resultPriority     = 0;
        config->resultServProvider = 0;
    }
}


float32 IfxVadc_Adc_getChannelConversionTime(IfxVadc_Adc_Channel *channel, IfxVadc_ConversionType conversionMode)
{
    return IfxVadc_getChannelConversionTime(channel->group->module.vadc, channel->group->groupId, IfxVadc_getChannelInputClass(channel->group->group, channel->channel), IfxVadc_getAdcAnalogFrequency(channel->group->module.vadc), IfxVadc_getAdcModuleFrequency(), conversionMode);
}


void IfxVadc_Adc_getGroupConfig(IfxVadc_Adc_Group *group, IfxVadc_Adc_GroupConfig *config)
{
    uint8       inputClassNum;
    Ifx_VADC_G *vadcG           = group->group;
    Ifx_VADC   *vadc            = group->module.vadc;
    float32     analogFrequency = IfxVadc_getAdcAnalogFrequency(vadc);

    config->groupId                    = group->groupId;
    config->module                     = &group->module;

    config->arbiter.arbiterRoundLength = IfxVadc_getArbiterRoundLength(vadcG);

    for (inputClassNum = 0; inputClassNum < IFXVADC_NUM_INPUTCLASSES; inputClassNum++)
    {
        config->inputClass[inputClassNum].resolution = IfxVadc_getGroupResolution(vadcG, inputClassNum);
        config->inputClass[inputClassNum].sampleTime = IfxVadc_getGroupSampleTime(vadcG, inputClassNum, analogFrequency);
    }

    if (IfxVadc_isRequestScanSlotEnabled(vadcG) == TRUE)
    {
        config->arbiter.requestSlotScanEnabled          = TRUE;
        config->scanRequest.requestSlotPrio             = IfxVadc_getScanSlotPriority(vadcG);
        config->scanRequest.requestSlotStartMode        = IfxVadc_getScanSlotStartMode(vadcG);

        config->scanRequest.triggerConfig.triggerSource = IfxVadc_getScanSlotTriggerInput(vadcG);
        config->scanRequest.triggerConfig.triggerMode   = IfxVadc_getScanSlotTriggerMode(vadcG);

        if (config->scanRequest.triggerConfig.triggerSource != IfxVadc_TriggerSource_15)
        {
            config->scanRequest.triggerConfig.gatingMode = IfxVadc_getScanSlotGatingMode(vadcG);
        }
        else if (config->scanRequest.triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            config->scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
        }

        config->scanRequest.triggerConfig.gatingSource = IfxVadc_getScanSlotGatingSource(vadcG);
        config->scanRequest.autoscanEnabled            = IfxVadc_isAutoScanEnabled(vadcG);
    }
    else
    {
        config->scanRequest.autoscanEnabled             = FALSE;
        config->scanRequest.triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled;
        config->scanRequest.triggerConfig.gatingSource  = IfxVadc_GatingSource_0;    /* Use CCU6061 TRIG0 */
        config->scanRequest.triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger;
        config->scanRequest.triggerConfig.triggerSource = IfxVadc_TriggerSource_0;   /* Trigger source taken from Gating Input */
    }

    if (IfxVadc_isRequestQueueSlotEnabled(vadcG) == TRUE)
    {
        config->arbiter.requestSlotQueueEnabled          = TRUE;
        config->queueRequest.requestSlotPrio             = IfxVadc_getQueueSlotPriority(vadcG);
        config->queueRequest.requestSlotStartMode        = IfxVadc_getQueueSlotStartMode(vadcG);

        config->queueRequest.triggerConfig.triggerSource = IfxVadc_getQueueSlotTriggerInput(vadcG);
        config->queueRequest.triggerConfig.triggerMode   = IfxVadc_getQueueSlotTriggerMode(vadcG);

        if (config->queueRequest.triggerConfig.triggerSource != IfxVadc_TriggerSource_15)
        {
            config->queueRequest.triggerConfig.gatingMode = IfxVadc_getQueueSlotGatingMode(vadcG);
        }
        else if (config->queueRequest.triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            config->queueRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
        }

        config->queueRequest.triggerConfig.gatingSource = IfxVadc_getQueueSlotGatingSource(vadcG);
        config->queueRequest.flushQueueAfterInit        = FALSE;
    }
    else
    {
        config->queueRequest.flushQueueAfterInit         = FALSE;
        config->queueRequest.triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled;
        config->queueRequest.triggerConfig.gatingSource  = IfxVadc_GatingSource_0;        /* Use CCU6061 TRIG0 */
        config->queueRequest.triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger;
        config->queueRequest.triggerConfig.triggerSource = IfxVadc_TriggerSource_0;       /* Trigger source taken from Gating Input */
    }

    if (IfxVadc_isRequestBackgroundScanSlotEnabled(vadcG) == TRUE)
    {
        config->arbiter.requestSlotBackgroundScanEnabled          = TRUE;
        config->backgroundScanRequest.requestSlotPrio             = IfxVadc_getBackgroundScanSlotPriority(vadcG);
        config->backgroundScanRequest.requestSlotStartMode        = IfxVadc_getBackgroundScanSlotStartMode(vadcG);

        config->backgroundScanRequest.triggerConfig.triggerSource = IfxVadc_getBackgroundScanTriggerInput(vadc);
        config->backgroundScanRequest.triggerConfig.triggerMode   = IfxVadc_getBackgroundScanTriggerMode(vadc);

        if (config->backgroundScanRequest.triggerConfig.triggerSource != IfxVadc_TriggerSource_15)
        {
            config->backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_getBackgroundScanGatingMode(vadc);
        }
        else if (config->backgroundScanRequest.triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            config->backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
        }

        config->backgroundScanRequest.triggerConfig.gatingSource = IfxVadc_getBackgroundScanGatingSource(vadc);
        config->backgroundScanRequest.autoBackgroundScanEnabled  = IfxVadc_isAutoBackgroundScanEnabled(vadc);
    }
    else
    {
        config->backgroundScanRequest.autoBackgroundScanEnabled   = FALSE;
        config->backgroundScanRequest.triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled;
        config->backgroundScanRequest.triggerConfig.gatingSource  = IfxVadc_GatingSource_0;  /* Use CCU6061 TRIG0 */
        config->backgroundScanRequest.triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger;
        config->backgroundScanRequest.triggerConfig.triggerSource = IfxVadc_TriggerSource_0; /* Trigger source taken from Gating Input */
    }

    config->master                 = IfxVadc_Adc_getMasterId(group->groupId, IfxVadc_getMasterIndex(vadcG));

    config->disablePostCalibration = ((IfxVadc_getGlobalConfigValue(vadc)).U >> (IFX_VADC_GLOBCFG_DPCAL0_OFF + group->groupId)) & 0x1;
}


IfxVadc_Status IfxVadc_Adc_initChannel(IfxVadc_Adc_Channel *channel, const IfxVadc_Adc_ChannelConfig *config)
{
    IfxVadc_Status    Status = IfxVadc_Status_noError;
    Ifx_VADC         *vadc   = IfxVadc_Adc_getVadcFromGroup(config->group);
    Ifx_VADC_G       *vadcG  = IfxVadc_Adc_getGroupRegsFromGroup(config->group);

    channel->group = config->group;
    IfxVadc_GroupId   groupIndex   = channel->group->groupId;
    IfxVadc_ChannelId channelIndex = config->channelId;

    /* Request Access to configuration registers */
    IfxVadc_enableAccess(vadc, IfxVadc_Protection_channelControl0 + groupIndex);

    /* Configure Channel */
    {
        IfxVadc_setReferenceInput(vadcG, channelIndex, config->reference);
        IfxVadc_storeGroupResult(vadcG, channelIndex, config->resultRegister);
        IfxVadc_setLowerBoundary(vadcG, channelIndex, config->lowerBoundary);
        IfxVadc_setUpperBoundary(vadcG, channelIndex, config->lowerBoundary);
        IfxVadc_setSyncRequest(vadcG, channelIndex, config->synchonize);
        IfxVadc_setChannelInputClass(vadcG, channelIndex, config->inputClass);
        IfxVadc_setChannelLimitCheckMode(vadcG, channelIndex, config->limitCheck);
        IfxVadc_setResultPosition(vadcG, channelIndex, config->rightAlignedStorage);
        IfxVadc_setBackgroundResultTarget(vadcG, channelIndex, config->globalResultUsage);
        IfxVadc_setBoundaryMode(vadcG, channelIndex, config->boundaryMode);
    }

    IfxVadc_enableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);

    if (config->backgroundChannel == FALSE)
    {
        IfxVadc_setGroupPriorityChannel(vadcG, channelIndex);
    }
    else
    {
        IfxVadc_setBackgroundPriorityChannel(vadcG, channelIndex);
    }

    IfxVadc_disableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);

    if (config->channelPriority > 0)
    {
        volatile Ifx_SRC_SRCR *src = IfxVadc_getSrcAddress(groupIndex, config->channelSrcNr);

        IfxVadc_setChannelEventNodePointer0(vadcG, config->channelSrcNr, channel->channel);
        IfxVadc_clearChannelRequest(vadcG, config->channelId);
        IfxSrc_init(src, config->channelServProvider, config->channelPriority);
        IfxSrc_enable(src);
    }
    else
    {
        /* do nothing */
        /* FIXME disable the interrupt generation? */
    }

    if (config->resultPriority > 0)
    {
        volatile Ifx_SRC_SRCR *src = IfxVadc_getSrcAddress(groupIndex, config->resultSrcNr);
        IfxVadc_enableFifoMode(vadcG, config->resultRegister);

        if (config->resultRegister < IfxVadc_ChannelResult_8)
        {
            IfxVadc_setResultNodeEventPointer0(vadcG, config->resultSrcNr, config->resultRegister);
        }
        else
        {
            IfxVadc_setResultNodeEventPointer1(vadcG, config->resultSrcNr, config->resultRegister);
        }

        IfxVadc_clearAllResultRequests(vadcG);
        IfxSrc_init(src, config->resultServProvider, config->resultPriority);
        IfxSrc_enable(src);
    }
    else
    {
        /* do nothing */
        /* FIXME disable the interrupt generation? */
    }

    IfxVadc_disableAccess(vadc, IfxVadc_Protection_channelControl0 + groupIndex);
    /* Software initialization */
    channel->resultreg = config->resultRegister;
    channel->channel   = config->channelId;

    return Status;
}


void IfxVadc_Adc_initChannelConfig(IfxVadc_Adc_ChannelConfig *config, const IfxVadc_Adc_Group *group)
{
    static const IfxVadc_Adc_ChannelConfig IfxVadc_Adc_defaultChannelConfig = {
        .channelId           = IfxVadc_ChannelId_0,
        .group               = NULL_PTR,
        .inputClass          = IfxVadc_InputClasses_group0,
        .reference           = IfxVadc_ChannelReference_standard,
        .resultRegister      = IfxVadc_ChannelResult_0,
        .globalResultUsage   = FALSE,
        .lowerBoundary       = IfxVadc_BoundarySelection_group0,
        .upperBoundary       = IfxVadc_BoundarySelection_group0,
        .boundaryMode        = IfxVadc_BoundaryExtension_standard,
        .limitCheck          = IfxVadc_LimitCheck_noCheck,
        .synchonize          = FALSE,
        .backgroundChannel   = FALSE,
        .rightAlignedStorage = FALSE,
        .resultPriority      = 0,
        .resultSrcNr         = IfxVadc_SrcNr_group0,
        .resultServProvider  = IfxSrc_Tos_cpu0,
        .channelPriority     = 0,
        .channelSrcNr        = IfxVadc_SrcNr_group0,
        .channelServProvider = IfxSrc_Tos_cpu0
    };
    *config       = IfxVadc_Adc_defaultChannelConfig;
    config->group = group;
}


IfxVadc_Status IfxVadc_Adc_initGroup(IfxVadc_Adc_Group *group, const IfxVadc_Adc_GroupConfig *config)
{
    IfxVadc_Status status = IfxVadc_Status_noError;
    Ifx_VADC      *vadc   = config->module->vadc;
    Ifx_VADC_G    *vadcG  = &vadc->G[config->groupId];

    /* check for write access */
    group->group   = vadcG;
    group->module  = *config->module;
    IfxVadc_GroupId groupIndex      = config->groupId;
    group->groupId = groupIndex;
    float32         analogFrequency = IfxVadc_getAdcAnalogFrequency(vadc);

    uint8           inputClassNum;

    /* Request Access to configuration registers */
    IfxVadc_enableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);

    if (config->arbiter.requestSlotQueueEnabled == TRUE)
    {
        /* Enable Arbiter slot, set Priority and start mode */
        IfxVadc_setArbiterPriority(vadcG, config->arbiter.requestSlotQueueEnabled, config->queueRequest.requestSlotPrio, config->queueRequest.requestSlotStartMode, IfxVadc_RequestSource_queue);
    }
    else
    {
        /* Disable the slot */
        IfxVadc_setArbiterPriority(vadcG, FALSE, IfxVadc_RequestSlotPriority_lowest, IfxVadc_RequestSlotStartMode_waitForStart, IfxVadc_RequestSource_queue);
    }

    if (config->arbiter.requestSlotScanEnabled == TRUE)
    {
        /* Setup Arbitration priority and turn on enabled slot */
        /* Enable Arbiter slot, set Priority and start mode */
        IfxVadc_setArbiterPriority(vadcG, config->arbiter.requestSlotScanEnabled, config->scanRequest.requestSlotPrio, config->scanRequest.requestSlotStartMode, IfxVadc_RequestSource_scan);
    }
    else
    {
        /* Disable the slot */
        IfxVadc_setArbiterPriority(vadcG, FALSE, IfxVadc_RequestSlotPriority_lowest, IfxVadc_RequestSlotStartMode_waitForStart, IfxVadc_RequestSource_scan);
    }

    if (config->arbiter.requestSlotBackgroundScanEnabled == TRUE)
    {
        /* Setup Arbitration priority and turn on enabled slot */
        /* Enable Arbiter slot, set Priority and start mode */
        IfxVadc_setArbiterPriority(vadcG, config->arbiter.requestSlotBackgroundScanEnabled, config->backgroundScanRequest.requestSlotPrio, config->backgroundScanRequest.requestSlotStartMode, IfxVadc_RequestSource_background);
    }
    else
    {
        /* Disable the slot */
        IfxVadc_setArbiterPriority(vadcG, FALSE, IfxVadc_RequestSlotPriority_lowest, IfxVadc_RequestSlotStartMode_waitForStart, IfxVadc_RequestSource_background);
    }

    /* master slave configuration */
    if (config->master != groupIndex)
    {
        uint8 masterIndex = IfxVadc_Adc_getMasterKernelIndex(groupIndex, config->master);
        IfxVadc_setMasterIndex(vadcG, masterIndex);
    }
    else
    {
        /* do nothing */
        /* FIXME check if something needs to be done? */
    }

    /* Setup arbiter */
    /* turn off the group during initialization, see UM for sync mode */
    IfxVadc_setAnalogConvertControl(vadcG, IfxVadc_AnalogConverterMode_off);

    IfxVadc_setArbitrationRoundLength(vadcG, config->arbiter.arbiterRoundLength);

    /* Setup queue request if enabled */
    if (config->arbiter.requestSlotQueueEnabled == TRUE)
    {
        const IfxVadc_Adc_QueueConfig *queueSlot = &config->queueRequest;

        /* configure external Trigger if enabled */
        if (queueSlot->triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            /* enable external trigger */
            IfxVadc_enableQueueSlotExternalTrigger(vadcG);
            IfxVadc_setQueueSlotTriggerOperatingConfig(vadcG, queueSlot->triggerConfig.triggerMode, queueSlot->triggerConfig.triggerSource);

            /* if last input is used the trigger input selection is extend by Gating inputs */
            if (queueSlot->triggerConfig.triggerSource == IfxVadc_TriggerSource_15)
            {
                IfxVadc_setQueueSlotGatingConfig(vadcG, queueSlot->triggerConfig.gatingSource, IfxVadc_GatingMode_always);
            }
            else
            {
                /* do nothing, gating is configured later */
            }
        }
        else
        {
            /* disable external trigger */
            IfxVadc_disableQueueSlotExternalTrigger(vadcG);
        }

        /* configure Gating if enabled */
        if ((queueSlot->triggerConfig.triggerSource != IfxVadc_TriggerSource_15))
        {
            IfxVadc_setQueueSlotGatingConfig(vadcG, queueSlot->triggerConfig.gatingSource, queueSlot->triggerConfig.gatingMode);
        }
        else
        {
            /* do nothing, handled by trigger settings */
        }

        IfxVadc_clearQueue(vadcG, (queueSlot->flushQueueAfterInit) ? 1 : 0);
    }
    else
    {
        /* do nothing */
    }

    /* Setup scan request if enabled */
    if (config->arbiter.requestSlotScanEnabled == TRUE)
    {
        const IfxVadc_Adc_ScanConfig *scanSlot = &config->scanRequest;

        /* configure external Trigger if enabled */
        if (scanSlot->triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            /* enable external trigger */
            IfxVadc_enableScanSlotExternalTrigger(vadcG);

            IfxVadc_setScanSlotTriggerConfig(vadcG, scanSlot->triggerConfig.triggerMode, scanSlot->triggerConfig.triggerSource);

            if (scanSlot->triggerConfig.triggerSource == IfxVadc_TriggerSource_15)    /* if last input is used the trigger input selection is extend by Gating inputs */
            {
                IfxVadc_setScanSlotGatingConfig(vadcG, scanSlot->triggerConfig.gatingSource, IfxVadc_GatingMode_always);
            }
            else
            {
                /* do nothing, gating is configured later */
            }
        }
        else
        {
            IfxVadc_disableScanSlotExternalTrigger(vadcG);
        }

        /* configure Gating if enabled */
        if ((scanSlot->triggerConfig.triggerSource != IfxVadc_TriggerSource_15))
        {
            IfxVadc_setScanSlotGatingConfig(vadcG, scanSlot->triggerConfig.gatingSource, scanSlot->triggerConfig.gatingMode);
        }
        else
        {
            /* do nothing, handled by trigger settings */
        }

        IfxVadc_setAutoScan(vadcG, scanSlot->autoscanEnabled ? 1 : 0);
    }
    else
    {
        /* do nothing */
    }

    if (config->arbiter.requestSlotBackgroundScanEnabled == TRUE)
    {
        const IfxVadc_Adc_BackgroundScanConfig *backgroundScanSlot = &config->backgroundScanRequest;

        /* configure external Trigger if enabled */
        if (backgroundScanSlot->triggerConfig.triggerMode != IfxVadc_TriggerMode_noExternalTrigger)
        {
            IfxVadc_enableBackgroundScanSlotExternalTrigger(vadc);

            IfxVadc_setBackgroundScanSlotTriggerConfig(vadc, backgroundScanSlot->triggerConfig.triggerMode, backgroundScanSlot->triggerConfig.triggerSource);

            if (backgroundScanSlot->triggerConfig.triggerSource == IfxVadc_TriggerSource_15)    /* if last input is used the trigger input selection is extend by Gating inputs */
            {
                IfxVadc_setBackgroundScanSlotGatingConfig(vadc, backgroundScanSlot->triggerConfig.gatingSource, IfxVadc_GatingMode_always);
            }
            else
            {
                /* do nothing, gating is configured later */
            }
        }
        else
        {
            /* do nothing */
        }

        /* configure Gating if enabled */
        if ((backgroundScanSlot->triggerConfig.triggerSource != IfxVadc_TriggerSource_15))
        {
            IfxVadc_setBackgroundScanSlotGatingConfig(vadc, backgroundScanSlot->triggerConfig.gatingSource, backgroundScanSlot->triggerConfig.gatingMode);
        }
        else
        {
            /* do nothing, handled by trigger settings */
        }

        IfxVadc_setAutoBackgroundScan(vadc, backgroundScanSlot->autoBackgroundScanEnabled ? 1 : 0);
    }
    else
    {
        /* do nothing */
    }

    /* turn on group after initialisation, only in master mode */
    IfxVadc_AnalogConverterMode convertMode = (config->master == groupIndex) ? IfxVadc_AnalogConverterMode_normalOperation : IfxVadc_AnalogConverterMode_off;
    IfxVadc_setAnalogConvertControl(vadcG, convertMode);

    /*  Post Calibration */
    IfxVadc_disablePostCalibration(vadc, groupIndex, config->disablePostCalibration);

    for (inputClassNum = 0; inputClassNum < IFXVADC_NUM_INPUTCLASSES; inputClassNum++)
    {
        /* configure Group input class registers */
        IfxVadc_setGroupResolution(vadcG, inputClassNum, config->inputClass[inputClassNum].resolution);
        /* Calculate Sample time ticks */
        IfxVadc_setGroupSampleTime(vadcG, inputClassNum, analogFrequency, config->inputClass[inputClassNum].sampleTime);
    }

    IfxVadc_disableAccess(vadc, IfxVadc_Protection_initGroup0 + groupIndex);

    return status;
}


void IfxVadc_Adc_initGroupConfig(IfxVadc_Adc_GroupConfig *config, IfxVadc_Adc *vadc)
{
    static const IfxVadc_Adc_GroupConfig IfxVadc_Adc_defaultGroupConfig = {
        .arbiter                              = {
            .arbiterRoundLength               = IfxVadc_ArbitrationRounds_4_slots,
            .requestSlotQueueEnabled          = FALSE,
            .requestSlotScanEnabled           = FALSE,
            .requestSlotBackgroundScanEnabled = FALSE,
        },
        .backgroundScanRequest                = {
            .autoBackgroundScanEnabled   = FALSE,
            .triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled,
            .triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger,
            .triggerConfig.gatingSource  = IfxVadc_GatingSource_0,
            .triggerConfig.triggerSource = IfxVadc_TriggerSource_0,
            .requestSlotPrio             = IfxVadc_RequestSlotPriority_low,
            .requestSlotStartMode        = IfxVadc_RequestSlotStartMode_waitForStart,
        },
        .scanRequest                          = {
            .autoscanEnabled             = FALSE,
            .triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled,
            .triggerConfig.gatingSource  = IfxVadc_GatingSource_0,
            .triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger,
            .triggerConfig.triggerSource = IfxVadc_TriggerSource_0,
            .requestSlotPrio             = IfxVadc_RequestSlotPriority_low,
            .requestSlotStartMode        = IfxVadc_RequestSlotStartMode_waitForStart,
        },
        .queueRequest                         = {
            .flushQueueAfterInit         = TRUE,
            .triggerConfig.gatingMode    = IfxVadc_GatingMode_disabled,
            .triggerConfig.gatingSource  = IfxVadc_GatingSource_0,
            .triggerConfig.triggerMode   = IfxVadc_TriggerMode_noExternalTrigger,
            .triggerConfig.triggerSource = IfxVadc_TriggerSource_0,
            .requestSlotPrio             = IfxVadc_RequestSlotPriority_low,
            .requestSlotStartMode        = IfxVadc_RequestSlotStartMode_waitForStart,
        },

        .inputClass[0].resolution = IfxVadc_ChannelResolution_12bit,
        .inputClass[0].sampleTime = 1.0e-6,                           /* Set sample time to 1us */
        .inputClass[1].resolution = IfxVadc_ChannelResolution_12bit,
        .inputClass[1].sampleTime = 1.0e-6,                           /* Set sample time to 1us */
    };

    *config                        = IfxVadc_Adc_defaultGroupConfig;
    config->groupId                = IfxVadc_GroupId_0;
    config->module                 = vadc;
    config->master                 = config->groupId;
    config->disablePostCalibration = FALSE;
}


IfxVadc_Status IfxVadc_Adc_initModule(IfxVadc_Adc *vadc, const IfxVadc_Adc_Config *config)
{
    IfxVadc_Status status  = IfxVadc_Status_noError;
    Ifx_VADC      *vadcSFR = config->vadc;
    vadc->vadc = vadcSFR;
    float32        analogFrequency;
    uint8          inputClassNum, groupNum;

    /* Enable VADC kernel clock */
    IfxVadc_enableModule(vadcSFR);
    IfxVadc_selectPowerSupplyVoltage(vadcSFR, config->supplyVoltage);

    /* Set Analog Frequency */
    if (IfxVadc_initializeFAdcI(vadcSFR, config->analogFrequency) == 0)
    {
        return IfxVadc_Status_notInitialised;
    }
    else
    {
        /* do nothing */
    }

    /* Set digital Frequency */
    IfxVadc_initializeFAdcD(vadcSFR, config->digitalFrequency);

    analogFrequency = IfxVadc_getAdcAnalogFrequency(vadcSFR);

    /* configure Global input class registers */
    for (inputClassNum = 0; inputClassNum < IFXVADC_NUM_GLOBAL_INPUTCLASSES; inputClassNum++)
    {
        /* configure ADC channel resolution ( conversion mode ) */
        IfxVadc_setGlobalResolution(vadcSFR, inputClassNum, config->globalInputClass[inputClassNum].resolution);
        /* configure Sample time ticks */
        IfxVadc_setGlobalSampleTime(vadcSFR, inputClassNum, analogFrequency, config->globalInputClass[inputClassNum].sampleTime);
    }

    /* Start up calibration is requested */
    if (config->startupCalibration == TRUE)
    {
        /* Ensure that all groups are enabled */
        for (groupNum = 0; groupNum < IFXVADC_NUM_ADC_GROUPS; groupNum++)
        {
            IfxVadc_enableAccess(vadcSFR, IfxVadc_Protection_initGroup0 + groupNum);
            IfxVadc_setAnalogConvertControl(&vadcSFR->G[groupNum], IfxVadc_AnalogConverterMode_normalOperation);
            IfxVadc_disableAccess(vadcSFR, IfxVadc_Protection_initGroup0 + groupNum);
        }

        // execute calibration
        IfxVadc_startupCalibration(vadcSFR);
    }

    return status;
}


void IfxVadc_Adc_initModuleConfig(IfxVadc_Adc_Config *config, Ifx_VADC *vadc)
{
    config->vadc                           = vadc;
    config->analogFrequency                = IFXVADC_DEFAULT_ANALOG_FREQ;

    config->digitalFrequency               = IfxVadc_getAdcDigitalFrequency(vadc);
    config->moduleFrequency                = IfxScuCcu_getSpbFrequency();
    config->globalInputClass[0].resolution = IfxVadc_ChannelResolution_12bit;
    config->globalInputClass[0].sampleTime = 1.0e-6;
    config->globalInputClass[1].resolution = IfxVadc_ChannelResolution_12bit;
    config->globalInputClass[1].sampleTime = 1.0e-6;
    config->startupCalibration             = FALSE;
    config->supplyVoltage                  = IfxVadc_LowSupplyVoltageSelect_5V;
}


void IfxVadc_Adc_initExternalMultiplexerModeConfig(IfxVadc_Adc_EmuxControl *emuxConfig, Ifx_VADC *vadc)
{
    emuxConfig->vadc                  = vadc;
    emuxConfig->channels              = 0;
    emuxConfig->groupId               = IfxVadc_GroupId_0;
    emuxConfig->emuxInterface         = IfxVadc_EmuxInterface_0;
    emuxConfig->startChannel          = IfxVadc_EmuxSelectValue_0;
    emuxConfig->code                  = IfxVadc_EmuxCodingScheme_binary;
    emuxConfig->sampleTimeControl     = IfxVadc_EmuxSampleTimeControl_settingChanges;
    emuxConfig->mode                  = IfxVadc_ExternalMultiplexerMode_softwareControl;
    emuxConfig->channelSelectionStyle = IfxVadc_ChannelSelectionStyle_channelNumber;
    IfxVadc_Adc_EmuxPinConfig defaultConfig = {
        .pins       = {NULL_PTR},
        .outputMode = IfxPort_OutputMode_pushPull,
        .padDriver  = 0,
    };

    emuxConfig->emuxOutPinConfig = defaultConfig;
}


void IfxVadc_Adc_initExternalMultiplexerMode(Ifx_VADC *vadc, const IfxVadc_Adc_EmuxControl *emuxControl)
{
    uint8       count = 0;
    Ifx_VADC_G *vadcG = &vadc->G[emuxControl->groupId];
    IfxVadc_setEmuxInterfaceForGroup(vadc, emuxControl->emuxInterface, emuxControl->groupId);

    for (count = 0; count < 3; count++)
    {
        if (emuxControl->emuxOutPinConfig.pins[count] != NULL_PTR)
        {
            IfxVadc_initEmuxPin(emuxControl->emuxOutPinConfig.pins[count], emuxControl->emuxOutPinConfig.outputMode, emuxControl->emuxOutPinConfig.padDriver);
        }
    }

    IfxVadc_configExternalMultiplexerMode(vadc, vadcG, emuxControl->mode, emuxControl->channels, emuxControl->startChannel, emuxControl->code, emuxControl->sampleTimeControl, emuxControl->channelSelectionStyle);
}
