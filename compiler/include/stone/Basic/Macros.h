#ifndef STONE_BASIC_MACROS_H
#define STONE_BASIC_MACROS_H

#ifndef STONE_BUG_REPORT_URL
#define STONE_BUG_REPORT_URL                                                   \
  "https://stonelang.org/contributing/#reporting-bugs"
#endif

#define STONE_BUG_REPORT_MESSAGE_BASE                                          \
  "Submit a bug report (" STONE_BUG_REPORT_URL ") and include the project"

#define STONE_BUG_REPORT_MESSAGE "please " STONE_BUG_REPORT_MESSAGE_BASE

#define STONE_CRASH_BUG_REPORT_MESSAGE                                         \
  "Please " STONE_BUG_REPORT_MESSAGE_BASE " and the crash backtrace."

#endif