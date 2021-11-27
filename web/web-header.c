#include "web-header.h"
#include <uhttp.h>

static const char *const sa_initial[web_header_id_max] = {
	[web_header_id__a_im]                             = "A-IM",
	[web_header_id__accept]                           = "Accept",
	[web_header_id__accept_ch]                        = "Accept-CH",
	[web_header_id__accept_charset]                   = "Accept-Charset",
	[web_header_id__accept_datetime]                  = "Accept-Datetime",
	[web_header_id__accept_encoding]                  = "Accept-Encoding",
	[web_header_id__accept_language]                  = "Accept-Language",
	[web_header_id__accept_patch]                     = "Accept-Patch",
	[web_header_id__accept_ranges]                    = "Accept-Ranges",
	[web_header_id__access_control_allow_credentials] = "Access-Control-Allow-Credentials",
	[web_header_id__access_control_allow_headers]     = "Access-Control-Allow-Headers",
	[web_header_id__access_control_allow_methods]     = "Access-Control-Allow-Methods",
	[web_header_id__access_control_allow_origin]      = "Access-Control-Allow-Origin",
	[web_header_id__access_control_expose_headers]    = "Access-Control-Expose-Headers",
	[web_header_id__access_control_max_age]           = "Access-Control-Max-Age",
	[web_header_id__access_control_request_headers]   = "Access-Control-Request-Headers",
	[web_header_id__access_control_request_method]    = "Access-Control-Request-Method",
	[web_header_id__age]                              = "Age",
	[web_header_id__allow]                            = "Allow",
	[web_header_id__alt_svc]                          = "Alt-Svc",
	[web_header_id__authorization]                    = "Authorization",
	[web_header_id__cache_control]                    = "Cache-Control",
	[web_header_id__connection]                       = "Connection",
	[web_header_id__content_disposition]              = "Content-Disposition",
	[web_header_id__content_encoding]                 = "Content-Encoding",
	[web_header_id__content_language]                 = "Content-Language",
	[web_header_id__content_length]                   = "Content-Length",
	[web_header_id__content_location]                 = "Content-Location",
	[web_header_id__content_md5]                      = "Content-MD5",
	[web_header_id__content_range]                    = "Content-Range",
	[web_header_id__content_type]                     = "Content-Type",
	[web_header_id__cookie]                           = "Cookie",
	[web_header_id__date]                             = "Date",
	[web_header_id__delta_base]                       = "Delta-Base",
	[web_header_id__etag]                             = "ETag",
	[web_header_id__expect]                           = "Expect",
	[web_header_id__expires]                          = "Expires",
	[web_header_id__forwarded]                        = "Forwarded",
	[web_header_id__from]                             = "From",
	[web_header_id__host]                             = "Host",
	[web_header_id__http2_settings]                   = "HTTP2-Settings",
	[web_header_id__if_match]                         = "If-Match",
	[web_header_id__if_modified_since]                = "If-Modified-Since",
	[web_header_id__if_none_match]                    = "If-None-Match",
	[web_header_id__if_range]                         = "If-Range",
	[web_header_id__if_unmodified_since]              = "If-Unmodified-Since",
	[web_header_id__im]                               = "IM",
	[web_header_id__last_modified]                    = "Last-Modified",
	[web_header_id__link]                             = "Link",
	[web_header_id__location]                         = "Location",
	[web_header_id__max_forwards]                     = "Max-Forwards",
	[web_header_id__origin]                           = "Origin",
	[web_header_id__p3p]                              = "P3P",
	[web_header_id__pragma]                           = "Pragma",
	[web_header_id__prefer]                           = "Prefer",
	[web_header_id__preference_applied]               = "Preference-Applied",
	[web_header_id__proxy_authenticate]               = "Proxy-Authenticate",
	[web_header_id__proxy_authorization]              = "Proxy-Authorization",
	[web_header_id__public_key_pins]                  = "Public-Key-Pins",
	[web_header_id__range]                            = "Range",
	[web_header_id__referer]                          = "Referer",
	[web_header_id__retry_after]                      = "Retry-After",
	[web_header_id__server]                           = "Server",
	[web_header_id__set_cookie]                       = "Set-Cookie",
	[web_header_id__strict_transport_security]        = "Strict-Transport-Security",
	[web_header_id__te]                               = "TE",
	[web_header_id__tk]                               = "Tk",
	[web_header_id__trailer]                          = "Trailer",
	[web_header_id__transfer_encoding]                = "Transfer-Encoding",
	[web_header_id__upgrade]                          = "Upgrade",
	[web_header_id__user_agent]                       = "User-Agent",
	[web_header_id__vary]                             = "Vary",
	[web_header_id__via]                              = "Via",
	[web_header_id__warning]                          = "Warning",
	[web_header_id__www_authenticate]                 = "WWW-Authenticate",
	[web_header_id__x_frame_options]                  = "X-Frame-Options",
};

struct web_header_s {
	refer_nstring_t name[web_header_id_max];
	refer_string_t id[web_header_id_max];
};

static void web_header_free_func(web_header_s *restrict r)
{
	uintptr_t i;
	for (i = 0; i < web_header_id_max; ++i)
	{
		if (r->name[i])
			refer_free(r->name[i]);
		if (r->id[i])
			refer_free(r->id[i]);
	}
}

web_header_s* web_header_alloc(void)
{
	struct web_header_s *restrict r;
	const char *const *restrict sa;
	const char *restrict name;
	uintptr_t i;
	r = (struct web_header_s *) refer_alloz(sizeof(struct web_header_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) web_header_free_func);
		sa = sa_initial;
		for (i = 0; i < web_header_id_max; ++i)
		{
			if (!(name = sa[i]))
				goto label_fail;
			r->name[i] = refer_dump_nstring(name);
			r->id[i] = uhttp_header_new_id(name);
			if (!r->name[i] || !r->id[i])
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

refer_nstring_t web_header_get_name(web_header_s *restrict r, web_header_id_t hid)
{
	if ((uint32_t) hid < web_header_id_max)
		return r->name[hid];
	return NULL;
}

refer_string_t web_header_get_id(web_header_s *restrict r, web_header_id_t hid)
{
	if ((uint32_t) hid < web_header_id_max)
		return r->id[hid];
	return NULL;
}
