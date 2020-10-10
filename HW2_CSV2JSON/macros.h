// clang-format off

// debug macros
#define DEBUG( format, ... ) printf ( "DEBUG >> " format "\n", __VA_ARGS__ )

#define COST_TIME( str ) DEBUG ( "%s cost : %lu secs.", str, time ( NULL ) - start )

#define RESET_TIME( ) start = time ( NULL )

#define INDENT "    "

#define SUPER_BRUTE_FORCE_JSON_FORMAT \
"{\n" \
INDENT "\"col_1\":%s,\n"    \
INDENT "\"col_2\":%s,\n"    \
INDENT "\"col_3\":%s,\n"    \
INDENT "\"col_4\":%s,\n"    \
INDENT "\"col_5\":%s,\n"    \
INDENT "\"col_6\":%s,\n"    \
INDENT "\"col_7\":%s,\n"    \
INDENT "\"col_8\":%s,\n"    \
INDENT "\"col_9\":%s,\n"    \
INDENT "\"col_10\":%s,\n"   \
INDENT "\"col_11\":%s,\n"   \
INDENT "\"col_12\":%s,\n"   \
INDENT "\"col_13\":%s,\n"   \
INDENT "\"col_14\":%s,\n"   \
INDENT "\"col_15\":%s,\n"   \
INDENT "\"col_16\":%s,\n"   \
INDENT "\"col_17\":%s,\n"   \
INDENT "\"col_18\":%s,\n"   \
INDENT "\"col_19\":%s,\n"   \
INDENT "\"col_20\":%s\n"    \
"},\n"

/**
 *  if use this macro
 *  every line is regex: INDENT"col_\d{1,2}":-?\d{1,10},\n
 *  so, at most needs : xxxx"col_xx":-1234567890,\n ==> 26 chars evey line
 *  and there are 20 elements, so we need char[520] for each cell ==> char[550]
 * */


// params for JSON FORMAT ABOVE
#define SUPER_BRUTE_FORCE_FORMAT_PARAMS \
list[0].c_str(), list[1].c_str(), list[2].c_str(), list[3].c_str(), list[4].c_str(),\
list[5].c_str(), list[6].c_str(), list[7].c_str(), list[8].c_str(), list[9].c_str(),\
list[10].c_str(), list[11].c_str(), list[12].c_str(), list[13].c_str(), list[14].c_str(),\
list[15].c_str(), list[16].c_str(), list[17].c_str(), list[18].c_str(), list[19].c_str()


// clang-format on