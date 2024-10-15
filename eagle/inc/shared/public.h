/* *****************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * eagle licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: luocaimin
 * Create: 2022-03-29
 * Description: provide const value defination
 * **************************************************************************** */
#ifndef EAGLE_PUBLIC_H__
#define EAGLE_PUBLIC_H__

#define FOUND 0
#define NOT_FOUND (-1)
#define FAILED (-1)
#define DEFAULT_VAL 0
#define EMPTY_FD (-1)
#define MAX_PEDDING_SOCKS 5
#define MS_TO_SEC 1000
#define ONE_HUNDRED 100

// Status code defination
#define INTERACT_SUCCEED 2003
#define INTERACT_FAIL 2002
#define NO_PROCESS 2000
#define PROCESS_EXISTED 2001
#define GET_UNKNOWN_TYPE 2005
#define GET_NO_DATA 2006
#define WRONG_TIME_RANGE 1303
#define IMPORT_WRONG_DIR_DATA 2007
#define IMPORT_WRONG_FORMAT_DATA 2008
#define CRC_FAIL 1202
#define UNKNOWN_CMD 2009
#define WRONG_BODY_LEN 2010
#define WRONG_PATH 1300
#define WRONG_FORMAT 1301
#define WRONG_VALUE 1302
#define LACK_PARAM 1304
#define UNKNOWN_PARAM 1305
#define NO_PERMISSION 1201
#define OVER_GET_DT_LEN 2010


// Max filename length
#define MAX_NAME_LEN 128
// Max MD5 length
#define MD5_LEN 33


// Max line length
#define MAX_LINE_LENGTH 256
// Max type collect items
#define MAX_TYPE_ITEM_CNT 500
// Max section name length
#define MAX_SECTION 23
// Max config name length
#define MAX_NAME 256
// Max config value length
#define MAX_VALUE 129
// Max key value length
#define MAX_KEY_LEN 32
// Max config param value
#define MAX_PARAM_VAL 128
// Max config data name
#define MAX_DT_NAME 60
// Max section name length
#define MAX_SEC_NAME 17
// Max section name length
#define MAX_FILE_NAME 178
#define MAX_PATH_NAME 128
#define MAX_FULL_NAME (MAX_FILE_NAME + MAX_PATH_NAME)
// Comment char
#define COMMENT_PREFIXES "#"
// Max config line length
#define MAX_LINE_NUM 3000
// Max log head length
#define MAX_LOG_LINE (MAX_LINE_NUM + MAX_STD_TIME)
// MAX time length
#define MAX_STD_TIME 32
#define MAX_FULL_TIME 25
// Gather
#define MAX_PERIOD 3600
// Sockt connetion closed
#define CONN_CLOSED (-753462)
#define READ_DONE 1
// Total number of items collected
#define TOTAL_COLL_ITEM_NUM 400
// The maximum length of the collected value
#define MAX_COLL_DATA 120
// Time value length
#define TIME_SEC_LEN 20
#define FILE_TIME_LEN 13
#define MAX_REGEX 64
#define MAX_FILE_LINE 256
// Maximum persistent data length
#define UNIT_FACTOR 1048576
#define NEED_SWITCH 1
#define NO_NEED_SWITCH 0
#define OPENED 1
#define CLOSED 0
#define READ_END "nonono"
// Data read buffer length
#define MAX_READ_BUFF 1024
#define MAX_CP_BUF 1024
#define CRT_DIR_MODE 0700
#define CURRENT_DIR "."
#define PARENT_DIR ".."
#define K_TO_M 1024
// Collected data name separator
#define DATA_NM_SEP '.'
#define DT_NM_SEP_STR "."
#define JIFF_FACTOR 100
// Cpu node name prefix length
#define CPU_NM_PX_LEN 7
#define CPU_USG_LINE_NUM 1
#define CPU_TOTAL_SECS_FIELD_NUM 1
#define CPU_IDLE_SECS_FIELD_NUM 2
#define SIGLE_CPU_IDLE_SECS_NUM 5
#define CPU_LOAD_LINE_NUM 1
#define CPU_ONE_MIN_LOAD_FIELD_NUM 1
#define CPU_FIVE_MIN_LOAD_FIELD_NUM 2
#define CPU_FIFTEEN_MIN_LOAD_FIELD_NUM 3

