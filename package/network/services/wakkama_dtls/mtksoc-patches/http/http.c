#include <stdlib.h>

#include <string.h>
#include <stdio.h>

#include "http_wakkama.h"

#include "liblwm2m.h" /* for lwm2m_malloc() and lwm2m_free() */

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/*-----------------------------------------------------------------------------------*/
/*- Variables -----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
static uint16_t current_mid = 0;

http_status_t http_error_code = NO_ERROR;
const char *http_error_message = "";
/*-----------------------------------------------------------------------------------*/
/*- LOCAL HELP FUNCTIONS ------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
static
uint16_t
http_log_2(uint16_t value)
{
  uint16_t result = 0;
  do {
    value = value >> 1;
    result++;
  } while (value);

  return result ? result - 1 : result;
}
/*-----------------------------------------------------------------------------------*/
static
uint32_t
http_parse_int_option(uint8_t *bytes, size_t length)
{
  uint32_t var = 0;
  size_t i = 0;
  while (i<length)
  {
    var <<= 8;
    var |= bytes[i++];
  }
  return var;
}
/*-----------------------------------------------------------------------------------*/
static
uint8_t
http_option_nibble(unsigned int value)
{
  if (value<13)
  {
    return value;
  }
  else if (value<=0xFF+13)
  {
    return 13;
  }
  else
  {
    return 14;
  }
}
/*-----------------------------------------------------------------------------------*/
static
size_t
http_set_option_header(unsigned int delta, size_t length, uint8_t *buffer)
{
  size_t written = 0;
  unsigned int *x = &delta;

  buffer[0] = http_option_nibble(delta)<<4 | http_option_nibble(length);

  /* avoids code duplication without function overhead */
  do
  {
    if (*x>268)
    {
      buffer[++written] = (*x-269)>>8;
      buffer[++written] = (*x-269);
    }
    else if (*x>12)
    {
      buffer[++written] = (*x-13);
    }
  }
  while (x!=(unsigned int *)&length && (x=(unsigned int *)&length));

  PRINTF("WRITTEN %u B opt header\n", written);

  return ++written;
}
/*-----------------------------------------------------------------------------------*/
static
size_t
http_serialize_int_option(unsigned int number, unsigned int current_number, uint8_t *buffer, uint32_t value)
{
  size_t i = 0;

  if (0xFF000000 & value) ++i;
  if (0xFFFF0000 & value) ++i;
  if (0xFFFFFF00 & value) ++i;
  if (0xFFFFFFFF & value) ++i;

  PRINTF("OPTION %u (delta %u, len %u)\n", number, number - current_number, i);

  i = http_set_option_header(number - current_number, i, buffer);

  if (0xFF000000 & value) buffer[i++] = (uint8_t) (value>>24);
  if (0xFFFF0000 & value) buffer[i++] = (uint8_t) (value>>16);
  if (0xFFFFFF00 & value) buffer[i++] = (uint8_t) (value>>8);
  if (0xFFFFFFFF & value) buffer[i++] = (uint8_t) (value);

  return i;
}
/*-----------------------------------------------------------------------------------*/
static
size_t
http_serialize_array_option(unsigned int number, unsigned int current_number, uint8_t *buffer, uint8_t *array, size_t length, char split_char)
{
  size_t i = 0;

  if (split_char!='\0')
  {
    size_t j;
    uint8_t *part_start = array;
    uint8_t *part_end = NULL;
    size_t temp_length;

    for (j = 0; j<=length; ++j)
    {
      if (array[j]==split_char || j==length)
      {
        part_end = array + j;
        temp_length = part_end-part_start;

        i += http_set_option_header(number - current_number, temp_length, &buffer[i]);
        memcpy(&buffer[i], part_start, temp_length);
        i += temp_length;

        PRINTF("OPTION type %u, delta %u, len %u, part [%.*s]\n", number, number - current_number, i, temp_length, part_start);

        ++j; /* skip the splitter */
        current_number = number;
        part_start = array + j;
      }
    } /* for */
  }
  else
  {
    i += http_set_option_header(number - current_number, length, &buffer[i]);
    memcpy(&buffer[i], array, length);
    i += length;

    PRINTF("OPTION type %u, delta %u, len %u\n", number, number - current_number, length);
  }

  return i;
}
/*-----------------------------------------------------------------------------------*/
static
size_t
http_serialize_multi_option(unsigned int number, unsigned int current_number, uint8_t *buffer, multi_option_t *array)
{
  size_t i = 0;
  multi_option_t * j;

  for (j = array; j != NULL; j= j->next)
  {
     i += http_set_option_header(number - current_number, j->len, &buffer[i]);
     current_number = number;
     memcpy(&buffer[i], j->data, j->len);
     i += j->len;
  } /* for */

  return i;
}
/*-----------------------------------------------------------------------------------*/
static
void
http_merge_multi_option(uint8_t **dst, size_t *dst_len, uint8_t *option, size_t option_len, char separator)
{
  /* Merge multiple options. */
  if (*dst_len > 0)
  {
    /* dst already contains an option: concatenate */
    (*dst)[*dst_len] = separator;
    *dst_len += 1;

    /* memmove handles 2-byte option headers */
    memmove((*dst)+(*dst_len), option, option_len);

    *dst_len += option_len;
  }
  else
  {
    /* dst is empty: set to option */
    *dst = option;
    *dst_len = option_len;
  }
}

