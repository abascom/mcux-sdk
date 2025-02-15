/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_SAI_EDMA_H_
#define _FSL_SAI_EDMA_H_

#include "fsl_edma.h"
#include "fsl_sai.h"

/*!
 * @addtogroup sai_edma SAI EDMA Driver
 * @ingroup sai
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SAI_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 4, 0)) /*!< Version 2.4.0 */
/*@}*/

typedef struct sai_edma_handle sai_edma_handle_t;

/*! @brief SAI eDMA transfer callback function for finish and error */
typedef void (*sai_edma_callback_t)(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

/*! @brief SAI DMA transfer handle, users should not touch the content of the handle.*/
struct sai_edma_handle
{
    edma_handle_t *dmaHandle;     /*!< DMA handler for SAI send */
    uint8_t nbytes;               /*!< eDMA minor byte transfer count initially configured. */
    uint8_t bytesPerFrame;        /*!< Bytes in a frame */
    uint8_t channelMask;          /*!< Enabled channel mask value, reference _sai_channel_mask */
    uint8_t channel;              /*!< Which data channel */
    uint8_t count;                /*!< The transfer data count in a DMA request */
    uint32_t state;               /*!< Internal state for SAI eDMA transfer */
    sai_edma_callback_t callback; /*!< Callback for users while transfer finish or error occurs */
    void *userData;               /*!< User callback parameter */
    uint8_t tcd[(SAI_XFER_QUEUE_SIZE + 1U) * sizeof(edma_tcd_t)]; /*!< TCD pool for eDMA transfer. */
    sai_transfer_t saiQueue[SAI_XFER_QUEUE_SIZE];                 /*!< Transfer queue storing queued transfer. */
    size_t transferSize[SAI_XFER_QUEUE_SIZE];                     /*!< Data bytes need to transfer */
    volatile uint8_t queueUser;                                   /*!< Index for user to queue transfer. */
    volatile uint8_t queueDriver; /*!< Index for driver to get the transfer data and size */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name eDMA Transactional
 * @{
 */

/*!
 * @brief Initializes the SAI eDMA handle.
 *
 * This function initializes the SAI master DMA handle, which can be used for other SAI master transactional APIs.
 * Usually, for a specified SAI instance, call this API once to get the initialized handle.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param base SAI peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param txDmaHandle eDMA handle pointer, this handle shall be static allocated by users.
 */
void SAI_TransferTxCreateHandleEDMA(I2S_Type *base,
                                    sai_edma_handle_t *handle,
                                    sai_edma_callback_t callback,
                                    void *userData,
                                    edma_handle_t *txDmaHandle);

/*!
 * @brief Initializes the SAI Rx eDMA handle.
 *
 * This function initializes the SAI slave DMA handle, which can be used for other SAI master transactional APIs.
 * Usually, for a specified SAI instance, call this API once to get the initialized handle.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param base SAI peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param rxDmaHandle eDMA handle pointer, this handle shall be static allocated by users.
 */
void SAI_TransferRxCreateHandleEDMA(I2S_Type *base,
                                    sai_edma_handle_t *handle,
                                    sai_edma_callback_t callback,
                                    void *userData,
                                    edma_handle_t *rxDmaHandle);

/*!
 * @brief Configures the SAI Tx audio format.
 *
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_TransferTxSetConfigEDMA
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred. This function also sets the eDMA parameter according to formatting requirements.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param format Pointer to SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If bit clock source is master
 * clock, this value should equals to masterClockHz in format.
 * @retval kStatus_Success Audio format set successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 */
void SAI_TransferTxSetFormatEDMA(I2S_Type *base,
                                 sai_edma_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz);

/*!
 * @brief Configures the SAI Rx audio format.
 *
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_TransferRxSetConfigEDMA
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred. This function also sets the eDMA parameter according to formatting requirements.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param format Pointer to SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If a bit clock source is the master
 * clock, this value should equal to masterClockHz in format.
 * @retval kStatus_Success Audio format set successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 */
void SAI_TransferRxSetFormatEDMA(I2S_Type *base,
                                 sai_edma_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz);

/*!
 * @brief Configures the SAI Tx.
 *
 * @note SAI eDMA supports data transfer in a multiple SAI channels if the FIFO Combine feature is supported.
 * To activate the multi-channel transfer enable SAI channels by filling the channelMask 
 * of sai_transceiver_t with the corresponding values of _sai_channel_mask enum, enable the FIFO Combine 
 * mode by assigning kSAI_FifoCombineModeEnabledOnWrite to the fifoCombine member of sai_fifo_combine_t 
 * which is a member of sai_transceiver_t.
 * This is an example of multi-channel data transfer configuration step.
   @code
    sai_transceiver_t config;
    SAI_GetClassicI2SConfig(&config, kSAI_WordWidth16bits, kSAI_Stereo, kSAI_Channel0Mask|kSAI_Channel1Mask);
    config.fifo.fifoCombine = kSAI_FifoCombineModeEnabledOnWrite;
    SAI_TransferTxSetConfigEDMA(I2S0, &edmaHandle, &config);
   @endcode
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param saiConfig sai configurations.
 */
void SAI_TransferTxSetConfigEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transceiver_t *saiConfig);

/*!
 * @brief Configures the SAI Rx.
 *
 * @note SAI eDMA supports data transfer in a multiple SAI channels if the FIFO Combine feature is supported.
 * To activate the multi-channel transfer enable SAI channels by filling the channelMask 
 * of sai_transceiver_t with the corresponding values of _sai_channel_mask enum, enable the FIFO Combine 
 * mode by assigning kSAI_FifoCombineModeEnabledOnRead to the fifoCombine member of sai_fifo_combine_t 
 * which is a member of sai_transceiver_t.
 * This is an example of multi-channel data transfer configuration step.
   @code
    sai_transceiver_t config;
    SAI_GetClassicI2SConfig(&config, kSAI_WordWidth16bits, kSAI_Stereo, kSAI_Channel0Mask|kSAI_Channel1Mask);
    config.fifo.fifoCombine = kSAI_FifoCombineModeEnabledOnRead;
    SAI_TransferRxSetConfigEDMA(I2S0, &edmaHandle, &config);
   @endcode
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param saiConfig sai configurations.
 */
void SAI_TransferRxSetConfigEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transceiver_t *saiConfig);

/*!
 * @brief Performs a non-blocking SAI transfer using DMA.
 *
 * @note This interface returns immediately after the transfer initiates. Call
 * SAI_GetTransferStatus to poll the transfer status and check whether the SAI transfer is finished.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param xfer Pointer to the DMA transfer structure.
 * @retval kStatus_Success Start a SAI eDMA send successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 * @retval kStatus_TxBusy SAI is busy sending data.
 */
status_t SAI_TransferSendEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SAI receive using eDMA.
 *
 * @note This interface returns immediately after the transfer initiates. Call
 * the SAI_GetReceiveRemainingBytes to poll the transfer status and check whether the SAI transfer is finished.
 *
 * @param base SAI base pointer
 * @param handle SAI eDMA handle pointer.
 * @param xfer Pointer to DMA transfer structure.
 * @retval kStatus_Success Start a SAI eDMA receive successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 * @retval kStatus_RxBusy SAI is busy receiving data.
 */
status_t SAI_TransferReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle, sai_transfer_t *xfer);

