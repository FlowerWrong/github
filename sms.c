//
// Created by king Yang on 2017/7/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "sds.h"
#include "csvparser.h"

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main() {
    sds mystring = sdsnew("Hello World!");
    printf("%s\n", mystring);
    sdsfree(mystring);


    CURL *curl;
    CURLcode res;


    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Accept: application/xml");
        chunk = curl_slist_append(chunk, "User-Agent: libcurl");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);


        //                                   file, delimiter, first_line_is_header?
        CsvParser *csvparser = CsvParser_new(".csv", ",", 0);
        CsvRow *row;

        char *baseurl = "http://sms.ue35.net/sms/interface/sendmess.htm?username=xxx&userpwd=xxx&mobilecount=1";

        while ((row = CsvParser_getRow(csvparser))) {
            printf("==NEW LINE==\n");
            const char **rowFields = CsvParser_getFields(row);


            char *url = concat(baseurl, "&mobiles=");
            url = concat(url, rowFields[0]);
            url = concat(url, "&content=");

            // char *content = "尊敬的力浦商家：您的后台账户已经开通，登陆账号：13431069247 登陆密码：069247（登录后请尽快修改密码）。苹果手机请在APP store搜索下载“力浦净水商家”，安卓手机各大市场下载“力浦净水商家”登录，操作中如有疑问，请联系力浦小丽：18122348705（微信同号）祝您生意兴隆！";
            char *content = concat("尊敬的力浦商家：您的后台账户已经开通，登陆账号：", rowFields[0]);
            content = concat(content, " 登陆密码：");
            content = concat(content, rowFields[1]);
            content = concat(content,
                             "（登录后请尽快修改密码）。苹果手机请在APP store搜索下载“力浦净水商家”，安卓手机各大市场下载“力浦净水商家”登录，操作中如有疑问，请联系力浦小丽：18122348705（微信同号）祝您生意兴隆！");

            char *output = curl_easy_escape(curl, content, strlen(content));

            url = concat(url, output);

            printf("url %s\n", url);

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            printf("\n");
            curl_free(output);
            free(url);
            free(content);


            printf("\n");
            CsvParser_destroy_row(row);
        }
        CsvParser_destroy(csvparser);

        /* always cleanup */
        curl_easy_cleanup(curl);
        /* free the custom headers */
        curl_slist_free_all(chunk);
    }

    return 0;
}