void
http_add_multi_option(multi_option_t **dst, uint8_t *option, size_t option_len, uint8_t is_static)
{
  multi_option_t *opt = (multi_option_t *)lwm2m_malloc(sizeof(multi_option_t));

  if (opt)
  {
    opt->next = NULL;
    opt->len = (uint8_t)option_len;
    if (is_static)
    {
      opt->data = option;
      opt->is_static = 1;
    }
    else
    {
        opt->is_static = 0;
        opt->data = (uint8_t *)lwm2m_malloc(option_len);
        if (opt->data == NULL)
        {
            lwm2m_free(opt);
            return;
        }
        memcpy(opt->data, option, option_len);
    }

    if (*dst)
    {
      multi_option_t * i = *dst;
      while (i->next)
      {
        i = i->next;
      }
      i->next = opt;
    }
    else
    {
      *dst = opt;
    }
  }
}
#ifdef COAP_PROTOCOL
void
free_multi_option(multi_option_t *dst)
{
  if (dst)
  {
    multi_option_t *n = dst->next;
    dst->next = NULL;
    if (dst->is_static == 0)
    {
        lwm2m_free(dst->data);
    }
    lwm2m_free(dst);
    free_multi_option(n);
  }
}
#endif
static
char *
prv_http_get_multi_option_as_string(multi_option_t * option, char prefix, char delimiter)
{
    size_t len = 0;
    multi_option_t * opt;
    char * output;

    for (opt = option; opt != NULL; opt = opt->next)
    {
       len += opt->len + 1;     // for separator
    }

    output = (char*) lwm2m_malloc(len + 1); // for String terminator
    if (output != NULL)
    {
        size_t i = 0;

        for (opt = option; opt != NULL; opt = opt->next)
        {
            if (i == 0){
                output[i] = prefix;
            } else {
                output[i] = delimiter;
            }
            i += 1;

            memmove(output + i, opt->data, opt->len);
            i += opt->len;
        }
        output[i] = 0;
    }

    return output;
}

char * http_get_multi_option_as_path_string(multi_option_t * option)
{
    return prv_http_get_multi_option_as_string(option, '/', '/');
}

char * http_get_multi_option_as_query_string(multi_option_t * option)
{
    return prv_http_get_multi_option_as_string(option, '?', '&');
}


static
bool
is_non_std_http_port(uint16_t port)
{
    return !(port == 0 || port == 5683 || port == 5684);
}

static
int nDigits(unsigned x) {
    if (x >= 1000000000) return 10;
    if (x >= 100000000)  return 9;
    if (x >= 10000000)   return 8;
    if (x >= 1000000)    return 7;
    if (x >= 100000)     return 6;
    if (x >= 10000)      return 5;
    if (x >= 1000)       return 4;
    if (x >= 100)        return 3;
    if (x >= 10)         return 2;
    return 1;
}
char * http_get_packet_uri_as_string(http_packet_t * packet)
{
    // We do not know the destination or if it is secured with dtls or not therefor
    // we omit the protocol part as well as host and port if not in the options
    //
    char * output;
    
    char * path_string = http_get_multi_option_as_path_string(packet->uri_path);
    size_t path_len = strlen(path_string);
    char * query_string = http_get_multi_option_as_query_string(packet->uri_query);
    size_t query_len = strlen(query_string);

    size_t len = 2 * sizeof(char); // "//"
    len += packet->uri_host_len;
    len += is_non_std_http_port(packet->uri_port) ? (int)(nDigits(packet->uri_port)*sizeof(char)) : 0;
    len += 1 > path_len ? 1 : path_len; // "/" or path
    len += query_len;
    
    output = (char *) lwm2m_malloc(len + 1);
    if(output == NULL){
      return NULL;
    }
    
    strcpy(output, "//");
    strncat(output, (char *)packet->uri_host, packet->uri_host_len);
    if (1 > path_len)
    {
        strcat(output, "/");
    }
    else
    {
        strncat(output, path_string, path_len);
    }
    strncat(output, query_string, query_len);
    
    output[len] = 0;
    
    lwm2m_free(path_string);
    lwm2m_free(query_string);
    
    return output;
}

/*-----------------------------------------------------------------------------------*/

static
int
http_get_variable(const uint8_t *buffer, size_t length, const char *name, const char **output)
{
  const uint8_t *start = NULL;
  const uint8_t *end = NULL;
  const uint8_t *value_end = NULL;
  size_t name_len = 0;

  /*initialize the output buffer first*/
  *output = 0;

  name_len = strlen(name);
  end = buffer + length;

  for (start = buffer; start + name_len < end; ++start){
    if ((start == buffer || start[-1] == '&') && start[name_len] == '=' &&
        strncmp(name, (char *)start, name_len)==0) {

      /* Point start to variable value */
      start += name_len + 1;

      /* Point end to the end of the value */
      value_end = (const uint8_t *) memchr(start, '&', end - start);
      if (value_end == NULL) {
        value_end = end;
      }

      *output = (char *)start;

      return (value_end - start);
    }
  }

  return 0;
}

