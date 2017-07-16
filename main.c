#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>
#include <json-c/json.h>

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */
    // char *header = userdata;
    // printf("%s\n", userdata);
    return nitems * size;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main() {
    for (int j = 1; j < 94; ++j) {
        CURL *curl;
        CURLcode res;

        struct string s;
        init_string(&s);

        json_object *json;                                      /* json post body */
        enum json_tokener_error jerr = json_tokener_success;    /* json parse error */


        curl = curl_easy_init();
        if (curl) {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, "Accept: application/vnd.github.v3+json");
            chunk = curl_slist_append(chunk, "User-Agent: libcurl");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

            char page[64];
            sprintf(page, "%d", j);

            curl_easy_setopt(curl, CURLOPT_URL, concat("https://api.github.com/user/starred?per_page=300&page=", page));
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long) CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERNAME, "FlowerWrong");
            curl_easy_setopt(curl, CURLOPT_PASSWORD, "changeme");

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            json = json_tokener_parse_verbose(s.ptr, &jerr);
            free(s.ptr);

            if (jerr != json_tokener_success) {
                /* error */
                fprintf(stderr, "ERROR: Failed to parse json string");
                /* free json object */
                json_object_put(json);
                return -1;
            }

            enum json_type type = json_object_get_type(json);
            printf("type: %d\n", type);
            switch (type) {
                case json_type_null:
                    printf("json_type_null\n");
                    break;
                case json_type_boolean:
                    printf("json_type_boolean\n");
                    break;
                case json_type_double:
                    printf("json_type_double\n");
                    break;
                case json_type_int:
                    printf("json_type_int\n");
                    break;
                case json_type_object:
                    printf("json_type_object\n");
                    break;
                case json_type_array:
                    printf("json_type_array\n");
                    int arraylen = json_object_array_length(json);
                    printf("Array Length: %d\n", arraylen);
                    int i;
                    json_object *jvalue;
                    char *key;
                    struct json_object *val;
                    for (i = 0; i < arraylen; i++) {
                        jvalue = json_object_array_get_idx(json, i);

                        // json_object *github_repo_full_name;
                        json_object_object_foreach(jvalue, key, val) {
                            type = json_object_get_type(val);
                            switch (type) {
                                case json_type_string:
                                    if (strcmp(key, "full_name") == 0) {
                                        sleep(0.01L);
                                        char *url = concat("https://api.github.com/user/starred/",
                                                           json_object_get_string(val));

                                        CURL *unstar_curl;
                                        CURLcode unstar_res;
                                        unstar_curl = curl_easy_init();
                                        curl_easy_setopt(unstar_curl, CURLOPT_HTTPHEADER, chunk);
                                        curl_easy_setopt(unstar_curl, CURLOPT_FOLLOWLOCATION, 1L);

                                        curl_easy_setopt(unstar_curl, CURLOPT_HTTPAUTH, (long) CURLAUTH_BASIC);
                                        curl_easy_setopt(unstar_curl, CURLOPT_USERNAME, "FlowerWrong");
                                        curl_easy_setopt(unstar_curl, CURLOPT_PASSWORD, "changeme");
                                        curl_easy_setopt(unstar_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                                        curl_easy_setopt(unstar_curl, CURLOPT_URL, url);
                                        curl_easy_setopt(unstar_curl, CURLOPT_WRITEFUNCTION, NULL);
                                        curl_easy_setopt(unstar_curl, CURLOPT_WRITEDATA, NULL);

                                        /* Perform the request, res will get the return code */
                                        unstar_res = curl_easy_perform(unstar_curl);
                                        /* Check for errors */
                                        if (unstar_res != CURLE_OK)
                                            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                                    curl_easy_strerror(unstar_res));

                                        /* always cleanup */
                                        curl_easy_cleanup(unstar_curl);
                                        free(url);
                                    }
                            }
                        }
                    }
                    break;
                case json_type_string:
                    printf("json_type_string\n");
                    break;
            }

            /* always cleanup */
            curl_easy_cleanup(curl);
            /* free the custom headers */
            curl_slist_free_all(chunk);
        }
    }
    return 0;
}