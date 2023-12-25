# 全链路内存马系列之 nginx 内存马

![](img/webshellattckchain.jpg)
其他内存马
- [ebpf 内核马](https://github.com/veo/ebpf_shell)
- [websocket 内存马](https://github.com/veo/wsMemShell)
- ### 注意
本项目不含有完整的利用工具，仅提供无害化测试程序、防御加固方案，以及研究思路讨论
- ### 测试程序使用方式
下载测试程序 [releases](https://github.com/veo/nginx_shell/releases) 

将下载的so放至目标服务器上，修改 nginx.conf 配置文件在第一行添加以下内容，path为路径，ngx_http_cre_module.so名称最好不修改。
```
load_module path/ngx_http_cre_module.so
```
然后重载nginx
```
nignx -s reload
```

POST HTTP header `vtoken: whoami` （测试程序只能使用 `whoami` 命令）
![](img/run.jpg)

- ### 一、技术特点
1. 无需临时编译（传统的 nignx so backdoor 需要临时编译）
2. 兼容支持大部分 nignx 版本
3. 无需额外组件支持


- ### 二、技术缺点
1. 有so文件落地
2. 需要 nignx -s reload 权限

- ### 三、技术原理
nginx module 支持动态加载so，通过 __attribute__的方式绕过nginx module version check，可以编译出适应所有nginx版本的module。
使用header_filter可以取得命令执行的参数，通过body_filter可以返回命令执行后的结果


- ### 四、研究中遇到的问题
1. 绕过nginx对于module的版本检测

    通过 __attribute__的方式绕过


2. 编写的module要兼容大部分版本

    使用较早版本的函数，不使用高版本新增函数


- ### 五、防御加固方案
1. 监测Nginx Module的加载，Nginx进程的行为
2. 查杀落地文件
3. 收敛 nignx -s reload 权限