/*-----------------------------------------------------------------------------------*/
uint16_t
http_get_mid()
{
  return ++current_mid;
}
/*-----------------------------------------------------------------------------------*/
/*- MEASSAGE PROCESSING -------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
void
http_init_message(void *packet, http_message_type_t type, uint8_t code, uint16_t mid)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  /* Important thing */
  memset(http_pkt, 0, sizeof(http_packet_t));

  http_pkt->type = type;
  http_pkt->code = code;
  http_pkt->mid = mid;
}

void
http_free_header(void *packet)
{
    http_packet_t *const http_pkt = (http_packet_t *) packet;

    free_multi_option(http_pkt->uri_path);
    free_multi_option(http_pkt->uri_query);
    free_multi_option(http_pkt->location_path);
    http_pkt->uri_path = NULL;
    http_pkt->uri_query = NULL;
    http_pkt->location_path = NULL;
}

/*-----------------------------------------------------------------------------------*/
size_t http_serialize_get_size(void *packet)
{
    http_packet_t *const http_pkt = (http_packet_t *) packet;
    size_t length = 0;

    length = HTTP_HEADER_LEN + http_pkt->payload_len + http_pkt->token_len;

    if (IS_OPTION(http_pkt, HTTP_OPTION_IF_MATCH))
    {
        length += HTTP_MAX_OPTION_HEADER_LEN + http_pkt->if_match_len;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_URI_HOST))
    {
        length += HTTP_MAX_OPTION_HEADER_LEN + http_pkt->uri_host_len;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_ETAG))
    {
        length += HTTP_MAX_OPTION_HEADER_LEN + http_pkt->etag_len;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_IF_NONE_MATCH))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_OBSERVE))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_URI_PORT))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_LOCATION_PATH))
    {
        multi_option_t * optP;

        for (optP = http_pkt->location_path ; optP != NULL ; optP = optP->next)
        {
            length += HTTP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_URI_PATH))
    {
        multi_option_t * optP;

        for (optP = http_pkt->uri_path ; optP != NULL ; optP = optP->next)
        {
            length += HTTP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_CONTENT_TYPE))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_MAX_AGE))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_URI_QUERY))
    {
        multi_option_t * optP;

        for (optP = http_pkt->uri_query ; optP != NULL ; optP = optP->next)
        {
            length += HTTP_MAX_OPTION_HEADER_LEN + optP->len;
        }
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_ACCEPT))
    {
        length += http_pkt->accept_num * HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_LOCATION_QUERY))
    {
        length += HTTP_MAX_OPTION_HEADER_LEN + http_pkt->location_query_len;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_BLOCK2))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_BLOCK1))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_SIZE))
    {
        // can be stored in extended fields
        length += HTTP_MAX_OPTION_HEADER_LEN;
    }
    if (IS_OPTION(http_pkt, HTTP_OPTION_PROXY_URI))
    {
        length += HTTP_MAX_OPTION_HEADER_LEN + http_pkt->proxy_uri_len;
    }

    if (http_pkt->payload_len)
    {
        // Account for the payload marker.
        length += 1;
    }

    return length;
}

