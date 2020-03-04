/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsmn.h"
#include "esp_system.h"

    /*
        { user: "johndoe",
          admin: false,
          uid: 1000, 
          groups:[
              "users", 
              "wheel", 
              "audio", 
              "video"],
         objects: {
             name: "lewin",
             site :"www.bilibili.com"}
        };
    */
static const char *JSON_STRING =
    "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000, \"groups\":[\"users\", \"wheel\", \"audio\", \"video\"],"
    "\"objects\": {\"name\": \"lewin\", \"site\" :\"www.bilibili.com\"}}";


static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if(tok->type == JSMN_STRING && 
       (int)strlen(s) == tok->end - tok->start &&
       strncmp(json + tok->start, s, tok->end - tok->start) == 0) 
    {
        return 0;
    }
    return -1;
}

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                 sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
        printf("Failed to parse JSON: %d\n", r);
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
    }

    for (i = 0; i < r; i++)
    {
        printf(" t %d = %.*s\n",i, t[i].end - t[i].start, JSON_STRING + t[i].start);
    }
    
    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++)
    {
        if (jsoneq(JSON_STRING, &t[i], "user") == 0)
        {
            /* We may use strndup() to fetch string value */
            printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,
                    JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "admin") == 0)
        {
            /* We may additionally check if the value is either "true" or "false" */
            printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
                JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "uid") == 0)
        {
            /* We may want to do strtol() here to get numeric value */
            printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
                    JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "groups") == 0)
        {
            int j;
            printf("- Groups:\n");
            if (t[i + 1].type != JSMN_ARRAY)
            {
                continue; /* We expect groups to be an array of strings */
            }
            for (j = 0; j < t[i + 1].size; j++)
            {
                jsmntok_t *g = &t[i + j + 2];
                printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
            }
            i += t[i + 1].size + 1;
        }
        else if (jsoneq(JSON_STRING, &t[i], "objects") == 0)
        {
            printf("- objects:\n");
            int length = t[i+1].size;
            i = i + 2;
            for (int k = 0; k < length; k++)
            {
                if (jsoneq(JSON_STRING, &t[i], "name") == 0)
                {
                    /* We may use strndup() to fetch string value */
                    printf(" - name: %.*s\n", t[i+1].end - t[i+1].start,
                            JSON_STRING + t[i+1].start);
                    i+=2;
                }
                else if (jsoneq(JSON_STRING, &t[i], "site") == 0)
                {
                    /* We may use strndup() to fetch string value */
                    printf(" - site: %.*s\n", t[i+1].end - t[i+1].start,
                            JSON_STRING + t[i+1].start);
                    i+=2;
                }
                else
                {
                    printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                            JSON_STRING + t[i].start);
                }
            }
        }
        else
        {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                    JSON_STRING + t[i].start);
        }
    }
}
