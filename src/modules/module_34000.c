/**
 * Author......: Netherlands Forensic Institute
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "modules.h"
#include "bitops.h"
#include "convert.h"
#include "shared.h"
#include "memory.h"

static const u32   ATTACK_EXEC    = ATTACK_EXEC_OUTSIDE_KERNEL;
static const u32   DGST_POS0      = 0;
static const u32   DGST_POS1      = 1;
static const u32   DGST_POS2      = 2;
static const u32   DGST_POS3      = 3;
static const u32   DGST_SIZE      = DGST_SIZE_8_16;
static const u32   HASH_CATEGORY  = HASH_CATEGORY_GENERIC_KDF;
static const char *HASH_NAME      = "Keepass4";
static const u64   KERN_TYPE      = 34000;
static const u32   OPTI_TYPE      = OPTI_TYPE_ZERO_BYTE
                                  | OPTI_TYPE_SLOW_HASH_DIMY_LOOP;
static const u64   OPTS_TYPE      = OPTS_TYPE_STOCK_MODULE
                                  | OPTS_TYPE_PT_GENERATE_LE
                                  | OPTS_TYPE_THREAD_MULTI_DISABLE
                                  | OPTS_TYPE_MP_MULTI_DISABLE;
static const u32   SALT_TYPE      = SALT_TYPE_EMBEDDED;
static const char *ST_PASS        = "hashcat";
static const char *ST_HASH        = "$keepass$*4*620*ef636ddf*1048576*19*2*2e61dd9760a265fd3132a8f99dd66ac22ef6a85bd639bf011ef5f3713490e50b*9f8e12935b439279c42730ad25bdace60096b45d6b18486c586376e3ab1c3b6c*03d9a29a67fb4bb500000400021000000031c1f2e6bf714350be5805216afc5aff03040000000100000004200000002e61dd9760a265fd3132a8f99dd66ac22ef6a85bd639bf011ef5f3713490e50b0b8b00000000014205000000245555494410000000ef636ddf8c29444b91f7a9a403e30a0c0401000000560400000013000000050100000049080000006c0200000000000005010000004d0800000000001000000000000401000000500400000002000000420100000053200000009f8e12935b439279c42730ad25bdace60096b45d6b18486c586376e3ab1c3b6c000710000000f803f0b2bc4b216ba33895047c0ed70900040000000d0a0d0a*ef8e2f3504892686d8dc8991c241bb83cbc5300792e8ab387d7d3cdf3c161129";
u32         module_attack_exec    (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ATTACK_EXEC;     }
u32         module_dgst_pos0      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS0;       }
u32         module_dgst_pos1      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS1;       }
u32         module_dgst_pos2      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS2;       }
u32         module_dgst_pos3      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS3;       }
u32         module_dgst_size      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_SIZE;       }
u32         module_hash_category  (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_CATEGORY;   }
const char *module_hash_name      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_NAME;       }
u64         module_kern_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return KERN_TYPE;       }
u32         module_opti_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTI_TYPE;       }
u64         module_opts_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTS_TYPE;       }
u32         module_salt_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return SALT_TYPE;       }
const char *module_st_hash        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_HASH;         }
const char *module_st_pass        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_PASS;         }

typedef struct argon2_tmp
{
  u32 state[4]; // just something.. why do we need this? It's always empty

} argon2_tmp_t;


typedef struct keepass4
{
  u32 masterseed[32]; // needs to be this big because of sha512 not sure why it cannot be 512bit
  u32 header[64];

} keepass4_t;

typedef struct argon2_options
{
  u32 type;
  u32 version;

  u32 iterations;
  u32 parallelism;
  u32 memory_usage_in_kib;

  u32 segment_length;
  u32 lane_length;
  u32 memory_block_count;

  u32 digest_len;

  keepass4_t keepass4;

} argon2_options_t;

#include "argon2_common.c"

static const char *SIGNATURE_ARGON2D_UUID  = "ef636ddf";
static const char *SIGNATURE_ARGON2ID_UUID = "9e298b19";

u64 module_esalt_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 esalt_size = (const u64) sizeof (argon2_options_t);

  return esalt_size;
}

int module_hash_decode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED void *digest_buf, MAYBE_UNUSED salt_t *salt, MAYBE_UNUSED void *esalt_buf, MAYBE_UNUSED void *hook_salt_buf, MAYBE_UNUSED hashinfo_t *hash_info, const char *line_buf, MAYBE_UNUSED const int line_len)
{
  u32 *digest = (u32 *) digest_buf;

  argon2_options_t *options  = (argon2_options_t *) esalt_buf;
  keepass4_t *keepass4  = &options->keepass4;

  hc_token_t token;

  memset (&token, 0, sizeof (hc_token_t));

  token.token_cnt  = 11;

  // 0. signature
  token.signatures_cnt    = 1;
  token.signatures_buf[0] = "$keepass$*";
  token.len[0]     = 10;
  token.sep[0]     = 0;
  token.attr[0]    = TOKEN_ATTR_VERIFY_SIGNATURE;

  // 1. keepassDB version
  token.len[1]     = 1;
  token.sep[1]     = '*';
  token.attr[1]    = TOKEN_ATTR_FIXED_LENGTH | TOKEN_ATTR_VERIFY_DIGIT;

  // 2. iterations
  token.len_min[2] = 1;
  token.len_max[2] = 5;
  token.sep[2]     = '*';
  token.attr[2]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_DIGIT;

  // 3. KDF UUID
  token.len_min[3] = 8;
  token.len_max[3] = 8;
  token.sep[3]     = '*';
  token.attr[3]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_HEX;

  // 4. memoryUsageInBytes
  token.len_min[4] = 3;
  token.len_max[4] = 12;
  token.sep[4]     = '*';
  token.attr[4]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_DIGIT;

  // 5. Argon version
  token.len_min[5] = 1;
  token.len_max[5] = 3;
  token.sep[5]     = '*';
  token.attr[5]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_DIGIT;

  // 6. parallelism
  token.len_min[6] = 1;
  token.len_max[6] = 3;
  token.sep[6]     = '*';
  token.attr[6]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_DIGIT;

  // 7. masterseed
  token.len_min[7] = 64;
  token.len_max[7] = 64;
  token.sep[7]     = '*';
  token.attr[7]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_HEX;

  // 8. transformseed (salt)
  token.len_min[8] = 64;
  token.len_max[8] = 64;
  token.sep[8]     = '*';
  token.attr[8]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_HEX;

  // 9. header
  token.len_min[9] = 506;
  token.len_max[9] = 506;
  token.sep[9]     = '*';
  token.attr[9]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_HEX;

  // 10. headerhmac (digest)
  token.len_min[10] = 64;
  token.len_max[10] = 64;
  token.sep[10]     = '*';
  token.attr[10]    = TOKEN_ATTR_VERIFY_LENGTH | TOKEN_ATTR_VERIFY_HEX;

  const int rc_tokenizer = input_tokenizer ((const u8 *) line_buf, line_len, &token);

  if (rc_tokenizer != PARSER_OK) return (rc_tokenizer);

  // 0. signature:
  const int sig_len = token.len[0];
  const u8 *sig_pos = token.buf[0];
  if (memcmp (token.signatures_buf[0],  sig_pos, sig_len) != 0) return (PARSER_SIGNATURE_UNMATCHED);

  // 1. keepassDB version
  const u8 *keepassdb_version_pos = token.buf[1];
  const u32 keepassdb_version = hc_strtoul ((const char *) keepassdb_version_pos, NULL, 10);
  if (keepassdb_version != 4) return (PARSER_HASH_VALUE); // we don't support anything else than 4

  // 2. iterations
  const u8 *it_pos  = token.buf[2];
  options->iterations          = hc_strtoul ((const char *) it_pos, NULL, 10);

  // 3. KDF UUID: sets argon2 type
  const int kdf_uuid_len = token.len[3];
  const u8 *kdf_uuid_pos = token.buf[3];
  const u8 kdf_uuid[8] = {0};
  hex_decode ((const u8 *) kdf_uuid_pos, kdf_uuid_len, (u8 *) kdf_uuid);
  if      (memcmp (SIGNATURE_ARGON2D_UUID,  kdf_uuid_pos, kdf_uuid_len) == 0) options->type = 0;
  else if (memcmp (SIGNATURE_ARGON2ID_UUID, kdf_uuid_pos, kdf_uuid_len) == 0) options->type = 2;
  else
    return (PARSER_HASH_VALUE);

  // 4. memoryUsageInBytes
  const u8 *mem_pos = token.buf[4];
  options->memory_usage_in_kib = hc_strtoul ((const char *) mem_pos, NULL, 10)/1024; // /1024 to go from bytes to KiB

  // 5. Argon version
  const u8 *ver_pos = token.buf[5];
  options->version             = hc_strtoul ((const char *) ver_pos, NULL, 10);

  // 6. parallelism
  const u8 *par_pos = token.buf[6];
  options->parallelism         = hc_strtoul ((const char *) par_pos, NULL, 10);

  // 7. masterseed
  const int masterseed_len = token.len[7];
  const u8 *masterseed_pos = token.buf[7];
  hex_decode ((const u8 *) masterseed_pos, masterseed_len, (u8 *) keepass4->masterseed);

  // 8. transformseed (salt)
  const int salt_len = token.len[8];
  const u8 *salt_pos = token.buf[8];

  salt->salt_iter = options->iterations * ARGON2_SYNC_POINTS;
  salt->salt_dimy = options->parallelism;
  salt->salt_len = hex_decode ((const u8 *) salt_pos, salt_len, (u8 *) salt->salt_buf);

  // 9. header
  const int header_len = token.len[9];
  const u8 *header_pos = token.buf[9];
  hex_decode ((const u8 *) header_pos, header_len, (u8 *) keepass4->header);

  // 10. headerhmac (digest): digest/ target hash
  const int digest_len = token.len[10];
  const u8 *digest_pos = token.buf[10];
  options->digest_len = hex_decode ((const u8 *) digest_pos, digest_len, (u8 *) digest);

  // check argon2 config
  if (options->version != 19 && options->version != 16) return (PARSER_HASH_VALUE);
  if (options->memory_usage_in_kib < 1) return (PARSER_HASH_VALUE);
  if (options->iterations < 1) return (PARSER_HASH_VALUE);
  if (options->parallelism < 1 || options->parallelism > 32) return (PARSER_HASH_VALUE);

  options->segment_length     = MAX (2, (options->memory_usage_in_kib / (ARGON2_SYNC_POINTS * options->parallelism)));
  options->lane_length        = options->segment_length * ARGON2_SYNC_POINTS;
  options->memory_block_count = options->lane_length * options->parallelism;

  return (PARSER_OK);
}

int module_hash_encode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const void *digest_buf, MAYBE_UNUSED const salt_t *salt, MAYBE_UNUSED const void *esalt_buf, MAYBE_UNUSED const void *hook_salt_buf, MAYBE_UNUSED const hashinfo_t *hash_info, char *line_buf, MAYBE_UNUSED const int line_size)
{
  u32 *digest = (u32 *) digest_buf;

  argon2_options_t *options  = (argon2_options_t *) esalt_buf;
  keepass4_t *keepass4  = &options->keepass4;

  // 7. masterseed
  char masterseed_hex[64] = { 0 };
  hex_encode( (const u8 *) keepass4->masterseed, 32, (u8 *) masterseed_hex);

  // 8. transformseed (salt)
  char salt_hex[64] = { 0 };
  hex_encode( (const u8 *) salt->salt_buf, 32, (u8 *) salt_hex);

  // 9. header
  char header_hex[506] = { 0 };
  hex_encode( (const u8 *) keepass4->header, 253, (u8 *) header_hex);

  // 10. headerhmac (digest)
  char digest_hex[64] = { 0 };
  hex_encode( (const u8 *) digest, 32, (u8 *) digest_hex);

  const char *argon_uuid = NULL;
  switch (options->type)
  {
    case 0: argon_uuid = SIGNATURE_ARGON2D_UUID;  break;
    case 2: argon_uuid = SIGNATURE_ARGON2ID_UUID; break;
  }

  u8 *out_buf = (u8 *) line_buf;

  const int out_len = snprintf ((char *) out_buf, line_size, "%s*%d*%d*%s*%d*%d*%d*%s*%s*%s*%s",
    "$keepass$",          // 0. signature
    4,                    // 1. keepassDB version
    options->iterations,  // 2. iterations
    argon_uuid,           // 3. KDF UUID
    options->memory_usage_in_kib*1024,  // 4. memoryUsageInBytes
    options->version,     // 5. Argon version
    options->parallelism, // 6. parallelism
    masterseed_hex,       // 7. masterseed
    salt_hex,             // 8. transformseed (salt)
    header_hex,           // 9. header
    digest_hex            // 10. headerhmac (digest)
  );

  return out_len;
}

void module_init (module_ctx_t *module_ctx)
{
  module_ctx->module_context_size             = MODULE_CONTEXT_SIZE_CURRENT;
  module_ctx->module_interface_version        = MODULE_INTERFACE_VERSION_CURRENT;

  module_ctx->module_attack_exec              = module_attack_exec;
  module_ctx->module_benchmark_esalt          = MODULE_DEFAULT;
  module_ctx->module_benchmark_hook_salt      = MODULE_DEFAULT;
  module_ctx->module_benchmark_mask           = MODULE_DEFAULT;
  module_ctx->module_benchmark_charset        = MODULE_DEFAULT;
  module_ctx->module_benchmark_salt           = MODULE_DEFAULT;
  module_ctx->module_bridge_name              = MODULE_DEFAULT;
  module_ctx->module_bridge_type              = MODULE_DEFAULT;
  module_ctx->module_build_plain_postprocess  = MODULE_DEFAULT;
  module_ctx->module_deep_comp_kernel         = MODULE_DEFAULT;
  module_ctx->module_deprecated_notice        = MODULE_DEFAULT;
  module_ctx->module_dgst_pos0                = module_dgst_pos0;
  module_ctx->module_dgst_pos1                = module_dgst_pos1;
  module_ctx->module_dgst_pos2                = module_dgst_pos2;
  module_ctx->module_dgst_pos3                = module_dgst_pos3;
  module_ctx->module_dgst_size                = module_dgst_size;
  module_ctx->module_dictstat_disable         = MODULE_DEFAULT;
  module_ctx->module_esalt_size               = module_esalt_size;
  module_ctx->module_extra_buffer_size        = argon2_module_extra_buffer_size;
  module_ctx->module_extra_tmp_size           = argon2_module_extra_tmp_size;
  module_ctx->module_extra_tuningdb_block     = argon2_module_extra_tuningdb_block;
  module_ctx->module_forced_outfile_format    = MODULE_DEFAULT;
  module_ctx->module_hash_binary_count        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_parse        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_save         = MODULE_DEFAULT;
  module_ctx->module_hash_decode_postprocess  = MODULE_DEFAULT;
  module_ctx->module_hash_decode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_decode_zero_hash    = MODULE_DEFAULT;
  module_ctx->module_hash_decode              = module_hash_decode;
  module_ctx->module_hash_encode_status       = MODULE_DEFAULT;
  module_ctx->module_hash_encode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_encode              = module_hash_encode;
  module_ctx->module_hash_init_selftest       = MODULE_DEFAULT;
  module_ctx->module_hash_mode                = MODULE_DEFAULT;
  module_ctx->module_hash_category            = module_hash_category;
  module_ctx->module_hash_name                = module_hash_name;
  module_ctx->module_hashes_count_min         = MODULE_DEFAULT;
  module_ctx->module_hashes_count_max         = MODULE_DEFAULT;
  module_ctx->module_hlfmt_disable            = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_size    = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_init    = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_term    = MODULE_DEFAULT;
  module_ctx->module_hook12                   = MODULE_DEFAULT;
  module_ctx->module_hook23                   = MODULE_DEFAULT;
  module_ctx->module_hook_salt_size           = MODULE_DEFAULT;
  module_ctx->module_hook_size                = MODULE_DEFAULT;
  module_ctx->module_jit_build_options        = argon2_module_jit_build_options;
  module_ctx->module_jit_cache_disable        = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_max         = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_min         = MODULE_DEFAULT;
  module_ctx->module_kernel_loops_max         = MODULE_DEFAULT;
  module_ctx->module_kernel_loops_min         = MODULE_DEFAULT;
  module_ctx->module_kernel_threads_max       = MODULE_DEFAULT;
  module_ctx->module_kernel_threads_min       = MODULE_DEFAULT;
  module_ctx->module_kern_type                = module_kern_type;
  module_ctx->module_kern_type_dynamic        = MODULE_DEFAULT;
  module_ctx->module_opti_type                = module_opti_type;
  module_ctx->module_opts_type                = module_opts_type;
  module_ctx->module_outfile_check_disable    = MODULE_DEFAULT;
  module_ctx->module_outfile_check_nocomp     = MODULE_DEFAULT;
  module_ctx->module_potfile_custom_check     = MODULE_DEFAULT;
  module_ctx->module_potfile_disable          = MODULE_DEFAULT;
  module_ctx->module_potfile_keep_all_hashes  = MODULE_DEFAULT;
  module_ctx->module_pwdump_column            = MODULE_DEFAULT;
  module_ctx->module_pw_max                   = MODULE_DEFAULT;
  module_ctx->module_pw_min                   = MODULE_DEFAULT;
  module_ctx->module_salt_max                 = MODULE_DEFAULT;
  module_ctx->module_salt_min                 = MODULE_DEFAULT;
  module_ctx->module_salt_type                = module_salt_type;
  module_ctx->module_separator                = MODULE_DEFAULT;
  module_ctx->module_st_hash                  = module_st_hash;
  module_ctx->module_st_pass                  = module_st_pass;
  module_ctx->module_tmp_size                 = argon2_module_tmp_size;
  module_ctx->module_unstable_warning         = MODULE_DEFAULT;
  module_ctx->module_warmup_disable           = MODULE_DEFAULT;
}