/*-----------------------------------------------------------------------------------*/
size_t
http_serialize_message(void *packet, uint8_t *buffer)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;
  uint8_t *option;
  unsigned int current_number = 0;

  /* Initialize */
  http_pkt->buffer = buffer;
  http_pkt->version = 1;

  PRINTF("-Serializing MID %u to %p, ", http_pkt->mid, http_pkt->buffer);

  /* set header fields */
  http_pkt->buffer[0]  = 0x00;
  http_pkt->buffer[0] |= HTTP_HEADER_VERSION_MASK & (http_pkt->version)<<HTTP_HEADER_VERSION_POSITION;
  http_pkt->buffer[0] |= HTTP_HEADER_TYPE_MASK & (http_pkt->type)<<HTTP_HEADER_TYPE_POSITION;
  http_pkt->buffer[0] |= HTTP_HEADER_TOKEN_LEN_MASK & (http_pkt->token_len)<<HTTP_HEADER_TOKEN_LEN_POSITION;
  http_pkt->buffer[1] = http_pkt->code;
  http_pkt->buffer[2] = (uint8_t) ((http_pkt->mid)>>8);
  http_pkt->buffer[3] = (uint8_t) (http_pkt->mid);

  /* set Token */
  PRINTF("Token (len %u)", http_pkt->token_len);
  option = http_pkt->buffer + HTTP_HEADER_LEN;
  for (current_number=0; current_number<http_pkt->token_len; ++current_number)
  {
    PRINTF(" %02X", http_pkt->token[current_number]);
    *option = http_pkt->token[current_number];
    ++option;
  }
  PRINTF("-\n");

  /* Serialize options */
  current_number = 0;

  PRINTF("-Serializing options at %p-\n", option);

  /* The options must be serialized in the order of their number */
  HTTP_SERIALIZE_BYTE_OPTION(   HTTP_OPTION_IF_MATCH,       if_match, "If-Match")
  HTTP_SERIALIZE_STRING_OPTION( HTTP_OPTION_URI_HOST,       uri_host, '\0', "Uri-Host")
  HTTP_SERIALIZE_BYTE_OPTION(   HTTP_OPTION_ETAG,           etag, "ETag")
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_IF_NONE_MATCH,  content_type-http_pkt->content_type, "If-None-Match") /* hack to get a zero field */
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_OBSERVE,        observe, "Observe")
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_URI_PORT,       uri_port, "Uri-Port")
  HTTP_SERIALIZE_MULTI_OPTION(  HTTP_OPTION_LOCATION_PATH,  location_path, "Location-Path")
  HTTP_SERIALIZE_MULTI_OPTION(  HTTP_OPTION_URI_PATH,       uri_path, "Uri-Path")
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_CONTENT_TYPE,   content_type, "Content-Format")
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_MAX_AGE,        max_age, "Max-Age")
  HTTP_SERIALIZE_MULTI_OPTION(  HTTP_OPTION_URI_QUERY,      uri_query, "Uri-Query")
  HTTP_SERIALIZE_ACCEPT_OPTION( HTTP_OPTION_ACCEPT,         accept, "Accept")
  HTTP_SERIALIZE_STRING_OPTION( HTTP_OPTION_LOCATION_QUERY, location_query, '&', "Location-Query")
  HTTP_SERIALIZE_BLOCK_OPTION(  HTTP_OPTION_BLOCK2,         block2, "Block2")
  HTTP_SERIALIZE_BLOCK_OPTION(  HTTP_OPTION_BLOCK1,         block1, "Block1")
  HTTP_SERIALIZE_INT_OPTION(    HTTP_OPTION_SIZE,           size, "Size")
  HTTP_SERIALIZE_STRING_OPTION( HTTP_OPTION_PROXY_URI,      proxy_uri, '\0', "Proxy-Uri")

  PRINTF("-Done serializing at %p----\n", option);

  /* Free allocated header fields */
  http_free_header(packet);

  /* Pack payload */
  /* Payload marker */
  if (http_pkt->payload_len)
  {
    *option = 0xFF;
    ++option;
  }

  memmove(option, http_pkt->payload, http_pkt->payload_len);

  PRINTF("-Done %u B (header len %u, payload len %u)-\n", http_pkt->payload_len + option - buffer, option - buffer, http_pkt->payload_len);

  PRINTF("Dump [0x%02X %02X %02X %02X  %02X %02X %02X %02X]\n",
      http_pkt->buffer[0],
      http_pkt->buffer[1],
      http_pkt->buffer[2],
      http_pkt->buffer[3],
      http_pkt->buffer[4],
      http_pkt->buffer[5],
      http_pkt->buffer[6],
      http_pkt->buffer[7]
    );

  return (option - buffer) + http_pkt->payload_len; /* packet length */
}
/*-----------------------------------------------------------------------------------*/
http_status_t
http_parse_message(void *packet, uint8_t *data, uint16_t data_len)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;
  uint8_t *current_option;
  uint32_t option_number = 0;
  uint32_t option_delta = 0;
  uint32_t option_length = 0;
  uint32_t *x;

  /* Initialize packet */
  memset(http_pkt, 0, sizeof(http_packet_t));

  /* pointer to packet bytes */
  http_pkt->buffer = data;

  /* parse header fields */
  http_pkt->version = (HTTP_HEADER_VERSION_MASK & http_pkt->buffer[0])>>HTTP_HEADER_VERSION_POSITION;
  http_pkt->type = (http_message_type_t) ((HTTP_HEADER_TYPE_MASK & http_pkt->buffer[0])>>HTTP_HEADER_TYPE_POSITION);
  http_pkt->token_len = MIN(HTTP_TOKEN_LEN, (HTTP_HEADER_TOKEN_LEN_MASK & http_pkt->buffer[0])>>HTTP_HEADER_TOKEN_LEN_POSITION);
  http_pkt->code = http_pkt->buffer[1];
  http_pkt->mid = http_pkt->buffer[2]<<8 | http_pkt->buffer[3];

  if (http_pkt->version != 1)
  {
    http_error_message = "http version must be 1";
    return BAD_REQUEST_4_00;
  }

  current_option = data + HTTP_HEADER_LEN;

  if (http_pkt->token_len != 0)
  {
      memcpy(http_pkt->token, current_option, http_pkt->token_len);
      SET_OPTION(http_pkt, HTTP_OPTION_TOKEN);

      PRINTF("Token (len %u) [0x%02X%02X%02X%02X%02X%02X%02X%02X]\n", http_pkt->token_len,
        http_pkt->token[0],
        http_pkt->token[1],
        http_pkt->token[2],
        http_pkt->token[3],
        http_pkt->token[4],
        http_pkt->token[5],
        http_pkt->token[6],
        http_pkt->token[7]
      ); /*FIXME always prints 8 bytes */
  }

  /* parse options */
  current_option += http_pkt->token_len;

  while (current_option < data+data_len)
  {
    /* Payload marker 0xFF, currently only checking for 0xF* because rest is reserved */
    if ((current_option[0] & 0xF0)==0xF0)
    {
      http_pkt->payload = ++current_option;
      http_pkt->payload_len = data_len - (http_pkt->payload - data);

      break;
    }

    option_delta = current_option[0]>>4;
    option_length = current_option[0] & 0x0F;
    ++current_option;

    /* avoids code duplication without function overhead */
    x = &option_delta;
    do
    {
      if (*x==13)
      {
        *x += current_option[0];
        ++current_option;
      }
      else if (*x==14)
      {
        *x += 255;
        *x += current_option[0]<<8;
        ++current_option;
        *x += current_option[0];
        ++current_option;
      }
    }
    while (x != &option_length && (x = &option_length));

    option_number += option_delta;

    if (current_option + option_length > data + data_len)
    {
        PRINTF("OPTION %u (delta %u, len %u) has invalid length.\n", option_number, option_delta, option_length);
        http_free_header(http_pkt);
        return BAD_REQUEST_4_00;
    }
    else
    {
        PRINTF("OPTION %u (delta %u, len %u): ", option_number, option_delta, option_length);
    }

    SET_OPTION(http_pkt, option_number);

    switch (option_number)
    {
      case HTTP_OPTION_CONTENT_TYPE:
        http_pkt->content_type = (http_content_type_t) http_parse_int_option(current_option, option_length);
        PRINTF("Content-Format [%u]\n", http_pkt->content_type);
        break;
      case HTTP_OPTION_MAX_AGE:
        http_pkt->max_age = http_parse_int_option(current_option, option_length);
        PRINTF("Max-Age [%lu]\n", http_pkt->max_age);
        break;
      case HTTP_OPTION_ETAG:
        http_pkt->etag_len = (uint8_t)(MIN(HTTP_ETAG_LEN, option_length));
        memcpy(http_pkt->etag, current_option, http_pkt->etag_len);
        PRINTF("ETag %u [0x%02X%02X%02X%02X%02X%02X%02X%02X]\n", http_pkt->etag_len,
          http_pkt->etag[0],
          http_pkt->etag[1],
          http_pkt->etag[2],
          http_pkt->etag[3],
          http_pkt->etag[4],
          http_pkt->etag[5],
          http_pkt->etag[6],
          http_pkt->etag[7]
        ); /*FIXME always prints 8 bytes */
        break;
      case HTTP_OPTION_ACCEPT:
        if (http_pkt->accept_num < HTTP_MAX_ACCEPT_NUM)
        {
          http_pkt->accept[http_pkt->accept_num] = http_parse_int_option(current_option, option_length);
          http_pkt->accept_num += 1;
          PRINTF("Accept [%u]\n", http_pkt->content_type);
        }
        break;
      case HTTP_OPTION_IF_MATCH:
        /*FIXME support multiple ETags */
        http_pkt->if_match_len = (uint8_t)(MIN(HTTP_ETAG_LEN, option_length));
        memcpy(http_pkt->if_match, current_option, http_pkt->if_match_len);
        PRINTF("If-Match %u [0x%02X%02X%02X%02X%02X%02X%02X%02X]\n", http_pkt->if_match_len,
          http_pkt->if_match[0],
          http_pkt->if_match[1],
          http_pkt->if_match[2],
          http_pkt->if_match[3],
          http_pkt->if_match[4],
          http_pkt->if_match[5],
          http_pkt->if_match[6],
          http_pkt->if_match[7]
        ); /*FIXME always prints 8 bytes */
        break;
      case HTTP_OPTION_IF_NONE_MATCH:
        http_pkt->if_none_match = 1;
        PRINTF("If-None-Match\n");
        break;

      case HTTP_OPTION_URI_HOST:
        http_pkt->uri_host = current_option;
        http_pkt->uri_host_len = option_length;
        PRINTF("Uri-Host [%.*s]\n", http_pkt->uri_host_len, http_pkt->uri_host);
        break;
      case HTTP_OPTION_URI_PORT:
        http_pkt->uri_port = http_parse_int_option(current_option, option_length);
        PRINTF("Uri-Port [%u]\n", http_pkt->uri_port);
        break;
      case HTTP_OPTION_URI_PATH:
        /* http_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        // http_merge_multi_option( (char **) &(http_pkt->uri_path), &(http_pkt->uri_path_len), current_option, option_length, 0);
        http_add_multi_option( &(http_pkt->uri_path), current_option, option_length, 1);
        PRINTF("Uri-Path [%.*s]\n", option_length, current_option);
        break;
      case HTTP_OPTION_URI_QUERY:
        /* http_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        // http_merge_multi_option( (char **) &(http_pkt->uri_query), &(http_pkt->uri_query_len), current_option, option_length, '&');
        http_add_multi_option( &(http_pkt->uri_query), current_option, option_length, 1);
        PRINTF("Uri-Query [%.*s]\n", option_length, current_option);
        break;

      case HTTP_OPTION_LOCATION_PATH:
        http_add_multi_option( &(http_pkt->location_path), current_option, option_length, 1);
        break;
      case HTTP_OPTION_LOCATION_QUERY:
        /* http_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        http_merge_multi_option( &(http_pkt->location_query), &(http_pkt->location_query_len), current_option, option_length, '&');
        PRINTF("Location-Query [%.*s]\n", option_length, current_option);
        break;

      case HTTP_OPTION_PROXY_URI:
        /*FIXME check for own end-point */
        http_pkt->proxy_uri = current_option;
        http_pkt->proxy_uri_len = option_length;
        /*TODO length > 270 not implemented (actually not required) */
        PRINTF("Proxy-Uri NOT IMPLEMENTED [%.*s]\n", http_pkt->proxy_uri_len, http_pkt->proxy_uri);
        http_error_message = "This is a constrained server (Contiki)";
        return PROXYING_NOT_SUPPORTED_5_05;

      case HTTP_OPTION_OBSERVE:
        http_pkt->observe = http_parse_int_option(current_option, option_length);
        PRINTF("Observe [%lu]\n", http_pkt->observe);
        break;
      case HTTP_OPTION_BLOCK2:
        http_pkt->block2_num = http_parse_int_option(current_option, option_length);
        http_pkt->block2_more = (http_pkt->block2_num & 0x08)>>3;
        http_pkt->block2_size = 16 << (http_pkt->block2_num & 0x07);
        http_pkt->block2_offset = (http_pkt->block2_num & ~0x0000000F)<<(http_pkt->block2_num & 0x07);
        http_pkt->block2_num >>= 4;
        PRINTF("Block2 [%lu%s (%u B/blk)]\n", http_pkt->block2_num, http_pkt->block2_more ? "+" : "", http_pkt->block2_size);
        break;
      case HTTP_OPTION_BLOCK1:
        http_pkt->block1_num = http_parse_int_option(current_option, option_length);
        http_pkt->block1_more = (http_pkt->block1_num & 0x08)>>3;
        http_pkt->block1_size = 16 << (http_pkt->block1_num & 0x07);
        http_pkt->block1_offset = (http_pkt->block1_num & ~0x0000000F)<<(http_pkt->block1_num & 0x07);
        http_pkt->block1_num >>= 4;
        PRINTF("Block1 [%lu%s (%u B/blk)]\n", http_pkt->block1_num, http_pkt->block1_more ? "+" : "", http_pkt->block1_size);
        break;
      case HTTP_OPTION_SIZE:
        http_pkt->size = http_parse_int_option(current_option, option_length);
        PRINTF("Size [%lu]\n", http_pkt->size);
        break;
      default:
        PRINTF("unknown (%u)\n", option_number);
        /* Check if critical (odd) */
        if (option_number & 1)
        {
          http_error_message = "Unsupported critical option";
          http_free_header(http_pkt);
          return BAD_OPTION_4_02;
        }
    }

    current_option += option_length;
  } /* for */
  PRINTF("-Done parsing-------\n");



  return NO_ERROR;
}
/*-----------------------------------------------------------------------------------*/
/*- REST FRAMEWORK FUNCTIONS --------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
int
http_get_query_variable(void *packet, const char *name, const char **output)
{
    /* unused parameters */
    (void)packet;
    (void)name;
    (void)output;