// Memery data location define
#define FIRST_MATCH_LINE 1
#define MEMINFO_DATA_FIELD_NUM 2
#define NUMA_MEM_DATA_FIELD_NUM 4

// Numa node name perfix length
#define NUMA_NM_PX_LEN 13
#define ZERO_USAGE "0"
#define ST_CHANGE "2"
#define FLOAT_ZERO_USAGE "0.00"
#define DAY_SECS 86400
#define FAILED_EXIT (-1)
#define EMPTY_STR ""

// Persistence module initialization state definition
#define PST_INIT_DONE 1
#define PST_NOT_INIT 0
#define TYPE_FIELD_NUM 1
#define TIME_FIELD_NUM 2
#define DT_NAME_FIELD_NUM 3
#define DT_TIME_FIELD_NUM 2
#define TAIL_LEN_FACTOR 2
#define BUF_WARN_RATE 2
#define ERR_CFG_NAME "ERR_CFG_NAME"

// Message related definition
#define MAX_HEAD_LEN 256
#define PATH_SEP_STR "/"
#define PATH_SEP_CHAR '/'
#define CPU_NM_PRX "cpu"
#define CPU_NM_LEN 8
#define CRC_TB_LEN 256
#define CRC_FACTOR 0xFFFFFFFF
#define CRC_ERROR (-489)
#define CRC_FILE_BUF_LEN 102400
#define SEC_TO_USEC 1000000

#define APP_ARGC 2
#define CRC_RIGHT_SHIFT_BIT 8

#define MIN_DT_FILE_SIZE 1
#define MAX_DT_FILE_SIZE 1024
#define MIN_SPACE_SIZE 1
#define MAX_SPACE_SIZE 10240
#define MIN_DEL_RATIO 1
#define MAX_DEL_RATIO 50
#define MIN_DT_FILE_DURATION 1
#define MAX_DT_FILE_DURATION 100

#define MIN_LOG_FILE_SIZE 1
#define MAX_LOG_FILE_SIZE 100
#define MIN_LOG_CMP_CNT 1
#define MAX_LOG_CMP_CNT 1000
#define MAX_LOG_FILE_SIZE 100
#define MAX_LOG_TOTAL_SIZE 10000
#define MIN_LOG_LEVEL 0
#define MAX_LOG_LEVEL 3
#define MIN_SV_PORT 1025
#define MAX_SV_PORT 65535
#define MAX_FS_TYPE_CNT 64

#define MIN_GTH_ITV 1
#define MAX_GTH_ITV 5
#define MAX_CT_LEN 104857600

#define MIN_COLL_DURATION 1
#define MAX_COLL_DURATION MAX_PERIOD

#define STR_LEN_FOR_LONG 22
#define WRONG_OBJ "coll_item_not_existed"

#define NO_MAP "NO_MAP"
#define ERR_VAL "-1"

enum RunStatus {
    EXIT = 0,
    KEEP_RUN
};

#define INVALID_FD (-1)
#define INVALID_INDEX (-1)
#define MAX_LICENT_NUM 3
#define THREAD_LOOP_INTERVAL 2000 // us
#define MAX_SYSID_LEN 20
#define TRUE 1
#define FALSE 0

enum RtnCode {
    SUCCESS = 0,
    ERR_COMMON = 1,
    ERR_TIMEOUT,
    ERR_SYS_EXCEPTION,
    ERR_NULL_POINTER,
    ERR_INVALIDE_PARAM,
    ERR_MODIFY_BAN_UPDATE_ATTR_CURRENTLY,
    ERR_PATH_NORMALIZE,
    ERR_PATH_VERIFY,
    ERR_NO_SERVICE_AVAILABLE,
    ERR_NOT_REGISTED = 100,
    ERR_OVER_MAX_CONNECTION,
    ERR_DISCONNECTED = 300,
    ERR_INVOKE_PWRAPI_FAILED,

    ERR_FILE_NOT_EXIST = 400,
    ERR_FILE_CONTENT_ERROR,
    ERR_DL_OPEN_FAILED,
    ERR_SERVICE_NOT_LOAD,
};

enum LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

enum EXIT_MODE {
    EXIT_MODE_RESTORE = 0,
    EXIT_MODE_KEEP_STATUS = 1
};

#endif
