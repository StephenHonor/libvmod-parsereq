

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