/*
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (IS_OPTION(http_pkt, HTTP_OPTION_URI_QUERY)) {
    return http_get_variable(http_pkt->uri_query, http_pkt->uri_query_len, name, output);
  }
*/
  return 0;
}

int
http_get_post_variable(void *packet, const char *name, const char **output)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (http_pkt->payload_len) {
    return http_get_variable(http_pkt->payload, http_pkt->payload_len, name, output);
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
http_set_status_code(void *packet, unsigned int code)
{
  if (code <= 0xFF)
  {
    ((http_packet_t *)packet)->code = (uint8_t) code;
    return 1;
  }
  else
  {
    return 0;
  }
}
/*-----------------------------------------------------------------------------------*/
/*- HEADER OPTION GETTERS AND SETTERS -----------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
unsigned int
http_get_header_content_type(void *packet)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_CONTENT_TYPE)) return -1;

  return http_pkt->content_type;
}

int
http_set_header_content_type(void *packet, unsigned int content_type)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->content_type = (http_content_type_t) content_type;
  SET_OPTION(http_pkt, HTTP_OPTION_CONTENT_TYPE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_accept(void *packet, const uint16_t **accept)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_ACCEPT)) return 0;

  *accept = http_pkt->accept;
  return http_pkt->accept_num;
}

int
http_set_header_accept(void *packet, uint16_t accept)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (http_pkt->accept_num < HTTP_MAX_ACCEPT_NUM)
  {
    http_pkt->accept[http_pkt->accept_num] = accept;
    http_pkt->accept_num += 1;

    SET_OPTION(http_pkt, HTTP_OPTION_ACCEPT);
  }
  return http_pkt->accept_num;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_max_age(void *packet, uint32_t *age)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_MAX_AGE)) {
    *age = HTTP_DEFAULT_MAX_AGE;
  } else {
    *age = http_pkt->max_age;
  }
  return 1;
}

int
http_set_header_max_age(void *packet, uint32_t age)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->max_age = age;
  SET_OPTION(http_pkt, HTTP_OPTION_MAX_AGE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_etag(void *packet, const uint8_t **etag)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_ETAG)) return 0;

  *etag = http_pkt->etag;
  return http_pkt->etag_len;
}

int
http_set_header_etag(void *packet, const uint8_t *etag, size_t etag_len)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->etag_len = (uint8_t)(MIN(HTTP_ETAG_LEN, etag_len));
  memcpy(http_pkt->etag, etag, http_pkt->etag_len);

  SET_OPTION(http_pkt, HTTP_OPTION_ETAG);
  return http_pkt->etag_len;
}
/*-----------------------------------------------------------------------------------*/
/*FIXME support multiple ETags */
int
http_get_header_if_match(void *packet, const uint8_t **etag)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_IF_MATCH)) return 0;

  *etag = http_pkt->if_match;
  return http_pkt->if_match_len;
}

