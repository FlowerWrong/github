//
// Created by king Yang on 2017/7/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>

#include "sds.h"

int main() {
    sds mystring = sdsnew("Hello World!");
    printf("%s\n", mystring);
    sdsfree(mystring);
    return 0;
}
