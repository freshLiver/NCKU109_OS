// clang-format off

// debug macros
#define DEBUG( format, ... ) printf ( "DEBUG >> " format "\n", __VA_ARGS__ )

#define COST_TIME( str ) DEBUG ( "%s cost : %lu secs.", str, time ( NULL ) - start )

#define RESET_TIME( ) start = time ( NULL )

#define INDENT "    "

#define SUPER_BRUTE_FORCE_JSON_FORMAT \
"{\n" \
"\"col_1\":%s,\n"    \
"\"col_2\":%s,\n"    \
"\"col_3\":%s,\n"    \
"\"col_4\":%s,\n"    \
"\"col_5\":%s,\n"    \
"\"col_6\":%s,\n"    \
"\"col_7\":%s,\n"    \
"\"col_8\":%s,\n"    \
"\"col_9\":%s,\n"    \
"\"col_10\":%s,\n"   \
"\"col_11\":%s,\n"   \
"\"col_12\":%s,\n"   \
"\"col_13\":%s,\n"   \
"\"col_14\":%s,\n"   \
"\"col_15\":%s,\n"   \
"\"col_16\":%s,\n"   \
"\"col_17\":%s,\n"   \
"\"col_18\":%s,\n"   \
"\"col_19\":%s,\n"   \
"\"col_20\":%s\n"    \
"}"

/**
 *  if use this macro
 *  every line is regex: "col_\d{1,2}":-?\d{1,10},\n
 *  so, at most needs : "col_xx":-1234567890,\n ==> 22 chars evey line
 *  and there are 20 elements, so we need char[440] for each cell ==> char[450]
 * */


// params for JSON FORMAT ABOVE
#define SUPER_BRUTE_FORCE_FORMAT_PARAMS \
list[0].c_str(), list[1].c_str(), list[2].c_str(), list[3].c_str(), list[4].c_str(),\
list[5].c_str(), list[6].c_str(), list[7].c_str(), list[8].c_str(), list[9].c_str(),\
list[10].c_str(), list[11].c_str(), list[12].c_str(), list[13].c_str(), list[14].c_str(),\
list[15].c_str(), list[16].c_str(), list[17].c_str(), list[18].c_str(), list[19].c_str()


// clang-format on