int
http_set_header_if_match(void *packet, const uint8_t *etag, size_t etag_len)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->if_match_len = (uint8_t)(MIN(HTTP_ETAG_LEN, etag_len));
  memcpy(http_pkt->if_match, etag, http_pkt->if_match_len);

  SET_OPTION(http_pkt, HTTP_OPTION_IF_MATCH);
  return http_pkt->if_match_len;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_if_none_match(void *packet)
{
  return IS_OPTION((http_packet_t *)packet, HTTP_OPTION_IF_NONE_MATCH) ? 1 : 0;
}

int
http_set_header_if_none_match(void *packet)
{
  SET_OPTION((http_packet_t *)packet, HTTP_OPTION_IF_NONE_MATCH);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_token(void *packet, uint8_t **token)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_TOKEN)) return 0;

  *token = http_pkt->token;
  return http_pkt->token_len;
}

int
http_set_header_token(void *packet, const uint8_t *token, size_t token_len)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->token_len = (uint8_t)(MIN(HTTP_TOKEN_LEN, token_len));
  memcpy(http_pkt->token, token, http_pkt->token_len);

  SET_OPTION(http_pkt, HTTP_OPTION_TOKEN);
  return http_pkt->token_len;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_proxy_uri(void *packet, const char **uri)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_PROXY_URI)) return 0;

  *uri = (const char *)http_pkt->proxy_uri;
  return http_pkt->proxy_uri_len;
}

