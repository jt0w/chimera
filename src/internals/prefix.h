#ifndef _CHIMERA_PREFIX_H_
#define _CHIMERA_PREFIX_H_

#ifdef CHIMERA_STRIP_PREFIX
#define todo             chimera_todo
#define log              chimera_log
#define shift            chimera_shift
#define array_len        chimera_array_len
#define da_len           chimera_da_len
#define da_get           chimera_da_get
#define da_push          chimera_da_push
#define da_push_buf      chimera_da_push_buf
#define da_push_mult     chimera_da_push_mult
#define da_free          chimera_da_free
#define LogLevel         Chimera_LogLevel
#define StringBuilder    Chimera_StringBuilder
#define sb_from_string   chimera_sb_from_string
#define StringView       Chimera_StringView
#define sv_from_string   chimera_sv_from_string
#define sv_from_data     chimera_sv_from_data
#define sv_trim_left     chimera_sv_trim_left
#define sv_trim_left     chimera_sv_trim_left
#define sv_trim          chimera_sv_trim
#define sv_cmp           chimera_sv_cmp
#define sv_from_sb       chimera_sv_from_sb
#define StringViews      Chimera_StringViews
#define sv_split_by_char chimera_sv_split_by_char
#define Cmd              Chimera_Cmd
#define cmd_push         chimera_cmd_push
#define cmd_exec         chimera_cmd_exec
#define rebuild_file     chimera_rebuild_file
#define read_file        chimera_read_file
#define create_dir       chimera_create_dir
#define file_exists      chimera_file_exists
#define temp_alloc       chimera_temp_alloc
#define temp_sprintf     chimera_temp_sprintf
#define temp_reset       chimera_temp_reset
#define COLOR_TEXT       CHIMERA_COLOR_TEXT
#define COLOR_ERROR      CHIMERA_COLOR_ERROR
#define COLOR_INFO       CHIMERA_COLOR_INFO
#define COLOR_WARN       CHIMERA_COLOR_WARN
#define COLOR_TRACE      CHIMERA_COLOR_TRACE
#endif

#endif // _CHIMERA_PREFIX_H_
