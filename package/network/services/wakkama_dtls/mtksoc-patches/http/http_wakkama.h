#ifndef HTTP_WAKKAMA_H_
#define HTTP_WAKKAMA_H_

#include <stdint.h>
#include <stddef.h> /* for size_t */

#define HTTP_DEFAULT_MAX_AGE                 60
#define HTTP_RESPONSE_TIMEOUT                2
#define HTTP_MAX_RETRANSMIT                  4
#define HTTP_ACK_RANDOM_FACTOR               1.5
#define HTTP_MAX_LATENCY                     100
#define HTTP_PROCESSING_DELAY                HTTP_RESPONSE_TIMEOUT

#define HTTP_MAX_TRANSMIT_WAIT               ((HTTP_RESPONSE_TIMEOUT * ( (1 << (HTTP_MAX_RETRANSMIT + 1) ) - 1) * HTTP_ACK_RANDOM_FACTOR))
#define HTTP_MAX_TRANSMIT_SPAN               ((HTTP_RESPONSE_TIMEOUT * ( (1 << HTTP_MAX_RETRANSMIT) - 1) * HTTP_ACK_RANDOM_FACTOR))
#define HTTP_EXCHANGE_LIFETIME               (HTTP_MAX_TRANSMIT_SPAN + (2 * HTTP_MAX_LATENCY) + HTTP_PROCESSING_DELAY)

#define HTTP_HEADER_LEN                      4 /* | version:0x03 type:0x0C tkl:0xF0 | code | mid:0x00FF | mid:0xFF00 | */
#define HTTP_ETAG_LEN                        8 /* The maximum number of bytes for the ETag */
#define HTTP_TOKEN_LEN                       8 /* The maximum number of bytes for the Token */
#define HTTP_MAX_ACCEPT_NUM                  2 /* The maximum number of accept preferences to parse/store */

#define HTTP_MAX_OPTION_HEADER_LEN           5

#define HTTP_HEADER_VERSION_MASK             0xC0
#define HTTP_HEADER_VERSION_POSITION         6
#define HTTP_HEADER_TYPE_MASK                0x30
#define HTTP_HEADER_TYPE_POSITION            4
#define HTTP_HEADER_TOKEN_LEN_MASK           0x0F
#define HTTP_HEADER_TOKEN_LEN_POSITION       0

#define HTTP_HEADER_OPTION_DELTA_MASK        0xF0
#define HTTP_HEADER_OPTION_SHORT_LENGTH_MASK 0x0F

/* Bitmap for set options */
enum { OPTION_MAP_SIZE = sizeof(uint8_t) * 8 };
#define SET_OPTION(packet, opt) {if (opt <= sizeof((packet)->options) * OPTION_MAP_SIZE) {(packet)->options[opt / OPTION_MAP_SIZE] |= 1 << (opt % OPTION_MAP_SIZE);}}
#define IS_OPTION(packet, opt) ((opt <= sizeof((packet)->options) * OPTION_MAP_SIZE)?(packet)->options[opt / OPTION_MAP_SIZE] & (1 << (opt % OPTION_MAP_SIZE)):0)

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a) : (b))
#endif /* MIN */

#ifndef MAX
#define MAX(a, b) ((a) > (b)? (a) : (b))
#endif /* MAX */

/* HTTP message types */
typedef enum {
  HTTP_TYPE_CON, /* confirmables */
  HTTP_TYPE_NON, /* non-confirmables */
  HTTP_TYPE_ACK, /* acknowledgements */
  HTTP_TYPE_RST  /* reset */
} http_message_type_t;

/* HTTP request method codes */
typedef enum {
  HTTP_GET = 1,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE
} http_method_t;

