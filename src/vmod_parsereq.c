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
VCL_STRING
vmod_next_key(VRT_CTX, VCL_ENUM type){
	return vmodreq_seek(ctx, vmod_convtype(type));
}

VCL_STRING
vmod_get_read_keylist(VRT_CTX){
	return vmodreq_seek(ctx, GET);
}
VCL_STRING
vmod_post_read_keylist(VRT_CTX){
	return vmodreq_seek(ctx, POST);
}
VCL_STRING
vmod_cookie_read_keylist(VRT_CTX){
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
VCL_STRING
vmod_current_key(VRT_CTX, VCL_ENUM type){
	return vmod_read_cur(ctx, vmod_convtype(type));

}

///////////////////////////////////////////////////////////////
//反復処理系
VCL_BOOL
vmod_iterate(VRT_CTX, VCL_ENUM type, VCL_STRING p){
	return vmod_read_iterate(ctx, p,vmod_convtype(type));
}


///////////////////////////////////////////////////////////////
//サイズ取得系関数
VCL_INT
vmod_size(VRT_CTX, VCL_ENUM type, VCL_STRING header)
{
	unsigned ret = 0;
	enum gethdr_e where = vmod_convhdrtype(ctx, type, &ret);
	if(ret){
		//headerの値を作る必要がある

        const struct gethdr_s hdr = {
            .what = HDR_REQ,
            .where = where
        };

		const char *val = VRT_GetHdr(ctx, &hdr);
		return val ? strlen(val) : 0;
	}else{
		return vmodreq_headersize(ctx, vmod_convtype(type) ,header);
	}
}

///////////////////////////////////////////////////////////////
//Value取得系関数
VCL_STRING
vmod_param(VRT_CTX, VCL_ENUM type ,VCL_STRING header){
	unsigned ret = 0;
	enum gethdr_e where = vmod_convhdrtype(ctx, type, &ret);
	if(ret){
		//headerの値を作る必要がある
        const struct gethdr_s hdr = {
            .what = HDR_REQ,
            .where = where
        };

		return VRT_GetHdr(ctx, &hdr);
	}

    return vmodreq_header(ctx, vmod_convtype(type) ,header);
}

VCL_STRING
vmod_post_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, POST,header);
}

VCL_STRING
vmod_get_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, GET,header);
}

VCL_STRING
vmod_cookie_header(VRT_CTX, VCL_STRING header){
	return vmodreq_header(ctx, COOKIE,header);
}

///////////////////////////////////////////////////////////////
//生body取得系関数
VCL_STRING
vmod_body(VRT_CTX, VCL_ENUM type){
	if(!vmodreq_get_raw(ctx->req)){
	    WRONG("please write \"parsereq.init();\" to 1st line in vcl_recv.");
	}
	struct vmod_request *c = vmodreq_get(ctx);
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
        case REQ:
        case AUTO:
        case NONE:
            // handle or ignore
            break;
        default:
            WRONG("Unhandled enum value in vmod_body");
	}

}

VCL_STRING
vmod_post_body(VRT_CTX){
	return vmod_body(ctx,  "post");
}

VCL_STRING
vmod_get_body(VRT_CTX){
	return vmod_body(ctx,  "get");
}

VCL_STRING
vmod_cookie_body(VRT_CTX){
	return vmod_body(ctx,  "cookie");
}
///////////////////////////////////////////////////////////////
//初期化、デバッグなどシステム系
VCL_VOID
vmod_init(VRT_CTX){
	struct vmod_request *c;
	c = vmodreq_get_raw(ctx->req);
	if(c){
		c->nowtype = NONE;
	}else{
		vmodreq_get(ctx);
	}
}

VCL_VOID
vmod_debuginit(VRT_CTX)
{
	setdebug();
	vmod_init(ctx);
}

VCL_VOID
vmod_setopt(VRT_CTX, const char *opt){
	if(!vmodreq_get_raw(ctx->req)){
		WRONG("please write \"parsereq.init();\" to 1st line in vcl_recv.");
	}
	struct vmod_request *c = vmodreq_get(ctx);
	if (!strcmp(opt, "enable_post_lookup")){
		c->opt_post_lookup = (1==1);
		return;
	}
}

VCL_INT
vmod_errcode(VRT_CTX){
	if(!vmodreq_get_raw(ctx->req)){
		WRONG("please write \"parsereq.init();\" to 1st line in vcl_recv.");
	}
	return vmodreq_get(ctx)->parse_ret;
}
