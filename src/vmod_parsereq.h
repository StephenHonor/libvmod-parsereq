#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <cache/cache_varnishd.h>
#include "vcl.h"
#include "vre.h"

#include "vsb.h"
#include "vtim.h"
#include "vcc_parsereq_if.h"
#include "vmod_abi.h"


#define POST_REQ_HDR "\024X-VMOD-PARSEREQ-PTR:"
#define POST_REQ_HDR_NAME "X-VMOD-PARSEREQ-PTR"



//#define DEBUG_HTCREAD
//#define DEBUG_SYSLOG

//////////////////////////////////////////
//Compatible use for HTC_Read
static int type_htcread = 0;
//HTC_Read ~3.0.2
typedef ssize_t HTC_READ302(struct http_conn *htc, void *d, size_t len);

//HTC_Read 3.0.3~
typedef ssize_t HTC_READ303(struct worker *w, struct http_conn *htc, void *d, size_t len);

//////////////////////////////////////////
//Hook
static unsigned           hook_done          = 0;
static vcl_func_f         *vmod_Hook_miss    = NULL;
static vcl_func_f         *vmod_Hook_pass    = NULL;
static vcl_func_f         *vmod_Hook_pipe    = NULL;
static vcl_func_f         *vmod_Hook_deliver = NULL;
static vcl_func_f         *vmod_Hook_error   = NULL;

static pthread_mutex_t    vmod_mutex = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////
//Debug
static unsigned           is_debug           = 0;

//////////////////////////////////////////
//for internal head
enum VMODREQ_TYPE { POST, GET, COOKIE, REQ, AUTO, NONE};

struct hdr {
	char *key;
	char *value;
	int  size;
	unsigned bin;
	unsigned array;
	VTAILQ_ENTRY(hdr) list;
};

struct vmod_headers {
	unsigned			magic;
#define VMOD_HEADERS_MAGIC 0x8d4d29ac
//	unsigned value_enabled;
	char *seek;
//	int count;//まだ値入れてないのであとで入れる
	VTAILQ_HEAD(, hdr) headers;
};



struct vmod_request {
	unsigned			magic;
#define VMOD_REQUEST_MAGIC 0x8d4f21af
	struct vmod_headers* post;
	struct vmod_headers* get;
	struct vmod_headers* cookie;
	
	struct vmod_headers* hdr_req;

	unsigned init_req;
	char seek_tmp[256];
	
	int  parse_ret;
	
	char *raw_post;
	int  size_post;

	char *raw_get;
	int  size_get;
	
	char *raw_cookie;
	int  size_cookie;

	unsigned  opt_post_lookup;
	
	enum VMODREQ_TYPE nowtype;
};



////////////////////////////////////////////////////
//user for parse
enum VMODREQ_PARSE{URL,MULTI,UNKNOWN};



ssize_t vmod_HTC_Read(struct worker *, struct http_conn *, void *, size_t );

static int vmod_Hook_unset_deliver(const struct vrt_ctx *);
static int vmod_Hook_unset_bereq(const struct vrt_ctx *);
static int vmod_Hook_unset_error(const struct vrt_ctx *);
static void vmod_Hook_Miss_opt_post_loopup(const struct vrt_ctx *);

static void vmodreq_headers_free(struct vmod_headers *);


const char *vmodreq_header(const struct vrt_ctx *ctx, enum VMODREQ_TYPE , const char *);
void vmodreq_sethead(const struct vrt_ctx *,struct vmod_request *, enum VMODREQ_TYPE ,const char *, const char *,int);
struct vmod_headers *vmodreq_getheaders(const struct vrt_ctx *,struct vmod_request *, enum VMODREQ_TYPE );
struct vmod_request *vmodreq_get(const struct vrt_ctx *);
struct vmod_request *vmodreq_init(const struct vrt_ctx *);
void vmodreq_init_cookie(const struct vrt_ctx *,struct vmod_request *);
void vmodreq_init_get(const struct vrt_ctx *,struct vmod_request *);
void vmodreq_init_post(const struct vrt_ctx *,struct vmod_request *);
struct vmod_request *vmodreq_get_raw(const struct vrt_ctx *);
static void vmodreq_free(struct vmod_request *);

int decodeForm_multipart(const struct vrt_ctx *,char *);
int vmodreq_get_parse(const struct vrt_ctx *);
int vmodreq_cookie_parse(const struct vrt_ctx *);
int vmodreq_reqbody(const struct vrt_ctx *ctx, char**,int*);
int vmodreq_post_parse(const struct vrt_ctx *);
void init_header(const struct vrt_ctx *ctx, enum gethdr_e);

const char *vmodreq_getheader(const struct vrt_ctx *,struct vmod_request *, enum VMODREQ_TYPE , const char *);
int vmodreq_getheadersize(const struct vrt_ctx *,struct vmod_request *, enum VMODREQ_TYPE , const char *);
struct hdr *vmodreq_getrawheader(const struct vrt_ctx *,struct vmod_request *, enum VMODREQ_TYPE , const char *);
int vmodreq_decode_urlencode(const struct vrt_ctx *,char *,enum VMODREQ_TYPE,char,char,int);

int vmodreq_hdr_count(const struct vrt_ctx *ctx, enum VMODREQ_TYPE );
const char *vmodreq_seek(const struct vrt_ctx *ctx, enum VMODREQ_TYPE );
void vmodreq_seek_reset(const struct vrt_ctx *ctx, enum VMODREQ_TYPE );

void debugmsg(const struct vrt_ctx *,const char*,...);

typedef int (*vcl_userdef_func)(const struct vrt_ctx *sp);

const char* vmod_read_cur(const struct vrt_ctx *ctx, enum VMODREQ_TYPE);
unsigned vmod_read_iterate(const struct vrt_ctx *ctx, const char* , enum VMODREQ_TYPE type);

int vmodreq_headersize(const struct vrt_ctx *ctx, enum VMODREQ_TYPE , const char *);
enum VMODREQ_TYPE vmod_convtype(const char*);
enum gethdr_e vmod_convhdrtype(const struct vrt_ctx *,const char*, unsigned*);
void gen_hdrtxt(const char *, char *, int);
int count_header(const struct vrt_ctx *ctx, enum gethdr_e );
struct http * vrt_selecthttp(const struct vrt_ctx *ctx, enum gethdr_e);
const char*get_header_key(const struct vrt_ctx *ctx, enum gethdr_e , int );
void header_iterate(const struct vrt_ctx *ctx, const char* , enum gethdr_e );
void setdebug();
void chkinit(const struct vrt_ctx *);
