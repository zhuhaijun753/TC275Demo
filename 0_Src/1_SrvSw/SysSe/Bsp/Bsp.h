/**
 * \file Bsp.h
 * \brief Board support package
 * \ingroup library_srvsw_sysse_bsp_bsp
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
 * \defgroup library_srvsw_sysse_bsp_bsp Board support package
 * This module implements the board support package
 * \ingroup library_srvsw_sysse_bsp
 *
 */

#ifndef BSP_H
#define BSP_H 1

#include "Ifx_Cfg.h"                   /* Do not remove this include */
#include "Cpu/Std/IfxCpu_Intrinsics.h" /* Do not remove this include */
#include "_Utilities/Ifx_Assert.h"     /* Do not remove this include */

#include "Cpu/Std/IfxCpu.h"
#include "Port/Std/IfxPort.h"
#include "Src/Std/IfxSrc.h"
#include "Stm/Std/IfxStm.h"
#include "Scu/Std/IfxScuCcu.h"

#ifndef BSP_DEFAULT_TIMER
/** Defined the default timer used */
#define BSP_DEFAULT_TIMER (&MODULE_STM0)
#endif

/******************************************************************************/
/*                           Function prototypes                              */
/******************************************************************************/

/** \addtogroup library_srvsw_sysse_bsp_bsp
 * \{ */
/** \name Interrupt APIs
 * \{ */
IFX_INLINE boolean areInterruptsEnabled(void);
IFX_INLINE boolean disableInterrupts(void);
IFX_INLINE void    enableInterrupts(void);
IFX_INLINE void    restoreInterrupts(boolean enabled);
IFX_INLINE void    forceDisableInterrupts(void);
/** \} */
/** \} */

/******************************************************************************/
/*                           Functions                                        */
/******************************************************************************/

/** \brief Return the status of the global interrupts
 *
 * \retval TRUE if the global interrupts are enabled.
 * \retval FALSE if the global interrupts are disabled.
 *
 * \see restoreInterrupts(), disableInterrupts()
 */
IFX_INLINE boolean areInterruptsEnabled(void)
{
    return IfxCpu_areInterruptsEnabled();
}


/** \brief Disable the global interrupts
 *
 * \retval TRUE if the global interrupts were enabled before the call to the function.
 * \retval FALSE if the global interrupts are disabled before the call to the function.
 *
 * \see areInterruptsEnabled(), restoreInterrupts()
 */
IFX_INLINE boolean disableInterrupts(void)
{
    return IfxCpu_disableInterrupts();
}


/** \brief Disable the global interrupts forcefully
 *
 *
 * \see areInterruptsEnabled(), restoreInterrupts()
 */
IFX_INLINE void forceDisableInterrupts(void)
{
    IfxCpu_forceDisableInterrupts();
}


/** \brief enable the global interrupts
 *
 */
IFX_INLINE void enableInterrupts(void)
{
    IfxCpu_enableInterrupts();
}


/** \brief Restore the state of the global interrupts.
 *
 * \param enabled if TRUE, re-enable the global interrupts, else do nothing.
 *
 * \return None.
 *
 * \see areInterruptsEnabled(), disableInterrupts()
 */
IFX_INLINE void restoreInterrupts(boolean enabled)
{
    IfxCpu_restoreInterrupts(enabled);
}


/******************************************************************************/
/*                           Macros                                           */
/******************************************************************************/
#define TIMER_COUNT       (11)                                      /**< \internal \brief number of timer values defined */
#define TIMER_INDEX_10NS  (0)                                       /**< \internal \brief Index of the time value 10ns*/
#define TIMER_INDEX_100NS (1)                                       /**< \internal \brief Index of the time value 100ns*/
#define TIMER_INDEX_1US   (2)                                       /**< \internal \brief Index of the time value 1us*/
#define TIMER_INDEX_10US  (3)                                       /**< \internal \brief Index of the time value 10us*/
#define TIMER_INDEX_100US (4)                                       /**< \internal \brief Index of the time value 100us*/
#define TIMER_INDEX_1MS   (5)                                       /**< \internal \brief Index of the time value 1ms*/
#define TIMER_INDEX_10MS  (6)                                       /**< \internal \brief Index of the time value 10ms*/
#define TIMER_INDEX_100MS (7)                                       /**< \internal \brief Index of the time value 100ms*/
#define TIMER_INDEX_1S    (8)                                       /**< \internal \brief Index of the time value 1s*/
#define TIMER_INDEX_10S   (9)                                       /**< \internal \brief Index of the time value 10s*/
#define TIMER_INDEX_100S  (10)                                      /**< \internal \brief Index of the time value 100s*/

/** \internal
 *  Array containing the time constants. This variable should not be used in the application. TimeConst_0s, TimeConst_10ns, ...  should be used instead
 *
 */
IFX_EXTERN Ifx_TickTime TimeConst[TIMER_COUNT];

/******************************************************************************/
/*                           Function prototypes                              */
/******************************************************************************/
/** \addtogroup library_srvsw_sysse_bsp_bsp
 * \{ */
/** \name Time APIs
 * \{ */