int
http_set_header_proxy_uri(void *packet, const char *uri)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->proxy_uri = (uint8_t *)uri;
  http_pkt->proxy_uri_len = strlen(uri);

  SET_OPTION(http_pkt, HTTP_OPTION_PROXY_URI);
  return http_pkt->proxy_uri_len;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_uri_host(void *packet, const char **host)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_URI_HOST)) return 0;

  *host = (char *)http_pkt->uri_host;
  return http_pkt->uri_host_len;
}

int
http_set_header_uri_host(void *packet, const char *host)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->uri_host = (uint8_t *)host;
  http_pkt->uri_host_len = strlen(host);

  SET_OPTION(http_pkt, HTTP_OPTION_URI_HOST);
  return http_pkt->uri_host_len;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_uri_path(void *packet, const char **path)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_URI_PATH)) return 0;

  *path = NULL; //http_pkt->uri_path;
  return 0; //http_pkt->uri_path_len;
}

int
http_set_header_uri_path(void *packet, const char *path)
{
  http_packet_t *http_pkt = (http_packet_t *) packet;
  int length = 0;

  free_multi_option(http_pkt->uri_path);
  http_pkt->uri_path = NULL;

  if (path[0]=='/') ++path;

  do
  {
      int i = 0;

      while (path[i] != 0 && path[i] != '/') i++;
      http_add_multi_option(&(http_pkt->uri_path), (uint8_t *)path, i, 0);

      if (path[i] == '/') i++;
      path += i;
      length += i;
  } while (path[0] != 0);

  SET_OPTION(http_pkt, HTTP_OPTION_URI_PATH);
  return length;
}

int
http_set_header_uri_path_segment(void *packet, const char *segment)
{
  http_packet_t *http_pkt = (http_packet_t *) packet;
  int length;

  if (segment == NULL || segment[0] == 0)
  {
      http_add_multi_option(&(http_pkt->uri_path), NULL, 0, 1);
      length = 0;
  }
  else
  {
      length = strlen(segment);
      http_add_multi_option(&(http_pkt->uri_path), (uint8_t *)segment, length, 0);
  }

  SET_OPTION(http_pkt, HTTP_OPTION_URI_PATH);
  return length;
}
/*-----------------------------------------------------------------------------------*/
uint16_t
http_get_header_uri_port(void *packet)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_URI_PORT)) return 0;

  return http_pkt->uri_port;
}

void
http_set_header_uri_port(void *packet, uint16_t port)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->uri_port = port;
  SET_OPTION(http_pkt, HTTP_OPTION_URI_PORT);
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_uri_query(void *packet, const char **query)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_URI_QUERY)) return 0;

  *query = NULL; //http_pkt->uri_query;
  return 0; //http_pkt->uri_query_len;
}

int
http_set_header_uri_query(void *packet, const char *query)
{
    int length = 0;
    http_packet_t *const http_pkt = (http_packet_t *) packet;

    free_multi_option(http_pkt->uri_query);
    http_pkt->uri_query = NULL;

    if (query[0]=='?') ++query;

    do
    {
        int i = 0;

        while (query[i] != 0 && query[i] != '&') i++;
        http_add_multi_option(&(http_pkt->uri_query), (uint8_t *)query, i, 0);

        if (query[i] == '&') i++;
        query += i;
        length += i;
    } while (query[0] != 0);

    SET_OPTION(http_pkt, HTTP_OPTION_URI_QUERY);
    return length;
 }

