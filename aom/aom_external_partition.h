/*
 * Copyright (c) 2021, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */
#ifndef AOM_AOM_AOM_EXTERNAL_PARTITION_H_
#define AOM_AOM_AOM_EXTERNAL_PARTITION_H_

/*!\defgroup aom_encoder AOMedia AOM/AV1 Encoder
 * \ingroup aom
 *
 * @{
 */
#include "./aom_integer.h"

/*!\file
 * \brief Provides function pointer definitions for the external partition.
 */

/*!\brief Current ABI version number
 *
 * \internal
 * If this file is altered in any way that changes the ABI, this value
 * must be bumped. Examples include, but are not limited to, changing
 * types, removing or reassigning enums, adding/removing/rearranging
 * fields to structures.
 */
#define AOM_EXT_PART_ABI_VERSION (1)

#ifdef __cplusplus
extern "C" {
#endif

/*!\brief Abstract external partition model handler
 */
typedef void *aom_ext_part_model_t;

/*!\brief Config information sent to the external partition model.
 *
 * For example, the maximum superblock size determined by the sequence header.
 */
typedef struct aom_ext_part_config {
  int superblock_size; /**< super block size (either 64x64 or 128x128) */
} aom_ext_part_config_t;

/*!\brief Features pass to the external model to make partition decisions.
 *
 * The encoder sends these features to the external model through
 * "func()" defined in .....
 *
 * NOTE: new member variables may be added to this structure in the future.
 * Once new features are finalized, bump the major version of libaom.
 */
typedef struct aom_partition_features {
  // Features are unclear yet
  // Candidates include:
  // mv
  int64_t sse; /**< sum squared error */
} aom_partition_features_t;

/*!\brief Partition decisions received from the external model.
 *
 * The encoder receives partition decisions and encodes the superblock
 * with the given partition type.
 * The encoder receives it from "func()" define in ....
 *
 * NOTE: new member variables may be added to this structure in the future.
 * Once new features are finalized, bump the major version of libaom.
 */
typedef struct aom_partition_decision {
  int is_final_decision;       /**< The flag whether it is the final decision */
  int partition_decision[256]; /**< Partition decisions */
} aom_partition_decision_t;

/*!\brief Encoding stats for the given partition decision.
 *
 * The encoding stats collected by encoding the superblock with the
 * given partition types.
 * The encoder sends the stats to the external model for training
 * or inference though "func()" defined in ....
 */
typedef struct aom_partition_stats {
  int rate;       /**< Rate cost of the block */
  int64_t dist;   /**< Distortion of the block */
  int64_t rdcost; /**< Rate-distortion cost of the block */
} aom_partition_stats_t;

/*!\brief Enum for return status.
 */
typedef enum aom_ext_part_status {
  AOM_EXT_PART_OK = 0,    /**< Status of success */
  AOM_EXT_PART_ERROR = 1, /**< Status of failure */
} aom_ext_part_status_t;

/*!\brief Callback of creating an external partition model.
 *
 * The callback is invoked by the encoder to create an external partition
 * model.
 *
 * \param[in] priv Callback's private data
 * \param[in] part_config Config information pointer for model creation
 * \param[out] ext_part_model Pointer to the model
 */
typedef aom_ext_part_status_t (*aom_ext_part_create_model_fn_t)(
    void *priv, const aom_ext_part_config_t *part_config,
    aom_ext_part_model_t *ext_part_model);

/*!\brief Callback of sending features to the external partition model.
 *
 * The callback is invoked by the encoder to send features to the external
 * partition model.
 *
 * \param[in] ext_part_model The external model
 * \param[in] part_features Pointer to the features
 */
typedef aom_ext_part_status_t (*aom_ext_part_send_features_fn_t)(
    aom_ext_part_model_t ext_part_model,
    const aom_partition_features_t *part_features);

/*!\brief Callback of receiving partition decisions from the external
 * partition model.
 *
 * The callback is invoked by the encoder to receive partition decisions from
 * the external partition model.
 *
 * \param[in] ext_part_model The external model
 * \param[in] ext_part_decision Pointer to the partition decisions
 */
typedef aom_ext_part_status_t (*aom_ext_part_get_decision_fn_t)(
    aom_ext_part_model_t ext_part_model,
    aom_partition_decision_t *ext_part_decision);

/*!\brief Callback of sending stats to the external partition model.
 *
 * The callback is invoked by the encoder to send encoding stats to
 * the external partition model.
 *
 * \param[in] ext_part_model The external model
 * \param[in] ext_part_stats Pointer to the encoding stats
 */
typedef aom_ext_part_status_t (*aom_ext_part_send_partition_stats_fn_t)(
    aom_ext_part_model_t ext_part_model,
    const aom_partition_stats_t *ext_part_stats);

/*!\brief Callback of deleting the external partition model.
 *
 * The callback is invoked by the encoder to delete the external partition
 * model.
 *
 * \param[in] ext_part_model The external model
 */
typedef aom_ext_part_status_t (*aom_ext_part_delete_model_fn_t)(
    aom_ext_part_model_t ext_part_model);

/*!\brief Callback function set for external partition model.
 *
 * Uses can enable external partition model by registering a set of
 * callback functions with the flag: AV1E_SET_EXTERNAL_PARTITION_MODEL
 */
typedef struct aom_ext_part_funcs {
  /*!
   * Create an external partition model.
   */
  aom_ext_part_create_model_fn_t create_model;

  /*!
   * Send features to the external partition model to make partition decisions.
   */
  aom_ext_part_send_features_fn_t send_features;

  /*!
   * Get partition decisions from the external partition model.
   */
  aom_ext_part_get_decision_fn_t get_partition_decision;

  /*!
   * Send stats of the current partition to the external model.
   */
  aom_ext_part_send_partition_stats_fn_t send_partition_stats;

  /*!
   * Delete the external partition model.
   */
  aom_ext_part_delete_model_fn_t delete_model;

  /*!
   * Private data for the external partition model.
   */
  void *priv;
} aom_ext_part_funcs_t;

/*!@} - end defgroup aom_encoder*/
#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AOM_AOM_AOM_EXTERNAL_PARTITION_H_
