#ifndef COMMON_UUID_H
#define COMMON_UUID_H

int parse_uuid(unsigned char uuid[16], const char *text);
int parse_hex_uuid(unsigned char uuid[16], const char *text);
void format_uuid(char text[37], const unsigned char uuid[16]);
int canonicalize_uuid(char uuid_text[37]);

int uuid5(char uuid[37], const char nsuuid[37], const void *name, size_t size);

#endif

