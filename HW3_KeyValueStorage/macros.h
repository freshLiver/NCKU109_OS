#define DEBUG( format, ... ) printf ( "DEBUG >>> " format "------- DEBUG END\n\n", __VA_ARGS__ )

#define MAX_READ 1000

#define PUT 1
#define GET 2
#define SCAN 3

#define CType 0
#define PKey 1
#define PValue 2
#define GKey 1
#define SKey1 1
#define SKey2 2

typedef int CmdType;
typedef long long LL;