/*!
 * @brief Terminate all SAI send.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortSendEDMA.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateSendEDMA(I2S_Type *base, sai_edma_handle_t *handle);

/*!
 * @brief Terminate all SAI receive.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortReceiveEDMA.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle);

/*!
 * @brief Aborts a SAI transfer using eDMA.
 *
 * This function only aborts the current transfer slots, the other transfer slots' information still kept
 * in the handler. If users want to terminate all transfer slots, just call SAI_TransferTerminateSendEDMA.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferAbortSendEDMA(I2S_Type *base, sai_edma_handle_t *handle);

/*!
 * @brief Aborts a SAI receive using eDMA.
 *
 * This function only aborts the current transfer slots, the other transfer slots' information still kept
 * in the handler. If users want to terminate all transfer slots, just call SAI_TransferTerminateReceiveEDMA.
 *
 * @param base SAI base pointer
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferAbortReceiveEDMA(I2S_Type *base, sai_edma_handle_t *handle);

/*!
 * @brief Gets byte count sent by SAI.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 * @param count Bytes count sent by SAI.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SAI_TransferGetSendCountEDMA(I2S_Type *base, sai_edma_handle_t *handle, size_t *count);

/*!
 * @brief Gets byte count received by SAI.
 *
 * @param base SAI base pointer
 * @param handle SAI eDMA handle pointer.
 * @param count Bytes count received by SAI.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SAI_TransferGetReceiveCountEDMA(I2S_Type *base, sai_edma_handle_t *handle, size_t *count);

/*!
 * @brief Gets valid transfer slot.
 *
 * This function can be used to query the valid transfer request slot that the application can submit.
 * It should be called in the critical section, that means the application could call it in the corresponding callback
 * function or disable IRQ before calling it in the application, otherwise, the returned value may not correct.
 *
 * @param base SAI base pointer
 * @param handle SAI eDMA handle pointer.
 * @retval valid slot count that application submit.
 */
uint32_t SAI_TransferGetValidTransferSlotsEDMA(I2S_Type *base, sai_edma_handle_t *handle);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