/* HTTP response codes */
typedef enum {
  NO_ERROR = 0,

  CREATED_2_01 = 65,                    /* CREATED */
  DELETED_2_02 = 66,                    /* DELETED */
  VALID_2_03 = 67,                      /* NOT_MODIFIED */
  CHANGED_2_04 = 68,                    /* CHANGED */
  CONTENT_2_05 = 69,                    /* OK */

  BAD_REQUEST_4_00 = 128,               /* BAD_REQUEST */
  UNAUTHORIZED_4_01 = 129,              /* UNAUTHORIZED */
  BAD_OPTION_4_02 = 130,                /* BAD_OPTION */
  FORBIDDEN_4_03 = 131,                 /* FORBIDDEN */
  NOT_FOUND_4_04 = 132,                 /* NOT_FOUND */
  METHOD_NOT_ALLOWED_4_05 = 133,        /* METHOD_NOT_ALLOWED */
  NOT_ACCEPTABLE_4_06 = 134,            /* NOT_ACCEPTABLE */
  PRECONDITION_FAILED_4_12 = 140,       /* BAD_REQUEST */
  REQUEST_ENTITY_TOO_LARGE_4_13 = 141,  /* REQUEST_ENTITY_TOO_LARGE */
  UNSUPPORTED_MEDIA_TYPE_4_15 = 143,    /* UNSUPPORTED_MEDIA_TYPE */

  INTERNAL_SERVER_ERROR_5_00 = 160,     /* INTERNAL_SERVER_ERROR */
  NOT_IMPLEMENTED_5_01 = 161,           /* NOT_IMPLEMENTED */
  BAD_GATEWAY_5_02 = 162,               /* BAD_GATEWAY */
  SERVICE_UNAVAILABLE_5_03 = 163,       /* SERVICE_UNAVAILABLE */
  GATEWAY_TIMEOUT_5_04 = 164,           /* GATEWAY_TIMEOUT */
  PROXYING_NOT_SUPPORTED_5_05 = 165,    /* PROXYING_NOT_SUPPORTED */

  /* Erbium errors */
  MEMORY_ALLOCATION_ERROR = 192,
  PACKET_SERIALIZATION_ERROR,

  /* Erbium hooks */
  MANUAL_RESPONSE

} http_status_t;

/* HTTP header options */
typedef enum {
  HTTP_OPTION_IF_MATCH = 1,       /* 0-8 B */
  HTTP_OPTION_URI_HOST = 3,       /* 1-255 B */
  HTTP_OPTION_ETAG = 4,           /* 1-8 B */
  HTTP_OPTION_IF_NONE_MATCH = 5,  /* 0 B */
  HTTP_OPTION_OBSERVE = 6,        /* 0-3 B */
  HTTP_OPTION_URI_PORT = 7,       /* 0-2 B */
  HTTP_OPTION_LOCATION_PATH = 8,  /* 0-255 B */
  HTTP_OPTION_URI_PATH = 11,      /* 0-255 B */
  HTTP_OPTION_CONTENT_TYPE = 12,  /* 0-2 B */
  HTTP_OPTION_MAX_AGE = 14,       /* 0-4 B */
  HTTP_OPTION_URI_QUERY = 15,     /* 0-270 B */
  HTTP_OPTION_ACCEPT = 17,        /* 0-2 B */
  HTTP_OPTION_TOKEN = 19,         /* 1-8 B */
  HTTP_OPTION_LOCATION_QUERY = 20, /* 1-270 B */
  HTTP_OPTION_BLOCK2 = 23,        /* 1-3 B */
  HTTP_OPTION_BLOCK1 = 27,        /* 1-3 B */
  HTTP_OPTION_SIZE = 28,          /* 0-4 B */
  HTTP_OPTION_PROXY_URI = 35,     /* 1-270 B */
  OPTION_MAX_VALUE = 0xFFFF
} http_option_t;

/* HTTP Content-Types */
typedef enum {
  TEXT_PLAIN = 0,
  TEXT_XML = 1, /* Indented types are not in the initial registry. */
  TEXT_CSV = 2,
  TEXT_HTML = 3,
  IMAGE_GIF = 21,
  IMAGE_JPEG = 22,
  IMAGE_PNG = 23,
  IMAGE_TIFF = 24,
  AUDIO_RAW = 25,
  VIDEO_RAW = 26,
  APPLICATION_LINK_FORMAT = 40,
  APPLICATION_XML = 41,
  APPLICATION_OCTET_STREAM = 42,
  APPLICATION_RDF_XML = 43,
  APPLICATION_SOAP_XML = 44,
  APPLICATION_ATOM_XML = 45,
  APPLICATION_XMPP_XML = 46,
  APPLICATION_EXI = 47,
  APPLICATION_FASTINFOSET = 48,
  APPLICATION_SOAP_FASTINFOSET = 49,
  APPLICATION_JSON = 50,
  APPLICATION_X_OBIX_BINARY = 51,
  CONTENT_MAX_VALUE = 0xFFFF
} http_content_type_t;

