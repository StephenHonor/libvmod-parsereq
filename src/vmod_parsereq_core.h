
#define POST_REQ_HDR "\024X-VMOD-PARSEREQ-PTR:"
#define POST_REQ_HDR_NAME "X-VMOD-PARSEREQ-PTR"

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


static int vmod_Hook_unset_deliver(const struct vrt_ctx *);
static int vmod_Hook_unset_bereq(const struct vrt_ctx *);
static int vmod_Hook_unset_error(const struct vrt_ctx *);
static void vmod_Hook_Miss_opt_post_loopup(const struct vrt_ctx *);

static void vmodreq_headers_free(struct vmod_headers *);
static void vmodreq_free(struct vmod_request *);