int
http_set_header_uri_query_segment(void *packet, const char *segment)
{
  http_packet_t *http_pkt = (http_packet_t *) packet;
  int length;

  if (segment == NULL || segment[0] == 0)
  {
      http_add_multi_option(&(http_pkt->uri_query), NULL, 0, 1);
      length = 0;
  }
  else
  {
      length = strlen(segment);
      http_add_multi_option(&(http_pkt->uri_query), (uint8_t *)segment, length, 0);
  }

  SET_OPTION(http_pkt, HTTP_OPTION_URI_QUERY);
  return length;
}

/*-----------------------------------------------------------------------------------*/
int
http_get_header_location_path(void *packet, const char **path)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_LOCATION_PATH)) return 0;

  *path = NULL; //http_pkt->location_path;
  return 0; //http_pkt->location_path_len;
}

int
http_set_header_location_path(void *packet, const char *path)
{
    http_packet_t *http_pkt = (http_packet_t *) packet;
    int length = 0;

    free_multi_option(http_pkt->location_path);
    http_pkt->location_path = NULL;

    if (path[0]=='/') ++path;

    do
    {
        int i = 0;

        while (path[i] != 0 && path[i] != '/') i++;
        http_add_multi_option(&(http_pkt->location_path), (uint8_t *)path, i, 0);

        if (path[i] == '/') i++;
        path += i;
        length += i;
    } while (path[0] != 0);

    SET_OPTION(http_pkt, HTTP_OPTION_LOCATION_PATH);
    return length;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_location_query(void *packet, const char **query)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_LOCATION_QUERY)) return 0;

  *query = (const char*)http_pkt->location_query;
  return http_pkt->location_query_len;
}

int
http_set_header_location_query(void *packet, char *query)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  while (query[0]=='?') ++query;

  http_pkt->location_query = (uint8_t *)query;
  http_pkt->location_query_len = strlen(query);

  SET_OPTION(http_pkt, HTTP_OPTION_LOCATION_QUERY);
  return http_pkt->location_query_len;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_observe(void *packet, uint32_t *observe)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_OBSERVE)) return 0;

  *observe = http_pkt->observe;
  return 1;
}

int
http_set_header_observe(void *packet, uint32_t observe)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->observe = 0x00FFFFFF & observe;
  SET_OPTION(http_pkt, HTTP_OPTION_OBSERVE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_block2(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_BLOCK2)) return 0;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = http_pkt->block2_num;
  if (more!=NULL) *more = http_pkt->block2_more;
  if (size!=NULL) *size = http_pkt->block2_size;
  if (offset!=NULL) *offset = http_pkt->block2_offset;

  return 1;
}

int
http_set_header_block2(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (size<16) return 0;
  if (size>2048) return 0;
  if (num>0x0FFFFF) return 0;

  http_pkt->block2_num = num;
  http_pkt->block2_more = more ? 1 : 0;
  http_pkt->block2_size = size;

  SET_OPTION(http_pkt, HTTP_OPTION_BLOCK2);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
int
http_get_header_block1(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_BLOCK1)) return 0;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = http_pkt->block1_num;
  if (more!=NULL) *more = http_pkt->block1_more;
  if (size!=NULL) *size = http_pkt->block1_size;
  if (offset!=NULL) *offset = http_pkt->block1_offset;

  return 1;
}

int
http_set_header_block1(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (size<16) return 0;
  if (size>2048) return 0;
  if (num>0x0FFFFF) return 0;

  http_pkt->block1_num = num;
  http_pkt->block1_more = more;
  http_pkt->block1_size = size;

  SET_OPTION(http_pkt, HTTP_OPTION_BLOCK1);
  return 1;
}

int
http_get_header_block(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
    if (1 == http_get_header_block1(packet, num, more, size, offset))
    {
        return 1;
    }

    return http_get_header_block1(packet, num, more, size, offset);
}


/*-----------------------------------------------------------------------------------*/
int
http_get_header_size(void *packet, uint32_t *size)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (!IS_OPTION(http_pkt, HTTP_OPTION_SIZE)) return 0;
  
  *size = http_pkt->size;
  return 1;
}

int
http_set_header_size(void *packet, uint32_t size)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->size = size;
  SET_OPTION(http_pkt, HTTP_OPTION_SIZE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/*- PAYLOAD -------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
int
http_get_payload(void *packet, const uint8_t **payload)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  if (http_pkt->payload) {
    *payload = http_pkt->payload;
    return http_pkt->payload_len;
  } else {
    *payload = NULL;
    return 0;
  }
}

int
http_set_payload(void *packet, const void *payload, size_t length)
{
  http_packet_t *const http_pkt = (http_packet_t *) packet;

  http_pkt->payload = (uint8_t *) payload;
  http_pkt->payload_len = (uint16_t)(length);

  return http_pkt->payload_len;
}
/*-----------------------------------------------------------------------------------*/