typedef struct _multi_option_t {
  struct _multi_option_t *next;
  uint8_t is_static;
  uint8_t len;
  uint8_t *data;
} multi_option_t;

/* Parsed message struct */
typedef struct {
  uint8_t *buffer; /* pointer to HTTP header / incoming packet buffer / memory to serialize packet */

  uint8_t version;
  http_message_type_t type;
  uint8_t code;
  uint16_t mid;

  uint8_t options[HTTP_OPTION_PROXY_URI / OPTION_MAP_SIZE + 1]; /* Bitmap to check if option is set */

  http_content_type_t content_type; /* Parse options once and store; allows setting options in random order  */
  uint32_t max_age;
  size_t proxy_uri_len;
  const uint8_t *proxy_uri;
  uint8_t etag_len;
  uint8_t etag[HTTP_ETAG_LEN];
  size_t uri_host_len;
  const uint8_t *uri_host;
  multi_option_t *location_path;
  uint16_t uri_port;
  size_t location_query_len;
  uint8_t *location_query;
  multi_option_t *uri_path;
  uint32_t observe;
  uint8_t token_len;
  uint8_t token[HTTP_TOKEN_LEN];
  uint8_t accept_num;
  uint16_t accept[HTTP_MAX_ACCEPT_NUM];
  uint8_t if_match_len;
  uint8_t if_match[HTTP_ETAG_LEN];
  uint32_t block2_num;
  uint8_t block2_more;
  uint16_t block2_size;
  uint32_t block2_offset;
  uint32_t block1_num;
  uint8_t block1_more;
  uint16_t block1_size;
  uint32_t block1_offset;
  uint32_t size;
  multi_option_t *uri_query;
  uint8_t if_none_match;

  uint16_t payload_len;
  uint8_t *payload;

} http_packet_t;

/* Option format serialization*/
#define HTTP_SERIALIZE_INT_OPTION(number, field, text)  \
    if (IS_OPTION(http_pkt, number)) { \
      PRINTF(text" [%u]\n", http_pkt->field); \
      option += http_serialize_int_option(number, current_number, option, http_pkt->field); \
      current_number = number; \
    }