IFX_INLINE Ifx_TickTime addTTime(Ifx_TickTime a, Ifx_TickTime b);
IFX_INLINE Ifx_TickTime elapsed(Ifx_TickTime since);
IFX_INLINE Ifx_TickTime getDeadLine(Ifx_TickTime timeout);
IFX_INLINE Ifx_TickTime getTimeout(Ifx_TickTime deadline);
IFX_EXTERN void         initTime(void);
IFX_INLINE boolean      isDeadLine(Ifx_TickTime deadLine);
IFX_INLINE Ifx_TickTime now(void);
IFX_INLINE Ifx_TickTime nowWithoutCriticalSection(void);
IFX_INLINE boolean      poll(volatile boolean *test, Ifx_TickTime timeout);
IFX_INLINE Ifx_TickTime timingNoInterruptEnd(Ifx_TickTime since, boolean interruptEnabled);
IFX_INLINE Ifx_TickTime timingNoInterruptStart(boolean *interruptEnabled);
IFX_INLINE void         wait(Ifx_TickTime timeout);
IFX_EXTERN void         waitPoll(void);
IFX_EXTERN void         waitTime(Ifx_TickTime timeout);

/** Prototype for wait() functions */
typedef void (*WaitTimeFunction)(Ifx_TickTime timeout);

#define TimeConst_0s    ((Ifx_TickTime)0)                           /**< \brief time constant equal to 1s */
#define TimeConst_10ns  (TimeConst[TIMER_INDEX_10NS])               /**< \brief time constant equal to 10ns */
#define TimeConst_100ns (TimeConst[TIMER_INDEX_100NS])              /**< \brief time constant equal to 100ns */
#define TimeConst_1us   (TimeConst[TIMER_INDEX_1US])                /**< \brief time constant equal to 1us */
#define TimeConst_10us  (TimeConst[TIMER_INDEX_10US])               /**< \brief time constant equal to 10us */
#define TimeConst_100us (TimeConst[TIMER_INDEX_100US])              /**< \brief time constant equal to 100us */
#define TimeConst_1ms   (TimeConst[TIMER_INDEX_1MS])                /**< \brief time constant equal to 1ms */
#define TimeConst_10ms  (TimeConst[TIMER_INDEX_10MS])               /**< \brief time constant equal to 10ms */
#define TimeConst_100ms (TimeConst[TIMER_INDEX_100MS])              /**< \brief time constant equal to 100ms */
#define TimeConst_1s    (TimeConst[TIMER_INDEX_1S])                 /**< \brief time constant equal to 1s */
#define TimeConst_10s   (TimeConst[TIMER_INDEX_10S])                /**< \brief time constant equal to 10s */
#define TimeConst_100s  (TimeConst[TIMER_INDEX_100S])               /**< \brief time constant equal to 100s */

/**\}*/
/**\}*/
/******************************************************************************/
/*                           Functions                                        */
/******************************************************************************/

/** \brief Return system timer value (critical section).
 *
 * The function IfxStm_get() is called in a critical section, disabling
 * the interrupts. The system timer value is limited to TIME_INFINITE.
 *
 * \return Returns system timer value.
 */
IFX_INLINE Ifx_TickTime now(void)
{
    Ifx_TickTime stmNow;
    boolean      interruptState;

    interruptState = disableInterrupts();
    stmNow         = (Ifx_TickTime)IfxStm_get(BSP_DEFAULT_TIMER) & TIME_INFINITE;
    restoreInterrupts(interruptState);

    return stmNow;
}


/** \brief Return system timer value (without critical section).
 *
 * The function IfxStm_get() is called. The system timer value is limited to TIME_INFINITE.
 *
 * \return Returns system timer value.
 */
IFX_INLINE Ifx_TickTime nowWithoutCriticalSection(void)
{
    Ifx_TickTime stmNow;

    stmNow = (Ifx_TickTime)IfxStm_get(BSP_DEFAULT_TIMER) & TIME_INFINITE;

    return stmNow;
}


/** \brief Add 2 Ifx_TickTime values and return the result
 *
 * \param a parameter a
 * \param b parameter b
 *
 * \return a + b. If either a or b is TIME_INFINITE, the result is TIME_INFINITE
 */
IFX_INLINE Ifx_TickTime addTTime(Ifx_TickTime a, Ifx_TickTime b)
{
    Ifx_TickTime result;

    if ((a == TIME_INFINITE) || (b == TIME_INFINITE))
    {
        result = TIME_INFINITE;
    }
    else
    {
        result = a + b;         /* FIXME check for overflow */
    }

    return result;
}


/** \brief Return the elapsed time in ticks.
 *
 * Return the elapsed time between the current time and the time passed as parameter
 *
 * \return Returns the elapsed time.
 */
IFX_INLINE Ifx_TickTime elapsed(Ifx_TickTime since)
{
    return now() - since;
}


/** \brief Return the time dead line.
 *
 * \param timeout Specifies the dead line from now: Deadline = Now + Timeout
 *
 * \return Return the time dead line.
 */
