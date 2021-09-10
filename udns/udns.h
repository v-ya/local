#ifndef _udns_h_
#define _udns_h_

#include <refer.h>

typedef enum udns_opcode_t {
	udns_opcode_query  = 0,
	udns_opcode_iquery = 1,
	udns_opcode_status = 2,
	udns_opcode_notify = 4,
	udns_opcode_update = 5,
	udns_opcode_dso    = 6,
} udns_opcode_t;

typedef enum udns_rcode_t {
	udns_rcode_okay            = 0,
	udns_rcode_error_format    = 1,
	udns_rcode_error_server    = 2,
	udns_rcode_error_name      = 3,
	udns_rcode_not_implemented = 4,
	udns_rcode_refused         = 5,
} udns_rcode_t;

typedef enum udns_class_t {
	udns_class_reserved   = 0, // Reserved
	udns_class_in         = 1, // Internet
	udns_class_unassigned = 2, // Unassigned
} udns_class_t;

typedef enum udns_type_t {
	udns_type_reserved   =  0, // Reserved
	udns_type_a          =  1, // a host address
	udns_type_ns         =  2, // an authoritative name server
	udns_type_md         =  3, // a mail destination (OBSOLETE - use MX)
	udns_type_mf         =  4, // a mail forwarder (OBSOLETE - use MX)
	udns_type_cname      =  5, // the canonical name for an alias
	udns_type_soa        =  6, // marks the start of a zone of authority
	udns_type_mb         =  7, // a mailbox domain name (EXPERIMENTAL)
	udns_type_mg         =  8, // a mail group member (EXPERIMENTAL)
	udns_type_mr         =  9, // a mail rename domain name (EXPERIMENTAL)
	udns_type_null       = 10, // a null RR (EXPERIMENTAL)
	udns_type_wks        = 11, // a well known service description
	udns_type_ptr        = 12, // a domain name pointer
	udns_type_hinfo      = 13, // host information
	udns_type_minfo      = 14, // mailbox or mail list information
	udns_type_mx         = 15, // mail exchange
	udns_type_txt        = 16, // text strings
	udns_type_rp         = 17, // for Responsible Person
	udns_type_afsdb      = 18, // for AFS Data Base location
	udns_type_x25        = 19, // for X.25 PSDN address
	udns_type_isdn       = 20, // for ISDN address
	udns_type_rt         = 21, // for Route Through
	udns_type_nsap       = 22, // for NSAP address, NSAP style A record
	udns_type_nsap_ptr   = 23, // for domain name pointer, NSAP style
	udns_type_sig        = 24, // for security signature
	udns_type_key        = 25, // for security key
	udns_type_px         = 26, // X.400 mail mapping information
	udns_type_gpos       = 27, // Geographical Position
	udns_type_aaaa       = 28, // IP6 Address
	udns_type_loc        = 29, // Location Information
	udns_type_nxt        = 30, // Next Domain (OBSOLETE)
	udns_type_eid        = 31, // Endpoint Identifier
	udns_type_nimloc     = 32, // Nimrod Locator
	udns_type_srv        = 33, // Server Selection
	udns_type_atma       = 34, // ATM Address
	udns_type_naptr      = 35, // Naming Authority Pointer
	udns_type_kx         = 36, // Key Exchanger
	udns_type_cert       = 37, // CERT
	udns_type_a6         = 38, // A6 (OBSOLETE - use AAAA)
	udns_type_dname      = 39, // DNAME
	udns_type_sink       = 40, // SINK
	udns_type_opt        = 41, // OPT
	udns_type_apl        = 42, // APL
	udns_type_ds         = 43, // Delegation Signer
	udns_type_sshfp      = 44, // SSH Key Fingerprint
	udns_type_ipseckey   = 45, // IPSECKEY
	udns_type_rrsig      = 46, // RRSIG
	udns_type_nsec       = 47, // NSEC
	udns_type_dnskey     = 48, // DNSKEY
	udns_type_dhcid      = 49, // DHCID
	udns_type_nsec3      = 50, // NSEC3
	udns_type_nsec3param = 51, // NSEC3PARAM
	udns_type_tlsa       = 52, // TLSA
	udns_type_smimea     = 53, // S/MIME cert association
	udns_type_unassigned = 54, // Unassigned
	udns_type_hip        = 55, // Host Identity Protocol
	udns_type_ninfo      = 56, // NINFO
	udns_type_rkey       = 57, // RKEY
	udns_type_talink     = 58, // Trust Anchor LINK
	udns_type_cds        = 59, // Child DS
	udns_type_cdnskey    = 60, // DNSKEY(s) the Child wants reflected in DS
	udns_type_openpgpkey = 61, // OpenPGP Key
	udns_type_csync      = 62, // Child-To-Parent Synchronization
	udns_type_zonemd     = 63, // Message Digest Over Zone Data
	udns_type_svcb       = 64, // Service Binding
	udns_type_https      = 65, // HTTPS Binding
} udns_type_t;

typedef const struct udns_inst_s udns_inst_s;
typedef struct udns_s udns_s;

typedef const struct udns_question_s {
	// name
	const char *name_string;
	const uint8_t *labels_data;
	uintptr_t labels_length;
	// type && class
	udns_type_t type;
	udns_class_t class;
} udns_question_s;

typedef const struct udns_resource_s {
	// name
	const char *name_string;
	const uint8_t *labels_data;
	uintptr_t labels_length;
	// data
	const char *data_parse;
	const uint8_t *data;
	uintptr_t length;
	// type && class
	udns_type_t type;
	udns_class_t class;
	// ttl
	uint32_t ttl;
} udns_resource_s;

udns_inst_s* udns_inst_alloc(void);

udns_s* udns_alloc(udns_inst_s *restrict inst);
void udns_clear(udns_s *restrict udns);
void udns_set_qr(udns_s *restrict udns, uintptr_t qr);
void udns_set_opcode(udns_s *restrict udns, udns_opcode_t opcode);
void udns_set_aa(udns_s *restrict udns, uintptr_t aa);
void udns_set_tc(udns_s *restrict udns, uintptr_t tc);
void udns_set_rd(udns_s *restrict udns, uintptr_t rd);
void udns_set_ra(udns_s *restrict udns, uintptr_t ra);
void udns_set_rcode(udns_s *restrict udns, udns_rcode_t rcode);
uintptr_t udns_get_qr(udns_s *restrict udns);
udns_opcode_t udns_get_opcode(udns_s *restrict udns);
uintptr_t udns_get_aa(udns_s *restrict udns);
uintptr_t udns_get_tc(udns_s *restrict udns);
uintptr_t udns_get_rd(udns_s *restrict udns);
uintptr_t udns_get_ra(udns_s *restrict udns);
udns_rcode_t udns_get_rcode(udns_s *restrict udns);
udns_s* udns_add_question(udns_s *restrict udns, udns_question_s *restrict question);
udns_s* udns_add_answer(udns_s *restrict udns, udns_resource_s *restrict answer);
udns_s* udns_add_authority(udns_s *restrict udns, udns_resource_s *restrict authority);
udns_s* udns_add_additional(udns_s *restrict udns, udns_resource_s *restrict additional);
udns_s* udns_add_question_info(udns_s *restrict udns, const char *restrict name, udns_type_t type, udns_class_t class);

udns_question_s* udns_question_alloc(const char *restrict name, udns_type_t type, udns_class_t class);
udns_question_s* udns_question_parse(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos);

#endif
