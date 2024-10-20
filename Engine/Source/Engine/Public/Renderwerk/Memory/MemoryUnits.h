#pragma once

#define RW_BYTE(x) x

#define RW_KIBIBYTE(x) (x / 1024ULL)
#define RW_MEBIBYTE(x) (x / 1024ULL / 1024ULL)
#define RW_GIBIBYTE(x) (x / 1024ULL / 1024ULL / 1024ULL)
#define RW_TEBIBYTE(x) (x / 1024ULL / 1024ULL / 1024ULL / 1024ULL)

#define RW_KILOBYTE(x) (x / 1000ULL)
#define RW_MEGABYTE(x) (x / 1000ULL / 1000ULL)
#define RW_GIGABYTE(x) (x / 1000ULL / 1000ULL / 1000ULL)
#define RW_TERABYTE(x) (x / 1000ULL / 1000ULL / 1000ULL / 1000ULL)