IFX_INLINE Ifx_TickTime getDeadLine(Ifx_TickTime timeout)
{
    Ifx_TickTime deadLine;

    if (timeout == TIME_INFINITE)
    {
        deadLine = TIME_INFINITE;
    }
    else
    {
        deadLine = now() + timeout;
    }

    return deadLine;
}


/** \brief Return the time until the dead line.
 *
 * \param deadline Specifies the dead line from now: Deadline = Now + Timeout
 *
 * \return Return the time until the dead line.
 */
IFX_INLINE Ifx_TickTime getTimeout(Ifx_TickTime deadline)
{
    Ifx_TickTime timeout;

    if (deadline == TIME_INFINITE)
    {
        timeout = TIME_INFINITE;
    }
    else
    {
        timeout = deadline - now();
    }

    return timeout;
}


/** \brief Return TRUE if the dead line is over.
 *
 * \param deadLine Specifies the dead line.
 *
 * \retval TRUE Returns TRUE if the dead line is over
 * \retval FALSE Returns FALSE if the dead line is not yet over
 */
IFX_INLINE boolean isDeadLine(Ifx_TickTime deadLine)
{
    boolean result;

    if (deadLine == TIME_INFINITE)
    {
        result = FALSE;
    }
    else
    {
        result = now() >= deadLine;
    }

    return result;
}


/** \brief Poll a variable for a time.
 *
 * \param test Specifies the variable to test.
 * \param timeout Specifies the maximal time the variable will be tested
 *
 * \retval TRUE Returns TRUE if the variable gets TRUE before the timeout elapse
 * \retval FALSE Returns FALSE if the variable is FALSE as the timeout elapse
 */
IFX_INLINE boolean poll(volatile boolean *test, Ifx_TickTime timeout)
{
    Ifx_TickTime deadLine = getDeadLine(timeout);

    while ((*test == FALSE) && (isDeadLine(deadLine) == FALSE))
    {}

    return *test;
}


/** \brief Return the elapsed time in system timer ticks, and enable the interrupts.
 *
 * The interrupts are enable by the function
 *
 * \param since time returned by timingNoInterruptStart()
 * \param interruptEnabled If TRUE, the interrupts will be enabled before the function exit. This parameter should be set to the value returned by \ref  timingNoInterruptStart()
 *
 * \return Returns the elapsed time.
 *
 * \see timingNoInterruptStart()
 */
IFX_INLINE Ifx_TickTime timingNoInterruptEnd(Ifx_TickTime since, boolean interruptEnabled)
{
    Ifx_TickTime stmNow;

    stmNow = nowWithoutCriticalSection();
    restoreInterrupts(interruptEnabled);

    return stmNow - since;
}


/** \brief Disable the interrupt and return system timer value.
 *
 * The interrupt remains disabled after the function call
 *
 * \return Returns system timer value.
 *
 * \see timingNoInterruptEnd()
 */
IFX_INLINE Ifx_TickTime timingNoInterruptStart(boolean *interruptEnabled)
{
    *interruptEnabled = disableInterrupts();

    return nowWithoutCriticalSection();
}


/** \brief Wait for a while.
 *
 * \param timeout Specifies the waiting time
 *
 * \return None.
 */
IFX_INLINE void wait(Ifx_TickTime timeout)
{
    Ifx_TickTime deadLine = getDeadLine(timeout);

    while (isDeadLine(deadLine) == FALSE)
    {}
}


#define PIN_DRIVER_STRONG_SHARP IfxPort_PadDriver_cmosAutomotiveSpeed1
#define Pin_setState(pin, mode)                            IfxPort_setPinState((pin)->port, (pin)->pinIndex, (mode))
#define Pin_setGroupState(pin, mask, data)                 IfxPort_setGroupState((pin)->port, (pin)->pinIndex, (mask), (data))
#define Pin_setMode(pin, mode)                             IfxPort_setPinMode((pin)->port, (pin)->pinIndex, (mode))
#define Pin_setDriver(pin, mode)                           IfxPort_setPinPadDriver((pin)->port, (pin)->pinIndex, (mode))
#define Pin_setStateHigh(pin)                              IfxPort_setPinHigh((pin)->port, (pin)->pinIndex)
#define Pin_setStateLow(pin)                               IfxPort_setPinLow((pin)->port, (pin)->pinIndex)
#define Pin_getState(pin)                                  IfxPort_getPinState((pin)->port, (pin)->pinIndex)
#define Pin_setGroupModeOutput(pin, mask, mode, outputIdx) IfxPort_setGroupModeOutput((pin)->port, (pin)->pinIndex, (mask), (mode), (outputIdx))
#define Pin_setGroupModeInput(pin, mask, mode)             IfxPort_setGroupModeInput((pin)->port, (pin)->pinIndex, (mask), (mode))
#define Pin_setGroupState(pin, mask, data)                 IfxPort_setGroupState((pin)->port, (pin)->pinIndex, (mask), (data))
#define Pin_getGroupState(pin, mask)                       IfxPort_getGroupState((pin)->port, (pin)->pinIndex, (mask))
#define Pin_enableEmgStop(pin)                             IfxPort_enableEmergencyStop((pin)->port, (pin)->pinIndex)
//------------------------------------------------------------------------------

#endif /* BSP_H */
