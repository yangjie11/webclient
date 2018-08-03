/*
 *  File      : webclient_post_sample.c
 *  COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-03    chenyong      the first version
 */

#include <string.h>

#include <rtthread.h>
#include <webclient.h>

#define POST_RESP_BUFSZ                1024
#define POST_HEADER_BUFSZ              1024

#define POST_LOCAL_URI                 "http://www.rt-thread.com/service/echo"

const char *post_data = "RT-Thread is an open source IoT operating system from China!";

int webclient_post_test(int argc, char **argv)
{
    struct webclient_session* session = RT_NULL;
    unsigned char *buffer = RT_NULL;
    char *header = RT_NULL;
    char *URI = RT_NULL;
    int index, ret = 0;
    int bytes_read;

    if (argc == 1)
    {
        URI = web_strdup(POST_LOCAL_URI);
    }
    else if (argc == 2)
    {
        URI = web_strdup(argv[1]);
    }
    else
    {
        LOG_E("webclient_post_test [URI]  - webclient POST request test.");
        return -1;
    }

    header = (char *) web_malloc(POST_HEADER_BUFSZ);
    if (header == RT_NULL)
    {
        LOG_E("no memory for header data.");
        ret = -RT_ENOMEM;
        goto __exit;

    }

    buffer = (unsigned char *) web_malloc(POST_RESP_BUFSZ);
    if (buffer == RT_NULL)
    {
        LOG_E("no memory for receive response buffer.");
        ret = -RT_ENOMEM;
        goto __exit;

    }

    /* build header for upload */
    rt_snprintf(header, POST_HEADER_BUFSZ, "Content-Length: %d\r\n", strlen(post_data));
    rt_snprintf(header, POST_HEADER_BUFSZ, "%sContent-Type: application/octet-stream\r\n", header);

    /* create webclient session and set header response size */
    session = webclient_session_create(POST_HEADER_BUFSZ, POST_RESP_BUFSZ);
    if (session == RT_NULL)
    {
        ret = -RT_ENOMEM;
        goto __exit;
    }

    /* send POST request by default header */
    if (webclient_post(session, URI, header, post_data) != 200)
    {
        LOG_E("webclient POST request failed, response(%d) error.", session->response);
        ret = -RT_ERROR;
        goto __exit;
    }

    LOG_I("webclient POST request response data :");
    do
    {
        bytes_read = webclient_read(session, buffer, POST_RESP_BUFSZ);
        if (bytes_read <= 0)
        {
            break;
        }

        for (index = 0; index < bytes_read; index++)
        {
            rt_kprintf("%c", buffer[index]);
        }
    } while (1);

    rt_kprintf("\n");

__exit:
    if (session)
    {
        webclient_close(session);
    }

    if (buffer)
    {
        web_free(buffer);
    }

    if (header)
    {
        web_free(header);
    }

    if (URI)
    {
        web_free(URI);
    }

    return ret;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(webclient_post_test, web_post_test, webclient_post_test [URI]  - webclient POST request test.);
#endif /* FINSH_USING_MSH */