#define HTTP_SERIALIZE_BYTE_OPTION(number, field, text)      \
    if (IS_OPTION(http_pkt, number)) { \
      PRINTF(text" %u [0x%02X%02X%02X%02X%02X%02X%02X%02X]\n", http_pkt->field##_len, \
        http_pkt->field[0], \
        http_pkt->field[1], \
        http_pkt->field[2], \
        http_pkt->field[3], \
        http_pkt->field[4], \
        http_pkt->field[5], \
        http_pkt->field[6], \
        http_pkt->field[7] \
      ); /*FIXME always prints 8 bytes */ \
      option += http_serialize_array_option(number, current_number, option, http_pkt->field, http_pkt->field##_len, '\0'); \
      current_number = number; \
    }
#define HTTP_SERIALIZE_STRING_OPTION(number, field, splitter, text)      \
    if (IS_OPTION(http_pkt, number)) { \
      PRINTF(text" [%.*s]\n", http_pkt->field##_len, http_pkt->field); \
      option += http_serialize_array_option(number, current_number, option, (uint8_t *) http_pkt->field, http_pkt->field##_len, splitter); \
      current_number = number; \
    }
#define HTTP_SERIALIZE_MULTI_OPTION(number, field, text)      \
        if (IS_OPTION(http_pkt, number)) { \
          PRINTF(text); \
          option += http_serialize_multi_option(number, current_number, option, http_pkt->field); \
          current_number = number; \
        }
#define HTTP_SERIALIZE_ACCEPT_OPTION(number, field, text)  \
    if (IS_OPTION(http_pkt, number)) { \
      int i; \
      for (i=0; i<http_pkt->field##_num; ++i) \
      { \
        PRINTF(text" [%u]\n", http_pkt->field[i]); \
        option += http_serialize_int_option(number, current_number, option, http_pkt->field[i]); \
        current_number = number; \
      } \
    }
#define HTTP_SERIALIZE_BLOCK_OPTION(number, field, text)      \
    if (IS_OPTION(http_pkt, number)) \
    { \
      uint32_t block = http_pkt->field##_num << 4; \
      PRINTF(text" [%lu%s (%u B/blk)]\n", http_pkt->field##_num, http_pkt->field##_more ? "+" : "", http_pkt->field##_size); \
      if (http_pkt->field##_more) block |= 0x8; \
      block |= 0xF & http_log_2(http_pkt->field##_size/16); \
      PRINTF(text" encoded: 0x%lX\n", block); \
      option += http_serialize_int_option(number, current_number, option, block); \
      current_number = number; \
    }

/* To store error code and human-readable payload */
extern const char *http_error_message;

uint16_t http_get_mid(void);

void http_init_message(void *packet, http_message_type_t type, uint8_t code, uint16_t mid);
size_t http_serialize_get_size(void *packet);
size_t http_serialize_message(void *packet, uint8_t *buffer);
http_status_t http_parse_message(void *request, uint8_t *data, uint16_t data_len);
void http_free_header(void *packet);

char * http_get_multi_option_as_path_string(multi_option_t * option);
char * http_get_multi_option_as_query_string(multi_option_t * option);
char * http_get_packet_uri_as_string(http_packet_t * packet);
void http_add_multi_option(multi_option_t **dst, uint8_t *option, size_t option_len, uint8_t is_static);
void free_multi_option(multi_option_t *dst);

int http_get_query_variable(void *packet, const char *name, const char **output);
int http_get_post_variable(void *packet, const char *name, const char **output);

/*-----------------------------------------------------------------------------------*/

int http_set_status_code(void *packet, unsigned int code);

unsigned int http_get_header_content_type(void *packet);
int http_set_header_content_type(void *packet, unsigned int content_type);

int http_get_header_accept(void *packet, const uint16_t **accept);
int http_set_header_accept(void *packet, uint16_t accept);

int http_get_header_if_match(void *packet, const uint8_t **etag);
int http_set_header_if_match(void *packet, const uint8_t *etag, size_t etag_len);

int http_get_header_if_none_match(void *packet);
int http_set_header_if_none_match(void *packet);

int http_get_header_token(void *packet, uint8_t **token);
int http_set_header_token(void *packet, const uint8_t *token, size_t token_len);

int http_get_header_proxy_uri(void *packet, const char **uri); /* In-place string might not be 0-terminated. */
int http_set_header_proxy_uri(void *packet, const char *uri);

int http_get_header_uri_host(void *packet, const char **host); /* In-place string might not be 0-terminated. */
int http_set_header_uri_host(void *packet, const char *host);

int http_get_header_uri_path(void *packet, const char **path); /* In-place string might not be 0-terminated. */
int http_set_header_uri_path(void *packet, const char *path);
int http_set_header_uri_path_segment(void *packet, const char *path);

uint16_t http_get_header_uri_port(void *packet);
void http_set_header_uri_port(void *packet, uint16_t port);

int http_get_header_uri_query(void *packet, const char **query); /* In-place string might not be 0-terminated. */
int http_set_header_uri_query(void *packet, const char *query);
int http_set_header_uri_query_segment(void *packet, const char *segment);

int http_get_header_location_path(void *packet, const char **path); /* In-place string might not be 0-terminated. */
int http_set_header_location_path(void *packet, const char *path); /* Also splits optional query into Location-Query option. */

int http_get_header_location_query(void *packet, const char **query); /* In-place string might not be 0-terminated. */
int http_set_header_location_query(void *packet, char *query);

int http_get_header_observe(void *packet, uint32_t *observe);
int http_set_header_observe(void *packet, uint32_t observe);

int http_get_header_size(void *packet, uint32_t *size);
int http_set_header_size(void *packet, uint32_t size);

int http_get_payload(void *packet, const uint8_t **payload);
int http_set_payload(void *packet, const void *payload, size_t length);

#endif /* HTTP_WAKKAMA_H_ */
