#include "vmod_parsereq.h"


///////////////////////////////////////////////////////////////
//キーのオフセット初期化
VCL_VOID
vmod_reset_offset(VRT_CTX, VCL_ENUM type){
	vmodreq_seek_reset(ctx, vmod_convtype(type));
}

VCL_VOID
vmod_post_seek_reset(VRT_CTX){
	vmodreq_seek_reset(ctx, POST);
}
VCL_VOID
vmod_get_seek_reset(VRT_CTX){
	vmodreq_seek_reset(ctx, GET);
}
VCL_VOID
vmod_cookie_seek_reset(VRT_CTX){
	vmodreq_seek_reset(ctx, COOKIE);
}

///////////////////////////////////////////////////////////////
//キーのオフセットを移動＋移動後の値取得
const char *vmod_next_key(VRT_CTX, VCL_ENUM type){
	return vmodreq_seek(ctx, vmod_convtype(type));
}

const char* vmod_get_read_keylist(VRT_CTX){
	return vmodreq_seek(ctx, GET);
}
const char* vmod_post_read_keylist(VRT_CTX){
	return vmodreq_seek(ctx, POST);
}
const char* vmod_cookie_read_keylist(VRT_CTX){
	return vmodreq_seek(ctx, COOKIE);
}



///////////////////////////////////////////////////////////////
//キーのオフセットを移動
VCL_VOID
vmod_next_offset(VRT_CTX, VCL_ENUM type){
	vmodreq_seek(ctx, vmod_convtype(type));

}

///////////////////////////////////////////////////////////////
//現在のキー名を取得
const char* vmod_current_key(VRT_CTX, VCL_ENUM type){
	return vmod_read_cur(ctx, vmod_convtype(type));

}

///////////////////////////////////////////////////////////////
//反復処理系
unsigned vmod_iterate(VRT_CTX, VCL_ENUM type, const char* p){
	return vmod_read_iterate(ctx, p,vmod_convtype(type));
}


///////////////////////////////////////////////////////////////
//サイズ取得系関数
int vmod_size(VRT_CTX, VCL_ENUM type, VCL_STRING header)
{
	unsigned ret = 0;
	enum gethdr_e where = vmod_convhdrtype(ctx, type, &ret);
	if(ret){
		//headerの値を作る必要がある
		char tmp[256];
		gen_hdrtxt(header, tmp, 256);
		char * val = VRT_GetHdr(ctx,  where, tmp);
		if(val){
			return strlen(VRT_GetHdr(ctx,  where, tmp));
		}else{
			return 0;
		}
	}else{
		return vmodreq_headersize(ctx, vmod_convtype(type) ,header);
	}
}

///////////////////////////////////////////////////////////////
//Value取得系関数
const char *vmod_param(VRT_CTX, VCL_ENUM type ,VCL_STRING header){
	unsigned ret = 0;
	enum gethdr_e where = vmod_convhdrtype(ctx, type, &ret);
	if(ret){
		//headerの値を作る必要がある
		char tmp[256];
		gen_hdrtxt(header, tmp, 256);
		return VRT_GetHdr(ctx,  where, tmp);
	}else{
		return vmodreq_header(ctx, vmod_convtype(type) ,header);
	}
}

const char *vmod_post_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, POST,header);
}

const char *vmod_get_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, GET,header);
}

const char *vmod_cookie_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, COOKIE,header);
}

///////////////////////////////////////////////////////////////
//生body取得系関数
const char* vmod_body(VRT_CTX, VCL_ENUM type){
	if(!vmodreq_get_raw(sp)){
		VRT_panic(ctx, "please write \"parsereq.init();\" to 1st line in vcl_recv.",vrt_magic_string_end);
	}
	struct vmod_request *c = vmodreq_get(sp);
	enum VMODREQ_TYPE t = vmod_convtype(type);
	switch(t){
		case POST:
			return c->raw_post;
			break;
		case GET:
			return c->raw_get;
			break;
		case COOKIE:
			return c->raw_cookie;
			break;
	}

}

const char* vmod_post_body(VRT_CTX){
	return vmod_body(ctx,  "post");
}

const char* vmod_get_body(VRT_CTX){
	return vmod_body(ctx,  "get");
}

const char* vmod_cookie_body(VRT_CTX){
	return vmod_body(ctx,  "cookie");
}
///////////////////////////////////////////////////////////////
//初期化、デバッグなどシステム系
VCL_VOID
vmod_init(VRT_CTX){
	struct vmod_request *c;
	c = vmodreq_get_raw(sp);
	if(c){
		c->nowtype = NONE;
	}else{
		vmodreq_get(sp);
	}
}

VCL_VOID
vmod_debuginit(VRT_CTX)
{
	setdebug();
	vmod_init(sp);
}

VCL_VOID
vmod_setopt(VRT_CTX, const char *opt){
	if(!vmodreq_get_raw(sp)){
		VRT_panic(ctx, "please write \"parsereq.init();\" to 1st line in vcl_recv.",vrt_magic_string_end);
	}
	struct vmod_request *c = vmodreq_get(sp);
	if (!strcmp(opt, "enable_post_lookup")){
		c->opt_post_lookup = (1==1);
		return;
	}
}

int vmod_errcode(VRT_CTX){
	if(!vmodreq_get_raw(sp)){
		VRT_panic(ctx, "please write \"parsereq.init();\" to 1st line in vcl_recv.",vrt_magic_string_end);
	}
	return vmodreq_get(sp)->parse_ret;
}
